# ifndef M_QAM_TRANSMITTER_H_
# define M_QAM_TRANSMITTER_H_

# include "netxpto_20200819.h"

# include "m_qam_mapper_20200819.h"
# include "discrete_to_continuous_time_20200819.h"
# include "pulse_shaper_20200819.h"
# include "iq_modulator_20200819.h"
# include "optical_amplifier_20200819.h"

class MQAMTransmitter : public SuperBlock 
{

public:

	//##############################################################################################################

	MQAMTransmitter(std::initializer_list<Signal*> inputSig, std::initializer_list<Signal*> outputSig);

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	// MQAMMapper_

	void setM(int mValue) { MQAMMapper_.setM(mValue); };
	void setIqAmplitudes(std::vector<std::vector<t_real>> iqAmplitudesValues) { MQAMMapper_.setIqAmplitudes(iqAmplitudesValues); };

	void setFirstTime(bool fTime) { MQAMMapper_.setFirstTime(fTime); };
	bool getFirstTime() { return MQAMMapper_.getFirstTime(); };

	// DiscreteToContinuousTime_

	void setNumberOfSamplesPerSymbol(int nSamplesPerSymbol) { DiscreteToContinuousTime_1_.setNumberOfSamplesPerSymbol(nSamplesPerSymbol); DiscreteToContinuousTime_2_.setNumberOfSamplesPerSymbol(nSamplesPerSymbol); };
	int const getNumberOfSamplesPerSymbol(void) { return DiscreteToContinuousTime_1_.getNumberOfSamplesPerSymbol(); };

	// PulseShapper_
	void setImpulseResponseTimeLength_symbolPeriods(int impResponseTimeLength) { PulseShaper_1_.setImpulseResponseTimeLength_symbolPeriods(impResponseTimeLength); PulseShaper_2_.setImpulseResponseTimeLength_symbolPeriods(impResponseTimeLength); };
	int const getImpulseResponseTimeLength_symbolPeriods(void) { return PulseShaper_1_.getImpulseResponseTimeLength_symbolPeriods(); };
	
	void setSeeBeginningOfImpulseResponse(bool opt) {
		PulseShaper_1_.setSeeBeginningOfImpulseResponse(opt);
		PulseShaper_2_.setSeeBeginningOfImpulseResponse(opt);
	};

	void setFilterType(pulse_shaper_filter_type fType) { PulseShaper_1_.setFilterType(fType); PulseShaper_2_.setFilterType(fType); };
	pulse_shaper_filter_type const getFilterType(void) { return PulseShaper_1_.getFilterType(); };

	void setRollOffFactor(double rOffFactor) { PulseShaper_1_.setRollOffFactor(rOffFactor); PulseShaper_2_.setRollOffFactor(rOffFactor); };
	double const getRollOffFactor() { return PulseShaper_1_.getRollOffFactor(); };

	void setPulseWidth_ps(double pWidth) { PulseShaper_1_.setPulseWidth_ps(pWidth); PulseShaper_2_.setPulseWidth_ps(pWidth); };
	double const getPulseWidth_ps() { return PulseShaper_1_.getPulseWidth_ps(); };

	void setPassiveFilterMode(bool pFilterMode) { PulseShaper_1_.setPassiveFilterMode(pFilterMode); PulseShaper_2_.setPassiveFilterMode(pFilterMode); };
	bool const getPassiveFilterMode() { return PulseShaper_1_.getPassiveFilterMode(); };

	void setPulseShapperFilenameInPhaseBranch(std::string fName) { PulseShaper_1_.setImpulseResponseFilename(fName); };
	void setPulseShapperFilenameQuadratureBranch(std::string fName) { PulseShaper_2_.setImpulseResponseFilename(fName); };

	void setSavePulseShapersImpulseResponse(bool sPulseShapersImpulseResponse) {
		PulseShaper_1_.setSaveImpulseResponse(sPulseShapersImpulseResponse);
		PulseShaper_2_.setSaveImpulseResponse(sPulseShapersImpulseResponse);
	};

	void setPulseShaperFilterType(std::string pulseShaperType);

	//void setRemovePulseShaperFilterDelay(bool removeDelay) { PulseShaper_1_.setRemoveFilterDelay(removeDelay); PulseShaper_2_.setRemoveFilterDelay(removeDelay); };
	//bool const getRemovePulseShaperFilterDelay() { return PulseShaper_1_.getRemoveFilterDelay(); };

	void setBTs(double bts) { PulseShaper_1_.setBTs(bts); PulseShaper_2_.setBTs(bts); };
	double const getBTs() { return PulseShaper_1_.getBTs(); };

	// BoosterAmplifier_
	void setGain_dB(t_real g) { BoosterAmplifier_.setGain_dB(g); };
	t_real getGain_dB(void) { BoosterAmplifier_.getGain_dB(); }

	void setNoiseFigure_dB(t_real nFigure_dB) { BoosterAmplifier_.setNoiseFigure_dB(nFigure_dB); };
	t_real getNoiseFigure_dB(void) { BoosterAmplifier_.getNoiseFigure_dB(); }

	//###############################################################################################################

private:

	// Input Parameters #############################################################################################

	
	//###############################################################################################################

	// Signals Declaration and Initialization #######################################################################

	Binary BinaryData_In{ "S0_BinaryData_In.sgn" };

	TimeDiscreteAmplitudeContinuousReal MQAMMapperOut_1{ "S1_MQAMMapperOut_1.sgn" };

	TimeDiscreteAmplitudeContinuousReal MQAMMapperOut_2{ "S2_MQAMMapperOut_2.sgn" };

	TimeContinuousAmplitudeContinuousReal DiscreteToContinuousTimeOut_1{ "S3_DiscreteToContinuousTimeOut_1.sgn" };

	TimeContinuousAmplitudeContinuousReal DiscreteToContinuousTimeOut_2{ "S4_DiscreteToContinuousTimeOut_2.sgn" };

	TimeContinuousAmplitudeContinuousReal PulseShaper_Out_1{ "S5_PulseShaper_Out_1.sgn" };

	TimeContinuousAmplitudeContinuousReal PulseShaper_Out_2{ "S6_PulseShaper_Out_2.sgn" };

	OpticalSignal TxLocalOscillator_In{ "S7_TxLocalOscillator_In.sgn" };

	OpticalSignal IQModulator_Out{ "S8_IQModulator_Out.sgn" };

	OpticalSignal BoosterAmplifier_Out{ "S9_BoosterAmplifier_Out.sgn" };

	// Blocks Declaration and Inicialization ########################################################################

	MQAMMapper MQAMMapper_{ { &BinaryData_In},{ &MQAMMapperOut_1, &MQAMMapperOut_2 } };

	DiscreteToContinuousTime DiscreteToContinuousTime_1_{ { &MQAMMapperOut_1 },{ &DiscreteToContinuousTimeOut_1 } };

	DiscreteToContinuousTime DiscreteToContinuousTime_2_{ { &MQAMMapperOut_2 },{ &DiscreteToContinuousTimeOut_2 } };

	PulseShaper PulseShaper_1_{ { &DiscreteToContinuousTimeOut_1 },{ &PulseShaper_Out_1 } };
	
	PulseShaper PulseShaper_2_{ { &DiscreteToContinuousTimeOut_2 },{ &PulseShaper_Out_2 } };

	IQModulator IQModulator_{ { &PulseShaper_Out_1, &PulseShaper_Out_2, &TxLocalOscillator_In },{ &IQModulator_Out } };

	OpticalAmplifier BoosterAmplifier_{ { &IQModulator_Out },{ &BoosterAmplifier_Out } };

	// ##############################################################################################################

};

# endif

