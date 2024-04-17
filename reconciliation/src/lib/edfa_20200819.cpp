# include "edfa_20200819.h"


EDFA::EDFA(std::initializer_list<Signal *> inputSignal, std::initializer_list<Signal *> outputSignal):SuperBlock(inputSignal, outputSignal)
{
	setGain_dB(gain_dB);
	setWavelength_m(wavelength_m);
	setNoiseFigure_dB(noiseFigure_dB);
	setSuperBlockFolderName("signals/SuperBlock_EDFA");
	setLogFileName("SuperBlock_EDFA.txt");
	setLogValue(true);
};

void EDFA::initialize()
{
	OpticalSignal_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	OpticalSignal_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	OpticalSignal_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	OpticalSignal_In.setCentralWavelength_m(inputSignals[0]->getCentralWavelength_m());

	setWavelength_m(inputSignals[0]->getCentralWavelength_m());

	WhiteNoise_Out.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	WhiteNoise_Out.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	WhiteNoise_Out.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	WhiteNoise_Out.setCentralWavelength_m(inputSignals[0]->getCentralWavelength_m());

	setSuperBlockSystem(
		{
			&IdealAmplifier_,
			&WhiteNoise_,
			&Add_
		}
	);

	setSignalsFolderName(getSuperBlockFolderName());

	outputSignals[0]->setSymbolPeriod(OpticalSignal_Out.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(OpticalSignal_Out.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(OpticalSignal_Out.getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

}

bool EDFA::runBlock()
{

	if (!std::filesystem::is_directory(getSignalsFolderName()) || !std::filesystem::exists(getSignalsFolderName())) {
		std::filesystem::create_directory(getSignalsFolderName());
	}

	int ready = inputSignals[0]->ready();
	int space = OpticalSignal_In.space();
	int process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_complex dataIn;
		inputSignals[0]->bufferGet(&dataIn);
		OpticalSignal_In.bufferPut(dataIn);
	}

	bool alive = SuperBlock::runBlock(getSignalsFolderName());

	if (getFirstTime()) {
		setOpenFile(false);
		setFirstTime(false);
	}

	ready = OpticalSignal_Out.ready();
	space = outputSignals[0]->space();

	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_complex opticalOut = OpticalSignal_Out.bufferGet();
		outputSignals[0]->bufferPut(opticalOut);
	}

	return alive;

}


