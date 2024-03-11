# ifndef LASER_H_
# define LASER_H_

# include "netxpto_20200819.h"

class Laser: public Block {

public:

	// ##########################################################################

	Laser(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig){};
	
	void initialize(void);
	bool runBlock(void);

	// ##########################################################################

	void setSamplingPeriod(double sPeriod) { samplingPeriod = sPeriod; }
	double getSamplingPeriod() const  { return samplingPeriod; }

	void setSymbolPeriod(double sPeriod) { symbolPeriod = sPeriod; }
	double getSymbolPeriod() const { return symbolPeriod; }

	void setCentralWavelength_m(double cWavelength_m) { centralWavelength_m = cWavelength_m;  }
	double getCentralWavelength_m() { return centralWavelength_m; }
	void setCentralWavelength_nm(double cWavelength_nm) { centralWavelength_m = cWavelength_nm*1E-9; }
	double getCentralWavelength_nm() { return centralWavelength_m*1E9; }

	void setFrequency(double freq) { centralWavelength_m = SPEED_OF_LIGHT / freq; }
	double getFrequency() { return (SPEED_OF_LIGHT / centralWavelength_m); }

	void setOpticalPower(double oPower) { opticalPower = oPower; }
	double getOpticalPower() const { return opticalPower; }
	void setOpticalPower_dBm(double oPower_dBm) { opticalPower = 1e-3*pow(10, oPower_dBm / 10); }
	double getOpticalPower_dBm() const { return 10*log10(opticalPower / 1e-3); }

	void setPhase(double lOscillatorPhase) { phase= lOscillatorPhase; }
	double getPhase() const { return phase; }

    void setRelativeIntensityNoise(double rIntensityNoise) { relativeIntensityNoise=rIntensityNoise; }
	double getRelativeIntensityNoise() const { return relativeIntensityNoise; }
	void setRelativeIntensityNoise_dB_per_Hz(double rIntensityNoise_dB_per_Hz) { relativeIntensityNoise = rIntensityNoise_dB_per_Hz; }
	double getRelativeIntensityNoise_dB_per_Hz() const { return relativeIntensityNoise; }

	void setLinewidth(double lwidth) { linewidth = lwidth; }
	double getLinewidth() const { return linewidth; }
	void setLinewidth_kHz(double lwidth_kHz) { linewidth = lwidth_kHz * 1E3; }
	double getLinewidth_kHz() const { return linewidth/1E3; }

private:


	/* Input Parameters */

	double samplingPeriod{ 1.0 };
	double symbolPeriod{ 1.0 };
	double centralWavelength_m{ 1550E-9 };
	double opticalPower{ 1e-3 };
	double phase{ 0.0 };
	double relativeIntensityNoise{ 0.0 };
	double linewidth{ 0.0 };

};

#endif 
