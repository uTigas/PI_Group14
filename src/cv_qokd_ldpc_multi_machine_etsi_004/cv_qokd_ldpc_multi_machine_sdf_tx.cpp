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



#include "ms_windows_console_output_common_20200819.h"
#include "ms_windows_network_aux_common_20200819.h"
#include "very_big_counter_20200819.h"

#include "dv_qkd_polarization_physical_layer_20200819.h"
#include "cv_qokd_ldpc_rx_20200819.h"


namespace tx
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
		param.setInputParametersFileName("input_parameters_tx.txt");
		param.readSystemInputParameters();

		Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii };
		Signal::t_header_type hType{ Signal::t_header_type::noHeader };
		t_unsigned_long maximumECSyndromeSize{ 972 };
		t_unsigned_long hashSize{ 20 };

		std::string signalsFolderName{ "signals" };

		// #####################################################################################################
		// ########################### Tx Signals Declaration and Inicialization ###############################
		// #####################################################################################################

		Binary Raw_Tx{ "S0_Raw_Key_Tx.sgn", (t_unsigned_long)param.buffSize, hType, sWriteMode };

		TimeDiscreteAmplitudeDiscreteInteger ParameterEstimation_SeedToRx_Tx{ "S1_ParameterEstimation_SeedToRx_Tx.sgn", 1, hType, sWriteMode };
		TimeDiscreteAmplitudeDiscreteInteger ParameterEstimation_RatioToRx_Tx{ "S2_ParameterEstimation_RatioToRx_Tx.sgn", 1, hType, sWriteMode };
		TimeDiscreteAmplitudeDiscreteInteger ParameterEstimation_NumberOfBitsPerEstimationBlockToRx_Tx{ "S3_ParameterEstimation_NumberOfBitsPerEstimationBlockToRx_Tx.sgn", 1, hType, sWriteMode };
		Binary ParameterEstimation_DataToRx_Tx{ "S4_ParameterEstimation_DataToRx_Tx.sgn",100, hType, sWriteMode };

		TimeDiscreteAmplitudeDiscreteInteger ParameterEstimation_SeedFromRx_Tx{ "S1_ParameterEstimation_SeedTFromRx_Tx.sgn", 1, hType, sWriteMode };
		TimeDiscreteAmplitudeDiscreteInteger ParameterEstimation_RatioFromRx_Tx{ "S2_ParameterEstimation_RatioFromRx_Tx.sgn", 1, hType, sWriteMode };
		TimeDiscreteAmplitudeDiscreteInteger ParameterEstimation_NumberOfBitsPerEstimationBlockFromRx_Tx{ "S3_ParameterEstimation_NumberOfBitsPerEstimationBlockFromRx_Tx.sgn", 1, hType, sWriteMode };
		Binary ParameterEstimation_DataFromRx_Tx{ "S4_ParameterEstimation_DataFromRx_Tx.sgn", 100, hType, sWriteMode };

		TimeContinuousAmplitudeDiscreteReal ParameterEstimation_EstimatedQBER_Tx{ "S5_ParameterEstimation_EstimatedQBER_Tx.sgn", 1, hType, sWriteMode };

		Binary ParameterEstimation_DataOut_Tx{ "S6_ParameterEstimation_DataOut_Tx.sgn", (t_unsigned_long)param.buffSize, hType, sWriteMode };

		Binary SindromeToRx_Tx{ "S7_SindromeToRx.sgn", maximumECSyndromeSize, hType, sWriteMode };
		Message HashToRx_Tx{ "S8_HashToRx_Tx.sgn", 1, hType, sWriteMode };

		Message MessagesToRx_Tx{ "S9_MessagesToRx_Tx.sgn", 10, hType, sWriteMode };
		Message MessagesFromRx_Tx{ "S10_MessagesFromRx_Tx.sgn", 10, hType, sWriteMode };

		Binary SindromePairsAckFromRx_Tx{ "S11_SindromePairsAckFromRx_Tx.sgn", 1, hType, sWriteMode };

		Binary Key_Tx{ "S12_Oblivious_Key_Tx.sgn", (t_unsigned_long)param.buffSize, hType, sWriteMode };

		TimeContinuousAmplitudeDiscreteReal PermutationsFromRx_Tx{ "SX_PermutationsFromRx.sgn",maximumECSyndromeSize, hType, sWriteMode };
		TimeContinuousAmplitudeDiscreteReal ToggleBERChange_Tx{ "SX_ToggleBERChange.sgn", (t_unsigned_long)param.buffSize, hType, sWriteMode };
		Binary BasesAckToRx_Tx{ "SX_BasesAckToRx.sgn", (t_unsigned_long)param.buffSize, hType, sWriteMode };
		Binary BasesFromRx_Tx{ "SX_BasesFromRx_Tx.sgn", (t_unsigned_long)param.buffSize, hType, sWriteMode };

		// #####################################################################################################
		// ########################### Tx Blocks Declaration and Inicialization ###################################
		// #####################################################################################################

		/*
		LoadAscii LoadAscii_Tx{ {},{&Raw_Tx} };
		LoadAscii_Tx.setAsciiFileName("tx_raw");
		LoadAscii_Tx.setAsciiFileNameTailNumber("0");
		LoadAscii_Tx.setAsciiFileNameTailNumberModulos(param.asciiFileNameTailNumberModulos);
		*/

		LoadEtsi004 LoadEtsi004_Tx{ {},{&Raw_Tx} };
		LoadEtsi004_Tx.setKeyStreamId(param.keyStreamId);
		LoadEtsi004_Tx.setPort(param.etsiPort);
		LoadEtsi004_Tx.setAddress(param.etsiAddress);
		LoadEtsi004_Tx.setNode1UUID(param.node1_uuid);
		LoadEtsi004_Tx.setNode2UUID(param.node2_uuid);
		LoadEtsi004_Tx.setApp(param.App);
		LoadEtsi004_Tx.setKeySource(param.keySource);
		LoadEtsi004_Tx.setSaveKeys(param.saveKeys);
		LoadEtsi004_Tx.setAsciiFileName("tx_raw");
		LoadEtsi004_Tx.setAsciiFileNameTailNumber("0");
		LoadEtsi004_Tx.setAsciiFileNameTailNumberModulos(param.asciiFileNameTailNumberModulos);

		DvQkdLdpcTxParameterEstimation DvQkdLdpcTxParameterEstimation_Tx{ {&Raw_Tx, &ParameterEstimation_SeedFromRx_Tx, &ParameterEstimation_RatioFromRx_Tx , &ParameterEstimation_NumberOfBitsPerEstimationBlockFromRx_Tx, &ParameterEstimation_DataFromRx_Tx}, 	{&ParameterEstimation_DataOut_Tx, &ParameterEstimation_SeedToRx_Tx, &ParameterEstimation_RatioToRx_Tx, &ParameterEstimation_NumberOfBitsPerEstimationBlockToRx_Tx, &ParameterEstimation_DataToRx_Tx, &ParameterEstimation_EstimatedQBER_Tx}, signalsFolderName, "DvQkdLDPCTx_parameter_estimation_report.txt", "DVQkdLDPCTx_parameter_estimation_berList.txt" };
		DvQkdLdpcTxParameterEstimation_Tx.setRole(DvQkdLdpcTxParameterEstimation::t_role::master);
		DvQkdLdpcTxParameterEstimation_Tx.setBypassParameterEstimation(param.bypassParameterEstimation);
		DvQkdLdpcTxParameterEstimation_Tx.setNumberOfBitsPerEstimationBlock(param.parameterEstimationNBits);

		CvQokdLdpcTxSindromeReconciliation CvQokdLdpcTxSindromeReconciliation_Tx{ {&ParameterEstimation_DataOut_Tx, &SindromePairsAckFromRx_Tx, &PermutationsFromRx_Tx, &ParameterEstimation_EstimatedQBER_Tx, &ToggleBERChange_Tx}, {&Key_Tx, &SindromeToRx_Tx, &HashToRx_Tx} };
		CvQokdLdpcTxSindromeReconciliation_Tx.setBypassErrorCorrection(param.bypassErrorCorrection);
		CvQokdLdpcTxSindromeReconciliation_Tx.setParityCheckMatrixFolderName(param.parityCheckMatrixFolderName);
		CvQokdLdpcTxSindromeReconciliation_Tx.setParityCheckMatrixFileName(param.parityCheckMatrixFileName);
		CvQokdLdpcTxSindromeReconciliation_Tx.setLdpcErrorCorrectionAlgorithMaxIterations(param.ldpcErrorCorrectionAlgorithMaxIterations);
		CvQokdLdpcTxSindromeReconciliation_Tx.setNumberOfDataBitsPerBitFillingPeriod(param.numberOfDataBitsPerBitFillingPeriod);
		CvQokdLdpcTxSindromeReconciliation_Tx.setNumberOfDummyBitsPerBitFillingPeriod(param.numberOfDummyBitsPerBitFillingPeriod);

		SaveAscii SaveAscii_Tx{ {&Key_Tx}, {} };
		SaveAscii_Tx.setAsciiFolderName(param.asciiFolderName);
		SaveAscii_Tx.setAsciiFileName("tx_key");
		SaveAscii_Tx.setAsciiFileNameTailNumber("0");
		SaveAscii_Tx.setAsciiFileNameTailNumberModulos(0);     //increments 1 indefinitevelly
		SaveAscii_Tx.setInsertEndLine(param.insertEndLine);
		SaveAscii_Tx.setReplacePreviousKeys(param.replacePreviousKeys);

//		SaveAscii SaveRawAscii_Tx{ {&Raw_Tx}, {} };
//		SaveRawAscii_Tx.setAsciiFolderName(param.rawKeyFolder);
//		SaveRawAscii_Tx.setAsciiFileName("tx_raw");
//		SaveRawAscii_Tx.setAsciiFileNameTailNumber("0");
//		SaveRawAscii_Tx.setAsciiFileNameTailNumberModulos(0);     //increments 1 indefinitevelly


		DvQkdLdpcTxMessageProcessorTransmitter DvQkdLdpcTxMessageProcessorTransmitter_Tx{ {&BasesAckToRx_Tx, &ParameterEstimation_SeedToRx_Tx, &ParameterEstimation_RatioToRx_Tx, &ParameterEstimation_NumberOfBitsPerEstimationBlockToRx_Tx, &ParameterEstimation_DataToRx_Tx,  &SindromeToRx_Tx, &HashToRx_Tx}, {&MessagesToRx_Tx} };

		IPTunnel IPTunnel_Client_Tx{ {&MessagesToRx_Tx}, {} };
		IPTunnel_Client_Tx.setLocalMachineIpAddress(param.txIpAddress);
		IPTunnel_Client_Tx.setRemoteMachineIpAddress(param.rxIpAddress);
		IPTunnel_Client_Tx.setRemoteMachinePort(param.rxReceivingPort);
		IPTunnel_Client_Tx.setVerboseMode(param.verboseMode);

		IPTunnel IPTunnel_Server_Tx{ {}, {&MessagesFromRx_Tx} };
		IPTunnel_Server_Tx.setLocalMachineIpAddress(param.txIpAddress);
		IPTunnel_Server_Tx.setRemoteMachineIpAddress(param.rxIpAddress);
		IPTunnel_Server_Tx.setLocalMachinePort(param.txReceivingPort);
		IPTunnel_Server_Tx.setVerboseMode(param.verboseMode);

		DvQkdLdpcTxMessageProcessorReceiver DvQkdLdpcTxMessageProcessorReceiver_Tx{ {&MessagesFromRx_Tx}, {&BasesFromRx_Tx, &ParameterEstimation_SeedFromRx_Tx, &ParameterEstimation_RatioFromRx_Tx, &ParameterEstimation_NumberOfBitsPerEstimationBlockFromRx_Tx, &ParameterEstimation_DataFromRx_Tx, &SindromePairsAckFromRx_Tx, &PermutationsFromRx_Tx, &ToggleBERChange_Tx} };
		DvQkdLdpcTxMessageProcessorReceiver_Tx.setHashLength(param.hashLength);

		// #####################################################################################################
		// ########################### System Declaration and Inicialization ###################################
		// #####################################################################################################

		MainSystem MainSystem_
		{
			{
				&LoadEtsi004_Tx,
				&DvQkdLdpcTxParameterEstimation_Tx,
				&CvQokdLdpcTxSindromeReconciliation_Tx,
				&SaveAscii_Tx,
//				&SaveRawAscii_Tx,
				&DvQkdLdpcTxMessageProcessorTransmitter_Tx,
				&IPTunnel_Client_Tx,
				&IPTunnel_Server_Tx,
				&DvQkdLdpcTxMessageProcessorReceiver_Tx,
			}
		};

		// #####################################################################################################
		// ##################################### Console #######################################################
		// #####################################################################################################

		Console Console_;

		BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Local_Machine_Address_{ &IPTunnel_Server_Tx, std::mem_fn(&IPTunnel::getLocalMachineIpAddress) };
		Console_.addGetFunction("Local Machine Address", &Local_Machine_Address_, value_type::t_string);

		BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Local_Machine_Port_{ &IPTunnel_Server_Tx, std::mem_fn(&IPTunnel::getLocalMachinePort) };
		Console_.addGetFunction("Local Machine Receiving Port", &Local_Machine_Port_, value_type::t_int);

		BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Remote_Machine_Address_{ &IPTunnel_Client_Tx, std::mem_fn(&IPTunnel::getRemoteMachineIpAddress) };
		Console_.addGetFunction("Remote Machine Address", &Remote_Machine_Address_, value_type::t_string);

		BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Remote_Machine_Port_{ &IPTunnel_Client_Tx, std::mem_fn(&IPTunnel::getRemoteMachinePort) };
		Console_.addGetFunction("Remote Machine Receiving Port", &Remote_Machine_Port_, value_type::t_int);

		//BlockGetFunction<std::_Mem_fn<std::string(LoadAscii::*)() const>, LoadAscii, std::string> Load_File_Name_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getAsciiFileFullName) };
		//Console_.addGetFunction("Load File Name", &Load_File_Name_, value_type::t_string);

		//BlockGetFunction<std::_Mem_fn<unsigned long int (LoadAscii::*)() const>, LoadAscii, unsigned long int> Number_Of_Loaded_Values_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getNumberOfLoadedValues) };
		//Console_.addGetFunction("Number of Loaded Values", &Number_Of_Loaded_Values_, value_type::t_unsigned_long_int);

		BlockGetFunction<std::_Mem_fn<bool (DvQkdLdpcTxParameterEstimation::*)() const>, DvQkdLdpcTxParameterEstimation, bool> Bypass_Parameter_Estimation_{ &DvQkdLdpcTxParameterEstimation_Tx, std::mem_fn(&DvQkdLdpcTxParameterEstimation::getBypassParameterEstimation) };
		Console_.addGetFunction("Bypass Parameter Estimation", &Bypass_Parameter_Estimation_, value_type::t_bool);

		BlockGetFunction<std::_Mem_fn<double (DvQkdLdpcTxParameterEstimation::*)() const>, DvQkdLdpcTxParameterEstimation, double> Total_Number_Of_Input_Bits_{ &DvQkdLdpcTxParameterEstimation_Tx, std::mem_fn(&DvQkdLdpcTxParameterEstimation::getTotalNumberOfInputBits) };
		Console_.addGetFunction("Total Number Of Input Bits", &Total_Number_Of_Input_Bits_, value_type::t_double);

		BlockGetFunction<std::_Mem_fn<double (DvQkdLdpcTxParameterEstimation::*)() const>, DvQkdLdpcTxParameterEstimation, double> Estimated_BER_{ &DvQkdLdpcTxParameterEstimation_Tx, std::mem_fn(&DvQkdLdpcTxParameterEstimation::getAverageEstimatedBER) };
		Console_.addGetFunction("Estimated BER", &Estimated_BER_, value_type::t_double);

		BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcTxSindromeReconciliation::*)() const>, CvQokdLdpcTxSindromeReconciliation, unsigned long int> NumberOfCalculatedSindromes_Tx{ &CvQokdLdpcTxSindromeReconciliation_Tx, std::mem_fn(&CvQokdLdpcTxSindromeReconciliation::getNumberOfCalculatedSindromes) };
		Console_.addGetFunction("Tx: Number Of Calculated Sindromes", &NumberOfCalculatedSindromes_Tx, value_type::t_unsigned_long_int);

		//BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcRxSindromeReconciliation::*)() const>, CvQokdLdpcRxSindromeReconciliation, unsigned long int> NumberOfValidatedSindromes_Rx{ &CvQokdLdpcRxSindromeReconciliation_Rx, std::mem_fn(&CvQokdLdpcRxSindromeReconciliation::getNumberOfValidatedSindromes) };
		//Console_.addGetFunction("Rx: Number Of Validated Sindromes", &NumberOfValidatedSindromes_Rx, value_type::t_unsigned_long_int);

		//BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcRxSindromeReconciliation::*)() const>, CvQokdLdpcRxSindromeReconciliation, unsigned long int> NumberOfNoMatchedSindromes_Rx{ &CvQokdLdpcRxSindromeReconciliation_Rx, std::mem_fn(&CvQokdLdpcRxSindromeReconciliation::getNumberOfNoMatchedSindromes) };
		//Console_.addGetFunction("Rx: Number Of No Matched Sindromes", &NumberOfNoMatchedSindromes_Rx, value_type::t_unsigned_long_int);

		//BlockGetFunction<std::_Mem_fn<double (CvQokdLdpcRxSindromeReconciliation::*)() const>, CvQokdLdpcRxSindromeReconciliation, double> PercentageOfNoMatchedSindromes_Rx{ &CvQokdLdpcRxSindromeReconciliation_Rx, std::mem_fn(&CvQokdLdpcRxSindromeReconciliation::getPercentageOfNoMatchedSindromes) };
		//Console_.addGetFunction("Rx: Percentage Of No Matched Sindromes", &PercentageOfNoMatchedSindromes_Rx, value_type::t_double);


		BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcTxSindromeReconciliation::*)() const>, CvQokdLdpcTxSindromeReconciliation, unsigned long int> NumberOfSavedPairs_Tx{ &CvQokdLdpcTxSindromeReconciliation_Tx, std::mem_fn(&CvQokdLdpcTxSindromeReconciliation::getNumberOfSavedPairs) };
		Console_.addGetFunction("Tx: Number of Saved Pairs", &NumberOfSavedPairs_Tx, value_type::t_unsigned_long_int);

		BlockGetFunction<std::_Mem_fn<unsigned long int (CvQokdLdpcTxSindromeReconciliation::*)() const>, CvQokdLdpcTxSindromeReconciliation, unsigned long int> NumberOfDiscardedPairs_Tx{ &CvQokdLdpcTxSindromeReconciliation_Tx, std::mem_fn(&CvQokdLdpcTxSindromeReconciliation::getNumberOfDiscardedPairs) };
		Console_.addGetFunction("Tx: Number of Discarded Pairs", &NumberOfDiscardedPairs_Tx, value_type::t_unsigned_long_int);

		BlockGetFunction<std::_Mem_fn<double (CvQokdLdpcTxSindromeReconciliation::*)() const>, CvQokdLdpcTxSindromeReconciliation, double> PercentageOfDiscardedPairs_Tx{ &CvQokdLdpcTxSindromeReconciliation_Tx, std::mem_fn(&CvQokdLdpcTxSindromeReconciliation::getPercentageOfDiscardedPairs) };
		Console_.addGetFunction("Tx: Percentage Of Discarded Pairs", &PercentageOfDiscardedPairs_Tx, value_type::t_double);

		MainSystem_.setConsole(&Console_);
		MainSystem_.setShowConsole(true);
		MainSystem_.setMsgOpenConsole("Tx System Console");
		MainSystem_.setShowConsolePeriodicity(100);
		MainSystem_.setLoadedInputParameters(param.getLoadedInputParameters());

		// #####################################################################################################
		// #################################### System Run #####################################################
		// #####################################################################################################

		MainSystem_.setLogValue(param.logValue);
		MainSystem_.setVerboseMode(param.verboseMode);

		CvQokdLdpcTxSindromeReconciliation_Tx.setVerboseMode(true);

		MainSystem_.run();
		MainSystem_.terminate();

		// #####################################################################################################

		auto finish = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> elapsed = finish - start;
		std::cout << "Elapsed time: " << elapsed.count() << " s\n";
		system("pause");

		return 0;
	}
}

int main(int argc, char* argv[])
{
	tx::main(argc, argv);
	//rx::main(argc, argv);

	return 0;
}
