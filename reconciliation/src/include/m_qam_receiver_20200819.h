# ifndef M_QAM_RECEIVER_H_
# define M_QAM_RECEIVER_H_

# include "netxpto_20200819.h"

# include "optical_amplifier_20200819.h"
# include "optical_filter_20200819.h"
# include "optical_hybrid_20200819.h"
# include "differential_photoreceiver_20200819.h"
# include "real_to_complex_20200819.h"
# include "linear_equalizer_20200819.h"
# include "complex_to_real_20200819.h"
# include "pulse_shaper_20200819.h"
# include "white_noise_20200819.h"
# include "add_20200819.h"
# include "sampler_20200819.h"
# include "decoder_20200819.h"

class MQAMReceiver : public SuperBlock {

public:

	//##############################################################################################################

	MQAMReceiver(std::initializer_list<Signal *> inputSig, std::initializer_list<Signal *> outputSig);

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	// PreAmplifier
	void setPreAmplifierSaveInternalSignals(bool preAmplifierSaveInternalSignals) { OpticalPreAmplifier_.setSaveInternalSignals(preAmplifierSaveInternalSignals); };
	void setPreAmplifierGain_dB(double preAmplifierGain_dB) { OpticalPreAmplifier_.setGain_dB(preAmplifierGain_dB); };
	void setPreAmplifierNoiseFigure_dB(double preAmplifierNoiseFigure_dB) { OpticalPreAmplifier_.setNoiseFigure_dB(preAmplifierNoiseFigure_dB); };

	// OpticalFilter
	void setOpticalFilterImpulseResponseTimeLength(int opticalFilterImpulseResponseTimeLength) { OpticalFilter_.setImpulseResponseTimeLength(opticalFilterImpulseResponseTimeLength); };

	// BalancedPhotodetector_X
	void  setPhotodiodesResponsivity(t_real Responsivity) { DifferentialPhotoreceiver_1_.setPhotodiodeResponsivity(Responsivity); DifferentialPhotoreceiver_2_.setPhotodiodeResponsivity(Responsivity); };
	void  setPhotodetectorThermalNoise(bool setPdThermalNoise) { DifferentialPhotoreceiver_1_.usePhotodiodeThermalNoise(setPdThermalNoise); DifferentialPhotoreceiver_2_.usePhotodiodeThermalNoise(setPdThermalNoise); };
	void  setPhotodetectorShotlNoise(bool setPdShotNoise) { DifferentialPhotoreceiver_1_.usePhotodiodeShotNoise(setPdShotNoise); DifferentialPhotoreceiver_2_.usePhotodiodeShotNoise(setPdShotNoise); };

	// TiAmplifier_X
	void setTiAmplifierGain(t_real gain) { DifferentialPhotoreceiver_1_.setTiAmplifierGain(gain); DifferentialPhotoreceiver_2_.setTiAmplifierGain(gain); };
	t_real getTiAmplifierGain(void) { DifferentialPhotoreceiver_1_.getGain(); };

	void setTiAmplifierInputNoisePowerSpectralDensity(t_real NoiseSpectralDensity) { DifferentialPhotoreceiver_1_.setTiAmplifierNoisePower(NoiseSpectralDensity); DifferentialPhotoreceiver_2_.setTiAmplifierNoisePower(NoiseSpectralDensity); };

	void setTiAmplifierFilterType(electrical_filter_type fType) { DifferentialPhotoreceiver_1_.setElectricalFilterType(fType); DifferentialPhotoreceiver_2_.setElectricalFilterType(fType); };

	//void setTiAmplifierElectricalFilterImpulseResponseType(std::string rxElectricalFilterImpulseResponseType);

	void setElectricalImpulseResponseFilename(std::string fName) { DifferentialPhotoreceiver_1_.setImpulseResponseFilename(fName);  DifferentialPhotoreceiver_2_.setImpulseResponseFilename(fName); }

	void setTiAmplifierElectricalSeeBeginningOfImpulseResponse(bool sBeginningOfImpulseResponse) { DifferentialPhotoreceiver_1_.setSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse);  DifferentialPhotoreceiver_2_.setSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse); };
	double const getTiAmplifierElectricalSeeBeginningOfImpulseResponse(void) { return  DifferentialPhotoreceiver_1_.getSeeBeginningOfImpulseResponse(); };

	void setTiAmplifierRollOffFactor(double rollOff) { DifferentialPhotoreceiver_1_.setElectricalFilterBandwidth(rollOff);  DifferentialPhotoreceiver_2_.setElectricalFilterBandwidth(rollOff);};
	//double const getTiAmplifierRollOffFactor() { return DifferentialPhotoreceiver_1_.getElectricalFilterBandwidth(); };

	//void setTiAmplifierPassiveFilterMode(bool pFilterMode) { DifferentialPhotoreceiver_1_.setTiAmplifierPassiveFilterMode(pFilterMode); TiAmplifier_2_.setTiAmplifierPassiveFilterMode(pFilterMode); }

	//void setTiAmplifierImpulseResponseTimeLength(int impResponseTimeLength) { TiAmplifier_1_.setTiAmplifierImpulseResponseTimeLength(impResponseTimeLength); TiAmplifier_2_.setTiAmplifierImpulseResponseTimeLength(impResponseTimeLength); };

	void setTiAmplifierSeeBeginningOfImpulseResponse(bool sBeginningOfImpulseResponse) { DifferentialPhotoreceiver_1_.setSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse); DifferentialPhotoreceiver_2_.setSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse); };
	double const getTiAmplifierSeeBeginningOfImpulseResponse(void) { return DifferentialPhotoreceiver_1_.getSeeBeginningOfImpulseResponse(); };

	//LinearEqualizer_
	void setLinearEqualizerFiberLength_km(double fLength_km) { LinearEqualizer_.setFiberLength_km(fLength_km); }
	void setLinearEqualizerFiberLength_m(double fLength_m) { LinearEqualizer_.setFiberLength_m(fLength_m); }
	void setLinearEqualizerStepLength_km(double fLength_km) { LinearEqualizer_.setStepLength_km(fLength_km); }
	void setLinearEqualizerStepLength_m(double fLength_m) { LinearEqualizer_.setStepLength_m(fLength_m); }
	void setLinearEqualizerDispersionCoeficient_ps_per_nm_per_km(double dispCoeff) { LinearEqualizer_.setDispersion_ps_per_nm_per_km(dispCoeff); }


	// WhiteNoise_X
	/*void setNoiseSamplingPeriod(t_real SamplingPeriod) {WhiteNoise_1_.setSamplingPeriod(SamplingPeriod); WhiteNoise_2_.setSamplingPeriod(SamplingPeriod); TiAmplifier_1_.setNoiseSamplingPeriod(SamplingPeriod); TiAmplifier_2_.setNoiseSamplingPeriod(SamplingPeriod); };
	void setNoiseSymbolPeriod(t_real nSymbolPeriod) { WhiteNoise_1_.setSymbolPeriod(nSymbolPeriod); WhiteNoise_2_.setSymbolPeriod(nSymbolPeriod); TiAmplifier_1_.setNoiseSymbolPeriod(nSymbolPeriod); TiAmplifier_2_.setNoiseSymbolPeriod(nSymbolPeriod); };

	void setThermalNoiseSpectralDensity(t_real NoiseSpectralDensity) { WhiteNoise_1_.setNoiseSpectralDensity(NoiseSpectralDensity); WhiteNoise_2_.setNoiseSpectralDensity(NoiseSpectralDensity); };
	void setThermalNoisePower(t_real NoiseSpectralDensity) { WhiteNoise_1_.setNoisePower(NoiseSpectralDensity); WhiteNoise_2_.setNoisePower(NoiseSpectralDensity); };
	void setThermalConstantPower(bool cp) { WhiteNoise_1_.setConstantPower(cp); WhiteNoise_2_.setConstantPower(cp); };
	void setSeeds(std::array<int, 2> noiseSeeds) { WhiteNoise_1_.setSeed(noiseSeeds[0]); WhiteNoise_2_.setSeed(noiseSeeds[1]); };
	void setSeedType(SeedType seedType) { WhiteNoise_1_.setSeedType(seedType); WhiteNoise_2_.setSeedType(seedType); };
	*/
	// ReceiverFilter_X
	void setReceiverFilterImpulseResponseType(std::string rxMFImpulseResponseType);
	void setReceiverFilterType(pulse_shaper_filter_type fType) { ReceiverFilter_1_.setFilterType(fType); ReceiverFilter_2_.setFilterType(fType); };

	void setReceiverFilterImpulseResponseTimeLength(int impResponseTimeLength) { ReceiverFilter_1_.setImpulseResponseTimeLength_symbolPeriods(impResponseTimeLength); ReceiverFilter_2_.setImpulseResponseTimeLength_symbolPeriods(impResponseTimeLength); };
	void setReceiverFilterFilterType(pulse_shaper_filter_type fType) { ReceiverFilter_1_.setFilterType(fType); ReceiverFilter_2_.setFilterType(fType); };
	void setReceiverFilterRollOffFactor(double rOffFactor) { ReceiverFilter_1_.setRollOffFactor(rOffFactor);  ReceiverFilter_2_.setRollOffFactor(rOffFactor); };
	void setReceiverFilterPassiveFilterMode(bool pFilterMode) { ReceiverFilter_1_.setPassiveFilterMode(pFilterMode); ReceiverFilter_2_.setPassiveFilterMode(pFilterMode); }
	void setReceiverFilterRrcNormalizeEnergy(bool ne) { ReceiverFilter_1_.setRrcNormalizeEnergy(ne); ReceiverFilter_2_.setRrcNormalizeEnergy(ne); };
	void setReceiverFilterImpulseResponseFilename(std::string fName) { ReceiverFilter_1_.setImpulseResponseFilename(fName); ReceiverFilter_2_.setImpulseResponseFilename(fName); }
	void setReceiverFilterSeeBeginningOfImpulseResponse(bool sBeginningOfImpulseResponse) { ReceiverFilter_1_.setSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse); ReceiverFilter_2_.setSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse); };
	double const getReceiverFilterSeeBeginningOfImpulseResponse(void) { return ReceiverFilter_1_.getSeeBeginningOfImpulseResponse(); };

	// Sampler_X
	void setSamplesToSkip(int sToSkip) { Sampler_1_.setSamplesToSkip(sToSkip); Sampler_2_.setSamplesToSkip(sToSkip); };
	//void setSamplerOpticalPower_dBm(double optPower_dBm) { B14.setOutputOpticalPower_dBm(optPower_dBm); B15.setOutputOpticalPower_dBm(optPower_dBm); };


	// Decoder_
	void setIqAmplitudes(std::vector<t_complex> iqAmplitudesValues) { Decoder_.setIqAmplitudes(iqAmplitudesValues); };
	std::vector<t_complex> const getIqAmplitudes(void) { return Decoder_.getIqAmplitudes(); };

	void setDecoderM(t_integer m) { Decoder_.setM(m); }
	void setDecoderScalingFactor(t_real sFactor) { Decoder_.setScalingFactor(sFactor); }


	//##############################################################################################################

private:

	// Input Parameters #############################################################################################

	std::string signalsFolderName{ "signals/SuperBlock_MQAMReceiver" };
	bool logValue{ true };
	std::string logFileName{ "SuperBlock_MQamReceiver.txt" };

	//###############################################################################################################

	// Signals Declaration and Inicialization #######################################################################

	OpticalSignal OpticalSignal_In{ "S0_OpticalSignal_In.sgn" };

	OpticalSignal OpticalPreAmplifier_Out{ "S1_OpticalPreAmplifier_Out.sgn" };

	OpticalSignal OpticalFilter_Out{ "S2_OpticalFilter_Out.sgn" };

	OpticalSignal LocalOscillator_In{ "S3_LocalOscillator_In.sgn" };
	
	OpticalSignal OpticalHybridOut_S_Plus_L{ "S4_OpticalHybridOut_S_Plus_L.sgn" }; // S+L
	OpticalSignal OpticalHybridOut_S_Minus_L{ "S5_OpticalHybridOut_S_Minus_L.sgn" }; // S-L
	OpticalSignal OpticalHybridOut_S_Plus_iL{ "S6_OpticalHybridOut_S_Plus_iL.sgn" }; // S+iL
	OpticalSignal OpticalHybridOut_S_Minus_iL{ "S7_OpticalHybridOut_S_Minus_iL.sgn" }; // S-iL

	TimeContinuousAmplitudeContinuousReal DifferentialPhotoreceiverOut_1{ "S8_DifferentialPhotoreceiverOut_1.sgn" }; // Photodiode's output
	TimeContinuousAmplitudeContinuousReal DifferentialPhotoreceiverOut_2{ "S9_DifferentialPhotoreceiverOut_2.sgn" }; // Photodiode's output


	//TimeContinuousAmplitudeContinuousReal BalancedPhotodetectorOut_1{ "S8_BalancedPhotodetectorOut_1.sgn" }; // Photodiode's output
	//TimeContinuousAmplitudeContinuousReal BalancedPhotodetectorOut_2{ "S9_BalancedPhotodetectorOut_2.sgn" }; // Photodiode's output

	//TimeContinuousAmplitudeContinuousReal TiAmplifierOut_1{ "S10_TiAmplifierOut_1.sgn" }; // TI_Amplifier output
	//TimeContinuousAmplitudeContinuousReal TiAmplifierOut_2{ "S11_TiAmplifierOut_2.sgn" }; // TI_Amplifier output

	TimeContinuousAmplitudeContinuousReal LinearEqualizerOut_1{ "S10_LinearEqualizerOut.sgn" }; // LinearEqualizer_
	TimeContinuousAmplitudeContinuousReal LinearEqualizerOut_2{ "S11_LinearEqualizerOut.sgn" }; // LinearEqualizer_


	TimeContinuousAmplitudeContinuousReal ReceiverFilterOut_1{ "S12_ReceiverFilterOut_1.sgn" }; // Filtered
	TimeContinuousAmplitudeContinuousReal ReceiverFilterOut_2{ "S13_ReceiverFilterOut_2.sgn" }; // Filtered

	
	//TimeContinuousAmplitudeContinuousReal WhiteNoiseOut_1{ "S16_WhiteNoiseOut_1.sgn" }; // White noise
	//TimeContinuousAmplitudeContinuousReal WhiteNoiseOut_2{ "S17_WhiteNoiseOut_2.sgn" }; // White noise

	//TimeContinuousAmplitudeContinuousReal AddOut_1{ "S18_AddOut_1.sgn" }; // Add
	//TimeContinuousAmplitudeContinuousReal AddOut_2{ "S19_AddOut_2.sgn" }; // Add

	TimeDiscreteAmplitudeContinuousReal SamplerOut_1{ "S14_SamplerOut_1.sgn" }; // Sampled 
	TimeDiscreteAmplitudeContinuousReal SamplerOut_2{ "S15_SamplerOut_2.sgn" }; // Sampled 

	Binary BinaryData_Out{ "S16_Binary_Data_Out.sgn" }; // recovery Sequence

	// Blocks Declaration and Inicialization ########################################################################

	OpticalAmplifier OpticalPreAmplifier_{ {&OpticalSignal_In}, {&OpticalPreAmplifier_Out} };

	OpticalFilter OpticalFilter_{ {&OpticalPreAmplifier_Out},{&OpticalFilter_Out} };

	OpticalHybrid OpticalHybrid_{ {&OpticalFilter_Out, &LocalOscillator_In },{ &OpticalHybridOut_S_Plus_L, &OpticalHybridOut_S_Minus_L, &OpticalHybridOut_S_Plus_iL, &OpticalHybridOut_S_Minus_iL } };

	DifferentialPhotoreceiver DifferentialPhotoreceiver_1_{ { &OpticalHybridOut_S_Plus_L, &OpticalHybridOut_S_Minus_L }, { &DifferentialPhotoreceiverOut_1 } };
	DifferentialPhotoreceiver DifferentialPhotoreceiver_2_{ { &OpticalHybridOut_S_Plus_iL, &OpticalHybridOut_S_Minus_iL }, { &DifferentialPhotoreceiverOut_2 } };

	LinearEqualizer LinearEqualizer_{ { &DifferentialPhotoreceiverOut_1, &DifferentialPhotoreceiverOut_2 }, { &LinearEqualizerOut_1, &LinearEqualizerOut_2 } };  // CD compensation

	PulseShaper ReceiverFilter_1_{ { &LinearEqualizerOut_1 },{ &ReceiverFilterOut_1 } };
	PulseShaper ReceiverFilter_2_{ { &LinearEqualizerOut_2 },{ &ReceiverFilterOut_2 } };

	//ComplexToReal ComplexToReal_{ { &LinearEqualizerOut }, { &LinearEqualizerOut_1, &LinearEqualizerOut_2 }};

	//WhiteNoise WhiteNoise_1_{ {},{ &WhiteNoiseOut_1 } };
	//WhiteNoise WhiteNoise_2_{ {},{ &WhiteNoiseOut_2 } };

	//Add Add_1_{ { &ReceiverFilterOut_1, &WhiteNoiseOut_1 },{ &AddOut_1 } };
	//Add Add_2_{ { &ReceiverFilterOut_2, &WhiteNoiseOut_2 },{ &AddOut_2 } };

	Sampler Sampler_1_{ { &ReceiverFilterOut_1 },{ &SamplerOut_1 } };
	Sampler Sampler_2_{ { &ReceiverFilterOut_2 },{ &SamplerOut_2 } };
	
	Decoder Decoder_{ { &SamplerOut_1, &SamplerOut_2 },{ &BinaryData_Out } };

	// ##############################################################################################################

};

#endif
