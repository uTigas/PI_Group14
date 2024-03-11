# ifndef TRANSIMPEDANCE_AMPLIFIER_H_
# define TRANSIMPEDANCE_AMPLIFIER_H_

# include "netxpto_20200819.h"

# include "ideal_amplifier_20200819.h"
# include "add_20200819.h"
# include "white_noise_20200819.h"
# include "electrical_filter_20200819.h" 


class TransimpedanceAmplifier : public SuperBlock {

public:

	//##############################################################################################################

	TransimpedanceAmplifier(std::initializer_list<Signal *> inputSignals, std::initializer_list<Signal *> outputSignals);
	TransimpedanceAmplifier(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName);

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	//void setNoiseVariance(double newNoiseVariance) { WhiteNoise_.setNoiseVariance(newNoiseVariance); }
	void setNoisePower(double uNoiseSpectralDensity_W_per_Hz) { WhiteNoise_.setNoiseSpectralDensity(uNoiseSpectralDensity_W_per_Hz); }

	void setNoiseSamplingPeriod(t_real samplingPeriod) { WhiteNoise_.setSamplingPeriod(samplingPeriod); }
	t_real getNoiseSamplingPeriod(void) { return WhiteNoise_.getSamplingPeriod(); };

	void setNoiseSymbolPeriod(t_real symPeriod) { WhiteNoise_.setSymbolPeriod(symPeriod); }
	t_real getNoiseSymbolPeriod(void) { return WhiteNoise_.getSymbolPeriod(); };

	void setGain(double ga) { IdealAmplifier_.setGain(ga); };
	double const getGain(void) { IdealAmplifier_.getGain(); }

	void setFilterType(electrical_filter_type filterType) { ElectricalFilter_.setFilterType(filterType); };
	electrical_filter_type getFilterType(void) { return ElectricalFilter_.getFilterType(); };

	void setCutoffFrequency(double cutoffFreq) { ElectricalFilter_.setCutoffFrequency(cutoffFreq); };
	double const getCutoffFrequency(void) { return ElectricalFilter_.getCutoffFrequency(); };

	void setImpulseResponseTimeLength(int impResponseTimeLength) { ElectricalFilter_.setImpulseResponseTimeLength(impResponseTimeLength); };
	int const getImpulseResponseTimeLength(void) { return ElectricalFilter_.getImpulseResponseTimeLength(); };

	void setImpulseResponse(std::vector<t_real> ir) { ElectricalFilter_.setImpulseResponse(ir); };
	std::vector<t_real> getImpulseResponse(void) { ElectricalFilter_.getImpulseResponse(); };

	void setImpulseResponseFilename(std::string fName) { ElectricalFilter_.setImpulseResponseFilename(fName); }
	std::string getImpulseResponseFilename(void) { ElectricalFilter_.getImpulseResponseFilename(); }

	void setImpulseResponseLength(int impResponseLength) { ElectricalFilter_.setImpulseResponseLength(impResponseLength); };
	int const getImpulseResponseLength(void) { return ElectricalFilter_.getImpulseResponseLength(); };

	void setSeeBeginningOfImpulseResponse(bool sBegginingOfImpulseResponse) { ElectricalFilter_.setSeeBeginningOfImpulseResponse(sBegginingOfImpulseResponse); };
	double const getSeeBeginningOfImpulseResponse(void) { return ElectricalFilter_.getSeeBeginningOfImpulseResponse(); };

	//###############################################################################################################

private:

	// #####################################################################################################
	// ################################## Input Parameters #################################################
	// #####################################################################################################

	t_real gain{ 1E4 };
	t_real noiseSpectralDensity_W_per_Hz{ 0.0 };
	electrical_filter_type filterType{ electrical_filter_type::LowPass };
	t_real cutOffFrequency_Hz{ 1E9 };
	/*
	std::string signalsFolderName{ "SuperBlock_TiAmplifier" };
	bool logValue{ true };
	std::string logFileName{ "SuperBlock_TiAmplifier.txt" };
	*/
	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	TimeContinuousAmplitudeContinuousReal ElectricalSignal_In{ "S0_ElectricalSignal_In.sgn" };
	TimeContinuousAmplitudeContinuousReal WhiteNoiseOut{ "S1_WhiteNoiseOut.sgn" }; // Amplified
	TimeContinuousAmplitudeContinuousReal AddOut{ "S2_AddOut.sgn" }; // Noise
	TimeContinuousAmplitudeContinuousReal IdealAmplifierOut{ "S3_IdealAmplifierOut.sgn" }; // Noisy Amplified Signal
	TimeContinuousAmplitudeContinuousReal ElectricalSignal_Out{ "S4_ElectricalFilterOut.sgn" }; // Filtered Noisy Signal

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	IdealAmplifier IdealAmplifier_{ { &ElectricalSignal_In },{ &IdealAmplifierOut } };
	WhiteNoise WhiteNoise_{ {},{&WhiteNoiseOut} };
	Add Add_{ {&IdealAmplifierOut, &WhiteNoiseOut},{ &AddOut } };
	ElectricalFilter ElectricalFilter_{ { &AddOut }, { &ElectricalSignal_Out } };

};


#endif
