#ifndef BOB_QKD_H_
#define BOB_QKD_H_

#include "netxpto_20200819.h"

//#include "../include/framer_20190326.h"
#include "dv_qkd_message_processor_receiver_20200819.h"
#include "dv_qkd_message_processor_transmitter_20200819.h"
#include "dv_qkd_basis_reconciliation_20200819.h"
#include "dv_qkd_error_correction_20200819.h"
#include "dv_qkd_privacy_amplification_20200819.h"
#include "dv_qkd_rx_incoming_data_processor_20200819.h"
#include "dv_qkd_parameter_estimation_20200819.h"
#include "ms_windows_console_output_common_20200819.h"
#include "ms_windows_network_aux_common_20200819.h"
#include "very_big_counter_20200819.h"

class DvQkdRx : public SuperBlock {


public:
	
	DvQkdRx(std::initializer_list<Signal *> inputSignals, std::initializer_list<Signal *> outputSignals) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ 8192 },
		buffSize{ 512 }
		{
			setLogFileName("SuperBlock_DvQkdRx.txt");
		};

	DvQkdRx(std::initializer_list<Signal *> inputSignals, std::initializer_list<Signal *> outputSignals, std::string sFolderName) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ 8192 },
		buffSize{ 512 }
		{
			setSignalsFolderName(sFolderName);
			setLogFileName("SuperBlock_DvQkdRx.txt");
		};

	DvQkdRx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce },
		buffSize{ 512 }
	{
		setLogFileName("SuperBlock_DvQkdRx.txt");
	};

	DvQkdRx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce},
		buffSize{ 512 }
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_DvQkdRx.txt");
	};

	DvQkdRx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName, t_integer maxSamplesToProcessAtOnce, unsigned long int buffersSize) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce },
		buffSize{ buffersSize }
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_DvQkdRx.txt");
	};


	void initialize(void);
	bool runBlock(void);


	// #####################################################################################################
	// ########################################## Public Methods ###########################################
	// #####################################################################################################
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
	void setConfidenceInterval(double ci) { ParameterEstimation_.setConfidence(ci); }
	void setIpAddress(std::string ip) { ipAddress = ip; }
	std::string getIpAddress(void) { return ipAddress; }

private:

	// Input Parameters #########################################################################################

	std::string signalsFolderName{ "signals/SuperBlock_DvQkdRx" };
	bool logValue{ true };
	std::string logFileName{ "SuperBlock_DvQkdRx.txt" };

	bool firstTime{ true };

	unsigned long int buffSize;
	t_integer maximumECSyndromeSize;
	
	int dispBitsInterval{ 50000 };
	int dispBitsTracker{ 0 };
	double receivedQubits{ 0 };
	double keyBits{ 0 };
	
	double qberUB{ 0 };
	double previousQberUB{ 0 };

	std::string reportFileName{ "bob_KeysReport.txt" };
	bool toPrint{ true };
	std::string ipAddress{ "running locally" };

	VeryBigCounter counterReceivedQubits;
	VeryBigCounter counterKeyBits;

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	Signal::t_write_mode sWriteMode = Signal::t_write_mode::Ascii;
	Signal::t_header_type hType = Signal::t_header_type::noHeader;

	TimeContinuousAmplitudeDiscreteReal BER_In{ "S0_BER_In.sgn", buffSize, hType, sWriteMode };
	Binary Data_In{ "S1_Data_In.sgn", buffSize, hType, sWriteMode };
	Binary Basis_In{ "S2_Basis_In.sgn", buffSize, hType, sWriteMode };
	Binary TransmissionMode_In{ "S3_TransmissionMode_In.sgn", buffSize,  hType, sWriteMode };
	Message Messages_In{ "S4_Messages_In.sgn", buffSize,  hType, sWriteMode };

	// Internal
	Binary DataIn{ "S5_DataIn.sgn", buffSize,  hType, sWriteMode };
	Binary BasisIn{ "S6_DataIn.sgn", buffSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal BasisFromAlice{ "S7_BasisFromAlice.sgn", buffSize,  hType, sWriteMode };
	Binary SiftedKeys{ "S8_DataRecon.sgn", buffSize+1,  hType, sWriteMode };
	Binary DataRecon{ "S9_DataRecon.sgn", buffSize+1,  hType, sWriteMode };
	Binary Error_Correction_Data_In{ "S10_Error_Correction_Data_In.sgn", buffSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal BasisToSend{ "S11_BasisToSend.sgn", buffSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal ParityFromAlice{ "S12_ParityFromAlice.sgn",maximumECSyndromeSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal ParityToSend{ "S13_ParityToSend.sgn",maximumECSyndromeSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal PermutationsToSend{ "S14_PermutationsToSend.sgn",maximumECSyndromeSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal SeedsFromAlice{ "S15_SeedsFromAlice.sgn", buffSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal BERFromAlice{ "S16_BERFromAlice.sgn",maximumECSyndromeSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal BERToAlice{ "S17_BERToAlice.sgn",maximumECSyndromeSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal BERToEC{ "S18_BERToEC.sgn", buffSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal BERToPA{ "S19_BERToPA.sgn", buffSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal UnusedSignal{ "S_20_UnusedSignal.sgn", buffSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal ToggleBERChange{ "S_21_ToggleBERChange.sgn", buffSize,  hType, sWriteMode };
	Binary DataCorr{ "S22_DataCorr.sgn", buffSize,  hType, sWriteMode };

	// Output
	Binary Key_Out{ "S23_Key_Out.sgn", buffSize,  hType, sWriteMode };
	Message Messages_Out{ "S24_Messages_Out.sgn", buffSize,  hType, sWriteMode };
	Binary Basis_Out{ "S25_Basis_Out.sgn", buffSize,  hType, sWriteMode };
	//Binary TransmissionMode_Out{"S16_TransmissionMode_Out.sgn"};

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	DvQkdRxIncomingDataProcessor BobIncomingDataProcessor_{ {&Data_In, &Basis_In }, { &DataIn, &BasisIn} };

	MessageProcessorReceiver MessageProcessorReceiver_{ {&Messages_In}, {&BasisFromAlice, &ParityFromAlice, &UnusedSignal, &SeedsFromAlice, &BERFromAlice} };

	BasisReconciliation BasisReconciliation_{ {&DataIn, &BasisIn, &BasisFromAlice}, {&DataRecon, &SiftedKeys,  &BasisToSend} };

//	ParameterEstimation ParameterEstimation_{ {&DataRecon, &BERFromAlice}, {&Error_Correction_Data_In, &BERToAlice, &BERToEC, &BERToPA} };
	ParameterEstimation ParameterEstimation_{ {&DataRecon, &BERFromAlice}, {&Error_Correction_Data_In, &BERToAlice, &BERToEC}, signalsFolderName };

	ErrorCorrection ErrorCorrection_{ {&Error_Correction_Data_In, &ParityFromAlice, &BERToEC}, {&DataCorr, &ParityToSend, &PermutationsToSend, &ToggleBERChange} };

	PrivacyAmplification PrivacyAmplification_{ {&DataCorr, &SeedsFromAlice,&BERToPA}, {&Key_Out} };

	MessageProcessorTransmitter MessageProcessorTransmitter_{ {&BasisToSend, &ParityToSend, &PermutationsToSend, &UnusedSignal, &BERToAlice, &ToggleBERChange}, {&Messages_Out} };

};

#endif

/*
#ifndef BOB_QKD_H_
#define BOB_QKD_H_

#include "../../../include/netxpto_20180815.h"

//#include "../include/framer_20190326.h"
#include "./message_processor_receiver_20190410.h"
#include "./message_processor_transmitter_20190410.h"
#include "./basis_reconciliation_20190410.h"
#include "./error_correction_20190410.h"
#include "./privacy_amplification_20190410.h"
#include "./bob_incoming_data_processor_20190410.h"
#include "./transmission_control_20190410.h"
#include "./parameter_estimation_20190528.h"


class DvQkdRx : public SuperBlock {


public:

	DvQkdRx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ 8192 }
	{
		setLogFileName("SuperBlock_DvQkdRx.txt");
	};

	DvQkdRx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, string sFolderName) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ 8192 }
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_DvQkdRx.txt");
	};

	DvQkdRx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce }
	{
		setLogFileName("SuperBlock_DvQkdRx.txt");
	};

	DvQkdRx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, string sFolderName, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce }
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_DvQkdRx.txt");
	};



	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };

	// #####################################################################################################
	// ########################################## Public Methods ###########################################
	// #####################################################################################################
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

	vector< t_message_type> transmitterMsgTypes{ BasisReconciliationMsg, ErrorCorrectionMsg,ErrorCorrectionPermutationsMsg,ParameterEstimationSeedAndBitsMsg };

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	TimeContinuousAmplitudeDiscreteReal BER_In{ "S01_BER_In.sgn" };
	TimeContinuousAmplitudeDiscreteReal Data_In{ "S02_Data_In.sgn" };
	Binary Basis_In{ "S03_Basis_In.sgn" };
	Binary TransmissionMode_In{ "S04_TransmissionMode_In.sgn" };
	Message Messages_In{ "S05_Messages_In.sgn" };

	// Internal
	TimeContinuousAmplitudeDiscreteReal DataIn{ "S07_DataIn.sgn" };
	Binary BasisIn{ "S08_DataIn.sgn" };
	TimeContinuousAmplitudeDiscreteReal BasisFromAlice{ "S09_BasisFromAlice.sgn" };
	Binary SiftedKeys{ "S10_DataRecon.sgn" };
	Binary DataRecon{ "S11_DataRecon.sgn" };
	Binary Error_Correction_Data_In{ "S11b_ErrorCorrectionIn.sgn" };
	TimeContinuousAmplitudeDiscreteReal BasisToSend{ "S12_BasisToSend.sgn" };
	TimeContinuousAmplitudeDiscreteReal ParityFromAlice{ "S13_ParityFromAlice.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal ParityToSend{ "S14_ParityToSend.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal PermutationsToSend{ "S15_PermutationsToSend.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal SeedsFromAlice{ "S16_SeedsFromAlice.sgn" };
	TimeContinuousAmplitudeDiscreteReal BERFromAlice{ "S16b_BERFromAlice.sgn" };
	TimeContinuousAmplitudeDiscreteReal BERToAlice{ "S16c_BERToAlice.sgn" };
	TimeContinuousAmplitudeDiscreteReal NotARealSignal{ "NotARealSignal.sgn" };
	Binary DataCorr{ "S17_DataCorr.sgn" };

	// Output
	Binary Key_Out{ "S18_Key_Out.sgn" };
	Message Messages_Out{ "S19_Messages_Out.sgn" };
	Binary Basis_Out{ "S20_Basis_Out.sgn" };
	//Binary TransmissionMode_Out{"S16_TransmissionMode_Out.sgn"};

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	DvQkdRxIncomingDataProcessor BobIncomingDataProcessor_{ {&TransmissionMode_In, &Data_In, &Basis_In }, { &DataIn, &BasisIn} };

	MessageProcessorReceiver MessageProcessorReceiver_{ {&Messages_In}, {&BasisFromAlice, &ParityFromAlice, &NotARealSignal,&SeedsFromAlice, &BERFromAlice} };

	BasisReconciliation BasisReconciliation_{ {&DataIn, &BasisIn, &BasisFromAlice}, {&DataRecon, &SiftedKeys, &BasisToSend} };

	//ParameterEstimation ParameterEstimation_{ {&DataRecon, &BERFromAlice}, {&Error_Correction_Data_In, &BERToAlice} };

	ErrorCorrection ErrorCorrection_{ {&DataRecon, &ParityFromAlice}, {&DataCorr, &ParityToSend, &PermutationsToSend} };

	//	BasisReconciliation BasisReconciliation_{ {&DataIn, &BasisIn, &BasisFromAlice}, {&Key_Out, &BasisToSend} };

	//	ErrorCorrection ErrorCorrection_{ {&DataRecon, &ParityFromAlice}, {&DataCorr, &ParityFromAlice} };

	PrivacyAmplification PrivacyAmplification_{ {&DataCorr, &SeedsFromAlice}, {&Key_Out} };

	MessageProcessorTransmitter MessageProcessorTransmitter_{ {&BasisToSend, &ParityToSend, &PermutationsToSend, &BERToAlice}, {&Messages_Out} };








};

#endif
*/
