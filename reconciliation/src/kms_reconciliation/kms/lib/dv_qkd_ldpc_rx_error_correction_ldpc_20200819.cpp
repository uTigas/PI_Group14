#include "dv_qkd_ldpc_rx_error_correction_20200819.h"

void DvQkdLdpcRxErrorCorrection::initialize(void)
{
	if (errorCorrectionMethod == DvQkdLdpcRxErrorCorrection::t_method::LDPC)
	{
		loadSparseParityCheckMatrix();

		sparseParityCheckMatrixTranspose = sparseParityCheckMatrix.transpose();

		calculatedSindromeSize = codeLength - codeRank;
		calculatedSindrome.resize(calculatedSindromeSize, 1);
		calculatedSindrome_posIn = 0;
		calculatedSindrome_posOut = calculatedSindrome.size();
		calculatedSindrome_newData = false;

		receivedSindromeSize = codeLength - codeRank;
		receivedSindrome.resize(receivedSindromeSize, 1);
		receivedSindrome_posIn = 0;
		receivedSindrome_posOut = receivedSindrome.size();
		receivedSindrome_newData = false;

		receivedDataSize = codeLength;
		receivedData.resize(receivedDataSize, 1);
		receivedData_posIn = 0;
		receivedData_posOut = receivedData.size();
		receivedData_newData = false;

		DataIn.resize((size_t)4*inputSignals[0]->getBufferLength());
		DataOut.resize(DataIn.size());
		SindromeIn.resize((size_t)4*inputSignals[1]->getBufferLength());
		SindromeOut.resize(SindromeIn.size());

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

bool DvQkdLdpcRxErrorCorrection::runBlock(void)
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
	if (process > 0 && receivedSindrome_posIn == receivedSindromeSize)
	{
		if (getVerboseMode())
		{
			std::ofstream file;
			file.open(getSignalsFolderName() + "/received_sindrome_rx.txt", std::ios_base::app);
			for (auto k = 0; k < receivedSindromeSize; k++)
			{
				if (k != 0) file << " ";
				file << std::to_string(receivedSindrome[k]);
			}
			file << std::endl;
			file.close();
		}
		receivedSindrome_newData = true;
	}

	ready = DataIn.ready();
	process = std::min( ready, receivedDataSize - receivedData_posIn);
	alive = !alive ? process > 0: alive;
	for (auto k = receivedData_posIn; k < receivedData_posIn + process; k++)
	{
		if (numberOfDummyBitsPerBitFillingPeriod)
		{
			if (inDataBitFillingCounter >= numberOfDataBitsPerBitFillingPeriod)
			{
				receivedData[k] = 0;
			}
			else
				receivedData[k] = DataIn.bufferGet();

			inDataBitFillingCounter = ++inDataBitFillingCounter % bitFillingPeriod;
		}
		else
			receivedData[k] = DataIn.bufferGet();
	}

	receivedData_posIn += process;
	if (process > 0 && receivedData_posIn == receivedDataSize)
		receivedData_newData = true;

	if (receivedData_newData && calculatedSindrome_posIn == 0 && calculatedSindrome_newData == false)
	{
		if (getVerboseMode())
		{
			std::ofstream file;
			file.open(getSignalsFolderName() + "/receivedData_rx.txt", std::ios_base::app);
			for (auto k = 0; k < receivedDataSize; k++)
			{
				if (k != 0 && k % 200 == 0) file << std::endl;
				file << std::to_string(receivedData[k]);
			}
			file << std::endl;
			file.close();
		}

		calculatedSindrome = sparseParityCheckMatrix * receivedData;
		calculatedSindrome = calculatedSindrome.unaryExpr([](const t_binary x) {return (t_binary)(x % 2);}); //to compute mod(sindrome,2)

		receivedData_posIn = receivedDataSize;
		receivedData_posOut = receivedDataSize;
		receivedData_newData = false;

		calculatedSindrome_posIn = calculatedSindromeSize;
		calculatedSindrome_posOut = 0;
		calculatedSindrome_newData = true;

		if (getVerboseMode())
		{
			std::ofstream file;
			file.open(getSignalsFolderName() + "/calculated_sindrome_rx.txt", std::ios_base::app);
			for (auto k = 0; k < calculatedSindromeSize; k++)
			{
				if (k != 0) file << " ";
				file << std::to_string(calculatedSindrome[k]);
			}
			file << std::endl;
			file.close();
		}
	}

	ready = calculatedSindromeSize - calculatedSindrome_posOut;
	space = SindromeOut.space();
	process = std::min(ready, space);
	if ( startSendingSindrome && process)
	{
		for (auto k = calculatedSindrome_posOut; k < calculatedSindrome_posOut + process; k++)
			SindromeOut.bufferPut(calculatedSindrome[k]);

		calculatedSindrome_posOut = calculatedSindrome_posOut + process;
		if (calculatedSindrome_posIn == calculatedSindromeSize) calculatedSindrome_posIn = 0;
	}

	if (calculatedSindrome_newData && receivedSindrome_newData)
	{
		numberOfCalculatedSindromes++;

		int sindromeNoMatch{ 0 };
		for (auto k = 0; k < calculatedSindromeSize; k++)
			if (calculatedSindrome[k] != receivedSindrome[k])
				sindromeNoMatch++;

		sindromeMatch = sindromeNoMatch == 0;
		if (!sindromeMatch && !startSendingSindrome)
			sindromeMatch = errorCorrectionLDPCAlgorithm();

		if (!sindromeMatch)
		{
			numberOfNoMatchedSindromes++;
			receivedData_posIn = 0;
		}
		else
		{
			receivedData_posOut = 0;
		}

		receivedSindrome_posIn = 0;
		receivedSindrome_newData = false;

		calculatedSindrome_newData = false;
	}
	
	ready = calculatedSindromeSize - calculatedSindrome_posOut;
	space = SindromeOut.space();
	process = std::min(ready, space);
	if (!startSendingSindrome && !calculatedSindrome_newData && process)
	{
		for (auto k = calculatedSindrome_posOut; k < calculatedSindrome_posOut + process; k++)
			SindromeOut.bufferPut(calculatedSindrome[k]);

		calculatedSindrome_posOut = calculatedSindrome_posOut + process;
		if (calculatedSindrome_posIn == calculatedSindromeSize) calculatedSindrome_posIn = 0;
	}

	ready = receivedDataSize - receivedData_posOut;
	space = DataOut.space();
	process = std::min(ready, space);
	if (process)
	{
		for (auto k = 0; k < process; k++)
		{
			if (numberOfDummyBitsPerBitFillingPeriod)
			{
				if (outDataBitFillingCounter < numberOfDataBitsPerBitFillingPeriod)
				{
					DataOut.bufferPut(receivedData[k]);
				}
				outDataBitFillingCounter = ++outDataBitFillingCounter % bitFillingPeriod;
			}
			else
				DataOut.bufferPut(receivedData[k]);	
		}


		receivedData_posOut = receivedData_posOut + process;
		if (receivedData_posIn == receivedDataSize) receivedData_posIn = 0;
		if (sindromeMatch) sindromeMatch = false;
	}

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
	process = std::min(ready, receivedDataSize - receivedData_posIn);
	alive = !alive ? process > 0: alive;
	for (auto k = receivedData_posIn; k < receivedData_posIn + process; k++)
	{
		if (numberOfDummyBitsPerBitFillingPeriod)
		{
			if (inDataBitFillingCounter >= numberOfDataBitsPerBitFillingPeriod)
			{
				receivedData[k] = 0;
			}
			else
				receivedData[k] = DataIn.bufferGet();

			inDataBitFillingCounter = ++inDataBitFillingCounter % bitFillingPeriod;
		}
		else
			receivedData[k] = DataIn.bufferGet();
	}

	receivedData_posIn += process;
	if (process > 0 && receivedData_posIn == receivedDataSize)
		receivedData_newData = true;
	
	return alive;
}

t_bool DvQkdLdpcRxErrorCorrection::errorCorrectionLDPCAlgorithm()
{

	for (int k = 0; k < codeLength; ++k)
	{
		if (receivedData[k] == 0)
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
				receivedData[k] = 1;
			else
				receivedData[k] = 0;
		}

		calculatedSindrome = sparseParityCheckMatrix * receivedData;
		calculatedSindrome = calculatedSindrome.unaryExpr([](const t_binary x) {return (t_binary)(x % 2);}); //to compute mod(sindrome,2)

		sindromeNoMatch = 0;
		for (auto k = 0; k < calculatedSindromeSize; k++)
			if (calculatedSindrome[k] != receivedSindrome[k])
				sindromeNoMatch++;

		if (sindromeNoMatch == 0)
		{
			if (getVerboseMode())
			{
				std::ofstream file;
				file.open( getSignalsFolderName() + "/new_sindrome_rx.txt", std::ios_base::app);
				if (file.is_open())
				{
					for (auto k = 0; k < calculatedSindromeSize; k++)
						file << std::to_string(calculatedSindrome[k]);
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

void DvQkdLdpcRxErrorCorrection::loadSparseParityCheckMatrix()
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
