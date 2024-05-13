# include "ideal_amplifier_20200819.h"


void IdealAmplifier::initialize(void){

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

}

bool IdealAmplifier::runBlock(void) {

	int ready = inputSignals[0]->ready();
	int space = outputSignals[0]->space();

	int process = std::min(ready, space);

	if (process == 0) return false;
	
	signal_value_type sTypeIn = inputSignals[0]->getValueType();
	signal_value_type sTypeOut= outputSignals[0]->getValueType();

	if (sTypeIn!=sTypeOut) {
        	std::cerr << "ERRO: ideal_amplifier.cpp (signal type mismatch!)" << "\n";
        	return false;
    	}

	switch (sTypeIn) {
	case signal_value_type::t_real:
	{
					  t_real inReal;
					  for (int k = 0; k < process; k++) {
						  inputSignals[0]->bufferGet(&inReal);
						  outputSignals[0]->bufferPut((t_real)gain*inReal);
					  }
					  break;
	}
	case signal_value_type::t_complex:
	{
						 t_complex inComplexValue;
						 for (int k = 0; k < process; k++) {
							 inputSignals[0]->bufferGet(&inComplexValue);
							 outputSignals[0]->bufferPut((t_complex)gain*inComplexValue);
						 }
						 break;
	}
	case signal_value_type::t_complex_xy:
	{
						   t_complex_xy inComplexValueXY;
						   for (int k = 0; k < process; k++) {
							   inputSignals[0]->bufferGet(&inComplexValueXY);
							   t_complex_xy outComplexValueXY = { gain*inComplexValueXY.x, gain*inComplexValueXY.y };
							   outputSignals[0]->bufferPut(outComplexValueXY);
							   break;
						   }
						   break;
	}
	}
	return true;
}

