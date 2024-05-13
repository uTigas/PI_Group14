#ifndef PARAMETER_ESTIMATION_H_
#define PARAMETER_ESTIMATION_H_

#include "netxpto_20200819.h"
#include "dv_qkd_message_processor_common_20200819.h"

//enum class parameter_estimation_role { Alice, Bob, Undefined };

class ParameterEstimation : public Block {

public:

	//##############################################################################################################

	ParameterEstimation(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) :
		Block(InputSig, OutputSig),
		outputFolderName{ "signals/" },
		reportOutputFileName{ "parameter_estimation_report.txt" },
		bersListFileName{ "parameter_estimation_berList.txt" }
	{};

	ParameterEstimation(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig, std::string bFolderName) :
		Block(InputSig, OutputSig),
		outputFolderName{ bFolderName + "/" },
		reportOutputFileName{ "parameter_estimation_report.txt" },
		bersListFileName{ "parameter_estimation_berList.txt" }
	{};

	ParameterEstimation(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig, std::string bFolderName, std::string bFileName) :
		Block(InputSig, OutputSig),
		outputFolderName{ bFolderName + "/" },
		reportOutputFileName{ bFileName },
		bersListFileName{ "parameter_estimation_berList.txt" }
	{};

	ParameterEstimation(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig, std::string bFolderName, std::string bFileName, std::string bLFileName) :
		Block(InputSig, OutputSig),
		outputFolderName{ bFolderName + "/" },
		reportOutputFileName{ bFileName },
		bersListFileName{ bLFileName }
	{};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	void setConfidence(double P) { alpha = 1 - P; }
	double const getConfidence(void) { return 1 - alpha; }

	void setNumberOfBitsToUse(const unsigned int nb) { numberOfBits = nb; };
	void setNumberOfBitsRequiredToStart(const unsigned int nbs) { numberOfBitsRequiredToStart = nbs; };
	void setBypassParameterEstimation(const bool bypass) { bypassParameterEstimation = bypass; }
	//void setParameterEstimationRole(const parameter_estimation_role role) { parameterEstimationRole = role; }
	void setThreshold(const t_real thr) { threshold = thr; }
	t_real getThreshold(void) { return threshold; }

	int64_t getInputBits(void) { return totalNumberOfInputBits; }
	int64_t getOutputBits(void) { return totalNumberOfOutputBits; }
	t_real getAverageEstimatedBER(void) { return averageBER; }
	t_real getLatestEstimatedBER(void) { return BER; }
	t_real getLatestEstimatedBERUB(void) { return BERUpperBound; }
	t_real getLatestEstimatedBERLB(void) { return BERLowerBound; }

	//##############################################################################################################

private:
	// Input parameters
	bool bypassParameterEstimation{ true };
	//parameter_estimation_role parameterEstimationRole{ parameter_estimation_role::Undefined };
	int64_t numberOfBitsRequiredToStart{ 1000 };
	int numberOfBits{ 10000 };
	t_real threshold{ 0.12 };


	bool waitingForComm{ false };
	std::vector <t_integer> inputKey{};
	std::vector <t_integer> outputKey{};
	std::vector <t_integer> bitsToUse{};
	std::vector <t_integer> bitsFromAlice{};
	std::vector <t_integer> bitsFromBob{};
	t_integer bitSamplingSeed{ 1 };
	t_real BER{ 0 };
	t_real BERUpperBound{ 0 };
	t_real BERLowerBound{ 0 };

	std::vector<t_integer> msgInput{};
	std::vector<t_integer> msgOutput{};
	bool sendingMsg{ false };
	bool finishedSendingMsg{ true };
	bool startedReceivingMsg{ false };
	bool finishedReceivingMsg{ true };
	t_integer numberOfValuesToReceive{ 0 };

	// Report metrics
	bool reportUpdated{ true };
	bool berListUpdated{ true };
	std::string reportOutputFileName;
	std::string bersListFileName;
	std::string outputFolderName;

	t_real averageBER{ 0 };
	t_real highestBER{ 0 };
	t_real lowestBER{ 1 };

	t_real highestBERUpperBound{ 0 };
	t_real lowestBERUpperBound{ 1 };
	t_real highestBERLowerBound{ 0 };
	t_real lowestBERLowerBound{ 1 };

	int64_t numberOfMeasurements{ 0 };
	int64_t totalNumberOfInputBits{ 0 };
	int64_t totalNumberOfOutputBits{ 0 };

	bool doEstimation{ false };

	bool firstPass{ true };

	double coincidences{ 0.0 };
	double receivedBits{ 0.0 };
	double lowestMinorant{ 1e-10 };

	double alpha{ 0.05 };
	double z{ 0 };
//	int m{ 0 };
//	int n{ 0 };
	
	// Private methods
	bool selectBitsToUse(const t_seed seedVal, const int64_t numberOfBits, std::vector <t_integer> &vectorInputKey, std::vector <t_integer> &vectorBitsToUse, int64_t intervalSize);
	void updateReportResults(void);
//	bool sendToMsg(std::vector <t_integer>& data, Signal& signalToMsg, bool& started);
};


#endif
