# ifndef PROGRAM_INCLUDE_LINEAR_EQUILIZER_H_
# define PROGRAM_INCLUDE_LINEAR_EQUILIZER_H_

# include "netxpto_20200819.h"

# include "../algorithms/fft/fft_20200819.h"


class LinearEqualizer : public Block {

public:

	//##############################################################################################################

	LinearEqualizer(std::vector<Signal*> InputSig, std::vector<Signal*> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	void setFiberLength_km(t_real fLength_km) { fiberLength_m = fLength_km * 1e3; }
	t_real getFiberLength_km(void) { return (fiberLength_m / 1e3); }

	void setFiberLength_m(t_real fLength_m) { fiberLength_m = fLength_m; }
	t_real getFiberLength_m(void) { return fiberLength_m; }

	void setStepLength_m(t_real sLength_m) { stepLength_m = sLength_m; }
	t_real getStepLength_m(void) { return stepLength_m; }

	void setStepLength_km(t_real sLength_km) { stepLength_m = sLength_km * 1E3; }
	t_real getStepLength_km(void) { return stepLength_m/1E3; }

	void setSaveInterval(t_real sDistanceInterval_m) { saveDistanceInterval_m = sDistanceInterval_m; }
	t_real getSaveInterval(void) { return saveDistanceInterval_m; }

	void setAttenuation_dB_per_km(t_real attenuation_dB_per_km) { attenuation_per_m = log(10) * attenuation_dB_per_km / (10 * pow(10, 3)); }
	t_real getAttenuation_dB_per_km() { return ((10 * pow(10, 3)) * attenuation_per_m / log(10)); }

	void setDispersion_ps_per_nm_per_km(t_real dispersion_ps_per_km_per_nm) { dispersion_s_per_m2 = dispersion_ps_per_km_per_nm * 1E-6; }
	t_real getDispersion_ps_per_nm_per_km() { return (dispersion_s_per_m2 * 1E6); }

	void setDispersionSlope_ps_per_nm2_per_km(t_real dispersionSlope_ps_per_km_per_nm2) { dispersionSlope_s_per_m3 = dispersionSlope_ps_per_km_per_nm2 * 1E3; }
	t_real getDispersionSlopeps_per_nm2_per_km() { return dispersionSlope_s_per_m3 * 1E-3; }

	void setUseBeta3(bool uBeta3) { useBeta3 = uBeta3; }
	bool getUseBeta3(void) { return useBeta3; }

	void setNonlinearCoefficient_per_W_per_m(t_real nCoefficient_per_W_per_m) { nonlinearCoefficient_per_W_per_m = nCoefficient_per_W_per_m; }
	t_real getNonlinearCoefficient_per_W_per_m() { return nonlinearCoefficient_per_W_per_m; }

	void setImpulseResponseLength(t_integer fImpulseResponseLength) { fiberImpulseResponseLength = fImpulseResponseLength; }
	t_integer getImpulseResponseLength() { return fiberImpulseResponseLength; }

	//##############################################################################################################

private:

	//# Input Parameters ############################################################################################

	t_real attenuation_per_m{ log(10) * 0.275 / (10 * pow(10, 3)) };	// attenuation(dB / km)
	t_real dispersion_s_per_m2{ 17.0E-6 };								// group-velocity dispersion (ps/km/nm)
	t_real dispersionSlope_s_per_m3{ 0.0 };								// 0.056E3
	t_real nonlinearCoefficient_per_W_per_m{ 0.0013 };					// non-linear coeffient (1/W/m)

	t_real fiberLength_m{ 10000.0 };									// fiber length (m)
	t_real	stepLength_m{ 1000.0 };										// step length (m) 

	bool useBeta3{ false };
	bool useSSFM{ false };

	t_integer overlapSaveBlockLength{ 512 };

	int fiberImpulseResponseSafeMarginParameter{ 2 };

	bool saveSignalDuringPropagation{false};
	std::string saveSignalDuringPropagationFolder{ "LinearEqualizer" };
	t_real saveDistanceInterval_m{ 1000.0 };

	bool debugMode{ true };

	//# State Variables #############################################################################################

	t_integer fiberImpulseResponseLength{ 0 };

	std::vector<t_complex> linearEffectHalfStep;
	std::vector<t_complex> linearEffectFullStep;

	std::vector<t_complex> signalTimeDomain;

	int positionIn{ -1 };
	int positionOut{ -1 };

	//# Auxiliary Functions #########################################################################################
	t_real saveRealVector(std::vector<t_real>& data, std::string str, int begin, int end);

	t_real saveComplexVector(std::vector<t_complex>& data, std::string str, int begin, int end);

};

# endif