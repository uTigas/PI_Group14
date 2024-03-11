# ifndef PROGRAM_INCLUDE_PHOTODIODE_PAIR_H_
# define PROGRAM_INCLUDE_PHOTODIODE_PAIR_H_

# include "netxpto_20200819.h"


// Simulates a photodiode
class PhotodiodePair : public Block {

	bool firstTime{ true };
	int aux = 0;
	bool firstPass{ true };
	double t = 0;

public:

	double frequencyMismatch{ 1.9441e+11 };
	double responsivity = 1;
	bool shotNoise = false;

	bool thermalNoise = false;
	t_real thermalNoiseAmplitude = 4.7626e-06;

	std::default_random_engine generatorAmp1;
	std::default_random_engine generatorAmp2;

	PhotodiodePair() {};
	PhotodiodePair(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig){};
	
	void initialize(void);
	bool runBlock(void);

	void setResponsivity(t_real Responsivity) { responsivity = Responsivity; }
	double const getResponsivity(void) { return responsivity; }

	void useNoise(bool sNoise) { shotNoise = sNoise; }

	void useThermalNoise(bool sNoise) { thermalNoise = sNoise; }
	void setThermalNoiseAmplitude(double amplitude) { thermalNoiseAmplitude = amplitude; }
	double const getThermalNoiseAmplitude(void) { return thermalNoiseAmplitude; };

private:
};


#endif // !PROGRAM_INCLUDE_PHOTODIODE_OLD_H_
