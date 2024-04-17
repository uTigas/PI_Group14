#ifndef DV_QKD_TX_H_
#define DV_QKD_TX_H_

#include "netxpto_20200819.h"

#include "dv_qkd_message_processor_receiver_20200819.h"
#include "dv_qkd_message_processor_transmitter_20200819.h"
#include "dv_qkd_basis_reconciliation_20200819.h"
#include "dv_qkd_error_correction_20200819.h"
#include "dv_qkd_privacy_amplification_20200819.h"
#include "dv_qkd_tx_incoming_data_processor_20200819.h"
#include "dv_qkd_parameter_estimation_20200819.h"
#include "ms_windows_console_output_common_20200819.h"
#include "ms_windows_network_aux_common_20200819.h"
#include "very_big_counter_20200819.h"

class DvQkdTx : public SuperBlock {
	
public:

	// #####################################################################################################

	DvQkdTx(std::initializer_list<Signal *> inputSignals, std::initializer_list<Signal *> outputSignals);
	DvQkdTx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName);
	DvQkdTx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, t_integer maxSamplesToProcessAtOnce);
	DvQkdTx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName, t_integer maxSamplesToProcessAtOnce);
	DvQkdTx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName, t_integer maxSamplesToProcessAtOnce, unsigned long int buffersSize);

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	void setErrorCorrectionMethod(t_string eCorrectionMethod) { ErrorCorrection_.setErrorCorrectionMethod(eCorrectionMethod == "LDPC" ? ErrorCorrection::t_method::LDPC : ErrorCorrection::t_method::Cascade); }
	void setErrorCorrectionPartitionSize(t_integer sz) { ErrorCorrection_.setPartitionSize(sz); }
	void setErrorCorrectionNumberOfPasses(t_integer np) { ErrorCorrection_.setNumberOfPasses(np); }
	void setDoublePartitionSize(bool db) { ErrorCorrection_.setDoublePartitionSize(db); }
	void setMaximumSyndromeSize(t_integer mss) { ErrorCorrection_.setMaximumSyndromeSize(mss); }
	void setMinimumSyndromeSize(t_integer minss) { ErrorCorrection_.setMinimumSyndromeSize(minss); }
	void setDoubleClickNumber(t_integer dcn) { BasisReconciliation_.setDoubleClickNumber(dcn); }
	void setNoClickNumber(t_integer ncn) { BasisReconciliation_.setNoClickNumber(ncn); }
	void setBer(t_real berValue) { PrivacyAmplification_.setBER(berValue); ErrorCorrection_.setBer(berValue); }
	void setSystematicSecurityParameter(t_integer ssp) { PrivacyAmplification_.setSystematicSecurityParameter(ssp); }
	void setBypassHash(bool bh) { PrivacyAmplification_.setBypassHashing(bh); }
	void setMinimumNumberOfPartitions(t_integer mnp) { ErrorCorrection_.setMinimumNumberOfPartitions(mnp); }
	void setParameterEstimationNumberBits(t_integer nb) { ParameterEstimation_.setNumberOfBitsToUse(nb); }
	void setParameterEstimationNumberBitsToStart(t_integer nbs) { ParameterEstimation_.setNumberOfBitsRequiredToStart(nbs); }
	void setPrint(bool print) { toPrint = print; }
	void setQberThreshold(const t_real th) { ParameterEstimation_.setThreshold(th); }
	void setPhotonsPerPulse(double ppp) { photonsPerPulse = ppp; }
	void setConfidenceInterval(double ci) { ParameterEstimation_.setConfidence(ci); }
	void setIpAddress(std::string ip) { ipAddress = ip; }
	std::string getIpAddress(void) { return ipAddress; }

private:

	// Input Parameters #########################################################################################

	std::string signalsFolderName{ "signals/SuperBlock_DvQkdTx" };
	bool logValue{ true };
	std::string logFileName{ "SuperBlock_DvQkdTx.txt" };
		
	bool firstTime{ true };

	unsigned long int buffSize;
	t_integer maximumECSyndromeSize;
	
	int dispBitsInterval{ 50000 };
	int dispBitsTracker{ 0 };
	//std::string folderName;
	double sentQubits{ 0 };
	double keyBits{ 0 };

	double qberUB{ 0 };
	double previousQberUB{ 0 };

	std::string reportFileName{ "alice_KeysReport.txt" };
	bool toPrint{ true };
	std::string ipAddress{ "running locally" };
	VeryBigCounter counterSentQubits;
	VeryBigCounter counterKeyBits;
	double photonsPerPulse{ 0.1 };

	// ##########################################################################################################

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	// Input
	Binary Data_In{ "S0_Data_In.sgn", buffSize };
	Binary Basis_In{ "S1_Basis_In.sgn", buffSize };
	Message Messages_In{ "S2_Messages_In.sgn", 1 };

	// Internal
	Binary DataIn{ "S3_DataIn.sgn", buffSize };
	Binary BasisIn{ "S4_BasisIn.sgn", buffSize };
	TimeContinuousAmplitudeDiscreteReal BasisFromBob{ "S5_BasisFromBob.sgn", buffSize };
	TimeContinuousAmplitudeDiscreteReal BasisToSend{ "S6_BasisToSend.sgn", buffSize };
	Binary SiftedKeys{ "S7_SiftedKeys.sgn", buffSize };
	Binary ErrorCorrectionIn{ "S8_ErrorCorrectionIn.sgn", buffSize };
	Binary DataRecon{ "S9_DataRecon.sgn", buffSize };

	TimeContinuousAmplitudeDiscreteReal ParityFromBob{ "S10_ParityFromBob.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal ParityToSend{ "S11_ParityToSend.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal PermutationsFromBob{ "S12_PermutationsFromBob.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal SeedsToBob{ "S13_SeedsToBob.sgn", buffSize };
	TimeContinuousAmplitudeDiscreteReal BERFromBob{ "S14_BERFromBob.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal BERToBob{ "S15_BERToBob.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal BERToEC{ "S16_BERToEC.sgn", buffSize };
	TimeContinuousAmplitudeDiscreteReal BERToPA{ "S17_BERToPA.sgn", buffSize };
	TimeContinuousAmplitudeDiscreteReal UnusedSignal{ "UnusedSignal.sgn", buffSize };
	TimeContinuousAmplitudeDiscreteReal ToggleBERChange{ "ToggleBERChange.sgn", buffSize };
	Binary DataCorr{ "S14_DataCorr.sgn", buffSize };

	// Output
	Binary Data_Out{ "S18_Data_Out.sgn", buffSize };
	Binary Basis_Out{ "S19_Basis_Out.sgn", buffSize };
	Binary AliceTransmissionMode_Out{ "S20_AliceTransmissionMode_Out.sgn", buffSize };
	Binary Key_Out{ "S21_Key_Out.sgn", buffSize };
	Message Messages_Out{ "S22_Messages_Out.sgn", 1 };

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	AliceIncomingDataProcessor AliceIncomingDataProcessor_{{&Data_In, &Basis_In }, { &DataIn, &BasisIn, &Data_Out, &Basis_Out }};

	MessageProcessorReceiver MessageProcessorReceiver_{ {&Messages_In}, {&BasisFromBob, &ParityFromBob,&PermutationsFromBob, &UnusedSignal, &BERFromBob, &ToggleBERChange} };

	BasisReconciliation BasisReconciliation_{ {&DataIn, &BasisIn, &BasisFromBob}, {&SiftedKeys, &DataRecon, &BasisToSend,} };

//	ParameterEstimation ParameterEstimation_{ {&DataRecon, &BERFromBob}, {&ErrorCorrectionIn, &BERToBob, &BERToEC, &BERToPA} };
	ParameterEstimation ParameterEstimation_{ {&DataRecon, &BERFromBob}, {&ErrorCorrectionIn, &BERToBob, &BERToEC}, signalsFolderName };

	ErrorCorrection ErrorCorrection_{ {&ErrorCorrectionIn, &ParityFromBob, &PermutationsFromBob,&BERToEC, &ToggleBERChange}, {&DataCorr, &ParityToSend} };

	PrivacyAmplification PrivacyAmplification_{ {&DataCorr,&BERToPA}, {&Key_Out, &SeedsToBob} };

	MessageProcessorTransmitter MessageProcessorTransmitter_{ {&BasisToSend, &ParityToSend, &UnusedSignal, &SeedsToBob, &BERToBob}, {&Messages_Out} };

};

#endif

/*
#ifndef ALICE_QKD_H_
#define ALICE_QKD_H_

#include "../../../include/netxpto_20180815.h"

#include "./framer_20190326.h"
#include "./message_processor_receiver_20190410.h"
#include "./message_processor_transmitter_20190410.h"
#include "./basis_reconciliation_20190410.h"
#include "./error_correction_20190410.h"
#include "./privacy_amplification_20190410.h"
#include "./alice_incoming_data_processor_20190410.h"
#include "./parameter_estimation_20190528.h"


class DvQkdTx : public SuperBlock {

public:

	// #####################################################################################################

	DvQkdTx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ 8192 }
	{
		setLogFileName("SuperBlock_AliceQKD.txt");
	};

	DvQkdTx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, string sFolderName) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ 8192 }
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_AliceQKD.txt");
	};

	DvQkdTx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce }
	{
		setLogFileName("SuperBlock_AliceQKD.txt");
	};

	DvQkdTx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, string sFolderName, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce }
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_AliceQKD.txt");
	};

	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };

	//##############################################################################################################

	void setErrorCorrectionPartitionSize(t_integer sz) { ErrorCorrection_.setPartitionSize(sz); }
	void setErrorCorrectionNumberOfPasses(t_integer np) { ErrorCorrection_.setNumberOfPasses(np); }
	void setDoublePartitionSize(bool db) { ErrorCorrection_.setDoublePartitionSize(db); }
	void setMaximumSyndromeSize(t_integer mss) { ErrorCorrection_.setMaximumSyndromeSize(mss); }
	void setMinimumSyndromeSize(t_integer minss) { ErrorCorrection_.setMinimumSyndromeSize(minss); }
	void setDoubleClickNumber(t_integer dcn) { BasisReconciliation_.setDoubleClickNumber(dcn); }
	void setNoClickNumber(t_integer ncn) { BasisReconciliation_.setNoClickNumber(ncn); }
	void setBer(t_integer berValue) { PrivacyAmplification_.setBER(berValue); }
	void setSystematicSecurityParameter(t_integer ssp) { PrivacyAmplification_.setSystematicSecurityParameter(ssp); }
	void setBypassHash(bool bh) { PrivacyAmplification_.setBypassHashing(bh); }
	void setMinimumNumberOfPartitions(t_integer mnp) { ErrorCorrection_.setMinimumNumberOfPartitions(mnp); }

private:

	// #####################################################################################################
	// ################################## Input Parameters #################################################
	// #####################################################################################################

	bool logValue{ true };
	t_integer maximumECSyndromeSize;
	vector< t_message_type> transmitterMsgTypes{ BasisReconciliationMsg, ErrorCorrectionMsg,PrivacyAmplificationSeedsMsg,ParameterEstimationSeedAndBitsMsg };

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	// Input
	Binary Data_In{ "S01_Data_In.sgn" };
	Binary Basis_In{ "S02_Basis_In.sgn" };
	Message Messages_In{ "S03_Messages_In.sgn" };

	// Internal
	Binary DataIn{ "S04_DataIn.sgn" };
	Binary BasisIn{ "S05_BasisIn.sgn" };
	TimeContinuousAmplitudeDiscreteReal BasisFromBob{ "S06_BasisFromBob.sgn" };
	TimeContinuousAmplitudeDiscreteReal BasisToSend{ "S07_BasisToSend.sgn" };
	Binary SiftedKeys{ "S08_SiftedKeys.sgn" };
	Binary ErrorCorrectionIn{ "S08b_ErrorCorrectionIn.sgn" };
	Binary DataRecon{ "S09_DataRecon.sgn" };
	TimeContinuousAmplitudeDiscreteReal ParityFromBob{ "S10_ParityFromBob.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal ParityToSend{ "S11_ParityToSend.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal PermutationsFromBob{ "S12_PermutationsFromBob.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal SeedsToBob{ "S13_SeedsToBob.sgn" };
	TimeContinuousAmplitudeDiscreteReal BERFromBob{ "S13b_BERFromBob.sgn" };
	TimeContinuousAmplitudeDiscreteReal BERToBob{ "S13c_BERToBob.sgn" };
	TimeContinuousAmplitudeDiscreteReal NotARealSignal{ "NotARealSignal.sgn" };
	Binary DataCorr{ "S14_DataCorr.sgn" };

	// Output
	Binary Data_Out{ "S15_Data_Out.sgn" };
	Binary Basis_Out{ "S16_Basis_Out.sgn" };
	Binary AliceTransmissionMode_Out{ "S17_AliceTransmissionMode_Out.sgn" };
	Binary Key_Out{ "S18_Key_Out.sgn" };
	Message Messages_Out{ "S19_Messages_Out.sgn" };

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	AliceIncomingDataProcessor AliceIncomingDataProcessor_{ {&Data_In, &Basis_In }, { &DataIn, &BasisIn, &AliceTransmissionMode_Out, &Data_Out, &Basis_Out } };

	MessageProcessorReceiver MessageProcessorReceiver_{ {&Messages_In}, {&BasisFromBob, &ParityFromBob,&PermutationsFromBob, &NotARealSignal, &BERFromBob} };

	BasisReconciliation BasisReconciliation_{ {&DataIn, &BasisIn, &BasisFromBob}, {&DataRecon, &SiftedKeys, &BasisToSend,} };
	//	ErrorCorrection ErrorCorrection_{ {&DataRecon, &ParityFromBob}, {&DataCorr, &ParityToSend} };

	//	ParameterEstimation ParameterEstimation_{ {&DataRecon, &BERFromBob}, {&ErrorCorrectionIn, &BERToBob} };
	//	BasisReconciliation BasisReconciliation_{ {&DataIn, &BasisIn, &BasisFromBob}, {&Key_Out, &BasisToSend} };

	ErrorCorrection ErrorCorrection_{ {&DataRecon, &ParityFromBob, &PermutationsFromBob}, {&DataCorr, &ParityToSend} };

	PrivacyAmplification PrivacyAmplification_{ {&DataCorr}, {&Key_Out, &SeedsToBob} };

	MessageProcessorTransmitter MessageProcessorTransmitter_{ {&BasisToSend, &ParityToSend, &SeedsToBob, &BERToBob}, {&Messages_Out} };

};

#endif
*/
