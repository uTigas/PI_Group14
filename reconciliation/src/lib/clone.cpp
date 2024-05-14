#include "clone.h"

void Clone::initialize(void) {
    for(Signal *s : outputSignals){
        s->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
        s->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
        s->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
        s->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());
    }
}

bool Clone::runBlock(void){

   	int space = outputSignals[0]->space();
	for (auto k : outputSignals) {
		int aux = k->space();
		space = std::min(space, aux);
	}

    int ready = inputSignals[0]->ready();
    int process = std::min(ready, space);

	if (process <= 0) return false;

    // assert that all output signals have the same type as the input signal
    signal_value_type sType = inputSignals[0]->getValueType();
    for(Signal *s : outputSignals){
        if(s->getValueType() != sType){
            return false;
        }
    }

    // copy the input signal to all output signals
    for(int i = 0; i < process; i++){
        t_real value;
        inputSignals[0]->bufferGet(&value);
        for(auto s : outputSignals){
            s->bufferPut(value);
        }
    }
    return true;

}