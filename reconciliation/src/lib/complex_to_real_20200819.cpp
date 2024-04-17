# include "complex_to_real_20200819.h"

void ComplexToReal::initialize(void) {

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());


	outputSignals[1]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[1]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

}

bool ComplexToReal::runBlock(void) {


	int ready = inputSignals[0]->ready();

	int space0 = outputSignals[0]->space();
	int space1 = outputSignals[1]->space();
	int space = std::min(space0, space1);

	int process = std::min(ready, space);

	if (process == 0) return false;

	// Signal type verification
	signal_value_type sTypeIN = inputSignals[0]->getValueType();
	signal_value_type sTypeOut0 = outputSignals[0]->getValueType();
	signal_value_type sTypeOut1 = outputSignals[1]->getValueType();


	// Only real input signals are acceptable.
	if (sTypeIN != signal_value_type::t_complex) {
		std::cerr << "ERROR: complex_to_real.cpp (input signal "
			"signal_value_type not ComplexValue)\n";
		return false;
	}
	if (sTypeOut0 != signal_value_type::t_real) {
		std::cerr << "ERROR: complex_to_real.cpp (output signal0 "
			"signal_value_type not RealValue)\n";
		return false;
	}
	// Only complex output signals are acceptable.
	if (sTypeOut1 != signal_value_type::t_real) {
		std::cerr << "ERROR: complex_to_real.cpp (output signal1 "
			"signal_value_type not RealValue)\n";
		return false;
	}


	// Buffer processing
	t_complex valueX;
	t_real re, im;
	for (int i = 0; i < process; i++) {

		inputSignals[0]->bufferGet(&valueX);

		re = real(valueX);
		im = imag(valueX);

		outputSignals[0]->bufferPut(re);
		outputSignals[1]->bufferPut(im);
	}

	return true;
}
