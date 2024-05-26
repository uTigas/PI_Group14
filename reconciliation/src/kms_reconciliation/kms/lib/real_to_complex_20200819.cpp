# include "real_to_complex_20200819.h"

void RealToComplex::initialize(void) {

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());


}



bool RealToComplex::runBlock(void) {
	
	
	int ready0 = inputSignals[0]->ready();
	int ready1 = inputSignals[1]->ready();
	int ready = std::min(ready0, ready1);
	
	int space = outputSignals[0]->space();
	
	int process = std::min(ready, space);
	
	if (process == 0) return false;
	
	// Signal type verification
	signal_value_type sType0   = inputSignals[0]->getValueType();
	signal_value_type sType1   = inputSignals[1]->getValueType();
	signal_value_type sTypeOut = outputSignals[0]->getValueType();
	
	/*
	// Only real input signals are acceptable.
	if (sType0 != RealValue){
		cout << "ERROR: real_to_complex.cpp (input signal0 "
		"signal_value_type not RealValue)\n";
		return false;
	}
	if (sType1 != RealValue){
		cout << "ERROR: real_to_complex.cpp (input signal1 "
			"signal_value_type not RealValue)\n";
		return false;
	}
	// Only complex output signals are acceptable.
	if (sTypeOut != ComplexValue){
		cout << "ERROR: real_to_complex.cpp (output signal "
			"signal_value_type not ComplexValue)\n";
		return false;
	}
	*/
	
	// Buffer processing
	t_real re, im;
	//t_complex valueX;
	for (int i = 0; i < process; i++) {
		
		inputSignals[0]->bufferGet(&re);
		inputSignals[1]->bufferGet(&im);
		
		t_complex valueX = {re, im};
		outputSignals[0]->bufferPut(valueX);

	}

	return true;
}
