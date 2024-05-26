#include "dv_qkd_rx_incoming_data_processor_20200819.h"

void DvQkdRxIncomingDataProcessor::initialize(void)
{
	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	outputSignals[1]->setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

}

bool DvQkdRxIncomingDataProcessor::runBlock(void)
{
	
	int ready{ MAX_BUFFER_LENGTH };
	int space{ MAX_BUFFER_LENGTH };

	for (auto k : inputSignals) { ready = (ready < k->ready()) ? ready : k->ready(); }

	for (auto k : outputSignals) { space = (space < k->space()) ? space : k->space(); }

	int process = std::min(ready, space);

	if (0 == process) return false;

	for (auto k = 0; k < process; ++k) 
	{

		t_binary dataIn;
		inputSignals[0]->bufferGet(&dataIn);
		outputSignals[0]->bufferPut(dataIn);
		
		t_binary basisIn;
		inputSignals[1]->bufferGet(&basisIn);
		outputSignals[1]->bufferPut(basisIn);
		
		numberOfSamples++;
	}

	return true;

}
