#ifndef CV_QOKD_TX_H_
#define CV_QOKD_TX_H_

#include "netxpto_20200819.h"

#include "dv_qkd_ldpc_tx_message_processor_receiver_20200819.h"
#include "dv_qkd_ldpc_tx_message_processor_transmitter_20200819.h"
#include "dv_qkd_tx_bases_reconciliation_20200819.h"
#include "cv_qokd_ldpc_tx_sindrome_reconciliation_20200819.h"
#include "dv_qkd_privacy_amplification_20200819.h"
#include "fork_20200819.h"
#include "dv_qkd_ldpc_tx_parameter_estimation_20200819.h"
#include "ms_windows_console_output_common_20200819.h"
#include "ms_windows_network_aux_common_20200819.h"
#include "very_big_counter_20200819.h"

class DvQkd_LDPC_Tx  : public SuperBlock {
	
public:

	// #####################################################################################################

	DvQkd_LDPC_Tx(std::initializer_list<Signal *> inputSignals, std::initializer_list<Signal *> outputSignals);
	DvQkd_LDPC_Tx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName);
	DvQkd_LDPC_Tx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, t_integer maxSamplesToProcessAtOnce);
	DvQkd_LDPC_Tx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName, t_integer maxSamplesToProcessAtOnce);
	DvQkd_LDPC_Tx(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals, std::string sFolderName, t_integer maxSamplesToProcessAtOnce, unsigned long int buffersSize);

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	// Fork


	// DvQkdTx_BasisReconciliation


	// DvQkdTx_ParameterEstimation
	void setBypassParameterEstimation(t_bool bParameterEstimation) { DvQkdLdpcTxParameterEstimation_.setBypassParameterEstimation(bParameterEstimation); };
	void setParameterEstimationNumberBits(t_integer nb) { DvQkdLdpcTxParameterEstimation_.setNumberOfBitsPerEstimationBlock(nb); }
	//void setParameterEstimationNumberBitsToStart(t_integer nbs) { DvQkdLdpcTxParameterEstimation_.setNumberOfBitsRequiredToStart(nbs); }
	void setQberThreshold(const t_real th) { DvQkdLdpcTxParameterEstimation_.setThreshold(th); }
	void setConfidenceInterval(double ci) { DvQkdLdpcTxParameterEstimation_.setConfidence(ci); }

	// DvQkdTx_ErrorCorrection
	void setBypassErrorCorrection(t_bool bErrorCorrection) { CvQokdLdpcTxSindromeReconciliation_.setBypassErrorCorrection(bErrorCorrection); };

	void setParityCheckMatrixFileName(t_string pCheckMatrixFileName) { parityCheckMatrixFileName = pCheckMatrixFileName; }
	t_string getParityCheckMatrixFileName(void) { return parityCheckMatrixFileName; }

	void setParityCheckMatrixFolderName(t_string pCheckMatrixFileName) { parityCheckMatrixFolderName = pCheckMatrixFileName; }
	t_string getParityCheckMatrixFolderName(void) { return parityCheckMatrixFolderName; }
	
	void setLdpcErrorCorrectionAlgorithMaxIterations(t_integer lErrorCorrectionAlgorithMaxIterations) { CvQokdLdpcTxSindromeReconciliation_.setLdpcErrorCorrectionAlgorithMaxIterations(lErrorCorrectionAlgorithMaxIterations); };
	t_integer getLdpcErrorCorrectionAlgorithMaxIterations(t_integer lErrorCorrectionAlgorithMaxIterations) const { CvQokdLdpcTxSindromeReconciliation_.getLdpcErrorCorrectionAlgorithMaxIterations(); };

	void setCodeLength(t_integer cLength) { CvQokdLdpcTxSindromeReconciliation_.setCodeLength(cLength); };
	t_integer getCodeLength(void) const { CvQokdLdpcTxSindromeReconciliation_.getCodeLength(); };

	void setCodeRank(t_integer cRank) { CvQokdLdpcTxSindromeReconciliation_.setCodeRank(cRank); };
	t_integer getCodeRank(void) const { CvQokdLdpcTxSindromeReconciliation_.getCodeRank(); };

	void setNumberOfNonZerosValuesInSparseParityCheckMatrix(t_integer nOfNonZerosValuesInSparseParityCheckMatrix) { CvQokdLdpcTxSindromeReconciliation_.setNumberOfNonZerosValuesInSparseParityCheckMatrix(nOfNonZerosValuesInSparseParityCheckMatrix); };
	t_integer getNumberOfNonZerosValuesInSparseParityCheckMatrix(void) const { CvQokdLdpcTxSindromeReconciliation_.getNumberOfNonZerosValuesInSparseParityCheckMatrix(); };

	void setNumberOfDataBitsPerBitFillingPeriod(t_integer nOfDataBitsPerBitFillingPeriod) { CvQokdLdpcTxSindromeReconciliation_.setNumberOfDataBitsPerBitFillingPeriod(nOfDataBitsPerBitFillingPeriod); }
	t_integer getNumberOfDataBitsPerBitFillingPeriod() const { return CvQokdLdpcTxSindromeReconciliation_.getNumberOfDataBitsPerBitFillingPeriod(); }
	
	void setNumberOfDummyBitsPerBitFillingPeriod(t_integer nOfDummyBitsPerBitFillingPeriod) { CvQokdLdpcTxSindromeReconciliation_.setNumberOfDummyBitsPerBitFillingPeriod(nOfDummyBitsPerBitFillingPeriod); }
	t_integer getNumberOfDummyBitsPerBitFillingPeriod() const { return CvQokdLdpcTxSindromeReconciliation_.getNumberOfDummyBitsPerBitFillingPeriod(); }

	// DvQkdTx_PrivacyAmplification
	void setSystematicSecurityParameter(t_integer ssp) { PrivacyAmplification_.setSystematicSecurityParameter(ssp); }
	void setBypassPrivacyAmplification(bool bh) { PrivacyAmplification_.setBypassPrivacyAmplification(bh); }

	void setPrint(bool print) { toPrint = print; }

	void setPhotonsPerPulse(double ppp) { photonsPerPulse = ppp; }
	double getPhotonsPerPulse() { return (photonsPerPulse); }

	void setIpAddress(std::string ip) { ipAddress = ip; }
	std::string getIpAddress(void) { return ipAddress; }

private:

	// Input Parameters #########################################################################################

	std::string signalsFolderName{ "signals/SuperBlock_DvQkdTx_LDPC" };

	std::string logFileName{ "SuperBlock_DvQkdTx_LDPC.txt" };
		
	bool firstTime{ true };

	unsigned long int buffSize;

	t_string parityCheckMatrixFolderName{ "." };
	t_string parityCheckMatrixFileName{ "spaceParityCheckMatrixFileName.txt" };

	int codeLength{ 1944 };
	t_integer maximumECSyndromeSize;

	int dispBitsInterval{ 4096 };
	int dispBitsTracker{ 0 };
	//std::string folderName;
	double sentQubits{ 0 };
	double keyBits{ 0 };

	double qberUB{ 0 };
	double previousQberUB{ 0 };

	std::string reportFileName{ "alice_KeysReport.txt" };
	bool toPrint{ false };
	std::string ipAddress{ "running locally" };
	VeryBigCounter counterSentQubits;
	VeryBigCounter counterKeyBits;
	double photonsPerPulse{ 0.1 };

	// ##########################################################################################################

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################
	Signal::t_write_mode sWriteMode = Signal::t_write_mode::Ascii;
	Signal::t_header_type hType = Signal::t_header_type::noHeader;

	Binary DataIn{ "S0_DataIn.sgn", buffSize, hType, sWriteMode };
	Binary BasesIn{ "S1_BasesIn.sgn", buffSize, hType, sWriteMode };

	Binary ForkOut_DataIn_0{ "S2_ForkOut_DataIn_0.sgn", buffSize, hType, sWriteMode };
	Binary ForkOut_BasesIn_0{ "S3_ForkOut_BasesIn_0.sgn", buffSize, hType, sWriteMode };

	Message MessagesFromRx{ "S4_MessagesFromRx.sgn", 10, hType, sWriteMode };

	Binary BasesFromRx{ "S5_BasesFromRx.sgn", buffSize, hType, sWriteMode };

	Binary BasesAckToRx{ "S6_BasesAckToRx.sgn", buffSize, hType, sWriteMode };
	Binary SiftedKeys{ "S7_SiftedKeys.sgn", buffSize, hType, sWriteMode };
	Binary SiftedKeys_Out{ "S8_SiftedKeys_Out.sgn", buffSize, hType, sWriteMode };
	
	Binary DataFromRx{ "S9_DataFromRx.sgn",maximumECSyndromeSize, hType, sWriteMode };

	Binary DvQkdLdpcTxParameterEstimation_DataOut{ "S10_DvQkdLdpcTxParameterEstimation_DataOut.sgn", buffSize, hType, sWriteMode };
	TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcTxParameterEstimation_SeedToRx{ "S11_DvQkdLdpcTxParameterEstimation_SeedToRx.sgn", 1, hType, sWriteMode };
	TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcTxParameterEstimation_RatioToRx{ "S11_DvQkdLdpcTxParameterEstimation_RatioToRx.sgn", 1, hType, sWriteMode };
	TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlock{ "S12_DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlock.sgn", 1, hType, sWriteMode };
	Binary DvQkdLdpcTxParameterEstimation_DataToRx{ "S13_DvQkdLdpcTxParameterEstimation_DataToRx.sgn",100, hType, sWriteMode };

	TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcTxParameterEstimation_SeedFromRx{ "S14_DvQkdLdpcRxParameterEstimation_SeedFromRx.sgn", 1, hType, sWriteMode };
	TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcTxParameterEstimation_RatioFromRx{ "S15_DvQkdLdpcRxParameterEstimation_RatioFromRx.sgn", 1, hType, sWriteMode };
	TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlockFromRx{ "S16_DvQkdLdpcRxParameterEstimation_NumberOfBitsPerEstimationBlockFromRx.sgn", 1, hType, sWriteMode };

	Binary DvQkdLdpcTxParameterEstimation_DataFromRx{ "S17_BERToTx.sgn", 100,  hType, sWriteMode };

	Binary SindromeFromRx{ "S11_SindromeFromRx.sgn", maximumECSyndromeSize, hType, sWriteMode };

	Binary DataOut{ "S18_DataOut.sgn", buffSize, hType, sWriteMode };
	Binary BasesOut{ "S19_BasesOut.sgn", buffSize, hType, sWriteMode };

	// Internal

	TimeContinuousAmplitudeDiscreteReal ParityToSend{ "S11_ParityToSend.sgn",maximumECSyndromeSize, hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal PermutationsFromRx{ "S12_PermutationsFromRx.sgn",maximumECSyndromeSize, hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal SeedsToBob{ "S13_SeedsToBob.sgn", buffSize, hType, sWriteMode };
	
	TimeContinuousAmplitudeDiscreteReal DvQkdLdpcTxParameterEstimation_EstimatedQBER{ "S16_BERToErrorCorrection.sgn", 2, hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal BERToPA{ "S17_BERToPA.sgn", buffSize, hType, sWriteMode };
	TimeContinuousAmplitudeDiscreteReal ToggleBERChange{ "ToggleBERChange.sgn", buffSize, hType, sWriteMode };

	Binary DataCorr{ "S14_DataCorr.sgn", buffSize, hType, sWriteMode };

	// Output

	Binary AliceTransmissionMode_Out{ "S20_AliceTransmissionMode_Out.sgn", buffSize, hType, sWriteMode };

	Binary SymmetricKeys_Out{ "S21_Key_Out.sgn", buffSize, hType, sWriteMode };

	Message Messages_Out{ "S22_Messages_Out.sgn", 10, hType, sWriteMode };

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	Fork Fork_{{&DataIn, &BasesIn }, { &ForkOut_DataIn_0, &ForkOut_BasesIn_0, &DataOut, &BasesOut }};

	DvQkdLdpcTxMessageProcessorReceiver DvQkdLdpcTxMessageProcessorReceiver_{ {&MessagesFromRx}, {&BasesFromRx, &DvQkdLdpcTxParameterEstimation_SeedFromRx, &DvQkdLdpcTxParameterEstimation_RatioFromRx, &DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlockFromRx, &DvQkdLdpcTxParameterEstimation_DataFromRx, &SindromeFromRx, &PermutationsFromRx, &ToggleBERChange} };

	DvQkdTxBasesReconciliation DvQkdTxBasesReconciliation_{ {&ForkOut_DataIn_0, &ForkOut_BasesIn_0, &BasesFromRx}, {&BasesAckToRx , &SiftedKeys, &SiftedKeys_Out } };

	DvQkdLdpcTxParameterEstimation DvQkdLdpcTxParameterEstimation_{ {&SiftedKeys, &DvQkdLdpcTxParameterEstimation_SeedFromRx, &DvQkdLdpcTxParameterEstimation_RatioFromRx , &DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlockFromRx, &DvQkdLdpcTxParameterEstimation_DataFromRx}, 	{&DvQkdLdpcTxParameterEstimation_DataOut, &DvQkdLdpcTxParameterEstimation_SeedToRx, &DvQkdLdpcTxParameterEstimation_RatioToRx, &DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlock, &DvQkdLdpcTxParameterEstimation_DataToRx, &DvQkdLdpcTxParameterEstimation_EstimatedQBER}, signalsFolderName, "DvQkdLDPCTx_parameter_estimation_report.txt", "DVQkdLDPCTx_parameter_estimation_berList.txt" };

	CvQokdLdpcTxSindromeReconciliation CvQokdLdpcTxSindromeReconciliation_{ {&DvQkdLdpcTxParameterEstimation_DataOut, &SindromeFromRx, &PermutationsFromRx, &DvQkdLdpcTxParameterEstimation_EstimatedQBER, &ToggleBERChange}, {&DataCorr, &ParityToSend} };

	PrivacyAmplification PrivacyAmplification_{ {&DataCorr,&BERToPA}, {&SymmetricKeys_Out, &SeedsToBob} };

	DvQkdLdpcTxMessageProcessorTransmitter DvQkdLdpcTxMessageProcessorTransmitter_{ {&BasesAckToRx, &DvQkdLdpcTxParameterEstimation_SeedToRx, &DvQkdLdpcTxParameterEstimation_RatioToRx, &DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlock, &DvQkdLdpcTxParameterEstimation_DataToRx,  &ParityToSend}, {&Messages_Out} };

};

#endif

