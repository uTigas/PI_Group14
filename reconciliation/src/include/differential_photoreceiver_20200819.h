# ifndef DIFFERENTIAL_PHOTORECEIVER_H_
# define DIFFERENTIAL_PHOTORECEIVER_H_

# include "netxpto_20200819.h"

# include "photodiode_20200819.h"
# include "subtractor_20200819.h"
# include "transimpedance_amplifier_20200819.h"


class DifferentialPhotoreceiver : public SuperBlock {

public:

	//##############################################################################################################

	DifferentialPhotoreceiver(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals);
	DifferentialPhotoreceiver(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName);

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	void setTiAmplifierNoisePower(double tAmplifierNoisePower) { TiAmplifier_.setNoisePower(tAmplifierNoisePower); 	}

	void setTiAmplifierGain(double tAmplifierGain) { TiAmplifier_.setGain(tAmplifierGain); };
	double const getGain(void) { TiAmplifier_.getGain(); }

	void setElectricalFilterType(electrical_filter_type eFilterType) { TiAmplifier_.setFilterType(eFilterType); };
	electrical_filter_type getFilterType(void) { return TiAmplifier_.getFilterType(); };

	void setElectricalFilterBandwidth(double eFilterBandwidth) { TiAmplifier_.setCutoffFrequency(eFilterBandwidth); };
	double const getCutoffFrequency(void) { return TiAmplifier_.getCutoffFrequency(); };

	void setImpulseResponseTimeLength(int impResponseTimeLength) { TiAmplifier_.setImpulseResponseTimeLength(impResponseTimeLength); };
	int const getImpulseResponseTimeLength(void) { return TiAmplifier_.getImpulseResponseTimeLength(); };

	void setImpulseResponse(std::vector<t_real> ir) { TiAmplifier_.setImpulseResponse(ir); };
	std::vector<t_real> getImpulseResponse(void) { TiAmplifier_.getImpulseResponse(); };

	void setImpulseResponseFilename(std::string fName) { TiAmplifier_.setImpulseResponseFilename(fName); }
	std::string getImpulseResponseFilename(void) { TiAmplifier_.getImpulseResponseFilename(); }

	void setImpulseResponseLength(int impResponseLength) { TiAmplifier_.setImpulseResponseLength(impResponseLength); };
	int const getImpulseResponseLength(void) { return TiAmplifier_.getImpulseResponseLength(); };

	void setSeeBeginningOfImpulseResponse(bool sBegginingOfImpulseResponse) { TiAmplifier_.setSeeBeginningOfImpulseResponse(sBegginingOfImpulseResponse); };
	double const getSeeBeginningOfImpulseResponse(void) { return TiAmplifier_.getSeeBeginningOfImpulseResponse(); };

	void setPhotodiodeResponsivity(double r) { Photodiode_1_.setResponsivity(r); Photodiode_2_.setResponsivity(r); };

	void setPhotodiodeJunctionResistance(double jResistance) { 
		Photodiode_1_.setJunctionResistance(jResistance); 
		Photodiode_2_.setJunctionResistance(jResistance); 
	};
	void setPhotodiodeJunctionCapacitance(double jCapacitance) { 
		Photodiode_1_.setJunctionCapacitance(jCapacitance); 
		Photodiode_2_.setJunctionCapacitance(jCapacitance); 
	};
	void setPhotodiodeLoadResistance(double lResistance) { 
		Photodiode_1_.setLoadResistance(lResistance); 
		Photodiode_2_.setLoadResistance(lResistance); 
	};

	void setPhotodiodeTemperature(int t) { Photodiode_1_.setTemperature(t); Photodiode_2_.setTemperature(t); };

	void usePhotodiodeThermalNoise(bool tNoise) { Photodiode_1_.useThermalNoise(tNoise); Photodiode_2_.useThermalNoise(tNoise); };

	void usePhotodiodeShotNoise(bool sNoise) { Photodiode_1_.useShotNoise(sNoise); Photodiode_2_.useShotNoise(sNoise); };


	//###############################################################################################################;

private:

	// #####################################################################################################
	// ################################## Input Parameters #################################################
	// #####################################################################################################

	/*
	std::string signalsFolderName{ "SuperBlock_DifferentialPhotoreceiver" };
	bool logValue{ true };
	std::string logFileName{ "SuperBlock_DifferentialPhotoreceiver.txt" };
	*/

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	OpticalSignal Photodiode_1_In{ "S0_Photodiode_1_In.sgn" };
	OpticalSignal Photodiode_2_In{ "S1_Photodiode_2_In.sgn" };
	TimeContinuousAmplitudeContinuousReal Photodiode_1_Out{ "S2_Photodiode_1_Out.sgn" };
	TimeContinuousAmplitudeContinuousReal Photodiode_2_Out{ "S3_Photodiode_2_Out.sgn" };
	TimeContinuousAmplitudeContinuousReal SubtractorOut{ "S4_SubtractorOut.sgn" };
	TimeContinuousAmplitudeContinuousReal TiAmplifierOut{ "S5_TiAmplifierOut.sgn" };

	// #####################################################################################################
	// ########################### Blocks Declaration and Initialization ###################################
	// #####################################################################################################

	Photodiode Photodiode_1_{ { &Photodiode_1_In },{ &Photodiode_1_Out } };
	Photodiode Photodiode_2_{ { &Photodiode_2_In },{ &Photodiode_2_Out } };
	Subtractor Subtractor_{ {&Photodiode_1_Out, &Photodiode_2_Out},{&SubtractorOut} };
	TransimpedanceAmplifier TiAmplifier_{ {&SubtractorOut},{ &TiAmplifierOut } };

};


#endif
