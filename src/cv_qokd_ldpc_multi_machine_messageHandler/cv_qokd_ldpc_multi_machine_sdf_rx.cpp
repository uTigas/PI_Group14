#include "netxpto_20200819.h"
#include "cv_qokd_ldpc_multi_machine_sdf.h"
#include "load_ascii_20200819.h"
#include "load_etsi_004_20200819.h"
#include "dv_qkd_ldpc_tx_parameter_estimation_20200819.h"
#include "cv_qokd_ldpc_tx_sindrome_reconciliation_20200819.h"
#include "save_ascii_20200819.h"
#include "dv_qkd_ldpc_tx_message_processor_transmitter_20200819.h"
#include "ip_tunnel_ms_windows_20200819.h"
#include "dv_qkd_ldpc_tx_message_processor_receiver_20200819.h"
#include "message_handler.h"

#include "ms_windows_console_output_common_20200819.h"
#include "ms_windows_network_aux_common_20200819.h"
#include "very_big_counter_20200819.h"

#include "dv_qkd_polarization_physical_layer_20200819.h"
#include "cv_qokd_ldpc_rx_20200819.h"


namespace rx
{
	int main(int argc, char* argv[])
	{

		ConsoleOutputCommon::prepareWindowsConsole();
		ConsoleOutputCommon::clrScreen();

		auto start = std::chrono::high_resolution_clock::now();


		// #####################################################################################################
		// ################################ Variables Inicialization ###########################################
		// #####################################################################################################

		DvQkdLdpcInputParameters param = DvQkdLdpcInputParameters();
		param.setInputParametersFileName("input_parameters_rx.txt");
		param.readSystemInputParameters();

		Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii };
		Signal::t_header_type hType{ Signal::t_header_type::noHeader };
		auto buffSize = (t_unsigned_long)param.buffSize;
		t_unsigned_long maximumECSyndromeSize{ 972 };
		t_unsigned_long hashSize{ 20 };

		std::string signalsFolderName{ "signals" };

		// #####################################################################################################
		// ########################### Rx Signals Declaration and Inicialization ###############################
		// #####################################################################################################

		Binary Raw_Rx{ "S0_Tx_Raw_Key_Tx.sgn", buffSize };
		Raw_Rx.setSaveSignal(param.saveSignals, sWriteMode);
		Raw_Rx.setNoHeader(true);

		Binary BasesFromRx_Rx{ "S2_Tx_BasesFromRx.sgn", 1, hType, sWriteMode };

		TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcTxParameterEstimation_SeedFromRx_Rx{ "S11_DvQkdLdpcTxParameterEstimation_SeedToRx.sgn", 1, hType, sWriteMode };
		TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcTxParameterEstimation_RatioFromRx_Rx{ "S11_DvQkdLdpcTxParameterEstimation_RatioToRx.sgn", 1, hType, sWriteMode };
		TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlockFromRx_Rx{ "S12_DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlock.sgn", 1, hType, sWriteMode };
		Binary DvQkdLdpcTxParameterEstimation_DataFromRx_Rx{ "S13_DvQkdLdpcTxParameterEstimation_DataToRx.sgn",100, hType, sWriteMode };

		Binary SindromeFromTx_Rx{ "S11_SindromeFromRx.sgn", maximumECSyndromeSize + hashSize, hType, sWriteMode };

		Binary SindromeToTx_Rx{ "S11_SindromeFromRx.sgn", maximumECSyndromeSize, hType, sWriteMode };
		Message HashToTx_Rx{ "S12_HashToTx_Rx.sgn", 1, hType, sWriteMode };

		Binary BasesAckToRx_Rx{ "S6_BasesAckToRx.sgn", (t_unsigned_long)param.buffSize, hType, sWriteMode };
		Binary SiftedKeys_Rx{ "S7_SiftedKeys.sgn", (t_unsigned_long)param.buffSize, hType, sWriteMode };
		Binary SiftedKeys_Out_Rx{ "S8_SiftedKeys_Out.sgn", (t_unsigned_long)param.buffSize, hType, sWriteMode };

		Binary DataFromRx_Rx{ "S9_DataFromRx.sgn",maximumECSyndromeSize, hType, sWriteMode };

		TimeContinuousAmplitudeDiscreteReal PermutationsFromRx_Rx{ "S12_PermutationsFromRx.sgn",maximumECSyndromeSize, hType, sWriteMode };

		TimeContinuousAmplitudeDiscreteReal ToggleBERChange_Rx{ "ToggleBERChange.sgn", buffSize, hType, sWriteMode };

		Binary DvQkdLdpcTxParameterEstimation_DataOut_Rx{ "S10_DvQkdLdpcTxParameterEstimation_DataOut.sgn", buffSize, hType, sWriteMode };
		TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcTxParameterEstimation_SeedToRx_Rx{ "S11_DvQkdLdpcTxParameterEstimation_SeedToRx.sgn", 1, hType, sWriteMode };
		TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcTxParameterEstimation_RatioToRx_Rx{ "S11_DvQkdLdpcTxParameterEstimation_RatioToRx.sgn", 1, hType, sWriteMode };
		TimeDiscreteAmplitudeDiscreteInteger DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlock_Rx{ "S12_DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlock.sgn", 1, hType, sWriteMode };
		Binary DvQkdLdpcTxParameterEstimation_DataToRx_Rx{ "S13_DvQkdLdpcTxParameterEstimation_DataToRx.sgn",100, hType, sWriteMode };

		TimeContinuousAmplitudeDiscreteReal DvQkdLdpcTxParameterEstimation_EstimatedQBER_Rx{ "S16_BERToErrorCorrection.sgn", 2, hType, sWriteMode };

		Binary Key_Rx{ "S0_Raw_Key_Tx.sgn", (t_unsigned_long)param.buffSize };

		Message MessagesToTx_Rx{ "S4_MessagesFromRx.sgn", 10, hType, sWriteMode };
		HandlerMessage MessagesToTx_Rx_{ "S4_MessagesFromRx.sgn", 10, hType, sWriteMode };

		Message MessagesFromTx{ "S0_MessagesFromTx.sgn", 10, hType, sWriteMode };
		HandlerMessage MessagesFromTx_{ "S0_MessagesFromTx.sgn", 10, hType, sWriteMode };

		Message MessagesToLoadFile_Rx{ "S17_MessagesFromRx.sgn", 10, hType, sWriteMode };
		HandlerMessage MessagesToLoadFile_Rx_{ "S17_MessagesFromRx.sgn", 10, hType, sWriteMode };

		Message MessagesFromLoadFile_Tx{ "S18_MessagesFromTx.sgn", 10, hType, sWriteMode };
		HandlerMessage MessagesFromLoadFile_Tx_{ "S18_MessagesFromTx.sgn", 10, hType, sWriteMode };

		// #####################################################################################################
		// ########################### Rx Blocks Declaration and Inicialization ###################################
		// #####################################################################################################

		IPTunnel IPTunnel_Server_Rx{ {},  {&MessagesFromTx_} };
		IPTunnel_Server_Rx.setLocalMachineIpAddress(param.rxIpAddress);
		IPTunnel_Server_Rx.setRemoteMachineIpAddress(param.txIpAddress);
		IPTunnel_Server_Rx.setLocalMachinePort(param.rxReceivingPort);
		IPTunnel_Server_Rx.setVerboseMode(param.verboseMode);

		DvQkdLdpcTxMessageProcessorReceiver DvQkdLdpcTxMessageProcessorReceiver_Rx{ {&MessagesFromTx}, {&BasesFromRx_Rx, &DvQkdLdpcTxParameterEstimation_SeedFromRx_Rx, &DvQkdLdpcTxParameterEstimation_RatioFromRx_Rx, &DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlockFromRx_Rx, &DvQkdLdpcTxParameterEstimation_DataFromRx_Rx, &SindromeFromTx_Rx, &PermutationsFromRx_Rx, &ToggleBERChange_Rx} };
		DvQkdLdpcTxMessageProcessorReceiver_Rx.setHashLength(param.hashLength);

		LoadAscii LoadAscii_Rx{ {},{&Raw_Rx} };
		LoadAscii_Rx.setAsciiFileName("raw_keys/rx_raw");
		LoadAscii_Rx.setAsciiFileNameTailNumber("0");
		LoadAscii_Rx.setAsciiFileNameTailNumberModulos(param.asciiFileNameTailNumberModulos);

		//////
		DestinationTranslationTable dttRxReceiver;
		dttRxReceiver.add("Msg_Rx", 0);
		MessageHandler DvQkdLdpcTxMessageProcessorReceiver_Rx_{ 
			{&MessagesFromTx_}, 
			{&MessagesFromTx},
			dttRxReceiver,
        	FUNCTIONING_AS_RX,
		};

		InputTranslationTable ittRxTransmitter;
		ittRxTransmitter.add(0, {"Msg_Tx", "Msg_Rx"});
		MessageHandler DvQkdLdpcTxMessageProcessorTransmitter_Rx_{ 
			{&MessagesToTx_Rx}, 
			{&MessagesToTx_Rx_},
			FUNCTIONING_AS_TX,
        	ittRxTransmitter
		};
		//////

		DvQkdLdpcRxParameterEstimation DvQkdLdpcRxParameterEstimation_Rx{ {&Raw_Rx, &DvQkdLdpcTxParameterEstimation_SeedFromRx_Rx, &DvQkdLdpcTxParameterEstimation_RatioFromRx_Rx , &DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlockFromRx_Rx, &DvQkdLdpcTxParameterEstimation_DataFromRx_Rx}, 	{&DvQkdLdpcTxParameterEstimation_DataOut_Rx, &DvQkdLdpcTxParameterEstimation_SeedToRx_Rx, &DvQkdLdpcTxParameterEstimation_RatioToRx_Rx, &DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlock_Rx, &DvQkdLdpcTxParameterEstimation_DataToRx_Rx, &DvQkdLdpcTxParameterEstimation_EstimatedQBER_Rx}, signalsFolderName, "DvQkdLDPCRx_parameter_estimation_report.txt", "DVQkdLDPCRx_parameter_estimation_berList.txt" };
		DvQkdLdpcRxParameterEstimation_Rx.setBypassParameterEstimation(param.bypassParameterEstimation);
		DvQkdLdpcRxParameterEstimation_Rx.setNumberOfBitsPerEstimationBlock(param.parameterEstimationNBits);

		CvQokdLdpcRxSindromeReconciliation CvQokdLdpcRxSindromeReconciliation_Rx{ {&DvQkdLdpcTxParameterEstimation_DataOut_Rx, &SindromeFromTx_Rx, &PermutationsFromRx_Rx, &DvQkdLdpcTxParameterEstimation_EstimatedQBER_Rx, &ToggleBERChange_Rx}, {&Key_Rx, &SindromeToTx_Rx} };
		CvQokdLdpcRxSindromeReconciliation_Rx.setBypassErrorCorrection(param.bypassErrorCorrection);
		CvQokdLdpcRxSindromeReconciliation_Rx.setParityCheckMatrixFolderName(param.parityCheckMatrixFolderName);
		CvQokdLdpcRxSindromeReconciliation_Rx.setParityCheckMatrixFileName(param.parityCheckMatrixFileName);
		CvQokdLdpcRxSindromeReconciliation_Rx.setLdpcErrorCorrectionAlgorithMaxIterations(param.ldpcErrorCorrectionAlgorithMaxIterations);
		CvQokdLdpcRxSindromeReconciliation_Rx.setNumberOfDataBitsPerBitFillingPeriod(param.numberOfDataBitsPerBitFillingPeriod);
		CvQokdLdpcRxSindromeReconciliation_Rx.setNumberOfDummyBitsPerBitFillingPeriod(param.numberOfDummyBitsPerBitFillingPeriod);
		CvQokdLdpcRxSindromeReconciliation_Rx.setHashLength(param.hashLength);
		CvQokdLdpcRxSindromeReconciliation_Rx.setKeyType(param.keyType);

		SaveAscii SaveAscii_Rx{ {&Key_Rx}, {} };
		SaveAscii_Rx.setAsciiFolderName("generated_keys");
		SaveAscii_Rx.setAsciiFileName("rx_key");
		SaveAscii_Rx.setAsciiFileNameTailNumber("0");
		SaveAscii_Rx.setAsciiFileNameTailNumberModulos(0);

		DvQkdLdpcTxMessageProcessorTransmitter DvQkdLdpcTxMessageProcessorTransmitter_Rx{ {&BasesAckToRx_Rx, &DvQkdLdpcTxParameterEstimation_SeedToRx_Rx, &DvQkdLdpcTxParameterEstimation_RatioToRx_Rx, &DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlock_Rx, &DvQkdLdpcTxParameterEstimation_DataToRx_Rx,  &SindromeToTx_Rx, &HashToTx_Rx}, {&MessagesToTx_Rx} };

		IPTunnel IPTunnel_Client_Rx{ {&MessagesToTx_Rx_}, {} };
		IPTunnel_Client_Rx.setLocalMachineIpAddress(param.rxIpAddress);
		IPTunnel_Client_Rx.setRemoteMachineIpAddress(param.txIpAddress);
		IPTunnel_Client_Rx.setRemoteMachinePort(param.txReceivingPort);
		IPTunnel_Client_Rx.setVerboseMode(param.verboseMode);

		DvQkdLdpcTxMessageProcessorTransmitter DvQkdLdpcTxMessageProcessorTransmitterLoadFile_Rx{ {&BasesAckToRx_Rx, &DvQkdLdpcTxParameterEstimation_SeedToRx_Rx, &DvQkdLdpcTxParameterEstimation_RatioToRx_Rx, &DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlock_Rx, &DvQkdLdpcTxParameterEstimation_DataToRx_Rx,  &SindromeToTx_Rx, &HashToTx_Rx}, {&MessagesToLoadFile_Rx} };
		DvQkdLdpcTxMessageProcessorReceiver DvQkdLdpcTxMessageProcessorReceiverLoadFile_Rx{ {&MessagesFromLoadFile_Tx}, {&BasesFromRx_Rx, &DvQkdLdpcTxParameterEstimation_SeedFromRx_Rx, &DvQkdLdpcTxParameterEstimation_RatioFromRx_Rx, &DvQkdLdpcTxParameterEstimation_NumberOfBitsPerEstimationBlockFromRx_Rx, &DvQkdLdpcTxParameterEstimation_DataFromRx_Rx, &SindromeFromTx_Rx, &PermutationsFromRx_Rx, &ToggleBERChange_Rx} };


		// #####################################################################################################
		// ########################### System Declaration and Inicialization ###################################
		// #####################################################################################################

		MainSystem MainSystem_
		{
			{
			&LoadAscii_Rx,
			&IPTunnel_Server_Rx,
			&DvQkdLdpcTxMessageProcessorReceiver_Rx,
			&DvQkdLdpcRxParameterEstimation_Rx,
			&CvQokdLdpcRxSindromeReconciliation_Rx,
			&SaveAscii_Rx,
			&DvQkdLdpcTxMessageProcessorTransmitter_Rx,
			&IPTunnel_Client_Rx,
			&DvQkdLdpcTxMessageProcessorTransmitter_Rx_,
			&DvQkdLdpcTxMessageProcessorReceiver_Rx_
			}
		};

		// #####################################################################################################
		// ##################################### Console #######################################################
		// #####################################################################################################

		Console Console_;

		BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Local_Machine_Address_{ &IPTunnel_Server_Rx, std::mem_fn(&IPTunnel::getLocalMachineIpAddress) };
		Console_.addGetFunction("Local Machine Address", &Local_Machine_Address_, value_type::t_string);

		BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Local_Machine_Port_{ &IPTunnel_Server_Rx, std::mem_fn(&IPTunnel::getLocalMachinePort) };
		Console_.addGetFunction("Local Machine Receiving Port", &Local_Machine_Port_, value_type::t_int);

		BlockGetFunction<std::_Mem_fn<std::string (IPTunnel::*)() const>, IPTunnel, std::string> Remote_Machine_Address_{ &IPTunnel_Client_Rx, std::mem_fn(&IPTunnel::getRemoteMachineIpAddress) };
		Console_.addGetFunction("Remote Machine Address", &Remote_Machine_Address_, value_type::t_string);

		BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Remote_Machine_Port_{ &IPTunnel_Client_Rx, std::mem_fn(&IPTunnel::getRemoteMachinePort) };
		Console_.addGetFunction("Remote Machine Receiving Port", &Remote_Machine_Port_, value_type::t_int);
		
		BlockGetFunction<std::_Mem_fn<bool (DvQkdLdpcRxParameterEstimation::*)() const>, DvQkdLdpcRxParameterEstimation, bool> Bypass_Parameter_Estimation_{ &DvQkdLdpcRxParameterEstimation_Rx, std::mem_fn(&DvQkdLdpcRxParameterEstimation::getBypassParameterEstimation) };
		Console_.addGetFunction("Bypass Parameter Estimation", &Bypass_Parameter_Estimation_, value_type::t_bool);

		BlockGetFunction<std::_Mem_fn<double (DvQkdLdpcRxParameterEstimation::*)() const>, DvQkdLdpcRxParameterEstimation, double> Total_Number_Of_Input_Bits_{ &DvQkdLdpcRxParameterEstimation_Rx, std::mem_fn(&DvQkdLdpcRxParameterEstimation::getTotalNumberOfInputBits) };
		Console_.addGetFunction("Total Number Of Input Bits", &Total_Number_Of_Input_Bits_, value_type::t_double);

		BlockGetFunction<std::_Mem_fn<double (DvQkdLdpcRxParameterEstimation::*)() const>, DvQkdLdpcRxParameterEstimation, double> Estimated_BER_{ &DvQkdLdpcRxParameterEstimation_Rx, std::mem_fn(&DvQkdLdpcRxParameterEstimation::getAverageEstimatedBER) };
		Console_.addGetFunction("Estimated BER", &Estimated_BER_, value_type::t_double);

		// BlockGetFunction<std::_Mem_fn<double (DvQkdLdpcRxParameterEstimation::*)() const>, DvQkdLdpcRxParameterEstimation, double> Transmit_Speed_{ &DvQkdLdpcRxParameterEstimation_Rx, std::mem_fn(&DvQkdLdpcRxParameterEstimation::getTransmitSpeed) };
		// Console_.addGetFunction("Speed (Bits/Sec)", &Transmit_Speed_, value_type::t_double);

		BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcRxSindromeReconciliation::*)() const>, CvQokdLdpcRxSindromeReconciliation, unsigned long int> NumberOfCalculatedSindromes_Rx{ &CvQokdLdpcRxSindromeReconciliation_Rx, std::mem_fn(&CvQokdLdpcRxSindromeReconciliation::getNumberOfCalculatedSindromes) };
		Console_.addGetFunction("Rx: Number Of Calculated Sindromes", &NumberOfCalculatedSindromes_Rx, value_type::t_unsigned_long_int);

		BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcRxSindromeReconciliation::*)() const>, CvQokdLdpcRxSindromeReconciliation, unsigned long int> NumberOfValidatedSindromes_Rx{ &CvQokdLdpcRxSindromeReconciliation_Rx, std::mem_fn(&CvQokdLdpcRxSindromeReconciliation::getNumberOfValidatedSindromes) };
		Console_.addGetFunction("Rx: Number Of Validated Sindromes", &NumberOfValidatedSindromes_Rx, value_type::t_unsigned_long_int);

		BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcRxSindromeReconciliation::*)() const>, CvQokdLdpcRxSindromeReconciliation, unsigned long int> NumberOfNoMatchedSindromes_Rx{ &CvQokdLdpcRxSindromeReconciliation_Rx, std::mem_fn(&CvQokdLdpcRxSindromeReconciliation::getNumberOfNoMatchedSindromes) };
		Console_.addGetFunction("Rx: Number Of No Matched Sindromes", &NumberOfNoMatchedSindromes_Rx, value_type::t_unsigned_long_int);

		BlockGetFunction<std::_Mem_fn<double (CvQokdLdpcRxSindromeReconciliation::*)() const>, CvQokdLdpcRxSindromeReconciliation, double> PercentageOfNoMatchedSindromes_Rx{ &CvQokdLdpcRxSindromeReconciliation_Rx, std::mem_fn(&CvQokdLdpcRxSindromeReconciliation::getPercentageOfNoMatchedSindromes) };
		Console_.addGetFunction("Rx: Percentage Of No Matched Sindromes", &PercentageOfNoMatchedSindromes_Rx, value_type::t_double);

		BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcRxSindromeReconciliation::*)() const>, CvQokdLdpcRxSindromeReconciliation, unsigned long int> NumberOfValidatedPairs_Rx{ &CvQokdLdpcRxSindromeReconciliation_Rx, std::mem_fn(&CvQokdLdpcRxSindromeReconciliation::getNumberOfValidatedPairs) };
		Console_.addGetFunction("Rx: Number Of Validated Pairs", &NumberOfValidatedPairs_Rx, value_type::t_unsigned_long_int);

		BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcRxSindromeReconciliation::*)() const>, CvQokdLdpcRxSindromeReconciliation, unsigned long int> NumberOfSavedPairs_Rx{ &CvQokdLdpcRxSindromeReconciliation_Rx, std::mem_fn(&CvQokdLdpcRxSindromeReconciliation::getNumberOfSavedPairs) };
		Console_.addGetFunction("Rx: Number Of Saved Pairs", &NumberOfSavedPairs_Rx, value_type::t_unsigned_long_int);

		BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcRxSindromeReconciliation::*)() const>, CvQokdLdpcRxSindromeReconciliation, unsigned long int> NumberOfDiscardedPairs_Rx{ &CvQokdLdpcRxSindromeReconciliation_Rx, std::mem_fn(&CvQokdLdpcRxSindromeReconciliation::getNumberOfDiscardedPairs) };
		Console_.addGetFunction("Rx: Number Of Discarded Pairs", &NumberOfDiscardedPairs_Rx, value_type::t_unsigned_long_int);

		BlockGetFunction<std::_Mem_fn<double (CvQokdLdpcRxSindromeReconciliation::*)() const>, CvQokdLdpcRxSindromeReconciliation, double> PercentageOfDiscardedPairs_Rx{ &CvQokdLdpcRxSindromeReconciliation_Rx, std::mem_fn(&CvQokdLdpcRxSindromeReconciliation::getPercentageOfDiscardedPairs) };
		Console_.addGetFunction("Rx: Percentage Of Discarded Pairs", &PercentageOfDiscardedPairs_Rx, value_type::t_double);


		MainSystem_.setConsole(&Console_);
		MainSystem_.setShowConsole(true);
		MainSystem_.setMsgOpenConsole("Rx System Console");
		MainSystem_.setShowConsolePeriodicity(1);

		// #####################################################################################################
		// #################################### System Run #####################################################
		// #####################################################################################################

		MainSystem_.setLogValue(param.logValue);
		MainSystem_.setVerboseMode(param.verboseMode);

		CvQokdLdpcRxSindromeReconciliation_Rx.setVerboseMode(true);
		
		MainSystem_.run();
		MainSystem_.terminate();

		// #####################################################################################################

		auto finish = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> elapsed = finish - start;
		std::cout << "Elapsed time: " << elapsed.count() << " s\n";
		system("pause");

		return 0;
	};
}


int main(int argc, char* argv[])
{
	//tx::main(argc, argv);
	rx::main(argc, argv);

	return 0;
}
