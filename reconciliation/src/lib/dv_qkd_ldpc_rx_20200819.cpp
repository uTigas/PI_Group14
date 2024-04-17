#include "dv_qkd_ldpc_rx_20200819.h"

DvQkdRx_LDPC::DvQkdRx_LDPC(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals) :
	SuperBlock(inputSignals, outputSignals),
	maximumECSyndromeSize{ 972 },
	buffSize{ 512 }
{
	setLogFileName("SuperBlock_DvQkdRx.txt");
};

DvQkdRx_LDPC::DvQkdRx_LDPC(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName) :
	SuperBlock(inputSignals, outputSignals),
	maximumECSyndromeSize{ 972 },
	buffSize{ 512 }
{
	setSignalsFolderName(sFolderName);
	setLogFileName("SuperBlock_DvQkdRx.txt");
};

DvQkdRx_LDPC::DvQkdRx_LDPC(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, t_integer maxSamplesToProcessAtOnce) :
	SuperBlock(inputSignals, outputSignals),
	maximumECSyndromeSize{ maxSamplesToProcessAtOnce },
	buffSize{ 512 }
{
	setLogFileName("SuperBlock_DvQkdRx.txt");
};

DvQkdRx_LDPC::DvQkdRx_LDPC(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName, t_integer maxSamplesToProcessAtOnce) :
	SuperBlock(inputSignals, outputSignals),
	maximumECSyndromeSize{ maxSamplesToProcessAtOnce },
	buffSize{ 512 }
{
	setSignalsFolderName(sFolderName);
	setLogFileName("SuperBlock_DvQkdRx.txt");
};

DvQkdRx_LDPC::DvQkdRx_LDPC(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName, t_integer maxSamplesToProcessAtOnce, unsigned long int buffersSize) :
	SuperBlock(inputSignals, outputSignals),
	maximumECSyndromeSize{ maxSamplesToProcessAtOnce },
	buffSize{ buffersSize }
{
	setSignalsFolderName(sFolderName);
	setLogFileName("SuperBlock_DvQkdRx.txt");
};

void DvQkdRx_LDPC::initialize(void) 
{
	// Setup Input Signals
	BasesIn.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	BasesIn.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	BasesIn.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	DataIn.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	DataIn.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	DataIn.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	MessagesFromTx.setSymbolPeriod(inputSignals[2]->getSymbolPeriod());
	MessagesFromTx.setSamplingPeriod(inputSignals[2]->getSamplingPeriod());
	MessagesFromTx.setFirstValueToBeSaved(inputSignals[2]->getFirstValueToBeSaved());

	// Configure blocks
	DvQkdLdpcRxErrorCorrection_.setParityCheckMatrixFolderName(parityCheckMatrixFolderName);
	DvQkdLdpcRxErrorCorrection_.setParityCheckMatrixFileName(parityCheckMatrixFileName);

	// Setup System Block
	setSuperBlockSystem(
		{
			&Fork_,
			&DvQkdLdpcRxMessageProcessorReceiver_,
			&BasesReconciliation_,
			&DvQkdLdpcRxParameterEstimation_,
			& DvQkdLdpcRxErrorCorrection_,
			&DvQkdLdpcRxMessageProcessorTransmitter_,
			&PrivacyAmplification_
		}
	);

	// Setup Output Signals
	// Bases to the optical channel
	outputSignals[0]->setSymbolPeriod(BasesIn.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(BasesIn.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(BasesIn.getFirstValueToBeSaved());

	// Messages to the Tx
	outputSignals[1]->setSymbolPeriod(MessagesFromTx.getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(MessagesFromTx.getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(MessagesFromTx.getFirstValueToBeSaved());

	// Sifted keys
	outputSignals[2]->setSymbolPeriod(DataIn.getSymbolPeriod());
	outputSignals[2]->setSamplingPeriod(DataIn.getSamplingPeriod());
	outputSignals[2]->setFirstValueToBeSaved(DataIn.getFirstValueToBeSaved());

	// Symmetric keys
	outputSignals[3]->setSymbolPeriod(DataIn.getSymbolPeriod());
	outputSignals[3]->setSamplingPeriod(DataIn.getSamplingPeriod());
	outputSignals[3]->setFirstValueToBeSaved(DataIn.getFirstValueToBeSaved());

	setLogFileName(logFileName);
	setSignalsFolderName(signalsFolderName);

	sindromeSize = codeLength - codeRank;
}

bool DvQkdRx_LDPC::runBlock(void) 
{

	std::string sFolderName = getSignalsFolderName();

	if (!std::filesystem::is_directory(sFolderName) || !std::filesystem::exists(sFolderName)) {
		std::filesystem::create_directory(sFolderName);
	}

	////////// INPUT SIGNALS /////////////

	auto ready = inputSignals[0]->ready();
	auto space = BasesIn.space();
	auto process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary basisIn;
		inputSignals[0]->bufferGet(&basisIn);
		BasesIn.bufferPut(basisIn);
	}

	ready = inputSignals[1]->ready();
	space = DataIn.space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary dataIn;
		inputSignals[1]->bufferGet(&dataIn);
		DataIn.bufferPut(dataIn);
	}
	
	dispBitsTracker = dispBitsTracker + process;
	counterReceivedQubits.sum(process);

	ready = inputSignals[2]->ready();
	space = MessagesFromTx.space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message messageIn;
		inputSignals[2]->bufferGet(&messageIn);
		MessagesFromTx.bufferPut(messageIn);
	}

	///////////// RUN BLOCKS //////////////

	bool alive = SuperBlock::runBlock(sFolderName);

	if (alive == false)
	{
		std::cout << "";
	}
	////////// OUTPUT SIGNALS ////////////

	if (firstTime) 
	{
		setOpenFile(false);
		firstTime = false;
	}

	ready = BasesOut.ready();
	space = outputSignals[0]->space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary value = BasesOut.bufferGet();
		outputSignals[0]->bufferPut(value);
	}

	ready = SymmetricKeysOut.ready();
	space = outputSignals[1]->space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary value = SymmetricKeysOut.bufferGet();
		outputSignals[1]->bufferPut(value);
	}


	ready = MessagesToTx.ready();
	space = outputSignals[2]->space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message *msg =MessagesToTx.bufferGet();
		outputSignals[2]->bufferPut(msg);
	}

	ready = SiftedKeysOut.ready();
	space = outputSignals[3]->space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary value = SiftedKeysOut.bufferGet();
		outputSignals[3]->bufferPut(value);
	}


	// ####################################
	counterKeyBits.sum(process);

	double threshold = DvQkdLdpcRxParameterEstimation_.getThreshold();
	double latestQberUB = DvQkdLdpcRxParameterEstimation_.getLatestEstimatedBERUpperBound();

	if (latestQberUB != qberUB) {
		previousQberUB = qberUB;
		qberUB = latestQberUB;
	}

	double topQberUB = std::max(qberUB, previousQberUB);

	if (topQberUB < threshold) {
		PrivacyAmplification_.setBER(topQberUB);	// Should be synced between parameter estimation and privacy amplification
													// i.e. when privacy amplification gets a new estimate, it waits until those bits reach
													// privacy amplification to make the change. A decent compromise is to use the highest of
													// two most recent estimates
	}

	if ((dispBitsTracker >= dispBitsInterval) && (toPrint)) {
		dispBitsTracker = 0;
		
		receivedQubits = counterReceivedQubits.getDouble();
		keyBits = counterKeyBits.getDouble();

		double bitRate = 1 / inputSignals[0]->getSymbolPeriod();
		double noClicksIn = 0;
		double doubleClicksIn = 0;

		double brBitsIn = 0;
		double brBitsOut = 0;

		double peBitsIn = (double)DvQkdLdpcRxParameterEstimation_.getTotalNumberOfInputBits();
		double peBitsOut = (double)DvQkdLdpcRxParameterEstimation_.getTotalNumberOfOutputBits();
		double averageQBER = (double)DvQkdLdpcRxParameterEstimation_.getAverageEstimatedBER();
		double latestQBER = (double)DvQkdLdpcRxParameterEstimation_.getAverageEstimatedBER();
		double qberLB = (double)DvQkdLdpcRxParameterEstimation_.getLatestEstimatedBERLowerBound();
		double confLevel = (double)DvQkdLdpcRxParameterEstimation_.getConfidence();

		double ecBitsIn = (double)DvQkdLdpcRxErrorCorrection_.getTotalNumberOfInputBits();
		double ecBitsOut = (double)DvQkdLdpcRxErrorCorrection_.getTotalNumberOfOutputBits();

		double paBitsIn = (double)PrivacyAmplification_.getTotalNumberOfInputBits();
		double paBitsOut = (double)PrivacyAmplification_.getTotalNumberOfOutputBits();

		std::string localIPAddress = NetworkAuxCommon::getIp();

		ConsoleOutputCommon::moveCursorTopLeft();
		ConsoleOutputCommon::clrLine(); printf("Bob\n");
		ConsoleOutputCommon::clrLine(); printf("#####################################################\n");
		ConsoleOutputCommon::clrLine(); printf("Received qubits rate:                  %.1e Hz\n", bitRate * (brBitsIn- noClicksIn) / brBitsIn);
		ConsoleOutputCommon::clrLine(); printf("Double click rate:                     %.1e Hz\n", bitRate * (doubleClicksIn) / brBitsIn);
		ConsoleOutputCommon::clrLine(); printf("Estimated QBER:                        %.5f %%\n", 100 * latestQBER);
		if (qberUB < threshold) {
			ConsoleOutputCommon::clrLine(); printf("QBER Upper Bound:                      %.5f %%\n", 100 * qberUB);
		}
		else {
			ConsoleOutputCommon::clrLine(); printf("QBER Upper Bound:                      %.5f %% - WARNING: QBER UPPER BOUND ABOVE SECURITY THRESHOLD\n", 100 * qberUB);
		}
		ConsoleOutputCommon::clrLine(); printf("QBER Lower Bound:                      %.5f %%\n", 100 * qberLB);
		ConsoleOutputCommon::clrLine(); printf("Confidence Level:                      %.5f %%\n", confLevel * 100);
		//	
//		ConsoleOutputCommon::clrLine(); printf("Average estimated QBER:                %.5f %%\n", 100 * averageQBER);
		ConsoleOutputCommon::clrLine(); printf("Sifted key rate:                       %.1e Hz\n", bitRate * brBitsOut / receivedQubits);
		ConsoleOutputCommon::clrLine(); printf("Key rate after QBER estimation:        %.1e Hz\n", bitRate * peBitsOut / receivedQubits);
		ConsoleOutputCommon::clrLine(); printf("Key rate after error correction:       %.1e Hz\n", bitRate * ecBitsOut / receivedQubits);
		ConsoleOutputCommon::clrLine(); printf("Key rate after privacy amplification:  %.1e Hz\n", bitRate * paBitsOut / receivedQubits);
		ConsoleOutputCommon::clrLine(); printf("Overall efficiency:                    %.5f %%\n", 100 * paBitsOut / receivedQubits);
		/*
		if (peBitsOut / peBitsIn > 0) {
			printf("Overall efficiency:                    %.5f %%\n", 100*paBitsOut / receivedQubits);
		}
		else {
			printf("Overall efficiency:                    not yet calculated\n");
		}
		*/
		ConsoleOutputCommon::clrLine(); printf("#####################################################\n");
		ConsoleOutputCommon::clrLine(); printf("Local IP Address:                      %s\n", localIPAddress.c_str());
		ConsoleOutputCommon::clrLine(); printf("Remote IP Address:                     %s\n", ipAddress.c_str());

		if ((double)brBitsOut / (double)brBitsIn < 0) {
			std::cout << "Something is wrong.";
		}

		if (getVerboseMode())
		{
			std::ofstream myfile;
			myfile.open(sFolderName + "/" + reportFileName);
			myfile << "Total received qubits:            " << receivedQubits << "\n";
			myfile << "Average QBER:                     " << averageQBER << "\n";
			myfile << "Key after basis reconciliation:   " << brBitsOut << "\n";
			myfile << "Key after parameter estimation:   " << peBitsOut << "\n";
			myfile << "Key after error correction:       " << ecBitsOut << "\n";
			myfile << "Final key size:                   " << paBitsOut << "\n";
			myfile << "##########################################################\n";
			myfile << "Basis reconciliation efficiency:  " << (double)brBitsOut / (double)brBitsIn << "\n";
			myfile << "Parameter estimation efficiency:  " << (double)peBitsOut / (double)peBitsIn << "\n";
			myfile << "Error correction efficiency:      " << (double)ecBitsOut / (double)ecBitsIn << "\n";
			myfile << "Privacy amplification key ratio:  " << (double)paBitsOut / (double)paBitsIn << "\n";
			myfile << "Overall efficiency:               " << (double)paBitsOut / (double)receivedQubits << "\n";
			myfile.close();
		}
	}
	/*
	ready = MessagesOut.ready();
	space = outputSignals[2]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message* messageOut = Messages_Out.bufferGet();
		outputSignals[2]->bufferPut(messageOut);
	}

	if (outputSignals.size() >= 4) {
		ready = SiftedKeys_0.ready();
		space = outputSignals[3]->space();
		process = min(ready, space);

		for (auto k = 0; k != process; k++)
		{
			t_binary siftedKeys = SiftedKeys_0.bufferGet();
			outputSignals[3]->bufferPut(siftedKeys);
		}
	}
	else {
		ready = SiftedKeys_0.ready();
		process = ready;
		t_binary siftedKeys;

		for (auto k = 0; k != process; k++)
		{
			siftedKeys = SiftedKeys_0.bufferGet();
		}
	}
	*/
	if (!alive && getVerboseMode())
	{
//		double bitRate = 1 / inputSignals[0]->getSymbolPeriod();
//		double noClicksIn = (double)BasisReconciliation_.getNumberOfNoClicks();
//		double doubleClicksIn = (double)BasisReconciliation_.getNumberOfDoubleClicks();

		double brBitsIn = 0;
		double brBitsOut = 0;

		double peBitsIn = (double)DvQkdLdpcRxParameterEstimation_.getTotalNumberOfInputBits();
		double peBitsOut = (double)DvQkdLdpcRxParameterEstimation_.getTotalNumberOfOutputBits();
		double averageQBER = (double)DvQkdLdpcRxParameterEstimation_.getAverageEstimatedBER();

		double ecBitsIn = (double)DvQkdLdpcRxErrorCorrection_.getTotalNumberOfInputBits();
		double ecBitsOut = (double)DvQkdLdpcRxErrorCorrection_.getTotalNumberOfOutputBits();

		double paBitsIn = (double)PrivacyAmplification_.getTotalNumberOfInputBits();
		double paBitsOut = (double)PrivacyAmplification_.getTotalNumberOfOutputBits();

		std::ofstream myfile;
		myfile.open(sFolderName + "/" + reportFileName);
		myfile << "Total received qubits:            " << receivedQubits << "\n";
		myfile << "Average QBER:                     " << averageQBER << "\n";
		myfile << "Key after basis reconciliation:   " << brBitsOut << "\n";
		myfile << "Key after parameter estimation:   " << peBitsOut << "\n";
		myfile << "Key after error correction:       " << ecBitsOut << "\n";
		myfile << "Final key size:                   " << paBitsOut << "\n";
		myfile << "##########################################################\n";
		myfile << "Basis reconciliation efficiency:  " << (double)brBitsOut / (double)brBitsIn << "\n";
		myfile << "Parameter estimation efficiency:  " << (double)peBitsOut / (double)peBitsIn << "\n";
		myfile << "Error correction efficiency:      " << (double)ecBitsOut / (double)ecBitsIn << "\n";
		myfile << "Privacy amplification key ratio:  " << (double)paBitsOut / (double)paBitsIn << "\n";
		myfile << "Overall efficiency:               " << (double)paBitsOut / (double)receivedQubits << "\n";
		myfile.close();
	}
	/*	ready = TransmissionMode_Out.ready();
		space = outputSignals[2]->space();
		process = min(ready, space);

		for (auto k = 0; k != process; k++)
		{
			t_binary transmissionOut = TransmissionMode_Out.bufferGet();
			outputSignals[2]->bufferPut(transmissionOut);
		}
	*/	/////////////////////////////////////

	return alive;
}
