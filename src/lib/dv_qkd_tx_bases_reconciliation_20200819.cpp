#include "dv_qkd_tx_bases_reconciliation_20200819.h"

void DvQkdTxBasesReconciliation::initialize(void)
{
	for (auto k : outputSignals)
	{
		k->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
		k->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
		k->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	}

}

bool DvQkdTxBasesReconciliation::runBlock(void)
{
	t_bool alive{ false };

	auto ready = inputSignals[0]->ready();
	auto space = DataIn.space();
	auto process = std::min(ready, space);
	if (process) alive = true;
	for (auto k = 0; k < process; k++)
	{
		t_binary dataIn{ 0 };
		inputSignals[0]->bufferGet(&dataIn);
		DataIn.bufferPut(dataIn);
	}

	ready = inputSignals[1]->ready();
	space = BasesIn.space();
	process = std::min(ready, space);
	if (process) alive = true;
	for (auto k = 0; k < process; k++)
	{
		t_binary basesIn{ 0 };
		inputSignals[1]->bufferGet(&basesIn);
		BasesIn.bufferPut(basesIn);
	}

	ready = inputSignals[2]->ready();
	space = outputSignals[0]->space();
	process = std::min(ready, space);
	space = MAX_BUFFER_LENGTH;
	auto outputSignalsSize{ outputSignals.size() };
	for (auto k = 1; k < outputSignalsSize; k++) space = std::min(space, outputSignals[k]->space());
	if (process && space) alive = true;
	int numberOfSiftedKeys{ 0 };
	int numberOfProcessedSamples{ 0 };
	for (auto k = 0; k < process && space; k++)
	{
		t_binary basesFromRx{ 0 };
		inputSignals[2]->bufferGet(&basesFromRx);

		t_binary basesIn = BasesIn.bufferGet();
		t_binary dataIn = DataIn.bufferGet();

		if (basesIn == basesFromRx)
		{
			outputSignals[0]->bufferPut((t_binary) 1);

			auto outputSignalsSize = outputSignals.size();
			for (auto k = 1; k < outputSignalsSize; k++)
				outputSignals[k]->bufferPut(dataIn);

			numberOfSiftedKeys++;
			space--;
		}
		else if ((basesFromRx == 0) || (basesFromRx == 1))
			outputSignals[0]->bufferPut((t_binary) 0);
		else
			outputSignals[0]->bufferPut((t_binary) basesFromRx);

		numberOfProcessedSamples++;
	}

	totalNumberOfSiftedKeys = totalNumberOfSiftedKeys + (double) numberOfSiftedKeys;
	totalTime = totalTime + numberOfProcessedSamples * inputSignals[0]->getSymbolPeriod();

	return alive;
}
