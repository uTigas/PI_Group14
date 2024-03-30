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


namespace tx_loadfile
{
	int main(int argc, char* argv[])
	{

		ConsoleOutputCommon::prepareWindowsConsole();
		ConsoleOutputCommon::clrScreen();

		auto start = std::chrono::high_resolution_clock::now();


		// #####################################################################################################
		// ################################ Variables Inicialization ###########################################
		// #####################################################################################################

//		DvQkdLdpcInputParameters param = DvQkdLdpcInputParameters();
//		param.setInputParametersFileName("input_parameters_tx.txt");
//		param.readSystemInputParameters();

		DvQkdLdpcInputParameters param_LoadFile = DvQkdLdpcInputParameters();
		param_LoadFile.setInputParametersFileName("input_parameters_tx_LoadFile.txt");
		param_LoadFile.readSystemInputParameters();



		Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii };
		Signal::t_header_type hType{Signal::t_header_type::noHeader};
		t_unsigned_long maximumECSyndromeSize{ 972 };
		t_unsigned_long hashSize{ 20 };

		std::string signalsFolderName{ "signals" };

		// #####################################################################################################
		// ########################### Tx Signals Declaration and Inicialization ###############################
		// #####################################################################################################

		Binary Raw_Tx{ "S0_Raw_Key_Tx.sgn", (t_unsigned_long) param_LoadFile.buffSize, hType, sWriteMode };

		TimeDiscreteAmplitudeDiscreteInteger ParameterEstimation_SeedToRx_Tx{ "S1_ParameterEstimation_SeedToRx_Tx.sgn", 1, hType, sWriteMode };
		TimeDiscreteAmplitudeDiscreteInteger ParameterEstimation_RatioToRx_Tx{ "S2_ParameterEstimation_RatioToRx_Tx.sgn", 1, hType, sWriteMode };
		TimeDiscreteAmplitudeDiscreteInteger ParameterEstimation_NumberOfBitsPerEstimationBlockToRx_Tx{ "S3_ParameterEstimation_NumberOfBitsPerEstimationBlock_Tx.sgn", 1, hType, sWriteMode };
		Binary ParameterEstimation_DataToRx_Tx{ "S4_ParameterEstimation_DataToRx_Tx.sgn",100, hType, sWriteMode };

		TimeDiscreteAmplitudeDiscreteInteger ParameterEstimation_SeedFromRx_Tx{ "S1_ParameterEstimation_SeedTFromRx_Tx.sgn", 1, hType, sWriteMode };
		TimeDiscreteAmplitudeDiscreteInteger ParameterEstimation_RatioFromRx_Tx{ "S2_ParameterEstimation_RatioFromRx_Tx.sgn", 1, hType, sWriteMode };
		TimeDiscreteAmplitudeDiscreteInteger ParameterEstimation_NumberOfBitsPerEstimationBlockFromRx_Tx{ "S3_ParameterEstimation_NumberOfBitsPerEstimationBlockFromRx_Tx.sgn", 1, hType, sWriteMode };
		Binary ParameterEstimation_DataFromRx_Tx{ "S4_ParameterEstimation_DataFromRx_Tx.sgn", 100, hType, sWriteMode };
				
		Binary ParameterEstimation_DataOut_Tx{ "S5_ParameterEstimation_DataOut_Tx.sgn", (t_unsigned_long) param_LoadFile.buffSize, hType, sWriteMode };
		TimeContinuousAmplitudeDiscreteReal ParameterEstimation_EstimatedQBER_Tx{ "S6_ParameterEstimation_EstimatedQBER_Tx.sgn", 2, hType, sWriteMode };

		Binary SindromeFromRx_Tx{ "S7_SindromeFromRx.sgn", maximumECSyndromeSize, hType, sWriteMode };
		TimeContinuousAmplitudeDiscreteReal PermutationsFromRx_Tx{ "S8_PermutationsFromRx.sgn",maximumECSyndromeSize, hType, sWriteMode };
		TimeContinuousAmplitudeDiscreteReal ToggleBERChange_Tx{ "S9_ToggleBERChange.sgn", (t_unsigned_long) param_LoadFile.buffSize, hType, sWriteMode };

		Binary Key_Tx{ "S10_Raw_Key_Tx.sgn", (t_unsigned_long) param_LoadFile.buffSize };

		Binary BasesAckToRx_Tx{ "S11_BasesAckToRx.sgn", (t_unsigned_long) param_LoadFile.buffSize, hType, sWriteMode };
		Binary SindromeToRx_Tx{ "S12_SindromeToRx.sgn", maximumECSyndromeSize, hType, sWriteMode };
		Message HashToRx_Tx{ "S13_HashToRx_Tx.sgn", 1, hType, sWriteMode };

		Message MessagesToRx_Tx{ "S14_MessagesToRx.sgn", 10, hType, sWriteMode };
		HandlerMessage MessagesToRx_Tx_{ "S14_MessagesToRx.sgn", 10, hType, sWriteMode };

		Message MessagesFromRx_Tx{ "S15_Tx_MessagesFromRx.sgn", 10, hType, sWriteMode };
		HandlerMessage MessagesFromRx_Tx_{ "S15_Tx_MessagesFromRx.sgn", 10, hType, sWriteMode };

		Binary BasesFromRx_Tx{ "S16_BasesFromRx_Tx.sgn", (t_unsigned_long)param_LoadFile.buffSize, hType, sWriteMode };

		Message MessagesToLoadFile_Tx{ "S17_MessagesToRx.sgn", 10, hType, sWriteMode };
		HandlerMessage MessagesToLoadFile_Tx_{ "S17_MessagesToRx.sgn", 10, hType, sWriteMode };

		Message MessagesFromLoadFile_Tx{ "S18_Tx_MessagesFromRx.sgn", 10, hType, sWriteMode };
		HandlerMessage MessagesFromLoadFile_Tx_{ "S18_Tx_MessagesFromRx.sgn", 10, hType, sWriteMode };

		// #####################################################################################################
		// ########################### Tx Blocks Declaration and Inicialization ###################################
		// #####################################################################################################

		LoadEtsi004 LoadEtsi004_Tx{ {},{&Raw_Tx} };
		LoadEtsi004_Tx.setKeyStreamId(param_LoadFile.keyStreamId);
		LoadEtsi004_Tx.setPort(param_LoadFile.etsiPort);
		LoadEtsi004_Tx.setAddress(param_LoadFile.etsiAddress);
		LoadEtsi004_Tx.setNode1UUID(param_LoadFile.node1_uuid);
		LoadEtsi004_Tx.setNode2UUID(param_LoadFile.node2_uuid);
		LoadEtsi004_Tx.setApp(param_LoadFile.App);
		LoadEtsi004_Tx.setKeySource(param_LoadFile.keySource);
		LoadEtsi004_Tx.setSaveKeys(param_LoadFile.saveKeys);
		LoadEtsi004_Tx.setAsciiFileName("tx_raw");
		LoadEtsi004_Tx.setAsciiFileNameTailNumber("0");
		LoadEtsi004_Tx.setAsciiFileNameTailNumberModulos(param_LoadFile.asciiFileNameTailNumberModulos);


//		DvQkdLdpcTxParameterEstimation DvQkdLdpcTxParameterEstimation_Tx{ {&Raw_Tx, &ParameterEstimation_SeedFromRx_Tx, &ParameterEstimation_RatioFromRx_Tx , &ParameterEstimation_NumberOfBitsPerEstimationBlockFromRx_Tx, &ParameterEstimation_DataFromRx_Tx}, 	{&ParameterEstimation_DataOut_Tx, &ParameterEstimation_SeedToRx_Tx, &ParameterEstimation_RatioToRx_Tx, &ParameterEstimation_NumberOfBitsPerEstimationBlockToRx_Tx, &ParameterEstimation_DataToRx_Tx, &ParameterEstimation_EstimatedQBER_Tx}, signalsFolderName, "DvQkdLDPCTx_parameter_estimation_report.txt", "DVQkdLDPCTx_parameter_estimation_berList.txt" };
//		DvQkdLdpcTxParameterEstimation_Tx.setRole(DvQkdLdpcTxParameterEstimation::t_role::master);
//		DvQkdLdpcTxParameterEstimation_Tx.setBypassParameterEstimation(param.bypassParameterEstimation);
//		DvQkdLdpcTxParameterEstimation_Tx.setNumberOfBitsPerEstimationBlock(param.parameterEstimationNBits);
//	
//		CvQokdLdpcTxSindromeReconciliation CvQokdLdpcTxSindromeReconciliation_Tx{ {&ParameterEstimation_DataOut_Tx, &SindromeFromRx_Tx, &PermutationsFromRx_Tx, &ParameterEstimation_EstimatedQBER_Tx, &ToggleBERChange_Tx}, {&Key_Tx, &SindromeToRx_Tx, &HashToRx_Tx} };
//		CvQokdLdpcTxSindromeReconciliation_Tx.setBypassErrorCorrection(param.bypassErrorCorrection);
//		CvQokdLdpcTxSindromeReconciliation_Tx.setParityCheckMatrixFolderName(param.parityCheckMatrixFolderName);
//		CvQokdLdpcTxSindromeReconciliation_Tx.setParityCheckMatrixFileName(param.parityCheckMatrixFileName);
//		CvQokdLdpcTxSindromeReconciliation_Tx.setLdpcErrorCorrectionAlgorithMaxIterations(param.ldpcErrorCorrectionAlgorithMaxIterations);
//		CvQokdLdpcTxSindromeReconciliation_Tx.setNumberOfDataBitsPerBitFillingPeriod(param.numberOfDataBitsPerBitFillingPeriod);
//		CvQokdLdpcTxSindromeReconciliation_Tx.setNumberOfDummyBitsPerBitFillingPeriod(param.numberOfDummyBitsPerBitFillingPeriod);
//		CvQokdLdpcTxSindromeReconciliation_Tx.setKeyType(param.keyType);

		// Oblivious keys
//		SaveAscii SaveAscii_Tx{ {&Key_Tx}, {} };
//		//SaveAscii_Tx.setAsciiFileName("tx_key");
//		SaveAscii_Tx.setAsciiFileName("tx_raw_input");
//		SaveAscii_Tx.setAsciiFileNameTailNumber("0");
//		SaveAscii_Tx.setAsciiFileNameTailNumberModulos(0);

		//////
//		DestinationTranslationTable dttTxReceiver;
//		dttTxReceiver.add("Msg_Tx", 0);
//		MessageHandler DvQkdLdpcTxMessageProcessorReceiver_Tx_{ 
//			{&MessagesFromRx_Tx_}, 
//			{&MessagesFromRx_Tx},
//			dttTxReceiver,
//        	FUNCTIONING_AS_RX,
//		};
//
//		InputTranslationTable ittTxTransmitter;
//		ittTxTransmitter.add(0, {"Msg_Rx", "Msg_Tx"});
//		MessageHandler DvQkdLdpcTxMessageProcessorTransmitter_Tx_{ 
//			{&MessagesToRx_Tx},
//			{&MessagesToRx_Tx_},
//			FUNCTIONING_AS_TX,
//        	ittTxTransmitter};
		//////

		////// Translation Table for LoadFile from ETSI
		DestinationTranslationTable dttTxReceiverLoadFile;
		dttTxReceiverLoadFile.add("Msg_Tx_LoadFile", 0);
		MessageHandler DvQkdLdpcTxMessageProcessorReceiverLoadFile_Tx_{ 
			{&MessagesFromLoadFile_Tx}, 
			{&MessagesFromLoadFile_Tx_},
			dttTxReceiverLoadFile,
        	FUNCTIONING_AS_RX,
		};

		InputTranslationTable ittTxTransmitterLoadFile;
		ittTxTransmitterLoadFile.add(0, {"Msg_Rx_LoadFile", "Msg_Tx_LoadFile"});
		MessageHandler DvQkdLdpcTxMessageProcessorTransmitterLoadFile_Tx_{ 
			{&MessagesToLoadFile_Tx},
			{&MessagesToLoadFile_Tx_},
			FUNCTIONING_AS_TX,
        	ittTxTransmitterLoadFile};
		//////


//		DvQkdLdpcTxMessageProcessorTransmitter DvQkdLdpcTxMessageProcessorTransmitter_Tx{ {&BasesAckToRx_Tx, &ParameterEstimation_SeedToRx_Tx, &ParameterEstimation_RatioToRx_Tx, &ParameterEstimation_NumberOfBitsPerEstimationBlockToRx_Tx, &ParameterEstimation_DataToRx_Tx,  &SindromeToRx_Tx, &HashToRx_Tx}, {&MessagesToRx_Tx} };
//
//		IPTunnel IPTunnel_Client_Tx{ {&MessagesToRx_Tx_}, {} };
//		IPTunnel_Client_Tx.setLocalMachineIpAddress(param.txIpAddress);
//		IPTunnel_Client_Tx.setRemoteMachineIpAddress(param.rxIpAddress);
//		IPTunnel_Client_Tx.setRemoteMachinePort(param.rxReceivingPort);
//		IPTunnel_Client_Tx.setVerboseMode(param.verboseMode);
//
//		IPTunnel IPTunnel_Server_Tx{ {}, {&MessagesFromRx_Tx_} };
//		IPTunnel_Server_Tx.setLocalMachineIpAddress(param.txIpAddress);
//		IPTunnel_Server_Tx.setRemoteMachineIpAddress(param.rxIpAddress);
//		IPTunnel_Server_Tx.setLocalMachinePort(param.txReceivingPort);
//		IPTunnel_Server_Tx.setVerboseMode(param.verboseMode);
//		
//		DvQkdLdpcTxMessageProcessorReceiver DvQkdLdpcTxMessageProcessorReceiver_Tx{ {&MessagesFromRx_Tx}, {&BasesFromRx_Tx, &ParameterEstimation_SeedFromRx_Tx, &ParameterEstimation_RatioFromRx_Tx, &ParameterEstimation_NumberOfBitsPerEstimationBlockFromRx_Tx, &ParameterEstimation_DataFromRx_Tx, &SindromeFromRx_Tx, &PermutationsFromRx_Tx, &ToggleBERChange_Tx} };


		// IP Tunnel for LoadFile from ETSI
		DvQkdLdpcTxMessageProcessorTransmitter DvQkdLdpcTxMessageProcessorTransmitterLoadFile_Tx{ {&BasesAckToRx_Tx, &ParameterEstimation_SeedToRx_Tx, &ParameterEstimation_RatioToRx_Tx, &ParameterEstimation_NumberOfBitsPerEstimationBlockToRx_Tx, &ParameterEstimation_DataToRx_Tx,  &SindromeToRx_Tx, &HashToRx_Tx}, {&MessagesToLoadFile_Tx} };
//		DvQkdLdpcTxMessageProcessorTransmitter DvQkdLdpcTxMessageProcessorTransmitterLoadFile_Tx{ {}, {&MessagesToLoadFile_Tx} };


		IPTunnel IPTunnel_Client_Tx_LoadFile{ {&MessagesToLoadFile_Tx_}, {} };	// From Tx (LoadFile) to App (Rx)
		IPTunnel_Client_Tx_LoadFile.setLocalMachineIpAddress(param_LoadFile.txIpAddress);
		IPTunnel_Client_Tx_LoadFile.setRemoteMachineIpAddress(param_LoadFile.rxIpAddress);
		IPTunnel_Client_Tx_LoadFile.setRemoteMachinePort(param_LoadFile.rxReceivingPort);
		IPTunnel_Client_Tx_LoadFile.setVerboseMode(param_LoadFile.verboseMode);
		

		IPTunnel IPTunnel_Server_Tx_LoadFile{ {}, {&MessagesFromLoadFile_Tx_} };  // To Tx (LoadFile) from Rx (App)
		IPTunnel_Server_Tx_LoadFile.setLocalMachineIpAddress(param_LoadFile.txIpAddress);
		IPTunnel_Server_Tx_LoadFile.setRemoteMachineIpAddress(param_LoadFile.rxIpAddress);
		IPTunnel_Server_Tx_LoadFile.setLocalMachinePort(param_LoadFile.txReceivingPort);
		IPTunnel_Server_Tx_LoadFile.setVerboseMode(param_LoadFile.verboseMode);
		
		DvQkdLdpcTxMessageProcessorReceiver DvQkdLdpcTxMessageProcessorReceiverLoadFile_Tx{ {&MessagesFromLoadFile_Tx}, {&BasesFromRx_Tx, &ParameterEstimation_SeedFromRx_Tx, &ParameterEstimation_RatioFromRx_Tx, &ParameterEstimation_NumberOfBitsPerEstimationBlockFromRx_Tx, &ParameterEstimation_DataFromRx_Tx, &SindromeFromRx_Tx, &PermutationsFromRx_Tx, &ToggleBERChange_Tx} };
//		DvQkdLdpcTxMessageProcessorReceiver DvQkdLdpcTxMessageProcessorReceiverLoadFile_Tx{ {&MessagesFromLoadFile_Tx}, {} };


		// #####################################################################################################
		// ########################### System Declaration and Inicialization ###################################
		// #####################################################################################################

		MainSystem MainSystem_
		{
			{
				//&LoadAscii_Tx,
//				&DvQkdLdpcTxParameterEstimation_Tx,
//				&CvQokdLdpcTxSindromeReconciliation_Tx,
//				&SaveAscii_Tx,
//				&DvQkdLdpcTxMessageProcessorTransmitter_Tx,
//				&IPTunnel_Client_Tx,
//				&IPTunnel_Server_Tx,
//				&DvQkdLdpcTxMessageProcessorReceiver_Tx,
//				&DvQkdLdpcTxMessageProcessorTransmitter_Tx_,
//				&DvQkdLdpcTxMessageProcessorReceiver_Tx_,

				&LoadEtsi004_Tx,
				&IPTunnel_Client_Tx_LoadFile,
				&IPTunnel_Server_Tx_LoadFile,
				&DvQkdLdpcTxMessageProcessorTransmitterLoadFile_Tx,
				&DvQkdLdpcTxMessageProcessorReceiverLoadFile_Tx,
				&DvQkdLdpcTxMessageProcessorTransmitterLoadFile_Tx_,
				&DvQkdLdpcTxMessageProcessorReceiverLoadFile_Tx_,
			}
		};

		// #####################################################################################################
		// ##################################### Console #######################################################
		// #####################################################################################################

		Console Console_;

		BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Local_Machine_Address_{ &IPTunnel_Server_Tx_LoadFile, std::mem_fn(&IPTunnel::getLocalMachineIpAddress) };
		Console_.addGetFunction("Local Machine Address", &Local_Machine_Address_, value_type::t_string);

		BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Local_Machine_Port_{ &IPTunnel_Server_Tx_LoadFile, std::mem_fn(&IPTunnel::getLocalMachinePort) };
		Console_.addGetFunction("Local Machine Receiving Port", &Local_Machine_Port_, value_type::t_int);

		BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Remote_Machine_Address_{ &IPTunnel_Client_Tx_LoadFile, std::mem_fn(&IPTunnel::getRemoteMachineIpAddress) };
		Console_.addGetFunction("Remote Machine Address", &Remote_Machine_Address_, value_type::t_string);

		BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Remote_Machine_Port_{ &IPTunnel_Client_Tx_LoadFile, std::mem_fn(&IPTunnel::getRemoteMachinePort) };
		Console_.addGetFunction("Remote Machine Receiving Port", &Remote_Machine_Port_, value_type::t_int);

		//BlockGetFunction<std::_Mem_fn<std::string(LoadAscii::*)() const>, LoadAscii, std::string> Load_File_Name_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getAsciiFileFullName) };
		//Console_.addGetFunction("Load File Name", &Load_File_Name_, value_type::t_string);

		//BlockGetFunction<std::_Mem_fn<unsigned long int (LoadAscii::*)() const>, LoadAscii, unsigned long int> Number_Of_Loaded_Values_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getNumberOfLoadedValues) };
		//Console_.addGetFunction("Number of Loaded Values", &Number_Of_Loaded_Values_, value_type::t_unsigned_long_int);

//		BlockGetFunction<std::_Mem_fn<bool (DvQkdLdpcTxParameterEstimation::*)() const>, DvQkdLdpcTxParameterEstimation, bool> Bypass_Parameter_Estimation_{ &DvQkdLdpcTxParameterEstimation_Tx, std::mem_fn(&DvQkdLdpcTxParameterEstimation::getBypassParameterEstimation) };
//		Console_.addGetFunction("Bypass Parameter Estimation", &Bypass_Parameter_Estimation_, value_type::t_bool);
//
//		BlockGetFunction<std::_Mem_fn<double (DvQkdLdpcTxParameterEstimation::*)() const>, DvQkdLdpcTxParameterEstimation, double> Total_Number_Of_Input_Bits_{ &DvQkdLdpcTxParameterEstimation_Tx, std::mem_fn(&DvQkdLdpcTxParameterEstimation::getTotalNumberOfInputBits) };
//		Console_.addGetFunction("Total Number Of Input Bits", &Total_Number_Of_Input_Bits_, value_type::t_double);
//
//		BlockGetFunction<std::_Mem_fn<double (DvQkdLdpcTxParameterEstimation::*)() const>, DvQkdLdpcTxParameterEstimation, double> Estimated_BER_{ &DvQkdLdpcTxParameterEstimation_Tx, std::mem_fn(&DvQkdLdpcTxParameterEstimation::getAverageEstimatedBER) };
//		Console_.addGetFunction("Estimated BER", &Estimated_BER_, value_type::t_double);
//		
//		BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcTxSindromeReconciliation::*)() const>, CvQokdLdpcTxSindromeReconciliation, unsigned long int> NumberOfCalculatedSindromes_Tx{ &CvQokdLdpcTxSindromeReconciliation_Tx, std::mem_fn(&CvQokdLdpcTxSindromeReconciliation::getNumberOfCalculatedSindromes) };
//		Console_.addGetFunction("Tx: Number Of Calculated Sindromes", &NumberOfCalculatedSindromes_Tx, value_type::t_unsigned_long_int);
//
//		BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcTxSindromeReconciliation::*)() const>, CvQokdLdpcTxSindromeReconciliation, unsigned long int> NumberOfSavedPairs_Tx{ &CvQokdLdpcTxSindromeReconciliation_Tx, std::mem_fn(&CvQokdLdpcTxSindromeReconciliation::getNumberOfSavedPairs) };
//		Console_.addGetFunction("Tx: Number of Saved Pairs", &NumberOfSavedPairs_Tx, value_type::t_unsigned_long_int);
//
//		BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcTxSindromeReconciliation::*)() const>, CvQokdLdpcTxSindromeReconciliation, unsigned long int> NumberOfDiscardedPairs_Tx{ &CvQokdLdpcTxSindromeReconciliation_Tx, std::mem_fn(&CvQokdLdpcTxSindromeReconciliation::getNumberOfDiscardedPairs) };
//		Console_.addGetFunction("Tx: Number of Discarded Pairs", &NumberOfDiscardedPairs_Tx, value_type::t_unsigned_long_int);
//
//		BlockGetFunction<std::_Mem_fn<double (CvQokdLdpcTxSindromeReconciliation::*)() const>, CvQokdLdpcTxSindromeReconciliation, double> PercentageOfDiscardedPairs_Tx{ &CvQokdLdpcTxSindromeReconciliation_Tx, std::mem_fn(&CvQokdLdpcTxSindromeReconciliation::getPercentageOfDiscardedPairs) };
//		Console_.addGetFunction("Tx: Percentage Of Discarded Pairs", &PercentageOfDiscardedPairs_Tx, value_type::t_double);

		MainSystem_.setConsole(&Console_);
		MainSystem_.setShowConsole(true);
		MainSystem_.setMsgOpenConsole("Tx Load File System Console");
		MainSystem_.setShowConsolePeriodicity(100);


		// #####################################################################################################
		// #################################### System Run #####################################################
		// #####################################################################################################

		MainSystem_.setLogValue(param_LoadFile.logValue);
		MainSystem_.setVerboseMode(param_LoadFile.verboseMode);

//		CvQokdLdpcTxSindromeReconciliation_Tx.setVerboseMode(true);
		MainSystem_.run();
		MainSystem_.terminate();
		// #####################################################################################################

		auto finish = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> elapsed = finish - start;
		std::cout << "Elapsed time: " << elapsed.count() << " s\n";
		//system("pause");

		return 0;
	};
}

int main(int argc, char* argv[])
{
	tx_loadfile::main(argc, argv);
	//rx::main(argc, argv);

	return 0;
}
