# include "m_qam_transmitter_20200819.h"

MQAMTransmitter::MQAMTransmitter(std::initializer_list<Signal *> inputSignals, std::initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals) 
{

	setPulseShapperFilenameInPhaseBranch("impulse_response_in_phase.imp");
	setPulseShapperFilenameQuadratureBranch("impulse_response_quadrature.imp");

};

void MQAMTransmitter::initialize()
{
	BinaryData_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	BinaryData_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	BinaryData_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	TxLocalOscillator_In.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	TxLocalOscillator_In.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	TxLocalOscillator_In.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	BoosterAmplifier_.setSuperBlockFolderName("signals/SuperBlock_MQAMTransmitter/SuperBlock_BoosterAmplifier");
	//BoosterAmplifier_.setSaveInternalSignals(param.boosterAmplifierSaveInternalSignals);
	BoosterAmplifier_.setSaveInternalSignals(true);
	BoosterAmplifier_.setLogFileName("log_BoosterAmplifier.txt");
	BoosterAmplifier_.setLogValue(true);

	setSuperBlockSystem(
		{	
			&MQAMMapper_,
			&DiscreteToContinuousTime_1_,
			&PulseShaper_1_,
			&DiscreteToContinuousTime_2_,
			&PulseShaper_2_,
			&IQModulator_,
			&BoosterAmplifier_
		}
	);

	outputSignals[0]->setSymbolPeriod(BoosterAmplifier_Out.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(BoosterAmplifier_Out.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(BoosterAmplifier_Out.getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(BoosterAmplifier_Out.getNumberOfValuesToBeSkipped());

};

bool MQAMTransmitter::runBlock()
{

	if (!std::filesystem::is_directory(getSignalsFolderName()) || !std::filesystem::exists(getSignalsFolderName())) {
		std::filesystem::create_directory(getSignalsFolderName());
	}

	int ready = inputSignals[0]->ready();
	int space = BinaryData_In.space();
	int process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary dataIn;
		inputSignals[0]->bufferGet(&dataIn);
		BinaryData_In.bufferPut(dataIn);
	}

	ready = inputSignals[1]->ready();
	space = TxLocalOscillator_In.space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_complex opticalIn;
		inputSignals[1]->bufferGet(&opticalIn);
		TxLocalOscillator_In.bufferPut(opticalIn);
	}

	bool alive = SuperBlock::runBlock(getSignalsFolderName());

//	if (getFirstTime()) {
		setOpenFile(false);
		setFirstTime(false);
//	}

	ready = BoosterAmplifier_Out.ready();
	space = outputSignals[0]->space();

	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_complex opticalOut = BoosterAmplifier_Out.bufferGet();
		outputSignals[0]->bufferPut(opticalOut);
	}

	return alive;
}


void MQAMTransmitter::setPulseShaperFilterType(std::string pulseShaperType)
{
	bool filterShapeNameOK{ false };
	if (pulseShaperType == "RaisedCosine")
	{
		setFilterType(pulse_shaper_filter_type::RaisedCosine);
		filterShapeNameOK = true;
	}
	if (pulseShaperType == "RootRaisedCosine")
	{
		setFilterType(pulse_shaper_filter_type::RootRaisedCosine);
		filterShapeNameOK = true;
	}
	if (pulseShaperType == "Gaussian")
	{
		setFilterType(pulse_shaper_filter_type::Gaussian);
		filterShapeNameOK = true;

	}
	if (pulseShaperType == "Square")
	{
		setFilterType(pulse_shaper_filter_type::Square);
		filterShapeNameOK = true;

	}
	if (pulseShaperType == "HyperbolicSecant")
	{
		setFilterType(pulse_shaper_filter_type::HyperbolicSecant);
		filterShapeNameOK = true;
	}
	if (pulseShaperType == "NoFilter")
	{
		setFilterType(pulse_shaper_filter_type::NoFilter);
		filterShapeNameOK = true;

	}
	if (!filterShapeNameOK)
	{
		std::cerr << "Wrong Pulse Shapper Name!!!" << std::endl;
	}

	return;
}