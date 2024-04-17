# ifndef PROGRAM_INCLUDE_PHOTODIODE_H_
# define PROGRAM_INCLUDE_PHOTODIODE_H_

# include "netxpto_20200819.h"



class Photodiode : public Block {

public:

	// #################################################################################

	Photodiode(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	// #################################################################################

	// Photodiode Responsivity
	void setResponsivity(double r) { responsivity = r; }
	double const getResponsivity(void) { return responsivity; }

	void setTemperature(double t) { temperature = t; }
	void setJunctionResistance(double jResistance) { 
		junctionResistance = jResistance; 
		effectiveNoiseBandwidth = (junctionResistance + loadResistance) / (4 * junctionResistance * loadResistance * junctionCapacitance);
	}
	void setJunctionCapacitance(double jCapacitance) { 
		junctionCapacitance = jCapacitance; 
		effectiveNoiseBandwidth = (junctionResistance + loadResistance) / (4 * junctionResistance * loadResistance * junctionCapacitance);
	}
	void setLoadResistance(double lResistance) { 
		loadResistance = lResistance; 
		effectiveNoiseBandwidth = (junctionResistance + loadResistance) / (4 * junctionResistance * loadResistance * junctionCapacitance);
	}

	// Thermal Noise
	void useThermalNoise(bool tNoise) { thermalNoise = tNoise; }
	bool getThermalNoise(void) { return thermalNoise; }
	
	// Shot Noise
	void useShotNoise(bool sNoise) { shotNoise = sNoise; }
	bool getShotNoise(void) { return shotNoise; }

private:

	bool firstTime{ true };

	double responsivity{ 1.0 };
	double temperature{ 300.0 };
	double junctionResistance{ 10.0e6 };
	double junctionCapacitance{ 100.0e-12 };
	double loadResistance{ 50.0 };
	double effectiveNoiseBandwidth{ (junctionResistance + loadResistance) / (4 * junctionResistance * loadResistance * junctionCapacitance) };
	
	bool thermalNoise{ false };
	bool shotNoise{ false };

	double samplingPeriod{ 0.0 };
	double symbolPeriod{ 0.0 };
	double wavelength{ 0.0 };
	double frequency{ 0.0 };
	int samplesPerSymbol{ 0 };

};

#endif // !PROGRAM_INCLUDE_PHOTODIODE_H_
