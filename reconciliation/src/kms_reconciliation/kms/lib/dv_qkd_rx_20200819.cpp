#include "dv_qkd_rx_20200819.h"

void DvQkdRx::initialize(void) 
{
	// Setup Input Signals
	Data_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	Data_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	Data_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	Basis_In.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	Basis_In.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	Basis_In.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	Messages_In.setSymbolPeriod(inputSignals[2]->getSymbolPeriod());
	Messages_In.setSamplingPeriod(inputSignals[2]->getSamplingPeriod());
	Messages_In.setFirstValueToBeSaved(inputSignals[2]->getFirstValueToBeSaved());

	// Configure blocks input parameters
	BasisReconciliation_.setBasisReconciliationRole(basis_reconciliation_role::Bob);
	ParameterEstimation_.setParameterEstimationRole(parameter_estimation_role::Bob);
	ErrorCorrection_.setErrorCorrectionRole(ErrorCorrection::t_role::Bob);
	//MessageProcessorTransmitter_.setSignalTypes(transmitterMsgTypes);

	// Setup System Block
	setSuperBlockSystem(
		{
			&BobIncomingDataProcessor_,
			&MessageProcessorReceiver_,
			&BasisReconciliation_,
			&ParameterEstimation_,
			&ErrorCorrection_,
			&MessageProcessorTransmitter_,
			&PrivacyAmplification_
		}
	);

	// Setup Output Signals
	outputSignals[0]->setSymbolPeriod(BasisIn.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(BasisIn.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(BasisIn.getFirstValueToBeSaved());

	outputSignals[1]->setSymbolPeriod(Key_Out.getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(Key_Out.getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(Key_Out.getFirstValueToBeSaved());

	outputSignals[2]->setSymbolPeriod(Messages_Out.getSymbolPeriod());
	outputSignals[2]->setSamplingPeriod(Messages_Out.getSamplingPeriod());
	outputSignals[2]->setFirstValueToBeSaved(Messages_Out.getFirstValueToBeSaved());

	// Optional sifted Key
	if (outputSignals.size() >= 4) {
		outputSignals[3]->setSymbolPeriod(Key_Out.getSymbolPeriod());
		outputSignals[3]->setSamplingPeriod(Key_Out.getSamplingPeriod());
		outputSignals[3]->setFirstValueToBeSaved(Key_Out.getFirstValueToBeSaved());
	}

	setLogValue(logValue);
	setLogFileName(logFileName);
	setSignalsFolderName(signalsFolderName);
}

bool DvQkdRx::runBlock(void) 
{

	std::string sFolderName = getSignalsFolderName();

	if (!std::filesystem::is_directory(sFolderName) || !std::filesystem::exists(sFolderName)) {
		std::filesystem::create_directory(sFolderName);
	}

	////////// INPUT SIGNALS /////////////

	int ready = inputSignals[0]->ready();
	int space = Data_In.space();
	int process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary dataIn;
		inputSignals[0]->bufferGet(&dataIn);
		Data_In.bufferPut(dataIn);
	}
	
	dispBitsTracker = dispBitsTracker + process;
	counterReceivedQubits.sum(process);

	ready = inputSignals[1]->ready();
	space = min(Basis_In.space(), outputSignals[0]->space());
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary basisIn;
		inputSignals[1]->bufferGet(&basisIn);
		Basis_In.bufferPut(basisIn);
		outputSignals[0]->bufferPut(basisIn);
	}

	ready = inputSignals[2]->ready();
	space = Messages_In.space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message messageIn;
		inputSignals[2]->bufferGet(&messageIn);
		Messages_In.bufferPut(messageIn);
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

	ready = Key_Out.ready();
	space = outputSignals[1]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary keyOut = Key_Out.bufferGet();
		outputSignals[1]->bufferPut(keyOut);
	}
	counterKeyBits.sum(process);

	double threshold = ParameterEstimation_.getThreshold();
	double latestQberUB = ParameterEstimation_.getLatestEstimatedBERUB();

	if (latestQberUB != qberUB) {
		previousQberUB = qberUB;
		qberUB = latestQberUB;
	}

	double topQberUB = max(qberUB, previousQberUB);

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
		double noClicksIn = (double)BasisReconciliation_.getNumberOfNoClicks();
		double doubleClicksIn = (double)BasisReconciliation_.getNumberOfDoubleClicks();

		double brBitsIn = (double)BasisReconciliation_.getReceivedQubits();
		double brBitsOut = (double)BasisReconciliation_.getReconciledBits();

		double peBitsIn = (double)ParameterEstimation_.getInputBits();
		double peBitsOut = (double)ParameterEstimation_.getOutputBits();
		double averageQBER = (double)ParameterEstimation_.getAverageEstimatedBER();
		double latestQBER = (double)ParameterEstimation_.getAverageEstimatedBER();
		double qberLB = (double)ParameterEstimation_.getLatestEstimatedBERLB();
		double confLevel = (double)ParameterEstimation_.getConfidence();

		double ecBitsIn = (double)ErrorCorrection_.getInputBits();
		double ecBitsOut = (double)ErrorCorrection_.getOutputBits();

		double paBitsIn = (double)PrivacyAmplification_.getInputBits();
		double paBitsOut = (double)PrivacyAmplification_.getOutputBits();

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
		ConsoleOutputCommon::clrLine();
		if (peBitsOut / peBitsIn > 0) {
			printf("Overall efficiency:                    %.5f %%\n", 100*paBitsOut / receivedQubits);
		}
		else {
			printf("Overall efficiency:                    not yet calculated\n");
		}
		ConsoleOutputCommon::clrLine(); printf("#####################################################\n");
		ConsoleOutputCommon::clrLine(); printf("Local IP Address:                      %s\n", localIPAddress.c_str());
		ConsoleOutputCommon::clrLine(); printf("Remote IP Address:                     %s\n", ipAddress.c_str());

		if ((double)brBitsOut / (double)brBitsIn < 0) {
			std::cout << "Something is wrong.";
		}

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

	ready = Messages_Out.ready();
	space = outputSignals[2]->space();
	process = min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_message* messageOut = Messages_Out.bufferGet();
		outputSignals[2]->bufferPut(messageOut);
	}

	if (outputSignals.size() >= 4) {
		ready = SiftedKeys.ready();
		space = outputSignals[3]->space();
		process = min(ready, space);

		for (auto k = 0; k != process; k++)
		{
			t_binary siftedKeys = SiftedKeys.bufferGet();
			outputSignals[3]->bufferPut(siftedKeys);
		}
	}
	else {
		ready = SiftedKeys.ready();
		process = ready;
		t_binary siftedKeys;

		for (auto k = 0; k != process; k++)
		{
			siftedKeys = SiftedKeys.bufferGet();
		}
	}

	if (!alive)
	{
//		double bitRate = 1 / inputSignals[0]->getSymbolPeriod();
//		double noClicksIn = (double)BasisReconciliation_.getNumberOfNoClicks();
//		double doubleClicksIn = (double)BasisReconciliation_.getNumberOfDoubleClicks();

		double brBitsIn = (double)BasisReconciliation_.getReceivedQubits();
		double brBitsOut = (double)BasisReconciliation_.getReconciledBits();

		double peBitsIn = (double)ParameterEstimation_.getInputBits();
		double peBitsOut = (double)ParameterEstimation_.getOutputBits();
		double averageQBER = (double)ParameterEstimation_.getAverageEstimatedBER();

		double ecBitsIn = (double)ErrorCorrection_.getInputBits();
		double ecBitsOut = (double)ErrorCorrection_.getOutputBits();

		double paBitsIn = (double)PrivacyAmplification_.getInputBits();
		double paBitsOut = (double)PrivacyAmplification_.getOutputBits();

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
