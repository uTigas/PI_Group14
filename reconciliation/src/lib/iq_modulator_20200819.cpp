# include "iq_modulator_20200819.h"

void IQModulator::initialize(void) 
{
	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

	outputSignals[0]->setCentralWavelength_m(inputSignals[2]->getCentralWavelength_m());
	outputSignals[0]->setCentralFrequency_Hz(inputSignals[2]->getCentralFrequency_Hz());

}

bool IQModulator::runBlock(void) 
{

	int ready0 = inputSignals[0]->ready();
	int ready1 = inputSignals[1]->ready();
	int ready2 = inputSignals[2]->ready();

	int ready = std::min(ready0, ready1);
	ready = std::min(ready, ready2);

	int space = outputSignals[0]->space();
	   
	int process = std::min(ready, space);

	if (process == 0) return false;

	t_real re, im;
	t_complex a_in;
	for (int i = 0; i < process; i++) {

		inputSignals[0]->bufferGet(&re);
		inputSignals[1]->bufferGet(&im);
		inputSignals[2]->bufferGet(&a_in);

		t_complex aux(re, im);
		t_complex value_out = a_in * aux;

		signal_value_type sType = outputSignals[0]->getValueType();

		switch (sType) {
			case signal_value_type::t_complex:
				outputSignals[0]->bufferPut(value_out);
				break;
			case signal_value_type::t_complex_xy:
				t_complex_xy value_out_XY = {value_out, (0, 0) };
				outputSignals[0]->bufferPut(value_out_XY);
				break;
		}
	}
	return true;

}

void IQModulator::terminate(void)
{

	int ready0 = inputSignals[0]->ready();
	int ready1 = inputSignals[1]->ready();
	int ready2 = inputSignals[2]->ready();

	int ready = std::min(ready0, ready1);
	ready = std::min(ready, ready2);

	int space = outputSignals[0]->space();

	int process = std::min(ready, space);

	if (process <= 0) return;

	t_real re, im;
	t_complex a_in;
	for (int i = 0; i < process; i++) {

		inputSignals[0]->bufferGet(&re);
		inputSignals[1]->bufferGet(&im);
		inputSignals[2]->bufferGet(&a_in);

		t_complex aux(re, im);
		t_complex value_out = a_in * aux;

		signal_value_type sType = outputSignals[0]->getValueType();

		switch (sType) {
		case signal_value_type::t_complex:
			outputSignals[0]->bufferPut(value_out);
			break;
		case signal_value_type::t_complex_xy:
			t_complex_xy value_out_XY = { value_out, (0, 0) };
			outputSignals[0]->bufferPut(value_out_XY);
			break;
		}
	}
	return;

}



