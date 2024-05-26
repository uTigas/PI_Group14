# ifndef WHITE_NOISE_H_
# define WHITE_NOISE_H_

# include "netxpto_20200819.h"

enum class SeedType { RandomDevice, DefaultDeterministic, SingleSelected};

class WhiteNoise : public Block {

public:

	//##############################################################################################################

	WhiteNoise(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};
	//MQamMapper(vector<Signal *> &InputSig, vector<Signal *> &OutputSig) :Block(InputSig, OutputSig) {};
	//MQamMapper() {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	void setSamplingPeriod(double sPeriod) {
		samplingPeriod = sPeriod;
		if (constantPower)
		{
			unilateralNoiseSpectralDensity_W_per_Hz = noisePower * (samplingPeriod); // Update noise power when sampling period changes
		}
		else {
			noisePower = unilateralNoiseSpectralDensity_W_per_Hz * (1 / samplingPeriod); // Update noise power when spectral density changes
		}

	};
	double getSamplingPeriod(void) { return samplingPeriod; };

	void setSymbolPeriod(double symPeriod) { symbolPeriod = symPeriod; };
	double getSymbolPeriod(void) { return symbolPeriod; };

	void setNoiseSpectralDensity(double SpectralDensity) {
		unilateralNoiseSpectralDensity_W_per_Hz = SpectralDensity;
		noisePower = unilateralNoiseSpectralDensity_W_per_Hz * (1 / samplingPeriod); // Update noise power when sampling period changes
	}

	void setNoisePower(double np) {
		noisePower = np;
		unilateralNoiseSpectralDensity_W_per_Hz = noisePower * (samplingPeriod); // Update noise power when sampling period changes
	}

	void setNoiseVariance(double nv) {
		noiseVariance = nv;
	}

	void setConstantPower(bool cp) { constantPower = cp; };

	double const getNoiseSpectralDensity(void) { return unilateralNoiseSpectralDensity_W_per_Hz; }

	void setSeedType(SeedType sType) { seedType = sType; };
	SeedType const getSeedType(void) { return seedType; };

	void setSeed(int newSeed) { seed = newSeed; }
	int getSeed(void) { return seed; }


private:

	bool firstTime{ true };

	SeedType seedType{ SeedType::RandomDevice };

	std::random_device randomDevice;

	std::default_random_engine generator1;
	std::default_random_engine generator2;
	std::default_random_engine generator3;
	std::default_random_engine generator4;

	std::array<int, std::mt19937::state_size> seed_data;

	int seed{ 1 };

	double unilateralNoiseSpectralDensity_W_per_Hz{ 1.5e-17 };		// Spectral density is considered to be one sided. If two sided, pass half the value.
	double samplingPeriod{ 1.0 };
	double symbolPeriod{ 1.0 };
	double noisePower{ unilateralNoiseSpectralDensity_W_per_Hz * 1 / samplingPeriod };
	double noiseVariance{ 0 };
	bool constantPower{ 0 };

};


#endif
