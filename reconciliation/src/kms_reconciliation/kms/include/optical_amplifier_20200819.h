# ifndef PROGRAM_INCLUDE_EDFA_H_
# define PROGRAM_INCLUDE_EDFA_H_

# include "netxpto_20200819.h"

# include "ideal_amplifier_20200819.h"
# include "add_20200819.h"
# include "white_noise_20200819.h"



class OpticalAmplifier : public SuperBlock 
{

	public:

	//##############################################################################################################

	OpticalAmplifier(std::initializer_list<Signal*> inputSignal, std::initializer_list<Signal*> outputSignal);

	void initialize(void);
	bool runBlock(void);
	//void terminate(void);
	

	
	//##############################################################################################################
	
	// IdealAmplifier_

	void setGain_dB(t_real g)
	{
		gain_dB = g;
		IdealAmplifier_.setGain(sqrt(pow(10, (gain_dB / 10))));
		WhiteNoise_.setNoiseSpectralDensity(PLANCK_CONSTANT * SPEED_OF_LIGHT * (1 / wavelength_m) * (pow(10, gain_dB / 10) - 1) * pow(10, noiseFigure_dB / 10) / 2);
	}

	t_real getGain_dB(void) { return gain_dB; }
			
	// WhiteNoise_

	void setNoiseFigure_dB(t_real nFigure_dB)
	{
		noiseFigure_dB = nFigure_dB;
		WhiteNoise_.setNoiseSpectralDensity(PLANCK_CONSTANT * SPEED_OF_LIGHT * (1 / wavelength_m) * (pow(10, gain_dB / 10) - 1) * pow(10, noiseFigure_dB / 10) / 2);
	}

	t_real getNoiseFigure_dB(void) { return noiseFigure_dB; }
	
	void setWavelength_m(t_real w_m) 
	{
			wavelength_m = w_m;
			WhiteNoise_.setNoiseSpectralDensity(PLANCK_CONSTANT*SPEED_OF_LIGHT*(1 / w_m)*(pow(10, gain_dB / 10) - 1) * pow(10, noiseFigure_dB / 10) / 2);
	}

	t_real getWavelength_m(void) { return wavelength_m; }


private:

	// Input Parameters ################################################################################

	t_real gain_dB{ 10.0 };
	t_real noiseFigure_dB{ 4.0 };
	t_real wavelength_m{ 1550.1e-9 };

		
	// ##################################################################################################

	// Signals Declaration and Inicialization ###########################################################

	OpticalSignal OpticalSignal_In{ "OpticalSignal_In.sgn" };
	OpticalSignal IdealAmplifier_Out{ "IdealAmplifier_Out.sgn" };
	OpticalSignal WhiteNoise_Out{ "WhiteNoise_Out.sgn" };
	OpticalSignal OpticalSignal_Out{ "OpticalSignal_Out.sgn" };

	//###############################################################################################################

	// Blocks Declaration and Inicialization ########################################################################

	IdealAmplifier IdealAmplifier_{ {&OpticalSignal_In}, {&IdealAmplifier_Out} };
	WhiteNoise WhiteNoise_{ {}, {&WhiteNoise_Out} };
	Add Add_{ {&IdealAmplifier_Out, &WhiteNoise_Out}, {&OpticalSignal_Out} };

	//###############################################################################################################
};

# endif // PROGRAM_INCLUDE_EDFA_H_
