#include "cv_qokd_ldpc_tx_20200819.h"

DvQkd_LDPC_Tx::DvQkd_LDPC_Tx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals) : 
	SuperBlock(inputSignals, outputSignals),
	maximumECSyndromeSize{ 972 },
	buffSize{ 512 }
{
	setLogFileName("SuperBlock_AliceQKD.txt");
};

DvQkd_LDPC_Tx::DvQkd_LDPC_Tx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName) :
	SuperBlock(inputSignals, outputSignals),
	maximumECSyndromeSize{ 972 },
	buffSize{ 512 }
{
	setSignalsFolderName(sFolderName);
	setLogFileName("SuperBlock_AliceQKD.txt");
};

DvQkd_LDPC_Tx::DvQkd_LDPC_Tx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, t_integer maxSamplesToProcessAtOnce) :
	SuperBlock(inputSignals, outputSignals),
	maximumECSyndromeSize{ maxSamplesToProcessAtOnce },
	buffSize{ 512 }
{
	setLogFileName("SuperBlock_AliceQKD.txt");
};

DvQkd_LDPC_Tx::DvQkd_LDPC_Tx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName, t_integer maxSamplesToProcessAtOnce) :
	SuperBlock(inputSignals, outputSignals),
	maximumECSyndromeSize{ maxSamplesToProcessAtOnce },
	buffSize{ 512 }
{
	setSignalsFolderName(sFolderName);
	setLogFileName("SuperBlock_AliceQKD.txt");
};

DvQkd_LDPC_Tx::DvQkd_LDPC_Tx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName, t_integer maxSamplesToProcessAtOnce, unsigned long int buffersSize) :
	SuperBlock(inputSignals, outputSignals),
	maximumECSyndromeSize{ maxSamplesToProcessAtOnce },
	buffSize{ buffersSize }
{
	setSignalsFolderName(sFolderName);
	setLogFileName("SuperBlock_DvQKDTx.txt");
};

void DvQkd_LDPC_Tx::initialize(void) 
{
	// Setup Input Signals
	DataIn.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	DataIn.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	DataIn.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	DataIn.setSaveSignal(getSaveInternalSignals(), Signal::t_write_mode::Ascii);
	DataIn.setNoHeader(true);

	BasesIn.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	BasesIn.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	BasesIn.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());
	BasesIn.setSaveSignal(getSaveInternalSignals(), Signal::t_write_mode::Ascii);
	BasesIn.setNoHeader(true);

	MessagesFromRx.setSymbolPeriod(inputSignals[2]->getSymbolPeriod());
	MessagesFromRx.setSamplingPeriod(inputSignals[2]->getSamplingPeriod());
	MessagesFromRx.setFirstValueToBeSaved(inputSignals[2]->getFirstValueToBeSaved());
	MessagesFromRx.setSaveSignal(getSaveInternalSignals(), Signal::t_write_mode::Ascii);
	MessagesFromRx.setNoHeader(true);

	// Configure blocks as necessary
	CvQokdLdpcTxSindromeReconciliation_.setParityCheckMatrixFolderName(parityCheckMatrixFolderName);
	CvQokdLdpcTxSindromeReconciliation_.setParityCheckMatrixFileName(parityCheckMatrixFileName);

	// Setup System Block
	setSuperBlockSystem(
		{
			&Fork_,
			&DvQkdLdpcTxMessageProcessorReceiver_,
			&DvQkdTxBasesReconciliation_,
			&DvQkdLdpcTxParameterEstimation_,
			&CvQokdLdpcTxSindromeReconciliation_,
			&DvQkdLdpcTxMessageProcessorTransmitter_,
			&PrivacyAmplification_
		}
	);

	// Setup Output Signals

	outputSignals[0]->setSymbolPeriod(DataOut.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(DataOut.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(DataOut.getFirstValueToBeSaved());

	outputSignals[1]->setSymbolPeriod(BasesOut.getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(BasesOut.getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(BasesOut.getFirstValueToBeSaved());

	outputSignals[2]->setSymbolPeriod(SymmetricKeys_Out.getSymbolPeriod());
	outputSignals[2]->setSamplingPeriod(SymmetricKeys_Out.getSamplingPeriod());
	outputSignals[2]->setFirstValueToBeSaved(SymmetricKeys_Out.getFirstValueToBeSaved());

	outputSignals[3]->setSymbolPeriod(Messages_Out.getSymbolPeriod());
	outputSignals[3]->setSamplingPeriod(Messages_Out.getSamplingPeriod());
	outputSignals[3]->setFirstValueToBeSaved(Messages_Out.getFirstValueToBeSaved());

	if (outputSignals.size() >= 5) 
	{
		outputSignals[4]->setSymbolPeriod(SymmetricKeys_Out.getSymbolPeriod());
		outputSignals[4]->setSamplingPeriod(SymmetricKeys_Out.getSamplingPeriod());
		outputSignals[4]->setFirstValueToBeSaved(SymmetricKeys_Out.getFirstValueToBeSaved());
	}

	
	//setLogValue(logValue);
	setLogFileName(logFileName);
	setSignalsFolderName(signalsFolderName);
}

bool DvQkd_LDPC_Tx::runBlock(void) 
{

	std::string sFolderName = getSignalsFolderName();

	if (!std::filesystem::is_directory(sFolderName) || !std::filesystem::exists(sFolderName)) 
	{
		std::filesystem::create_directory(sFolderName);
	}

	////////// INPUT SIGNALS /////////////

	int ready = inputSignals[0]->ready();
	int space = DataIn.space();
	int process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary dataIn;
		inputSignals[0]->bufferGet(&dataIn);
		DataIn.bufferPut(dataIn);
	}

	ready = inputSignals[1]->ready();
	space = BasesIn.space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary basisIn;
		inputSignals[1]->bufferGet(&basisIn);
		BasesIn.bufferPut(basisIn);
	}

	ready = inputSignals[2]->ready();
	space = MessagesFromRx.space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message messageIn;
		inputSignals[2]->bufferGet(&messageIn);
		MessagesFromRx.bufferPut(messageIn);
	}

	///////////// RUN BLOCKS //////////////

	bool alive = SuperBlock::runBlock(sFolderName);

	////////// OUTPUT SIGNALS ////////////

	if (firstTime) {
		setOpenFile(false);
		firstTime = false;
	}

	ready = DataOut.ready();
	space = outputSignals[0]->space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary dataOut = DataOut.bufferGet();
		outputSignals[0]->bufferPut(dataOut);
	}
	
	counterSentQubits.sum(process);
	dispBitsTracker = dispBitsTracker + process;

	ready = BasesOut.ready();
	space = outputSignals[1]->space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary basisOut = BasesOut.bufferGet();
		outputSignals[1]->bufferPut(basisOut);
	}

	ready = SymmetricKeys_Out.ready();
	space = outputSignals[2]->space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary keyOut = SymmetricKeys_Out.bufferGet();
		outputSignals[2]->bufferPut(keyOut);
	}

	counterKeyBits.sum(process);

	ready = SiftedKeys_Out.ready();
	space = outputSignals[4]->space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
			t_binary keyOut = SiftedKeys_Out.bufferGet();
			outputSignals[4]->bufferPut(keyOut);
	}


	double threshold = DvQkdLdpcTxParameterEstimation_.getThreshold();
	double latestQberUB = DvQkdLdpcTxParameterEstimation_.getLatestEstimatedBERUpperBound();

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

		sentQubits = counterSentQubits.getDouble();
		keyBits = counterKeyBits.getDouble();

		double bitRate = 1/inputSignals[0]->getSymbolPeriod();

		double brBitsIn = 0;
		double brBitsOut = 0;

		double siftedKeyRate = DvQkdTxBasesReconciliation_.getSiftedKeyRate();

		double peBitsIn = DvQkdLdpcTxParameterEstimation_.getTotalNumberOfInputBits();
		double peBitsOut = DvQkdLdpcTxParameterEstimation_.getTotalNumberOfOutputBits();
		double latestQBER = DvQkdLdpcTxParameterEstimation_.getLatestEstimatedBER();
		double averageQBER = DvQkdLdpcTxParameterEstimation_.getAverageEstimatedBER();
		double qberLB = DvQkdLdpcTxParameterEstimation_.getLatestEstimatedBERLowerBound();
		double confLevel = DvQkdLdpcTxParameterEstimation_.getConfidence();

		double ecBitsIn = (double)CvQokdLdpcTxSindromeReconciliation_.getTotalNumberOfInputBits();
		double ecBitsOut = (double)CvQokdLdpcTxSindromeReconciliation_.getTotalNumberOfOutputBits();

		uint64_t numberOfCalculatedSindromes = CvQokdLdpcTxSindromeReconciliation_.getNumberOfCalculatedSindromes();
		uint64_t numberOfNoMatchedSindromes = 0; // CvQokdLdpcTxSindromeReconciliation_.getNumberOfNoMatchedSindromes();

		double paBitsIn = (double)PrivacyAmplification_.getTotalNumberOfInputBits();
		double paBitsOut = (double)PrivacyAmplification_.getTotalNumberOfOutputBits();

		std::string localIPAddress = NetworkAuxCommon::getIp();

		ConsoleOutputCommon::moveCursorTopLeft();
		ConsoleOutputCommon::clrLine(); printf("Tx\n");
		ConsoleOutputCommon::clrLine(); printf("#####################################################\n");
		ConsoleOutputCommon::clrLine(); printf("Photons per pulse:                     %.1e\n", photonsPerPulse);
		ConsoleOutputCommon::clrLine(); printf("Transmitted qubits rate:               %.1e Hz\n", bitRate);
		ConsoleOutputCommon::clrLine(); printf("Sifted key rate:                       %.5e Hz\n", siftedKeyRate);
		ConsoleOutputCommon::clrLine(); printf("Average estimated QBER:                %.5f %%\n", 100 *averageQBER);
		ConsoleOutputCommon::clrLine(); printf("Latest estimated QBER:                 %.5f %%\n", 100*latestQBER);
		if (qberUB < threshold) {
			ConsoleOutputCommon::clrLine(); printf("QBER Upper Bound:                      %.5f %%\n", 100 * qberUB);
		}
		else {
			ConsoleOutputCommon::clrLine(); printf("QBER Upper Bound:                      %.5f %% - WARNING: QBER UPPER BOUND ABOVE SECURITY THRESHOLD\n", 100 * qberUB);
		}
		ConsoleOutputCommon::clrLine(); printf("QBER Lower Bound:                      %.5f %%\n", 100*qberLB);
		ConsoleOutputCommon::clrLine(); printf("Confidence Level:                      %.5f %%\n", confLevel * 100);
//		ConsoleOutputCommon::clrLine(); printf("Average estimated QBER:                %.5e %%\n", 100 * averageQBER);
		ConsoleOutputCommon::clrLine(); printf("Key rate after QBER estimation:        %.5e Hz\n", bitRate * peBitsOut/sentQubits);
		ConsoleOutputCommon::clrLine(); std::cout << "Number of calculated sindromes:        " << numberOfCalculatedSindromes << std::endl;
		ConsoleOutputCommon::clrLine(); std::cout << "Number of no matched sindromes:        " << numberOfNoMatchedSindromes << std::endl;

		ConsoleOutputCommon::clrLine(); printf("Key rate after error correction:       %.5e Hz\n", bitRate * ecBitsOut/sentQubits);
		ConsoleOutputCommon::clrLine(); printf("Key rate after privacy amplification:  %.5e Hz\n", bitRate * paBitsOut/sentQubits);
		ConsoleOutputCommon::clrLine(); printf("Overall efficiency:                    %.5f %%\n", 100 * paBitsOut / sentQubits);
		/*
		if (peBitsOut / peBitsIn > 0) {
			printf("Overall efficiency:                    %.5f %%\n", 100*paBitsOut / sentQubits);
		}
		else {
			printf("Overall efficiency:                    not yet calculated\n");
		}
		*/
		ConsoleOutputCommon::clrLine(); printf("#####################################################\n");
		ConsoleOutputCommon::clrLine(); printf("Local IP Address:                      %s\n", localIPAddress.c_str());
		ConsoleOutputCommon::clrLine(); printf("Remote IP Address:                     %s\n", ipAddress.c_str());
		ConsoleOutputCommon::clrRemainingScreen();


		if (getVerboseMode())
		{
			std::ofstream myfile;
			myfile.open(sFolderName + "/" + reportFileName);
			myfile << "Total received qubits:            " << sentQubits << "\n";
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
			myfile << "Overall efficiency:               " << (double)paBitsOut / (double)sentQubits << "\n";
			myfile.close();
		}
	}

	ready = Messages_Out.ready();
	space = outputSignals[3]->space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message* messageOut = Messages_Out.bufferGet();
		outputSignals[3]->bufferPut(*messageOut);
	}

	if (!alive && getVerboseMode())
	{
	//	double bitRate = 1 / inputSignals[0]->getSymbolPeriod();

		double brBitsIn = 0;
		double brBitsOut = 0;

		double peBitsIn = (double)DvQkdLdpcTxParameterEstimation_.getTotalNumberOfInputBits();
		double peBitsOut = (double)DvQkdLdpcTxParameterEstimation_.getTotalNumberOfOutputBits();
		double averageQBER = (double)DvQkdLdpcTxParameterEstimation_.getAverageEstimatedBER();

		double ecBitsIn = (double)CvQokdLdpcTxSindromeReconciliation_.getTotalNumberOfInputBits();
		double ecBitsOut = (double)CvQokdLdpcTxSindromeReconciliation_.getTotalNumberOfOutputBits();

		double paBitsIn = (double)PrivacyAmplification_.getTotalNumberOfInputBits();
		double paBitsOut = (double)PrivacyAmplification_.getTotalNumberOfOutputBits();

		std::ofstream myfile;
		myfile.open(sFolderName + "/" + reportFileName);
		myfile << "Total received qubits:            " << sentQubits << "\n";
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
		myfile << "Overall efficiency:               " << (double)paBitsOut / (double)sentQubits << "\n";
		myfile.close();
	}
	/////////////////////////////////////

	return alive;
}
