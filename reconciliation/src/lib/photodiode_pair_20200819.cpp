#include "photodiode_pair_20200819.h"


void PhotodiodePair::initialize(void){

	setFirstTime(false);

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

}

bool PhotodiodePair::runBlock(void) {

	double samplingPeriod = inputSignals[0]->getSamplingPeriod();

	int ready1 = inputSignals[0]->ready();
	int ready2 = inputSignals[1]->ready();
	int ready = std::min(ready1, ready2);

	int space = outputSignals[0]->space();

	int process = std::min(ready, space);

	if (process == 0) return false;

	std::normal_distribution<double> distribution(0, 1);
	double noiseAmp1;
	double noiseAmp2;

	double wavelength = inputSignals[0]->getCentralWavelength_m();

	unsigned seed = (unsigned) std::chrono::system_clock::now().time_since_epoch().count();

	generatorAmp1.seed(seed);
	generatorAmp2.seed(seed);

	for (int i = 0; i < process; i++) {

		noiseAmp1 = distribution(generatorAmp1);
		noiseAmp2 = distribution(generatorAmp2);


		t_complex input1;
		inputSignals[0]->bufferGet(&input1);
		t_complex input2;
		inputSignals[1]->bufferGet(&input2);

		t_real power1 = abs(input1)*abs(input1) * 4;
		t_real power2 = abs(input2)*abs(input2) * 4;
		
		t_real noise1;
		t_real noise2;
		t_real noise3;
		t_real noise4;

		t_real current1 = responsivity*power1;
		t_real current2 = responsivity*power2;
		
		// Implementation of thermal noise
		if (thermalNoise) {
			current1 += thermalNoiseAmplitude * distribution(generatorAmp1);
			current2 += thermalNoiseAmplitude * distribution(generatorAmp2);
		}

		// Current difference calculation.	
		t_real out = current1 - current2;

		if (shotNoise)
		{
			noise1 = sqrt(PLANCK_CONSTANT*SPEED_OF_LIGHT / (samplingPeriod*wavelength))*noiseAmp1*(sqrt(power1));
			noise2 = sqrt(PLANCK_CONSTANT*SPEED_OF_LIGHT / (samplingPeriod*wavelength))*noiseAmp2*(sqrt(power2));
			noise3 = sqrt(PLANCK_CONSTANT*SPEED_OF_LIGHT / (samplingPeriod*wavelength))*noiseAmp1*(sqrt(PLANCK_CONSTANT*SPEED_OF_LIGHT / (samplingPeriod*wavelength))*noiseAmp1 / 4);
			noise4 = sqrt(PLANCK_CONSTANT*SPEED_OF_LIGHT / (samplingPeriod*wavelength))*noiseAmp2*(sqrt(PLANCK_CONSTANT*SPEED_OF_LIGHT / (samplingPeriod*wavelength))*noiseAmp2 / 4);

			power1 = power1 + noise1 + noise3;
			power2 = power2 + noise2 + noise4;
			current1 = responsivity*power1;
			current2 = responsivity*power2;
			out = current1 - current2;
		}
		outputSignals[0]->bufferPut(out);
	}
	return true;
}
