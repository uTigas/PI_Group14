# include "optical_filter_20200819.h"

OpticalFilter::OpticalFilter(std::initializer_list<Signal*> inputSignal, std::initializer_list<Signal*> outputSignal):SuperBlock(inputSignal, outputSignal)
{
	setBandwidth_Hz( bandwidth_Hz );
	setFilterType( filterType );
	setImpulseResponseTimeLength( impulseResponseTimeLength );

	setSuperBlockFolderName("signals/SuperBlock_OpticalFilter");
	setLogFileName("SuperBlock_OpticalFilter.txt");
	setLogValue(true);
};

void OpticalFilter::initialize()
{
	OpticalSignal_In.setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	OpticalSignal_In.setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	OpticalSignal_In.setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	OpticalSignal_In.setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

	OpticalSignal_In.setCentralWavelength_m(inputSignals[0]->getCentralWavelength_m());

	setSuperBlockSystem(
		{
			&ComplexToReal_,
			&ElectricalFilter_1_,
			&ElectricalFilter_2_,
			&RealToComplex_
		}
	);

	setSignalsFolderName(getSuperBlockFolderName());

	outputSignals[0]->setSymbolPeriod(OpticalSignal_Out.getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(OpticalSignal_Out.getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(OpticalSignal_Out.getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(OpticalSignal_Out.getNumberOfValuesToBeSkipped());

}

bool OpticalFilter::runBlock()
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