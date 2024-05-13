#include "dv_qkd_rx_bases_reconciliation_20200819.h"

void DvQkdRxBasesReconciliation::initialize(void)
{
	for (auto k : outputSignals)
	{
		k->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
		k->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
		k->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	}
}

bool DvQkdRxBasesReconciliation::runBlock(void)
{
	t_bool alive{ false };

	auto ready = BasesIn.ready();
	auto space = outputSignals[0]->space();
	auto process = std::min(ready, space);
	if (process) alive = true;
	for (; process--;)
		outputSignals[0]->bufferPut((t_binary) BasesIn.bufferGet());

	ready = inputSignals[0]->ready();
	space = DataIn.space();
	process = std::min(ready, space);
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
		BasesIn.bufferPut(basesIn);			// BasesToTx
	}

	ready = std::min(DataIn.ready(), inputSignals[2]->ready());
	process = ready;
	space = MAX_BUFFER_LENGTH;
	auto outputSignalsSize{ outputSignals.size() };
	for (auto k = 1; k < outputSignalsSize; k++) space = std::min(space, outputSignals[k]->space());
	if (process  && space) alive = true;
	for (auto k = 0; k < process && space; k++)
	{
		t_binary basesAckFromTx{ 0 };
		inputSignals[2]->bufferGet(&basesAckFromTx);

		t_binary dataIn = DataIn.bufferGet();

		if (basesAckFromTx == 1)
		{
			auto outputSignalsSize = outputSignals.size();
			for (auto k = 1; k < outputSignalsSize; k++)
				outputSignals[k]->bufferPut(dataIn);

			space--;
		}
	}

	ready = BasesIn.ready();
	space = outputSignals[0]->space();
	process = std::min(ready, space);
	if (process) alive = true;
	for (; process--;)
		outputSignals[0]->bufferPut((t_binary)BasesIn.bufferGet());

	return alive;
}
