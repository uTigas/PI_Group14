# include "netxpto_20180118.h"

# include "m_qam_transmitter_20180118.h"
# include "i_homodyne_receiver_20180124.h"
# include "bit_decider_20170818.h"
# include "bit_error_rate_20171810.h"
# include "local_oscillator_20180130.h"
# include "balanced_beam_splitter_20180124.h"
# include "photodiode_old_20180118.h"
# include "ti_amplifier_20180102.h"
# include "sampler_20171116.h"
# include "sink_20180118.h"

int main(){

	// #####################################################################################################
	// #################################### System Input Parameters ########################################
	// #####################################################################################################

	int numberOfBitsReceived(-1);
	int numberOfBitsGenerated(1000);
	int samplesPerSymbol(16);
	int pLength = 5;
	double bitPeriod = 20e-12;
	double rollOffFactor = 0.3;
	double signalOutputPower_dBm = -20;
	double localOscillatorPower_dBm = 0;
	double localOscillatorPhase = 0;
	vector<t_iqValues> iqAmplitudeValues = { {-1, 0} , {1, 0} };
	array<t_complex, 4> transferMatrix = { { 1 / sqrt(2), 1 / sqrt(2), 1 / sqrt(2), -1 / sqrt(2)} };
	double responsivity = 1;
	double amplification = 1e6;
	double electricalNoiseAmplitude = 5e-4*sqrt(2);
	int samplesToSkip = 8 * samplesPerSymbol;
	int bufferLength = 20;
	bool shotNoise(false);
	
	// #####################################################################################################
	// ########################### Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	Binary S0("S0.sgn");
	S0.setBufferLength(bufferLength);

	OpticalSignal S1("S1.sgn");
	S1.setBufferLength(bufferLength);

	OpticalSignal S2("S2.sgn");
	S2.setBufferLength(bufferLength);

	OpticalSignal S3("S3.sgn");
	S3.setBufferLength(bufferLength);

	OpticalSignal S4("S4.sgn");
	S4.setBufferLength(bufferLength);

	TimeContinuousAmplitudeContinuousReal S5("S5.sgn");
	S5.setBufferLength(bufferLength);
	
	TimeDiscreteAmplitudeContinuousReal S6("S6.sgn");
	S6.setBufferLength(bufferLength);

	Binary S7("S7.sgn");
	S7.setBufferLength(bufferLength);

	Binary S8("S8.sgn");
	S8.setBufferLength(bufferLength);

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	MQamTransmitter B1{ vector<Signal*> {}, vector<Signal*> {&S1, &S0} };
	B1.setNumberOfBits(numberOfBitsGenerated);
	B1.setOutputOpticalPower_dBm(signalOutputPower_dBm);
	B1.setMode(PseudoRandom);
	B1.setBitPeriod(bitPeriod);
	B1.setPatternLength(pLength);
	B1.setIqAmplitudes(iqAmplitudeValues);
	B1.setNumberOfSamplesPerSymbol(samplesPerSymbol);
	B1.setRollOffFactor(rollOffFactor);
	B1.setSaveInternalSignals(true);
	B1.setPulseShaperFilter(RaisedCosine);
	B1.setSeeBeginningOfImpulseResponse(false);



	LocalOscillator B2{ vector<Signal*> { }, vector<Signal*> { &S2 } };
	B2.setOpticalPower_dBm(localOscillatorPower_dBm);
	B2.setPhase(localOscillatorPhase);
	B2.setSamplingPeriod(bitPeriod / samplesPerSymbol);
	B2.setSymbolPeriod(bitPeriod);

	BalancedBeamSplitter B3{ vector<Signal*> {&S1, &S2}, vector<Signal*> {&S3, &S4} };
	B3.setTransferMatrix(transferMatrix);

	I_HomodyneReceiver B4{ vector<Signal*> {&S3, &S4}, vector<Signal*> {&S5} };
	B4.useShotNoise(true);
	B4.setElectricalNoiseSpectralDensity(electricalNoiseAmplitude);
	B4.setGain(amplification);
	B4.setResponsivity(responsivity);
	B4.setSaveInternalSignals(true);

	Sampler B5{ vector<Signal*> {&S5}, vector<Signal*> {&S6} };
	B5.setSamplesToSkip(samplesToSkip);
	
	BitDecider B6{ vector<Signal*> {&S6}, vector<Signal*> {&S7} };

	BitErrorRate B7{ vector<Signal*> {&S0, &S7}, vector<Signal*> {&S8} };

	Sink B8{ vector<Signal*> { &S8 }, vector<Signal*> {} };
	B8.setNumberOfSamples(numberOfBitsGenerated);
	B8.setDisplayNumberOfSamples(true);


	// #####################################################################################################
	// ########################### System Declaration and Inicialization ###################################
	// #####################################################################################################

	System MainSystem{ vector<Block*> { &B1, &B2, &B3, &B4, &B5, &B6, &B7, &B8} };

	// #####################################################################################################
	// #################################### System Run #####################################################
	// #####################################################################################################
	MainSystem.setLogValue(false);
	MainSystem.run();

	return 0;

}
