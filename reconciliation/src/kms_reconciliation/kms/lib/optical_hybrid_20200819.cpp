#include "optical_hybrid_20200819.h"


void OpticalHybrid::initialize(void){

	setFirstTime(false);

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

	outputSignals[0]->setCentralWavelength_m(inputSignals[0]->getCentralWavelength_m());
	outputSignals[0]->setCentralFrequency_Hz(inputSignals[0]->getCentralFrequency_Hz());

	outputSignals[1]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[1]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

	outputSignals[1]->setCentralWavelength_m(inputSignals[1]->getCentralWavelength_m());
	outputSignals[1]->setCentralFrequency_Hz(inputSignals[1]->getCentralFrequency_Hz());

	outputSignals[2]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[2]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[2]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[2]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

	outputSignals[2]->setCentralWavelength_m(inputSignals[0]->getCentralWavelength_m());
	outputSignals[2]->setCentralFrequency_Hz(inputSignals[0]->getCentralFrequency_Hz());

	outputSignals[3]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[3]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[3]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[3]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

	outputSignals[3]->setCentralWavelength_m(inputSignals[1]->getCentralWavelength_m());
	outputSignals[3]->setCentralFrequency_Hz(inputSignals[1]->getCentralFrequency_Hz());
}


bool OpticalHybrid::runBlock(void){

	int ready0 = inputSignals[0]->ready();
	int ready1 = inputSignals[1]->ready();
	int ready = std::min(ready0, ready1);

	int space0 = outputSignals[0]->space();
	int space1 = outputSignals[1]->space();
	int space2 = outputSignals[2]->space();
	int space3 = outputSignals[3]->space();
	int spacea = std::min(space0, space1);
	int spaceb = std::min(space2, space3);
	int space = std::min(spacea, spaceb);

	int process = std::min(ready, space);

	if (process == 0) return false;

	std::complex<double> imaginary(0, 1);
		 
	std::complex<double> div(0.5, 0);

	for (int i = 0; i < process; i++) {

		t_complex ina;
		t_complex inb;
		inputSignals[0]->bufferGet(&ina);
		inputSignals[1]->bufferGet(&inb);

		t_complex outa = div * ina + div * inb;
		t_complex outb = div * ina - div * inb;
		t_complex outc = div * ina + imaginary*div * inb;
		t_complex outd = div * ina - imaginary*div * inb;
		outputSignals[0]->bufferPut((t_complex) outa);
		outputSignals[1]->bufferPut((t_complex) outb);
		outputSignals[2]->bufferPut((t_complex) outc);
		outputSignals[3]->bufferPut((t_complex) outd);

	}
	return true;
}
