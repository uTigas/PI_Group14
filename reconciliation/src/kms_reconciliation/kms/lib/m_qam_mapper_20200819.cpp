# include "m_qam_mapper_20200819.h"


void MQAMMapper::initialize(void){

	for (auto k : outputSignals)
	{
		k->setSymbolPeriod(log2(m) * inputSignals[0]->getSymbolPeriod());
		k->setSamplingPeriod(log2(m) * inputSignals[0]->getSamplingPeriod());
	}

}

bool MQAMMapper::runBlock(void) {

	auto ready = inputSignals[0]->ready();
	auto space = outputSignals[0]->space();
	for (auto k : outputSignals)
	{
		space = (space < k->space()) ? space : k->space();
	}

	int nBinaryValues = (int)log2(m);

	if (firstTime) {
		if (space > 0) {
			space = space - 1;						// the first output sample is going to be zero, to make the system causal
			for (auto s : outputSignals)
			{
				s->bufferPut((t_real)0.0);
			}
		};
		auxBinaryValue = nBinaryValues;
		firstTime = false;
	}

	auto process = (ready <= (nBinaryValues * space)) ? ready : space;

	if (process <= 0) return false;

	for (auto k{ 0 }; k!=process; ++k) {

		t_binary binaryValue;
		inputSignals[0]->bufferGet(&binaryValue);
		auxSignalNumber = auxSignalNumber + (int) pow(2, auxBinaryValue - 1) * binaryValue;
		auxBinaryValue--;

		if (auxBinaryValue == 0) {

			unsigned int v{ 0 };
			auto vMax{ iqAmplitudes[0].size()-1 };
			for (auto s : outputSignals)
			{
				s->bufferPut(iqAmplitudes[auxSignalNumber][v]);
				if (v < vMax) v++;
			}

			auxBinaryValue = nBinaryValues;
			auxSignalNumber = 0;
		}
	}

	return true;
}

void MQAMMapper::setIqAmplitudes(std::vector<std::vector<t_real>> iqAmplitudesValues){
	m = (int) iqAmplitudesValues.size();
	iqAmplitudes.resize(m);
	iqAmplitudes = iqAmplitudesValues; 
};

void MQAMMapper::setM(int mValue)
{
	m = mValue;

	iqAmplitudes.resize(m);
	switch (m) 
	{
		case 2:
			iqAmplitudes = { {0.0, 0.0 }, {1.0, 0.0} };
			break;
		case 4:
			iqAmplitudes = { { 1.0, 1.0 }, { -1.0, 1.0 }, { -1.0, -1.0 }, { 1.0, -1.0 } };
			break;
		case 16:
			iqAmplitudes = { { -3.0, -3.0 },{ -3.0, -1.0 },{ -3.0, 3.0 },{ -3.0, 1.0 },{ -1.0, -3.0 },{ -1.0, -1.0 },{ -1.0, 3.0 },{ -1.0, 1.0 },{ 3.0, -3.0 },{ 3.0, -1.0 }, { 3.0, 3.0 },{ 3.0, 1.0 },{ 1.0, -3.0 },{ 1.0, -1.0 },{ 1.0, 3.0 },{ 1.0, 1.0 } };
	};
}; 
