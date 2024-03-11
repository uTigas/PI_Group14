#ifndef DV_QKD_RX_LDPC_H_
#define DV_QKD_RX_LDPC_H_

#include "netxpto_20200819.h"

#include "fork_20200819.h"
#include "dv_qkd_ldpc_rx_message_processor_receiver_20200819.h"
#include "dv_qkd_ldpc_rx_message_processor_transmitter_20200819.h"
#include "dv_qkd_rx_bases_reconciliation_20200819.h"
#include "dv_qkd_ldpc_rx_error_correction_20200819.h"
#include "dv_qkd_privacy_amplification_20200819.h"
//#include "dv_qkd_rx_incoming_data_processor_20200819.h"
#include "dv_qkd_ldpc_rx_parameter_estimation_20200819.h"
#include "ms_windows_console_output_common_20200819.h"
#include "ms_windows_network_aux_common_20200819.h"
#include "very_big_counter_20200819.h"

class DvQkdRx_LDPC : public SuperBlock {


public:
	
	DvQkdRx_LDPC(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals);
	DvQkdRx_LDPC(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName);
	DvQkdRx_LDPC(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, t_integer maxSamplesToProcessAtOnce);
	DvQkdRx_LDPC(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName, t_integer maxSamplesToProcessAtOnce);
	DvQkdRx_LDPC(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName, t_integer maxSamplesToProcessAtOnce, unsigned long int buffersSize);

	void initialize(void);
	bool runBlock(void);


	// #####################################################################################################
	
	/*
	void setErrorCorrectionMethod(t_string eCorrectionMethod) { ErrorCorrection_.setErrorCorrectionMethod(eCorrectionMethod == "LDPC" ? ErrorCorrection::t_method::LDPC : ErrorCorrection::t_method::Cascade); }
	void setErrorCorrectionPartitionSize(t_integer sz) { ErrorCorrection_.setPartitionSize(sz); }
	void setErrorCorrectionNumberOfPasses(t_integer np) { ErrorCorrection_.setNumberOfPasses(np); }
	void setDoublePartitionSize(bool db) { ErrorCorrection_.setDoublePartitionSize(db); }
	void setMaximumSyndromeSize(t_integer mss) { ErrorCorrection_.setMaximumSyndromeSize(mss); }
	void setMinimumSyndromeSize(t_integer minss) { ErrorCorrection_.setMinimumSyndromeSize(minss); }
	*/
	//void setBer(t_real berValue) { PrivacyAmplification_.setBER(berValue); ErrorCorrection_.setBer(berValue); }
	void setSystematicSecurityParameter(t_integer ssp) { PrivacyAmplification_.setSystematicSecurityParameter(ssp); }
	void setBypassPrivacyAmplification(bool bh) { PrivacyAmplification_.setBypassPrivacyAmplification(bh); }
	//void setMinimumNumberOfPartitions(t_integer mnp) { ErrorCorrection_.setMinimumNumberOfPartitions(mnp); }
	void setBypassParameterEstimation(t_bool bParameterEstimation) { DvQkdLdpcRxParameterEstimation_.setBypassParameterEstimation(bParameterEstimation); };
	void setParameterEstimationNumberBits(t_integer nb) { DvQkdLdpcRxParameterEstimation_.setNumberOfBitsPerEstimationBlock(nb); }

	// DvQkdRx_ErrorCorrection

	void setBypassErrorCorrection(t_bool bErrorCorrection) { DvQkdLdpcRxErrorCorrection_.setBypassErrorCorrection(bErrorCorrection); };

	void setParityCheckMatrixFileName(t_string pCheckMatrixFileName) { parityCheckMatrixFileName = pCheckMatrixFileName; }
	t_string getParityCheckMatrixFileName(void) { return parityCheckMatrixFileName; }

	void setParityCheckMatrixFolderName(t_string pCheckMatrixFileName) { parityCheckMatrixFolderName = pCheckMatrixFileName; }
	t_string getParityCheckMatrixFolderName(void) { return parityCheckMatrixFolderName; }

	void setLdpcErrorCorrectionAlgorithMaxIterations(t_integer lErrorCorrectionAlgorithMaxIterations) { DvQkdLdpcRxErrorCorrection_.setLdpcErrorCorrectionAlgorithMaxIterations(lErrorCorrectionAlgorithMaxIterations); };
	t_integer getLdpcErrorCorrectionAlgorithMaxIterations(t_integer lErrorCorrectionAlgorithMaxIterations) const { DvQkdLdpcRxErrorCorrection_.getLdpcErrorCorrectionAlgorithMaxIterations(); };

	void setCodeLength(t_integer cLength) { DvQkdLdpcRxErrorCorrection_.setCodeLength(cLength); };
	t_integer getCodeLength(void) const { DvQkdLdpcRxErrorCorrection_.getCodeLength(); };

	void setCodeRank(t_integer cRank) { DvQkdLdpcRxErrorCorrection_.setCodeRank(cRank); };
	t_integer getCodeRank(void) const { DvQkdLdpcRxErrorCorrection_.getCodeRank(); };

	void setNumberOfNonZerosValuesInSparseParityCheckMatrix(t_integer nOfNonZerosValuesInSparseParityCheckMatrix) { DvQkdLdpcRxErrorCorrection_.setNumberOfNonZerosValuesInSparseParityCheckMatrix(nOfNonZerosValuesInSparseParityCheckMatrix); };
	t_integer getNumberOfNonZerosValuesInSparseParityCheckMatrix(void) const { DvQkdLdpcRxErrorCorrection_.getNumberOfNonZerosValuesInSparseParityCheckMatrix(); };

	void setNumberOfDataBitsPerBitFillingPeriod(t_integer nOfDataBitsPerBitFillingPeriod) { DvQkdLdpcRxErrorCorrection_.setNumberOfDataBitsPerBitFillingPeriod(nOfDataBitsPerBitFillingPeriod); }
	t_integer getNumberOfDataBitsPerBitFillingPeriod() const { return DvQkdLdpcRxErrorCorrection_.getNumberOfDataBitsPerBitFillingPeriod(); }

	void setNumberOfDummyBitsPerBitFillingPeriod(t_integer nOfDummyBitsPerBitFillingPeriod) { DvQkdLdpcRxErrorCorrection_.setNumberOfDummyBitsPerBitFillingPeriod(nOfDummyBitsPerBitFillingPeriod); }
	t_integer getNumberOfDummyBitsPerBitFillingPeriod() const { return DvQkdLdpcRxErrorCorrection_.getNumberOfDummyBitsPerBitFillingPeriod(); }

	//void setParameterEstimationNumberBitsToStart(t_integer nbs) { DvQkdLdpcRxParameterEstimation_.setNumberOfBitsRequiredToStart(nbs); }
	void setPrint(bool print) { toPrint = print; }
	void setQberThreshold(const t_real th) { DvQkdLdpcRxParameterEstimation_.setThreshold(th); }
	void setConfidenceInterval(double ci) { DvQkdLdpcRxParameterEstimation_.setConfidence(ci); }
	void setIpAddress(std::string ip) { ipAddress = ip; }
	std::string getIpAddress(void) { return ipAddress; }

private:

	// Input Parameters #########################################################################################

	std::string signalsFolderName{ "signals/SuperBlock_DvQkdRx_LDPC" };

	std::string logFileName{ "SuperBlock_DvQkdRx_LDPC.txt" };

	bool firstTime{ true };

	unsigned long int buffSize;
	int codeLength{ 1944 };
	int codeRank{ 972 };
	int sindromeSize{ codeLength - codeRank };
	t_integer maximumECSyndromeSize;
	
	t_string parityCheckMatrixFolderName{ "." };
	t_string parityCheckMatrixFileName{ "spaceParityCheckMatrixFileName.txt" };

	int dispBitsInterval{ 50000 };
	int dispBitsTracker{ 0 };
	double receivedQubits{ 0 };
	double keyBits{ 0 };
	
	double qberUB{ 0 };
	double previousQberUB{ 0 };

	std::string reportFileName{ "bob_KeysReport.txt" };
	bool toPrint{ false };
	std::string ipAddress{ "running locally" };

	VeryBigCounter counterReceivedQubits;
	VeryBigCounter counterKeyBits;

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	Signal::t_write_mode sWriteMode = Signal::t_write_mode::Ascii;
	Signal::t_header_type hType = Signal::t_header_type::noHeader;

	Binary BasesIn{ "S0_BasesIn.sgn", buffSize, hType, sWriteMode }; // input signal 0

	Binary ForkOut_BasesIn_0{ "S1_ForkOut_BasesIn.sgn", buffSize, hType, sWriteMode };

	Binary BasesOut{ "S2_ForkOut_BasesOut.sgn", buffSize, hType, sWriteMode }; // output signal 0

	Binary DataIn{ "S3_DataIn.sgn", buffSize, hType, sWriteMode }; // input signal 1

	Binary BasesToTx{ "S4_BasesReconciliation_BasesToTx.sgn", buffSize,  hType, sWriteMode };

	Message MessagesToTx{ "S5_MessagesToTx.sgn", 10,  hType, sWriteMode }; // output signal 1

	Message MessagesFromTx{ "S6_MessagesFromTx.sgn", 10,  hType, sWriteMode }; // input signal 2

	Binary BasesAckFromTx{ "S7_BasesAckFromTx.sgn", buffSize,  hType, sWriteMode };

	Binary DvQkdLdpcRxParameterEstimation_DataFromTx{ "S10_DvQkdLdpcRxParameterEstimation_DataFromTx.sgn", 100, hType, sWriteMode };

	Binary SiftedKeys_0{ "S8_BasesReconciliation_SiftedKey_0.sgn", buffSize,  hType, sWriteMode };
	Binary SiftedKeysOut{ "S9_BasesReconciliation_SiftedKeyOut.sgn", buffSize,  hType, sWriteMode }; // output signal 2

	Binary DvQkdLdpcRxParameterEstimation_DataOut{ "S10_ParameterEstimation_DataOut.sgn", buffSize,  hType, sWriteMode };
	TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcRxParameterEstimation_SeedFromTx{ "S11_DvQkdLdpcRxParameterEstimation_SeedFromTx.sgn", 1, hType, sWriteMode };
	TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcRxParameterEstimation_RatioFromTx{ "S11_DvQkdLdpcRxParameterEstimation_RatioFromTx.sgn", 1, hType, sWriteMode };
	TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcRxParameterEstimation_NumberOfBitsPerEstimationBlockFromTx{ "S12_DvQkdLdpcRxParameterEstimation_NumberOfBitsPerEstimationBlockFromTx.sgn", 1, hType, sWriteMode };

	TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcRxParameterEstimation_SeedToTx{ "S13_DvQkdLdpcRxParameterEstimation_SeedToTx.sgn", 1, hType, sWriteMode };
	TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcRxParameterEstimation_RatioToTx{ "S14_DvQkdLdpcRxParameterEstimation_RatioToTx.sgn", 1, hType, sWriteMode };
	TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcRxParameterEstimation_NumberOfBitsPerEstimationBlockToTx{ "S15_DvQkdLdpcRxParameterEstimation_NumberOfBitsPerEstimationBlockToTx.sgn", 1, hType, sWriteMode };



	Binary SindromeToTx{ "S11_SindromeToTx.sgn", sindromeSize, hType, sWriteMode };

	Binary SindromeFromTx{ "S12_SindromeFromTx.sgn", sindromeSize,  hType, sWriteMode };

	Binary SymmetricKeysOut{ "S13_SymmetricKeyOut.sgn", codeLength,  hType, sWriteMode }; // output signal 3

//	Binary TransmissionMode_In{ "S3_TransmissionMode_In.sgn", buffSize,  hType, sWriteMode };
	
	TimeContinuousAmplitudeDiscreteReal BER_In{ "S0_BER_In.sgn", buffSize, hType, sWriteMode };

	// Internal
	//Binary DataIn{ "S5_DataIn.sgn", buffSize,  hType, sWriteMode };
	//Binary BasesIn{ "S6_DataIn.sgn", buffSize,  hType, sWriteMode };
	
	
	


	TimeContinuousAmplitudeDiscreteReal PermutationsToSend{ "S14_PermutationsToSend.sgn",maximumECSyndromeSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal SeedsFromTx{ "S15_SeedsFromTx.sgn", buffSize,  hType, sWriteMode };
	Binary DvQkdLdpcRxParameterEstimation_DataToTx{ "S17_BERToTx.sgn", 100,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal DvQkdLdpcRxParameterEstimation_EstimatedQBER{ "S18_BERToErrorCorrection.sgn", 2,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal BERToPA{ "S19_BERToPA.sgn", buffSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal UnusedSignal{ "S_20_UnusedSignal.sgn", buffSize,  hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal ToggleBERChange{ "S_21_ToggleBERChange.sgn", buffSize,  hType, sWriteMode };
	Binary DataCorr{ "S22_DataCorr.sgn", buffSize,  hType, sWriteMode };

	// Output
	//Binary Key_Out{ "S23_Key_Out.sgn", buffSize,  hType, sWriteMode };
	//Message Messages_Out{ "S24_Messages_Out.sgn", 1,  hType, sWriteMode };
	//Binary Bases_Out{ "S25_Bases_Out.sgn", buffSize,  hType, sWriteMode };
	//Binary TransmissionMode_Out{"S16_TransmissionMode_Out.sgn"};

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	Fork Fork_{ { &BasesIn }, {&ForkOut_BasesIn_0, &BasesOut} };

	DvQkdLdpcRxMessageProcessorReceiver DvQkdLdpcRxMessageProcessorReceiver_{ {&MessagesFromTx}, {&BasesAckFromTx, &DvQkdLdpcRxParameterEstimation_SeedFromTx, &DvQkdLdpcRxParameterEstimation_RatioFromTx, &DvQkdLdpcRxParameterEstimation_NumberOfBitsPerEstimationBlockFromTx, &DvQkdLdpcRxParameterEstimation_DataFromTx, &SindromeFromTx} };

	DvQkdRxBasesReconciliation BasesReconciliation_{ {&DataIn, &ForkOut_BasesIn_0, &BasesAckFromTx}, {&BasesToTx, &SiftedKeys_0, &SiftedKeysOut} };

	DvQkdLdpcRxParameterEstimation DvQkdLdpcRxParameterEstimation_{ {&SiftedKeys_0, &DvQkdLdpcRxParameterEstimation_SeedFromTx, &DvQkdLdpcRxParameterEstimation_RatioFromTx , &DvQkdLdpcRxParameterEstimation_NumberOfBitsPerEstimationBlockFromTx, &DvQkdLdpcRxParameterEstimation_DataFromTx}, {&DvQkdLdpcRxParameterEstimation_DataOut, &DvQkdLdpcRxParameterEstimation_SeedToTx, &DvQkdLdpcRxParameterEstimation_RatioToTx, &DvQkdLdpcRxParameterEstimation_NumberOfBitsPerEstimationBlockToTx, &DvQkdLdpcRxParameterEstimation_DataToTx, &DvQkdLdpcRxParameterEstimation_EstimatedQBER}, signalsFolderName, "DvQkdRx_parameter_estimation_report.txt", "DVQkdRx_parameter_estimation_berList.txt" };
	
	DvQkdLdpcRxErrorCorrection DvQkdLdpcRxErrorCorrection_{ {&DvQkdLdpcRxParameterEstimation_DataOut, &SindromeFromTx, &DvQkdLdpcRxParameterEstimation_EstimatedQBER}, {&DataCorr, &SindromeToTx, &PermutationsToSend, &ToggleBERChange} };

	PrivacyAmplification PrivacyAmplification_{ {&DataCorr, &SeedsFromTx,&BERToPA}, {&SymmetricKeysOut} };

	DvQkdLdpcRxMessageProcessorTransmitter DvQkdLdpcRxMessageProcessorTransmitter_{ {&BasesToTx, &DvQkdLdpcRxParameterEstimation_SeedToTx, &DvQkdLdpcRxParameterEstimation_RatioToTx, &DvQkdLdpcRxParameterEstimation_NumberOfBitsPerEstimationBlockToTx, &DvQkdLdpcRxParameterEstimation_DataToTx, &SindromeToTx, &PermutationsToSend, &ToggleBERChange}, {&MessagesToTx} };

};

#endif
