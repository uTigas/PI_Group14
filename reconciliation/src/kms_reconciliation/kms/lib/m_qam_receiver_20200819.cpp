# include "m_qam_receiver_20200819.h"

MQAMReceiver::MQAMReceiver(std::initializer_list<Signal *> inputSignals, std::initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals) 
{
	setElectricalImpulseResponseFilename("impulse_response_in_phase.imp");
	setElectricalImpulseResponseFilename("impulse_response_quadrature.imp");
};

void MQAMReceiver::initialize()
{
	OpticalSignal_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	OpticalSignal_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	OpticalSignal_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	OpticalSignal_In.setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

	LocalOscillator_In.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	LocalOscillator_In.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	LocalOscillator_In.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	OpticalPreAmplifier_.setSuperBlockFolderName("signals/SuperBlock_MQAMReceiver/SuperBlock_PreAmplifier");
	OpticalPreAmplifier_.setLogFileName("log_PreAmplifier.txt");
	OpticalPreAmplifier_.setSaveInternalSignals(true);
	OpticalPreAmplifier_.setLogValue(true);

	OpticalFilter_.setSuperBlockFolderName("signals/SuperBlock_MQAMReceiver/SuperBlock_OpticalFilter");
	OpticalFilter_.setBandwidth_Hz(400e9);
	OpticalFilter_.setFilterType(electrical_filter_type::LowPass);
	OpticalFilter_.setLogFileName("log_OpticalFilter.txt");
	OpticalFilter_.setSaveInternalSignals(true);

	DifferentialPhotoreceiver_1_.setSuperBlockFolderName(getSuperBlockFolderName() + "/SuperBlock_DifferentialPhotoreceiver_1");
	DifferentialPhotoreceiver_1_.setLogFileName("SuperBlock_DifferentialPhotoreceiver_1.txt");
	DifferentialPhotoreceiver_1_.setLogValue(true);
	
	DifferentialPhotoreceiver_2_.setSuperBlockFolderName(getSuperBlockFolderName() + "/SuperBlock_DifferentialPhotoreceiver_2");
	DifferentialPhotoreceiver_2_.setLogFileName("SuperBlock_DifferentialPhotoreceiver_2.txt");
	DifferentialPhotoreceiver_2_.setLogValue(true);


	setSuperBlockSystem(
		{	
			&OpticalPreAmplifier_,
			&OpticalFilter_,
			&OpticalHybrid_,
			&DifferentialPhotoreceiver_1_,
			&DifferentialPhotoreceiver_2_,
			&LinearEqualizer_,
			&ReceiverFilter_1_,
			&ReceiverFilter_2_,
			&Sampler_1_,
			&Sampler_2_,
			&Decoder_
		}
	);

	outputSignals[0]->setSymbolPeriod(BinaryData_Out.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(BinaryData_Out.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(BinaryData_Out.getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(1);

	setLogValue(logValue);
	setLogFileName(logFileName);
	setSignalsFolderName(signalsFolderName);
};

bool MQAMReceiver::runBlock()
{

	if (!std::filesystem::is_directory(signalsFolderName) || !std::filesystem::exists(signalsFolderName)) {
		std::filesystem::create_directory(signalsFolderName);
	}

	int ready = inputSignals[0]->ready();
	int space = OpticalSignal_In.space();
	int process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_complex opticalIn;
		inputSignals[0]->bufferGet(&opticalIn);
		OpticalSignal_In.bufferPut(opticalIn);
	}

	ready = inputSignals[1]->ready();
	space = LocalOscillator_In.space();
	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_complex opticalIn;
		inputSignals[1]->bufferGet(&opticalIn);
		LocalOscillator_In.bufferPut(opticalIn);
	}

	bool alive = SuperBlock::runBlock(signalsFolderName);

	if (getFirstTime()) {
		setOpenFile(false);
		setFirstTime(false);
	}

	ready = BinaryData_Out.ready();
	space = outputSignals[0]->space();

	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_binary binaryDataOut = BinaryData_Out.bufferGet();
		outputSignals[0]->bufferPut((t_binary) binaryDataOut);
	}

	return alive;
};

/*
void MQAMReceiver::setTiAmplifierElectricalFilterImpulseResponseType(std::string rxElectricalFilterImpulseResponseType)
{

	bool filterShapeNameOK{ false };
	if (rxElectricalFilterImpulseResponseType == "RaisedCosine")
	{
		setTiAmplifierFilterType(pulse_shaper_filter_type::RaisedCosine);
		filterShapeNameOK = true;
	}
	if (rxElectricalFilterImpulseResponseType == "RootRaisedCosine")
	{
		setTiAmplifierFilterType(pulse_shaper_filter_type::RootRaisedCosine);
		filterShapeNameOK = true;
	}
	if (rxElectricalFilterImpulseResponseType == "Gaussian")
	{
		setTiAmplifierFilterType(pulse_shaper_filter_type::Gaussian);
		filterShapeNameOK = true;

	}
	if (rxElectricalFilterImpulseResponseType == "Square")
	{
		setTiAmplifierFilterType(pulse_shaper_filter_type::Square);
		filterShapeNameOK = true;
	}
	if (rxElectricalFilterImpulseResponseType == "NoFilter")
	{
		setTiAmplifierFilterType(pulse_shaper_filter_type::NoFilter);
		filterShapeNameOK = true;
	}
	if (!filterShapeNameOK)
	{
		std::cerr << "Wrong Pulse Shapper Name!!!" << std::endl;
	}
}
*/
void MQAMReceiver::setReceiverFilterImpulseResponseType(std::string rxReceiverFilterImpulseResponseType)
{

	bool filterShapeNameOK{ false };
	if (rxReceiverFilterImpulseResponseType == "RaisedCosine")
	{
		setReceiverFilterType(pulse_shaper_filter_type::RaisedCosine);
		filterShapeNameOK = true;
	}
	if (rxReceiverFilterImpulseResponseType == "RootRaisedCosine")
	{
		setReceiverFilterType(pulse_shaper_filter_type::RootRaisedCosine);
		filterShapeNameOK = true;
	}
	if (rxReceiverFilterImpulseResponseType == "Gaussian")
	{
		setReceiverFilterType(pulse_shaper_filter_type::Gaussian);
		filterShapeNameOK = true;

	}
	if (rxReceiverFilterImpulseResponseType == "Square")
	{
		setReceiverFilterType(pulse_shaper_filter_type::Square);
		filterShapeNameOK = true;
	}
	if (rxReceiverFilterImpulseResponseType == "NoFilter")
	{
		setReceiverFilterType(pulse_shaper_filter_type::NoFilter);
		filterShapeNameOK = true;
	}
	if (!filterShapeNameOK)
	{
		std::cerr << "Wrong Pulse Shapper Name!!!" << std::endl;
	}
}