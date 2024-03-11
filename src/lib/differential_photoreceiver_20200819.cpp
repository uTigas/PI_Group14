# include "differential_photoreceiver_20200819.h"

DifferentialPhotoreceiver::DifferentialPhotoreceiver(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals):SuperBlock(inputSignals, outputSignals)
{
	/*
	setSuperBlockFolderName(signalsFolderName);
	setLogFileName(logFileName);
	setLogValue(true);
	*/
};

DifferentialPhotoreceiver::DifferentialPhotoreceiver(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName):SuperBlock(inputSignals, outputSignals)
{
	/*
	signalsFolderName = sFolderName;
	setSuperBlockFolderName(signalsFolderName);
	setLogFileName(logFileName);
	setLogValue(true);
	*/
};

void DifferentialPhotoreceiver::initialize()
{
	Photodiode_1_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	Photodiode_1_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	Photodiode_1_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	Photodiode_1_In.setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

	Photodiode_2_In.setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	Photodiode_2_In.setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	Photodiode_2_In.setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());
	Photodiode_2_In.setNumberOfValuesToBeSkipped(inputSignals[1]->getNumberOfValuesToBeSkipped());

	TiAmplifier_.setLogValue(true);
	TiAmplifier_.setLogFileName("SuperBlock_TransimpedanceAmplifier.txt");
	TiAmplifier_.setSuperBlockFolderName(getSuperBlockFolderName() + "/SuperBlock_TransimpedanceAmplifier" );
	
	setSuperBlockSystem(
		{
			&Photodiode_1_,
			&Photodiode_2_,
			&Subtractor_,
			&TiAmplifier_
		}
	);

	outputSignals[0]->setSymbolPeriod(TiAmplifierOut.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(TiAmplifierOut.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(TiAmplifierOut.getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(TiAmplifierOut.getNumberOfValuesToBeSkipped());

	/*
	setSuperBlockFolderName("signals/SuperBlock_DifferentialPhotoreceiver");
	setLogFileName("SuperBlock_DifferentialPhotoreceiver.txt");
	setLogValue(true);
	*/
};

bool DifferentialPhotoreceiver::runBlock()
{

	if (!std::filesystem::is_directory(getSignalsFolderName()) || !std::filesystem::exists(getSignalsFolderName())) {
		std::filesystem::create_directory(getSignalsFolderName());
	}

	auto ready = std::min(inputSignals[0]->ready(), inputSignals[1]->ready());
	auto space = std::min(Photodiode_1_In.space(), Photodiode_2_In.space());

	auto process = std::min(ready, space);

	for (auto k = 0; k < process; k++)
	{
		t_complex aux;
		inputSignals[0]->bufferGet(&aux);
		Photodiode_1_In.bufferPut(aux);
		inputSignals[1]->bufferGet(&aux);
		Photodiode_2_In.bufferPut(aux);

	}

	bool alive = SuperBlock::runBlock(getSignalsFolderName());

	if (getFirstTime()) {
		setOpenFile(false);
		setFirstTime(false);
	}

	ready = TiAmplifierOut.ready();
	space = outputSignals[0]->space();

	process = std::min(ready, space);

	for (auto k = 0; k != process; k++)
	{
		t_real signalOut = TiAmplifierOut.bufferGet();
		outputSignals[0]->bufferPut(signalOut);
	}

	return alive;
}