#ifndef DV_QKD_TX_PARAMETER_ESTIMATION_H_
#define DV_QKD_TX_PARAMETER_ESTIMATION_H_

#include "netxpto_20200819.h"
#include "dv_qkd_message_processor_common_20200819.h"



class DvQkdLdpcTxParameterEstimation : public Block {

public:

	enum class t_role { undefined, master, slave };

	//##############################################################################################################

	DvQkdLdpcTxParameterEstimation(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) :
		Block(InputSig, OutputSig),
		outputFolderName{ "signals/" },
		reportOutputFileName{ "dv_qkd_ldpc_tx_parameter_estimation_report.txt" },
		bersListFileName{ "dv_qkd_ldpc_tx_parameter_estimation_berList.txt" }
	{};

	DvQkdLdpcTxParameterEstimation(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig, std::string bFolderName) :
		Block(InputSig, OutputSig),
		outputFolderName{ bFolderName + "/" },
		reportOutputFileName{ "dv_qkd_ldpc_tx_parameter_estimation_report.txt" },
		bersListFileName{ "dv_qkd_ldpc_tx_parameter_estimation_berList.txt" }
	{};

	DvQkdLdpcTxParameterEstimation(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig, std::string bFolderName, std::string bFileName) :
		Block(InputSig, OutputSig),
		outputFolderName{ bFolderName + "/" },
		reportOutputFileName{ bFileName },
		bersListFileName{ "dv_qkd_ldpc_tx_parameter_estimation_berList.txt" }
	{};

	DvQkdLdpcTxParameterEstimation(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig, std::string bFolderName, std::string bFileName, std::string bLFileName) :
		Block(InputSig, OutputSig),
		outputFolderName{ bFolderName + "/" },
		reportOutputFileName{ bFileName },
		bersListFileName{ bLFileName }
	{};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	bool getBypassParameterEstimation(void) const { return bypassParameterEstimation; }

	void setRole(t_role r) { role = r; };
	t_role getRole(void) const { return role; };

	void setConfidence(double P) { alpha = 1 - P; }
	double const getConfidence(void) { return 1 - alpha; }

	void setRatio(t_integer r) { ratio = r; };
	void setNumberOfBitsPerEstimationBlock(t_integer nb) { numberOfBitsPerEstimationBlock = nb; };
	void setBypassParameterEstimation(bool bypass) { bypassParameterEstimation = bypass; }
	void setParameterEstimationRole(t_role r) { role = r; }

	void setThreshold(t_real thr) { threshold = thr; }
	t_real getThreshold(void) { return threshold; }

	t_real getTotalNumberOfInputBits(void) const { return totalNumberOfInputBits; }
	t_real getTotalNumberOfOutputBits(void) const { return totalNumberOfOutputBits; }

	t_real getLatestEstimatedBER(void) const { return BER; }
	t_real getAverageEstimatedBER(void) const { return averageBER; }
	
	t_real getLatestEstimatedBERUpperBound(void) { return BERUpperBound; }
	t_real getLatestEstimatedBERLowerBound(void) { return BERLowerBound; }

	//##############################################################################################################

private:

	// Input parameters
	t_bool bypassParameterEstimation{ true };
	t_bool writeBerList{ true };
	t_role role{ t_role::master };
	t_real threshold{ 0.12 };

	// Sate variables
	t_integer seed{ 0 };
	t_integer ratio{ 10 };
	t_integer numberOfBitsPerEstimationBlock{ 100 };
	t_integer numberOfBitsSendToDataToBerEstimation{ -1 };				//-1 means seed needs to be generated
	t_integer numberOfProcessedBits{ 0 };
	
	netxpto::UniformRandomRealNumbers rand_tx{ seed,netxpto::GeneratorValue::mt19937 };

	CircularBuffer<t_binary> dataInToBerEstimation{ 0 };
	CircularBuffer<t_binary> dataToRx{ 0 };
	CircularBuffer<t_binary> dataFromRx{ 0 };
	CircularBuffer<t_integer> seed_buffer{ 10 };
	CircularBuffer<t_integer> ratio_buffer{ 10 };
	CircularBuffer<t_integer> numberOfBitsPerEstimationBlockFromRx_buffer{ 10 };

	t_real BER{ 0.0 };
	t_real BERUpperBound{ 0 };
	t_real BERLowerBound{ 0 };


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

	t_real numberOfEstimations{ 0 };
	t_real totalNumberOfInputBits{ 0 };
	t_real totalNumberOfOutputBits{ 0 };

	bool doEstimation{ false };

	t_real coincidences{ 0.0 };
	t_real receivedBits{ 0.0 };
	t_real lowestMinorant{ 1e-10 };

	t_real alpha{ 0.05 };
	t_real z{ 0 };
	
	// Private methods
	void updateReportResults(void);
};


#endif
