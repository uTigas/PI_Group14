# include "transimpedance_amplifier_20200819.h"

TransimpedanceAmplifier::TransimpedanceAmplifier(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals):SuperBlock(inputSignals, outputSignals)
{
};

TransimpedanceAmplifier::TransimpedanceAmplifier(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName):SuperBlock(inputSignals, outputSignals)
{
	//signalsFolderName = sFolderName;
};

void TransimpedanceAmplifier::initialize()
{
	ElectricalSignal_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	ElectricalSignal_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	ElectricalSignal_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	ElectricalSignal_In.setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

	setSuperBlockSystem(
		{
			&IdealAmplifier_,
			&WhiteNoise_,
			&Add_,
			&ElectricalFilter_
		}
	);

	outputSignals[0]->setSymbolPeriod(ElectricalSignal_Out.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(ElectricalSignal_Out.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(ElectricalSignal_Out.getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(ElectricalSignal_Out.getNumberOfValuesToBeSkipped());
/*
	setLogValue(logValue);
	setLogFileName(logFileName);
	setSignalsFolderName(signalsFolderName);
*/
};

bool TransimpedanceAmplifier::runBlock()
{

	if (!std::filesystem::is_directory(getSignalsFolderName()) || !std::filesystem::exists(getSignalsFolderName())) {
		std::filesystem::create_directory(getSignalsFolderName());
	}

	int ready = inputSignals[0]->ready();
	int space = ElectricalSignal_In.space();
	int process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real signalIn;
		inputSignals[0]->bufferGet(&signalIn);
		ElectricalSignal_In.bufferPut(signalIn);
	}

	bool alive = SuperBlock::runBlock(getSignalsFolderName());

	if (getFirstTime()) {
		setOpenFile(false);
		setFirstTime(false);
	}

	ready = ElectricalSignal_Out.ready();
	space = outputSignals[0]->space();

	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real signalOut = ElectricalSignal_Out.bufferGet();
		outputSignals[0]->bufferPut(signalOut);
	}

	return alive;
}