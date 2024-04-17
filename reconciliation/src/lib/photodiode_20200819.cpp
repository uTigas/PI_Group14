# include "photodiode_20200819.h"


void Photodiode::initialize(void)
{

	setFirstTime(false);

	samplingPeriod = inputSignals[0]->getSamplingPeriod();
	symbolPeriod = inputSignals[0]->getSymbolPeriod();
	wavelength = inputSignals[0]->getCentralWavelength_m();
	frequency = inputSignals[0]->getCentralFrequency_Hz();
	samplesPerSymbol = (int)round(symbolPeriod / samplingPeriod);

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());

}


bool Photodiode::runBlock(void)
{

	int ready = inputSignals[0]->ready();
	int space = outputSignals[0]->space();
	int process = std::min(ready, space);

	if (!process) return false;

	// random engine for thermal noise
	unsigned seed1 = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine thermalNoiseRandomEngineGenerator;
	thermalNoiseRandomEngineGenerator.seed(seed1);
	// random engine for shot noise
	unsigned seed2 = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine shotNoiseRandomEngineGenerator;
	shotNoiseRandomEngineGenerator.seed(seed2);
	
	std::normal_distribution<double> distribution(0, 1);
	t_real noiseAmp;
	t_real current;
	t_real shotCurrent = 0;
	t_real thermalCurrent = 0;
	
	for (int i = 0; i < process; ++i)
	{
		t_complex input1;
		inputSignals[0]->bufferGet(&input1);
		
		// The 2 factor is compensating the bandpass signal representation
		// At the moment, this factor doesn't seem to agree with the one used in the laser block.
		t_real power = 2 * abs(input1) * abs(input1);

		current = responsivity * power;

		if (thermalNoise)
		{
			noiseAmp = distribution(thermalNoiseRandomEngineGenerator);
			thermalCurrent = sqrt(4 * BOLTZMANN_CONSTANT * temperature / junctionResistance * effectiveNoiseBandwidth) * noiseAmp;

		}
		if (shotNoise) 
		{
			noiseAmp = distribution(shotNoiseRandomEngineGenerator);
			shotCurrent = sqrt(2 * ELECTRON_CHARGE * current * effectiveNoiseBandwidth ) * noiseAmp;	

		}

		current += thermalCurrent + shotCurrent;
		outputSignals[0]->bufferPut(current);

	}
	return true;
}