# include "discrete_to_continuous_time_20200819.h"

void DiscreteToContinuousTime::initialize(void) {

	outputSignals[0]->setSymbolPeriod (inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod (inputSignals[0]->getSamplingPeriod() / numberOfSamplesPerSymbol);
	outputSignals[0]->setSamplesPerSymbol (numberOfSamplesPerSymbol);
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

}

bool DiscreteToContinuousTime::runBlock(void) {

	bool alive{ false };

	int ready = inputSignals[0]->ready();
	int space = outputSignals[0]->space();
	
	signal_value_type inSignalType = inputSignals[0]->getValueType();

	if (index != 0) {
		int length = std::min(numberOfSamplesPerSymbol - index, space);
		if (length > 0) alive = true;

		switch (inSignalType) {
			case signal_value_type::t_real:
				for (int i = 0; i < length; i++) {
					outputSignals[0]->bufferPut((t_real) 0.0);
					index++;
				}
				index = index % numberOfSamplesPerSymbol;
				break;
			default: std::cerr << "ERRO: discrete_to_continuous_time.cpp - invalide signal type" << "\n";
		}
		
	};

	if (index != 0) return alive;

	int length = std::min(space, ready*numberOfSamplesPerSymbol);
	if (length <= 0)
		return alive;
	else
		alive = true;

	switch (inSignalType) {
		case signal_value_type::t_real:
			t_real value;
			for (int i = 0; i < length; i++) {
				if (index != 0) {
					outputSignals[0]->bufferPut((t_real) 0.0);
				}
				else {
					(inputSignals[0])->bufferGet(&value);
					outputSignals[0]->bufferPut(value);
				}
				space--;
				index++;
				index = index % numberOfSamplesPerSymbol;
			}
			break;
		default: std::cerr << "ERRO: discrete_to_continuous_time.cpp - invalide signal type" << "\n";
	};

	return alive;
};
