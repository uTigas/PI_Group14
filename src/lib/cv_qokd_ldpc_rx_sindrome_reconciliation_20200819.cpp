#include "cv_qokd_ldpc_rx_sindrome_reconciliation_20200819.h"

void CvQokdLdpcRxSindromeReconciliation::initialize(void)
{
	if (errorCorrectionMethod == CvQokdLdpcRxSindromeReconciliation::t_method::LDPC)
	{
		loadSparseParityCheckMatrix();

		sparseParityCheckMatrixTranspose = sparseParityCheckMatrix.transpose();

		calculatedSindromeSize = codeLength - codeRank;

		calculatedSindrome_0.resize(calculatedSindromeSize, 1);
		calculatedSindrome_0_posIn = 0;
		calculatedSindrome_0_posOut = calculatedSindrome_0.size();
		calculatedSindrome_0_newData = false;

		calculatedSindrome_1.resize(calculatedSindromeSize, 1);
		calculatedSindrome_1_posIn = 0;
		calculatedSindrome_1_posOut = calculatedSindrome_1.size();
		calculatedSindrome_1_newData = false;

		receivedSindromeSize = codeLength - codeRank;
		receivedSindrome.resize(receivedSindromeSize, 1);
		receivedSindrome_posIn = 0;
		receivedSindrome_posOut = receivedSindrome.size();
		receivedSindrome_newData = false;

		receivedHash.resize(hashLength);

		receivedDataSize_0 = codeLength;
		receivedData_0.resize(receivedDataSize_0, 1);
		receivedData_0_posIn = 0;
		receivedData_0_posOut = receivedData_0.size();
		receivedData_0_newData = false;

		receivedDataSize_1 = codeLength;
		receivedData_1.resize(receivedDataSize_1, 1);
		receivedData_1_posIn = 0;
		receivedData_1_posOut = receivedData_1.size();
		receivedData_1_newData = false;

		DataIn.resize((size_t)4*inputSignals[0]->getBufferLength());
		DataOut.resize(DataIn.size());
		SindromeIn.resize((size_t)4*inputSignals[1]->getBufferLength());
		SindromeOut.resize(4*calculatedSindromeSize);

		prioriMessageProbabilities.resize(codeLength, 1);

		M.resize(codeLength - codeRank, codeLength);
		M.reserve(numberOfNonZerosValuesInSparseParityCheckMatrix);

		E.resize(codeLength - codeRank, codeLength);
		E.reserve(numberOfNonZerosValuesInSparseParityCheckMatrix);

		tripletList.reserve(getNumberOfNonZerosValuesInSparseParityCheckMatrix());

		bitFillingPeriod = numberOfDataBitsPerBitFillingPeriod + numberOfDummyBitsPerBitFillingPeriod;
	}
	else
	{
		std::cerr << "DVQkdRxErrorCorrectionLDPC::initialize() - wrong error correction method";
	}

	startSendingSindrome = (getErrorCorrectionRole() == t_role::Rx && getErrorCorrectionMode() == t_mode::InverseReconciliation) || (getErrorCorrectionRole() == t_role::Tx && getErrorCorrectionMode() == t_mode::DirectReconciliation);
}

bool CvQokdLdpcRxSindromeReconciliation::runBlock(void)
{
	if (bypassErrorCorrection)
	{
		auto ready = inputSignals[0]->ready();
		auto space = outputSignals[0]->space();
		auto process = std::min(ready, space);
		totalNumberOfInputBits += process;
		totalNumberOfOutputBits += process;

		for (; process--;)
		{
			t_binary dataIn{ 0 };
			inputSignals[0]->bufferGet(&dataIn);
			outputSignals[0]->bufferPut(dataIn);
		}

		for (auto k = 1; k < inputSignals.size(); k++)
		{
			ready = inputSignals[k]->ready();
			space = inputSignals[k]->space();
			process = std::min(ready, space);
			for (; process--;)
				inputSignals[k]->bufferGet();
		}
		return false;
	}

	bool alive{ false };

	Eigen::Index ready = DataOut.ready();
	Eigen::Index space = outputSignals[0]->space();
	Eigen::Index process = std::min(ready, space);
	alive = !alive ? process > 0: alive;
	totalNumberOfOutputBits += process;
	for (; process--;)
		outputSignals[0]->bufferPut(DataOut.bufferGet());
	
	ready = SindromeOut.ready();
	space = outputSignals[1]->space();
	process = std::min(ready, space);
	alive = !alive ? process > 0: alive;
	for (; process--;)
		outputSignals[1]->bufferPut(SindromeOut.bufferGet());

	ready = inputSignals[0]->ready();
	space = DataIn.space();
	process = std::min(ready, space);
	alive = !alive ? process > 0: alive;
	totalNumberOfInputBits += process;
	for (; process--;)
	{
		t_binary val{ 0 };
		inputSignals[0]->bufferGet(&val);
		DataIn.bufferPut(val);
	}

	ready = inputSignals[1]->ready();
	space = SindromeIn.space();
	process = std::min(ready, space);
	alive = !alive ? process > 0: alive;
	for (; process--;)
	{
		t_binary val{ 0 };
		inputSignals[1]->bufferGet(&val);
		SindromeIn.bufferPut(val);
	}

	ready = SindromeIn.ready();
	process = std::min(ready, receivedSindromeSize - receivedSindrome_posIn);
	alive = !alive ? process > 0: alive;
	for (auto k = receivedSindrome_posIn; k < receivedSindrome_posIn + process; k++)
		receivedSindrome(k, 0) = SindromeIn.bufferGet();
	receivedSindrome_posIn += process;
	ready = SindromeIn.ready();
	process = std::min(ready, receivedSindromeSize + hashLength - receivedSindrome_posIn);
	alive = !alive ? process > 0: alive;
	for (auto k = receivedSindrome_posIn; k < receivedSindrome_posIn + process; k++) {
		receivedHash[k - receivedSindromeSize] = SindromeIn.bufferGet();
	}

	receivedSindrome_posIn += process;
	if (process > 0 && receivedSindrome_posIn == receivedSindromeSize+hashLength)
	{
		if (getVerboseMode())
		{
			std::ofstream file;
			file.open(getSignalsFolderName() + "/received_sindrome_hash_rx.txt", std::ios_base::app);
			for (auto k = 0; k < receivedSindromeSize; k++)
			{
				if (k != 0) file << " ";
				file << std::to_string(receivedSindrome[k]);
			}
			for (auto k = 0; k < hashLength; k++)
			{
				if (k == 0) file << " ";
				file << receivedHash[k];
			}
			file << std::endl;
			file.close();
		}
		receivedSindrome_newData = true;
	}

	ready = DataIn.ready();
	space = *pt_receivedDataSize - *pt_receivedData_posIn;
	process = std::min(ready, space);
	alive = !alive ? process > 0: alive;
	while (process)
	{
		if (numberOfDummyBitsPerBitFillingPeriod)
		{
			if (*pt_inDataBitFillingCounter >= numberOfDataBitsPerBitFillingPeriod)
			{
				(*pt_receivedData)[*pt_receivedData_posIn] = 0;
			}
			else
			{
				(*pt_receivedData)[*pt_receivedData_posIn] = DataIn.bufferGet();
				ready--;
			}
			*pt_inDataBitFillingCounter = ++(*pt_inDataBitFillingCounter) % bitFillingPeriod;
		}
		else
		{
			(*pt_receivedData)[*pt_receivedData_posIn] = DataIn.bufferGet();
			ready--;
		}
		space--;
		(*pt_receivedData_posIn) = (*pt_receivedData_posIn) + 1;
		if ((ready == 0) || (space == 0)) break;
	}

	if (process > 0 && *pt_receivedData_posIn == *pt_receivedDataSize)
		*pt_receivedData_newData = true;

	if (*pt_receivedData_newData && *pt_calculatedSindrome_posIn == 0 && *pt_calculatedSindrome_newData == false)
	{
		if (getVerboseMode())
		{
			std::ofstream file;
			file.open(getSignalsFolderName() + "/receivedData_rx.txt", std::ios_base::app);
			for (auto k = 0; k < *pt_receivedDataSize; k++)
			{
				if (k != 0 && k % 200 == 0) file << std::endl;
				file << std::to_string((*pt_receivedData)[k]);
			}
			file << std::endl;
			file.close();
		}

		*pt_calculatedSindrome = sparseParityCheckMatrix * *pt_receivedData;
		*pt_calculatedSindrome = (*pt_calculatedSindrome).unaryExpr([](const t_binary x) {return (t_binary)(x % 2);}); //to compute mod(sindrome,2)

		numberOfCalculatedSindromes++;

		*pt_receivedData_posIn = *pt_receivedDataSize;
		*pt_receivedData_posOut = *pt_receivedDataSize;
		*pt_receivedData_newData = false;

		if (getVerboseMode())
		{
			std::ofstream file;
			file.open(getSignalsFolderName() + "/calculated_sindrome_rx.txt", std::ios_base::app);
			for (auto k = 0; k < *pt_calculatedSindromeSize; k++)
			{
				if (k != 0) file << " ";
				file << std::to_string((*pt_calculatedSindrome)[k]);
			}
			file << std::endl;
			file.close();
		}

		*pt_calculatedSindrome_posIn = *pt_calculatedSindromeSize;
		*pt_calculatedSindrome_newData = true;

		if (process_block == 1)
		{
			if (startSendingSindrome)
			{
				calculatedSindrome_1_posOut = 0;
				startedSendingSindrome_1 = true;
			}

			process_block = 0;

			pt_receivedDataSize = &receivedDataSize_0;
			pt_receivedData = &receivedData_0;
			pt_receivedData_posIn = &receivedData_0_posIn;
			pt_receivedData_posOut = &receivedData_0_posOut;
			pt_receivedData_newData = &receivedData_0_newData;

			pt_calculatedSindrome = &calculatedSindrome_0;
			pt_calculatedSindromeSize = &calculatedSindromeSize;
			pt_calculatedSindrome_posIn = &calculatedSindrome_0_posIn;
			pt_calculatedSindrome_posOut = &calculatedSindrome_0_posOut;
			pt_calculatedSindrome_newData = &calculatedSindrome_0_newData;

			pt_inDataBitFillingCounter = &inDataBitFillingCounter_0;
		}
		else
		{
			if (startSendingSindrome)
			{
				calculatedSindrome_0_posOut = 0;
				startedSendingSindrome_0 = true;
			}

			process_block = 1;

			pt_receivedDataSize = &receivedDataSize_1;
			pt_receivedData = &receivedData_1;
			pt_receivedData_posIn = &receivedData_1_posIn;
			pt_receivedData_posOut = &receivedData_1_posOut;
			pt_receivedData_newData = &receivedData_1_newData;

			pt_calculatedSindrome = &calculatedSindrome_1;
			pt_calculatedSindromeSize = &calculatedSindromeSize;
			pt_calculatedSindrome_posIn = &calculatedSindrome_1_posIn;
			pt_calculatedSindrome_posOut = &calculatedSindrome_1_posOut;
			pt_calculatedSindrome_newData = &calculatedSindrome_1_newData;

			pt_inDataBitFillingCounter = &inDataBitFillingCounter_1;
		}
	}

	ready = calculatedSindromeSize - calculatedSindrome_0_posOut;
	space = SindromeOut.space();
	process = std::min(ready, space);
	if (startedSendingSindrome_0 && (!startedSendingSindrome_1) && process)
	{
		for (auto k = calculatedSindrome_0_posOut; k < calculatedSindrome_0_posOut + process; k++)
			SindromeOut.bufferPut(calculatedSindrome_0[k]);

		calculatedSindrome_0_posOut = calculatedSindrome_0_posOut + process;
		if (calculatedSindrome_0_posIn == calculatedSindromeSize) calculatedSindrome_0_posIn = 0;

		if (calculatedSindrome_0_posOut == calculatedSindromeSize)
			startedSendingSindrome_0 = false;
	}

	ready = calculatedSindromeSize - calculatedSindrome_1_posOut;
	space = SindromeOut.space();
	process = std::min(ready, space);
	if (startedSendingSindrome_1 && (!startedSendingSindrome_0) && process)
	{
		for (auto k = calculatedSindrome_1_posOut; k < calculatedSindrome_1_posOut + process; k++)
			SindromeOut.bufferPut(calculatedSindrome_1[k]);

		calculatedSindrome_1_posOut = calculatedSindrome_1_posOut + process;
		if (calculatedSindrome_1_posIn == calculatedSindromeSize) calculatedSindrome_1_posIn = 0;

		if (calculatedSindrome_1_posOut == calculatedSindromeSize)
			startedSendingSindrome_1 = false;
	}

	// Perform error correction using LDPC for the first sindrome
	// and compare the sindromes in orderto find if they match.
	if ((compareSindrome == 0) && (calculatedSindrome_0_newData) && receivedSindrome_newData)
	{
		numberOfValidatedSindromes++;

		int sindromeNoMatch{ 0 };
		for (auto k = 0; k < calculatedSindromeSize; k++)
			if (calculatedSindrome_0[k] != receivedSindrome[k])
				sindromeNoMatch++;

		sindromeMatch = sindromeNoMatch == 0;
		if (!sindromeMatch && !startSendingSindrome)
			sindromeMatch = errorCorrectionLDPCAlgorithm(&calculatedSindrome_0, &receivedData_0);

		///////////////////////  SHA-1 VERIFICATION  ///////////////////////
		if (sindromeMatch)
		{
			CryptoPP::SHA1 hash;
			std::stringstream ss;
			std::string calculatedHash;
			std::string receivedHashString(receivedHash.begin(), receivedHash.end());

			ss << receivedData_0;
			std::string matrix_string = ss.str();

			hash.Update((const uint8_t*)matrix_string.data(), matrix_string.size());

			calculatedHash.resize(hash.DigestSize());
			hash.Final((uint8_t*)&calculatedHash[0]);

			if (calculatedHash == receivedHashString) {
				sindromeMatch = 1;
			}
			else {
				sindromeMatch = 0;
			}
			if (getVerboseMode())
			{
				std::ofstream file;
				file.open(getSignalsFolderName() + "/calculated_hash_rx.txt", std::ios_base::app);
				file << "Received Hash:   " << receivedHashString << std::endl;
				file << "Calculated Hash: " << calculatedHash << std::endl;
				file << "Sindrome Match: ";
				sindromeMatch ? file << "true" : file << "false";
				file << std::endl;
				file.close();
			}
		}

		////////////////////////////////////////////////////////////////////


		if (getVerboseMode())
		{
			std::ofstream file;
			file.open(getSignalsFolderName() + "/correctedData_rx.txt", std::ios_base::app);
			if (sindromeMatch)
				file << "Sindrome 0: true\n";
			else
				file << "Sindrome 0: false\n";
			for (auto k = 0; k < receivedDataSize_0; k++)
			{
				if (k != 0 && k % 200 == 0) file << std::endl;
				file << std::to_string(receivedData_0[k]);
			}
			file << std::endl;
			file.close();
		}

		sindromeMatch_0 = sindromeMatch;

		if (!sindromeMatch) numberOfNoMatchedSindromes++;

		receivedSindrome_posIn = 0;
		receivedSindrome_newData = false;

		calculatedSindrome_0_newData = false;

		compareSindrome = 1;

	}
	
	// Perform error correction using LDPC for the second sindrome
	// and compare the sindromes in orderto find if they match.
	if ((compareSindrome==1) && (calculatedSindrome_1_newData) && receivedSindrome_newData && SindromeOut.space())
	{
		numberOfValidatedSindromes++;

		int sindromeNoMatch{ 0 };
		for (auto k = 0; k < calculatedSindromeSize; k++)
			if (calculatedSindrome_1[k] != receivedSindrome[k])
				sindromeNoMatch++;

		sindromeMatch = sindromeNoMatch == 0;
		if (!sindromeMatch && !startSendingSindrome)
			sindromeMatch = errorCorrectionLDPCAlgorithm(&calculatedSindrome_1, &receivedData_1);  

		///////////////////////  SHA-1 VERIFICATION  ///////////////////////
		if (sindromeMatch)
		{
			CryptoPP::SHA1 hash;
			std::stringstream ss;
			std::string calculatedHash;
			std::string receivedHashString(receivedHash.begin(), receivedHash.end());

			ss << receivedData_1;
			std::string matrix_string = ss.str();

			hash.Update((const uint8_t*)matrix_string.data(), matrix_string.size());

			calculatedHash.resize(hash.DigestSize());
			hash.Final((uint8_t*)&calculatedHash[0]);

			if (calculatedHash == receivedHashString) {
				sindromeMatch = 1;
			}
			else {
				sindromeMatch = 0;
			}
			if (getVerboseMode())
			{
				std::ofstream file;
				file.open(getSignalsFolderName() + "/calculated_hash_rx.txt", std::ios_base::app);
				file << "Received Hash:   " << receivedHashString << std::endl;
				file << "Calculated Hash: " << calculatedHash << std::endl;
				file << "Sindrome Match: ";
				sindromeMatch ? file << "true" : file << "false";
				file << std::endl;
				file.close();
			}
		}

		////////////////////////////////////////////////////////////////////

		if (getVerboseMode())
		{
			std::ofstream file;
			file.open(getSignalsFolderName() + "/correctedData_rx.txt", std::ios_base::app);
			if (sindromeMatch)
				file << "Sindrome 1: true\n";
			else
				file << "Sindrome 1: false\n";
			for (auto k = 0; k < receivedDataSize_1; k++)
			{
				if (k != 0 && k % 200 == 0) file << std::endl;
				file << std::to_string(receivedData_1[k]);
			}
			file << std::endl;
			file.close();
		}
		
		sindromeMatch_1 = sindromeMatch;

		if (!sindromeMatch)
			numberOfNoMatchedSindromes++;
		else
			numberOfMatchedSindromes++;

		receivedSindrome_posIn = 0;
		receivedSindrome_newData = false;

		calculatedSindrome_1_newData = false;

		compareSindrome = 0;

		numberOfValidatedPairs++;

		// Set state variables and send ACK message to TX
		// These are the possible cases (all implemented by the if/else)
		//  SM1 SM2 ACK
		//  0   0   00 (00 for sym keys)
		//  0   1   11 (01 for sym keys)
		//  1   0   11 (10 for sym keys)
		//  1   1   11 (11 for sym keys)
		if (sindromeMatch_0==0 && sindromeMatch_1==0) // 00
		{
			SindromeOut.bufferPut((t_binary)0);
			SindromeOut.bufferPut((t_binary)0);
			receivedData_0_posIn = 0;
			inDataBitFillingCounter_0 = 0;
			receivedData_1_posIn = 0;
			inDataBitFillingCounter_1 = 0;
			numberOfDiscardedPairs++;
		}
		else // 01 or 10 or 11
		{
			if(sym && sindromeMatch_0 && sindromeMatch_1){
				SindromeOut.bufferPut((t_binary)1);
				SindromeOut.bufferPut((t_binary)1);
			}
			else if(sym && sindromeMatch_0){
				SindromeOut.bufferPut((t_binary)1);
				SindromeOut.bufferPut((t_binary)0);
			}
			else if(sym && sindromeMatch_1){
				SindromeOut.bufferPut((t_binary)0);
				SindromeOut.bufferPut((t_binary)1);
			}
			else {
				SindromeOut.bufferPut((t_binary)1);
				SindromeOut.bufferPut((t_binary)1);
			}
			receivedData_0_posOut = 0;
			outDataBitFillingCounter_0 = 0;
			receivedData_1_posOut = 0;
			outDataBitFillingCounter_1 = 0;
			numberOfSavedPairs++;
		}

		calculatedSindrome_0_posIn = 0;
		calculatedSindrome_1_posIn = 0;
	}
	
	ready = std::min(receivedDataSize_0 - receivedData_0_posOut, receivedDataSize_1 - receivedData_1_posOut);
	space = (Eigen::Index) DataOut.space();

	// Put the key data in the output signal buffer
	if ((ready>0) & (space>=2))
	{
		while((ready>0) & (space>=2))
		{
			if (numberOfDummyBitsPerBitFillingPeriod)
			{
				if (*pt_outDataBitFillingCounter < numberOfDataBitsPerBitFillingPeriod)
				{
					if(sym && sindromeMatch_0 && sindromeMatch_1)
					{
						DataOut.bufferPut((t_binary)receivedData_0[receivedData_0_posOut]);
						DataOut.bufferPut((t_binary)receivedData_1[receivedData_1_posOut]);
						space -= 2;
					}
					else if (sindromeMatch_0)
					{
						DataOut.bufferPut((t_binary)0);
						DataOut.bufferPut((t_binary)receivedData_0[receivedData_0_posOut]);
						space -= 2;
					}
					else
					{
						DataOut.bufferPut((t_binary)1);
						DataOut.bufferPut((t_binary)receivedData_1[receivedData_1_posOut]);
						space -= 2;
					}
					
				}
				*pt_outDataBitFillingCounter = ++(*pt_outDataBitFillingCounter) % bitFillingPeriod;
			}
			else
			{
				if(sym && sindromeMatch_0 && sindromeMatch_1)
				{
					DataOut.bufferPut((t_binary)receivedData_0[receivedData_0_posOut]);
					DataOut.bufferPut((t_binary)receivedData_1[receivedData_1_posOut]);
					space -= 2;
				}
				else if (sindromeMatch_0)
				{
					DataOut.bufferPut((t_binary)0);
					DataOut.bufferPut(receivedData_0[receivedData_0_posOut]);
					space -= 2;
				}
				
				else
				{
					DataOut.bufferPut((t_binary)1);
					DataOut.bufferPut(receivedData_1[receivedData_1_posOut]);
					space -= 2;
				}
				
			}
			receivedData_0_posOut++;
			receivedData_1_posOut++;
			ready--;
		}

		if (receivedData_0_posIn == receivedDataSize_0)
		{
			receivedData_0_posIn = 0;
			inDataBitFillingCounter_0 = 0;
		}
		if (receivedData_1_posIn == receivedDataSize_1)
		{
			receivedData_1_posIn = 0;
			inDataBitFillingCounter_1 = 0;
		}

		if (sindromeMatch) sindromeMatch = false;
	}

	// Put data from the buffers into the output signals
	ready = DataOut.ready();
	space = outputSignals[0]->space();
	process = std::min(ready, space);
	alive = !alive ? process > 0: alive;
	totalNumberOfOutputBits += process;
	for (; process--;)
		outputSignals[0]->bufferPut(DataOut.bufferGet());
	
	ready = SindromeOut.ready();
	space = outputSignals[1]->space();
	process = std::min(ready, space);
	alive = !alive ? process > 0: alive;
	for (; process--;)
		outputSignals[1]->bufferPut(SindromeOut.bufferGet());

	// Receive data from input signals
	ready = SindromeIn.ready();
	process = std::min(ready, receivedSindromeSize - receivedSindrome_posIn);
	alive = !alive ? process > 0: alive;
	for (auto k = receivedSindrome_posIn; k < receivedSindrome_posIn + process; k++)
		receivedSindrome(k, 0) = SindromeIn.bufferGet();

	receivedSindrome_posIn += process;
	if (process > 0 && receivedSindrome_posIn == receivedSindromeSize)
	{
		if (getVerboseMode())
		{
			std::ofstream file;
			file.open("/received_sindrome_rx.txt", std::ios_base::app);
			for (auto k = 0; k < receivedSindromeSize; k++)
				file << std::to_string(receivedSindrome[k]);
			file.close();
		}
		receivedSindrome_newData = true;
	}
	
	ready = DataIn.ready();
	process = std::min(ready, *pt_receivedDataSize - *pt_receivedData_posIn);
	alive = !alive ? process > 0: alive;
	for (auto k = *pt_receivedData_posIn; k < *pt_receivedData_posIn + process; k++)
	{
		if (numberOfDummyBitsPerBitFillingPeriod)
		{
			if (*pt_inDataBitFillingCounter >= numberOfDataBitsPerBitFillingPeriod)
			{
				(*pt_receivedData)[k] = 0;
			}
			else
				(*pt_receivedData)[k] = DataIn.bufferGet();

			*pt_inDataBitFillingCounter = ++(*pt_inDataBitFillingCounter) % bitFillingPeriod;
		}
		else
			(*pt_receivedData)[k] = DataIn.bufferGet();
	}

	*pt_receivedData_posIn += process;
	if (process > 0 && *pt_receivedData_posIn == *pt_receivedDataSize)
		*pt_receivedData_newData = true;
	
	return alive;
}

t_bool CvQokdLdpcRxSindromeReconciliation::errorCorrectionLDPCAlgorithm(Eigen::Matrix<t_binary, Eigen::Dynamic, 1>* pt_calculatedSindrome, Eigen::Matrix<t_binary, Eigen::Dynamic, 1>* pt_receivedData)
{

	for (int k = 0; k < codeLength; ++k)
	{
		if ((*pt_receivedData)[k] == 0)
			prioriMessageProbabilities[k] = log((1 - p_f) / p_f);
		else
			prioriMessageProbabilities[k] = log(p_f / (1 - p_f));
	}

	for (int k = 0; k < sparseParityCheckMatrix.outerSize(); ++k)   // iterates over the columns
	{
		for (Eigen::SparseMatrix<t_binary>::InnerIterator h(sparseParityCheckMatrix, k); h; ++h)  //iterates over the non-zero elements of each column
		{
			tripletList.push_back(T((int)h.row(), k, prioriMessageProbabilities[k]));
		}
	}

	M.setFromTriplets(tripletList.begin(), tripletList.end());
	tripletList.resize(0);

	auto sindromeNoMatch{ 0 };
	for (auto m = 0; m < ldpcErrorCorrectionAlgorithMaxIterations; ++m) 
	{
		for (int k = 0; k < sparseParityCheckMatrixTranspose.outerSize(); ++k)   // iterates over the columns of H^T, i.e. the lines of H
		{
			for (Eigen::SparseMatrix<t_binary>::InnerIterator h(sparseParityCheckMatrixTranspose, k); h; ++h)  //iterates over the non-zero elements of each column of H^T, i.e. over the non-zero elements of each line of H
			{
				t_real prod{ 1 };
				for (Eigen::SparseMatrix<t_binary>::InnerIterator o(sparseParityCheckMatrixTranspose, k); o; ++o)  //iterates over the non-zero elements of each column of H^T, i.e. over the non-zero elements of each line of H
				{
					if (o.row() != h.row())
						prod = prod * tanh(M.coeff(k, (int)o.row()) / 2);
				}
				tripletList.push_back(T(k, (int)h.row(), log(1 + (1 - 2 * (t_real)receivedSindrome[k]) * prod) - log(1 - (1 - 2 * (t_real)receivedSindrome[k]) * prod)));
			}
		}

		E.setFromTriplets(tripletList.begin(), tripletList.end());
		tripletList.resize(0);

		for (int k = 0; k < codeLength; ++k)
		{
			t_real sum{ 0 };
			for (Eigen::SparseMatrix<t_binary>::InnerIterator h(sparseParityCheckMatrix, k); h; ++h)
			{
				sum = sum + E.coeff((int)h.row(), k);
			}
			auto L = sum + prioriMessageProbabilities[k];
			if (L <= 0.0)
				(*pt_receivedData)[k] = 1;
			else
				(*pt_receivedData)[k] = 0;
		}

		(*pt_calculatedSindrome) = sparseParityCheckMatrix * (*pt_receivedData);
		*pt_calculatedSindrome = (*pt_calculatedSindrome).unaryExpr([](const t_binary x) {return (t_binary)(x % 2);}); //to compute mod(sindrome,2)

		sindromeNoMatch = 0;
		for (auto k = 0; k < *pt_calculatedSindromeSize; k++)
			if ((*pt_calculatedSindrome)[k] != receivedSindrome[k])
				sindromeNoMatch++;

		if (sindromeNoMatch == 0)
		{
			if (getVerboseMode())
			{
				std::ofstream file;
				file.open( getSignalsFolderName() + "/new_sindrome_rx.txt", std::ios_base::app);
				if (file.is_open())
				{
					for (auto k = 0; k < *pt_calculatedSindromeSize; k++)
						file << std::to_string((*pt_calculatedSindrome)[k]);
					file << std::endl;
					file.close();
				}
				else
					std::cerr << "DV_QKD_LDPC_TX_Error_Correction: Error opening file";
			}
			break;
		}

		for (int k = 0; k < sparseParityCheckMatrix.outerSize(); ++k)   // iterates over the columns of H
		{
			for (Eigen::SparseMatrix<t_binary>::InnerIterator h(sparseParityCheckMatrix, k); h; ++h)  //iterates over the non-zero elements of each column of H
			{
				t_real sum{ 0 };
				for (Eigen::SparseMatrix<t_binary>::InnerIterator o(sparseParityCheckMatrix, k); o; ++o)  //iterates over the non-zero elements of each column of H^T, i.e. over the non-zero elements of each line of H
				{
					if (o.row() != h.row())
						sum = sum + E.coeff((int)o.row(), k);
				}
				tripletList.push_back(T((int)h.row(), k, sum + prioriMessageProbabilities[k]));
			}
		}
		M.setFromTriplets(tripletList.begin(), tripletList.end());
		tripletList.resize(0);
	}

	if (sindromeNoMatch > 0)
		return false;
	else
		return true;
}

void CvQokdLdpcRxSindromeReconciliation::loadSparseParityCheckMatrix()
{
	auto pos_a = parityCheckMatrixFileName.find("_n");
	auto pos_b = parityCheckMatrixFileName.find("_k");
	codeLength = stoi(parityCheckMatrixFileName.substr(pos_a + 2, pos_b - pos_a - 2));

	pos_a = parityCheckMatrixFileName.find("_k");
	pos_b = parityCheckMatrixFileName.find("_nonZeros");
	codeRank = stoi(parityCheckMatrixFileName.substr(pos_a + 2, pos_b - pos_a - 2));

	pos_a = parityCheckMatrixFileName.find("_nonZeros");
	pos_b = parityCheckMatrixFileName.find(".dat");
	numberOfNonZerosValuesInSparseParityCheckMatrix = stoi(parityCheckMatrixFileName.substr(pos_a + 9, pos_b - pos_a - 9));

	sparseParityCheckMatrix.resize(codeLength - codeRank, codeLength);
	sparseParityCheckMatrix.reserve(numberOfNonZerosValuesInSparseParityCheckMatrix);

	if (parityCheckMatrixFileName.substr(parityCheckMatrixFileName.find_last_of(".") + 1) == "txt")
	{
		std::fstream fp;
		fp.open(parityCheckMatrixFolderName + "/" + parityCheckMatrixFileName, std::ios::in);
		if (!fp.is_open()) std::cerr << "Error in dv_qkd_ldpc_error_correction_.cpp: file sparseParityCheckMatrix.txt not found!!!!";
		double data;
		int row{ 0 }, column{ 0 };
		while (true)
		{
			fp >> data;
			if (data != 0)
				sparseParityCheckMatrix.insert(row, column) = (uint8_t)data;
			column++;
			if (column == codeLength)
			{
				column = 0;
				row++;
			}
			if (row == codeLength - codeRank)
				break;
		}
		fp.close();
	} 
	else if (parityCheckMatrixFileName.substr(parityCheckMatrixFileName.find_last_of(".") + 1) == "dat")
	{
		std::ifstream parityCheckMatrixFile(parityCheckMatrixFolderName + "/" + parityCheckMatrixFileName);
		if (!parityCheckMatrixFile.is_open()) std::cerr << "Error in dv_qkd_ldpc_error_correction_.cpp: file parityCheckMatrixFileName not found!!!!";;
		size_t bufferSize = (codeLength - codeRank) * codeLength;
		std::unique_ptr<char[]> buffer(new char[bufferSize]);
		parityCheckMatrixFile.read(buffer.get(), bufferSize);

		int row{ 0 }, column{ 0 };
		uint8_t data{ 0 };
		char* ptr{ &buffer[0] };
		for (auto k = 0; k < bufferSize; ++k)
		{
			data = *ptr - '0';
			if (data != 0)
				sparseParityCheckMatrix.insert(row, column) = (uint8_t)data;
			ptr++;
			column++;
			if (column == codeLength)
			{
				column = 0;
				row++;
			}
		}
	}
	else
	{
		std::cerr << "DVQkdRxErrorCorrectionLDPC::loadSparseParityCheckMatrix() - wrong file extension";
	}

	if (getVerboseMode())
	{
		std::ofstream file;
		file.open(getSignalsFolderName() + "/SparseParityCheckMatrix.txt", std::ios_base::out);
		if (file.is_open())
		{
			file << "Position of no-zero values (row, column)" << std::endl;
			for (int k = 0; k < sparseParityCheckMatrix.outerSize(); ++k)   // iterates over the columns of H
			{
				for (Eigen::SparseMatrix<t_binary>::InnerIterator it(sparseParityCheckMatrix, k); it; ++it)
					file << "(" << it.row() << ", " << it.col() << ")" << std::endl;
			}
			file.close();
		}
		else
			std::cerr << "Error in dv_qkd_ldpc_rx_error_correction_.cpp: file not open!!!";
	}
	return;
}
