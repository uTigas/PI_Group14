# include "laser_20200819.h"

void Laser::initialize(void) {

	outputSignals[0]->setSamplingPeriod(samplingPeriod);

	outputSignals[0]->setSymbolPeriod(symbolPeriod);

	outputSignals[0]->setCentralWavelength_m(centralWavelength_m);

}


bool Laser::runBlock(void) {

	int process = outputSignals[0]->space();

	if (process == 0) return false;

	double outOpticalPower = opticalPower;
	double wavelength = outputSignals[0]->getCentralWavelength_m();
	double dt = outputSignals[0]->getSamplingPeriod();

	for (auto k = 0; k < process; k++) {
		double amplitude = sqrt(0.5) * sqrt(opticalPower + sqrt(relativeIntensityNoise / dt)* netxpto::randn());
		phase = phase + linewidth * netxpto::randn();
		t_complex out{ amplitude * cos(phase), amplitude * sin(phase) };
		phase = phase + 2 * PI * dt;
		outputSignals[0]->bufferPut((t_complex)out);
	}

	return true;
}