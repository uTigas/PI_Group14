#ifndef DV_QKD_LDPC_SDF_H
#define DV_QKD_LDPC_SDF_H

#include "netxpto_20200819.h"
//#include "load_etsi_004_20200819.h"


class MainSystem : public System
{

public:
	MainSystem(std::initializer_list<Block*> MainSystem) : System(MainSystem) {};

};


//double getPhotonsPerPulse(DvQkd_LDPC_Tx* pDvQkd_LDPC_Tx) { pDvQkd_LDPC_Tx->getPhotonsPerPulse(); }


class DvQkdLdpcInputParameters : public SystemInputParameters {

public:
	//INPUT PARAMETERS

	t_string txIpAddress{"127.0.0.1"};
	t_string rxIpAddress{ "127.0.0.1" };
	t_integer txReceivingPort{ 54000 };
	t_integer rxReceivingPort{ 54001 };
	t_integer etsiPort{ 24300 };
	t_string etsiAddress{ "127.0.0.1" };
	t_string node1_uuid{ "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa" };
	t_string node2_uuid{ "bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbbbbbb" };
	t_string App{ "AtoB" };
	t_string keyStreamId{ "" };
	t_string keySource{ "ascii" };
	t_string rawKeyFolder{ "rawKeys" };
	t_bool saveKeys{ true };

	t_string defaultInputFname{ "input_parameters_0.txt" };
	t_bool verboseMode{ true };
	t_bool saveSignals{ true };
	t_bool saveSymmetricKeys{ true };
	t_bool logValue{ true };
	t_integer buffSize{ 4096 };
	t_bool bypassParameterEstimation{ false };
	t_bool bypassErrorCorrection{ false };
	t_bool bypassPrivacyAmplification{ false };

	double bitRate{ 1e3 };

	t_string errorCorrectionMethod{ "LDPC" };
	t_string parityCheckMatrixFileName{ "sparseParityCheckMatrix.txt" };
	t_string parityCheckMatrixFolderName{ "." };
	t_integer ldpcErrorCorrectionAlgorithMaxIterations{ 5 };
	t_integer codeLength{ 1944 };
	t_integer codeRank{ 972 };
	t_integer numberOfNonZerosValuesInSparseParityCheckMatrix{ 6885 };
	t_integer hashLength{ 0 };

	t_integer asciiFileNameTailNumberModulos{ 5 };
	

	t_real probabilityOfDoubleClick{ 1e-7 };
	t_integer doubleClickNumber{ 2 };
	t_integer noClickNumber{ 3 };

	int numberOfBits{ 2'000'000'000 };

	t_real qber{ 0.03 };
	t_integer numberOfDataBitsPerBitFillingPeriod{ 0 };
	t_integer numberOfDummyBitsPerBitFillingPeriod{ 0 };

	// 04-2024
	// KMS
	t_string sthIpAddress{"127.0.0.1"};
	t_string kpsIpAddress{"127.0.0.1"};
	t_integer sthPort{54000};
	t_integer kpsPort{54001};

	t_string nthIpAddress{"127.0.0.1"};
	t_string appIpAddress{"127.0.0.1"};
	t_integer nthPort{54002};
	t_integer appPort{54003};

	t_string syncAipAddress{"127.0.0.1"};
	t_string syncBipAddress{"127.0.0.1"};
	t_integer syncAport{54004};
	t_integer syncBport{54005};

	t_integer saveType{0};
	t_integer numKeys{ 50 };
	std::string etsiSource{ "KMS" };
	std::string etsiDest{ "KeyProvider" };
	t_integer keyType{ 0 };
	t_integer keyChunkSize{ 32 };
	t_integer etsiMode{ 0 };
	t_integer maxBps{ 0 };
	t_integer minBps{ 0 };
	t_integer jitter{ 0 };
	t_integer priority{ 0 };
	t_integer timeout{ 0 };
	t_integer ttl{ 0 };
	std::string metaMimetype{ "JSON" };

	t_integer fileType{ 0 };
	t_string cntFirstVal{ "0" };
	t_integer cntLastVal{ INT_MAX };
	std::string txFileName{ "sym_tx" };
	std::string rxFileName{ "sym_rx" };
	std::string provider{ "Tx" };
	t_integer seed{ 0 };
	//


	t_integer parameterEstimationNBits{ 100 };
	t_integer parameterEstimationNBitsToStart{ 100 };

	t_integer minimumSyndromeSize{ 50000 };
	t_integer maximumSyndromeSize{ minimumSyndromeSize };


	double detectorEfficiency{ 0.25 };			// percentage
	double fiberAttenuation_dB{ 0.2 };			// dB/km
	double fiberLength{ 50 };					// km
	double insertionLosses_dB{ 2 };				// dB
	double numberOfPhotonsPerInputPulse{ 0.1 };	// statistical average
	double deadTime{ 0.00001 };	// statistical average
	bool printRecv{ false };
	double qberThreshold{ 0.15 };
	double confidenceInterval{ 0.95 };
	t_string ipAddress{ "127.0.0.1" };

	// Initializes default input parameters
	DvQkdLdpcInputParameters() : SystemInputParameters() {
		initializeInputParameterMap();
	}

	// Initializes input parameters according to the program arguments
	// Usage: .\bb84_st_sdf.exe <input_parameters.txt> <output_directory>
	DvQkdLdpcInputParameters(int argc, char* argv[]) : SystemInputParameters(argc, argv) {
		setInputParametersFileName(defaultInputFname);
		initializeInputParameterMap();
		readSystemInputParameters();
	}

	// Each parameter must be added to the parameter map by calling addInputParameter(std::string,param*)
	void initializeInputParameterMap()
	{
		addInputParameter("verboseMode", &verboseMode);
		addInputParameter("saveSignals", &saveSignals);
		addInputParameter("saveSymmetricKeys", &saveSymmetricKeys);
		addInputParameter("logValue", &logValue);
		addInputParameter("txIpAddress", &txIpAddress);
		addInputParameter("txReceivingPort", &txReceivingPort);
		addInputParameter("rxIpAddress", &rxIpAddress);
		addInputParameter("rxReceivingPort", &rxReceivingPort);
		addInputParameter("asciiFileNameTailNumberModulos", &asciiFileNameTailNumberModulos);
		addInputParameter("buffSize", &buffSize);
		addInputParameter("bypassParameterEstimation", &bypassParameterEstimation);
		addInputParameter("bypassErrorCorrection", &bypassErrorCorrection);
		addInputParameter("bypassPrivacyAmplification", &bypassPrivacyAmplification);
		addInputParameter("bitRate", &bitRate);
		addInputParameter("parityCheckMatrixFolderName", &parityCheckMatrixFolderName);
		addInputParameter("parityCheckMatrixFileName", &parityCheckMatrixFileName);
		addInputParameter("codeLength", &codeLength);
		addInputParameter("codeRank", &codeRank);
		addInputParameter("ldpcErrorCorrectionAlgorithMaxIterations", &ldpcErrorCorrectionAlgorithMaxIterations);
		addInputParameter("numberOfNonZerosValuesInSparseParityCheckMatrix", &numberOfNonZerosValuesInSparseParityCheckMatrix);
		addInputParameter("hashLength", &hashLength);
		addInputParameter("numberOfBits", &numberOfBits);
		addInputParameter("parameterEstimationNBits", &parameterEstimationNBits);
		addInputParameter("parameterEstimationNBitsToStart", &parameterEstimationNBitsToStart);
		addInputParameter("errorCorrectionMethod", &errorCorrectionMethod);
		addInputParameter("maximumSyndromeSize", &maximumSyndromeSize);
		addInputParameter("minimumSyndromeSize", &minimumSyndromeSize);
		addInputParameter("qber", &qber);
		addInputParameter("numberOfDataBitsPerBitFillingPeriod", &numberOfDataBitsPerBitFillingPeriod);
		addInputParameter("numberOfDummyBitsPerBitFillingPeriod", &numberOfDummyBitsPerBitFillingPeriod);
		addInputParameter("detectorEfficiency", &detectorEfficiency);
		addInputParameter("fiberAttenuation_dB", &fiberAttenuation_dB);
		addInputParameter("fiberLength", &fiberLength);
		addInputParameter("insertionLosses_dB", &insertionLosses_dB);
		addInputParameter("numberOfPhotonsPerInputPulse", &numberOfPhotonsPerInputPulse);
		addInputParameter("deadTime", &deadTime);
		addInputParameter("printRecv", &printRecv);
		addInputParameter("qberThreshold", &qberThreshold);
		addInputParameter("confidenceInterval", &confidenceInterval);
		addInputParameter("keyType", &keyType);
		addInputParameter("keyChunkSize", &keyChunkSize);
		addInputParameter("etsiMode", &etsiMode);
		addInputParameter("maxBps", &maxBps);
		addInputParameter("minBps", &minBps);
		addInputParameter("jitter", &jitter);
		addInputParameter("priority", &priority);
		addInputParameter("timeout", &timeout);
		addInputParameter("ttl", &ttl);
		addInputParameter("metaMimetype", &metaMimetype);
		addInputParameter("etsiSource", &etsiSource);
		addInputParameter("etsiDest", &etsiDest);
		addInputParameter("fileType", &fileType);
		addInputParameter("cntFirstVal", &cntFirstVal);
		addInputParameter("cntLastVal", &cntLastVal);
		addInputParameter("txFileName", &txFileName);
		addInputParameter("rxFileName", &rxFileName);
		addInputParameter("seed", &seed);
		addInputParameter("numKeys", &numKeys);
		addInputParameter("provider", &provider);

		addInputParameter("sthIpAddress", &sthIpAddress);
		addInputParameter("kpsIpAddress", &kpsIpAddress);
		addInputParameter("sthPort", &sthPort);
		addInputParameter("kpsPort", &kpsPort);

		addInputParameter("nthIpAddress", &nthIpAddress);
		addInputParameter("appIpAddress", &appIpAddress);
		addInputParameter("nthPort", &nthPort);
		addInputParameter("appPort", &appPort);

		addInputParameter("syncAipAddress", &syncAipAddress);
		addInputParameter("syncBipAddress", &syncBipAddress);
		addInputParameter("syncAport", &syncAport);
		addInputParameter("syncBport", &syncBport);

		addInputParameter("saveType", &saveType);
		
	}
};

#endif