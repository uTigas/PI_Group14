#ifndef ERROR_CORRECTION_H_
#define ERROR_CORRECTION_H_

#include "netxpto_20200819.h"
#include "dv_qkd_message_processor_common_20200819.h"

class ErrorCorrection : public Block 
{

public:

	//##############################################################################################################

	ErrorCorrection(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	enum class t_role { Tx, Rx, Undefined };
	enum class t_method { Cascade, LDPC };

	//##############################################################################################################

	void setErrorCorrectionRole(ErrorCorrection::t_role eCorretionRole) { errorCorrectionRole = eCorretionRole; }
	ErrorCorrection::t_role getErrorCorrectionRole() { return errorCorrectionRole; }

	void setErrorCorrectionMethod(ErrorCorrection::t_method eCorretionMethod) { errorCorrectionMethod = eCorretionMethod; }
	ErrorCorrection::t_method getErrorCorrectionMethod() { return errorCorrectionMethod; }

	void setPartitionSize(t_integer wsz) { if (wsz < 0 || wsz > 150) { wsz = 150; } partitionSize = wsz; currentPartitionSize = wsz; }

	void setBer(t_real berValue) { BER = berValue; partitionSize = ber2partitionSize(q, berValue); }

	void setNumberOfPasses(t_integer np) { numPasses = np; }

	void setDoublePartitionSize(bool db) { doubleWindowSize = db; }

	void setMaximumSyndromeSize(t_integer maxss) { maximumSyndromeSize = maxss; }

	void setMinimumSyndromeSize(t_integer minss) { minimumSyndromeSize = minss; }

	void setMinimumNumberOfPartitions(t_integer mnp) { minimumNumberOfPartitions = mnp; }

	int64_t getInputBits(void) { return inputBits; }

	int64_t getOutputBits(void) { return outputBits; }

	int64_t getNumberOfExchangedParities(void) { return measuredParities; }

private:

	// Input Parameters #####################################################################################################

	ErrorCorrection::t_role errorCorrectionRole{ ErrorCorrection::t_role::Undefined };
	ErrorCorrection::t_method errorCorrectionMethod{ ErrorCorrection::t_method::Cascade };

	// Input Parameters for the CASCADE method ##############################################################################

	t_integer partitionSize{ 10 };
	t_integer numPasses{ 4 };
	t_bool doubleWindowSize{ false };
	t_integer minimumSyndromeSize{ 512 };
	t_integer maximumSyndromeSize{ 512 };	// Maximum buffer sizes limit this. Can be increased as long as it is not higher than the variables
	t_real BER{ 0 };										// maximumECSyndromeSize on the DvQkdTx and DvQkdRx blocks. This variable cannot be defined after initialization, as
	t_real minimumBER{ 0.005 };										// currently it is not possible to change the buffer sizes.

	// Input Parameters for the LDPC method #################################################################################

	Eigen::Index codeLength{ 1944 };
	Eigen::Index codeRank{ 972 };
	Eigen::Index numberOfNonZerosValuesInParityCheckMatrix{ 6885 };

	// State Variables ######################################################################################################

	// State variables for the CASCADE method ###############################################################################

	int64_t inputBits{ 0 };
	int64_t outputBits{ 0 };
	int64_t measuredParities{ 0 };

	t_integer partitionSizeLB{ 0 };
	bool alreadyUsedBER{ true };
	
	t_integer numBits{ 0 };
	t_integer maxData{ 512 };
	t_integer currentPartitionSize{ 10 };
	unsigned int currentPass{ 0 };
	t_integer numberOfPartitions{ 0 };
	bool errorCorrectionStatus{ false };
	bool currentPassStarted{ false };
	bool lookBackDone{ false };
	bool doingLookBack{ false };
	bool lookBackStarted{ false };
	std::vector <t_integer> scrambledKey{};
	std::vector <t_integer> scrambledKeyIdx{};
	std::vector<t_integer> startPositions{};
	std::vector<t_integer> endPositions{};
	std::vector<t_integer> totalSearchSpace{};
	std::vector <t_integer> parities{};
	std::vector <t_integer> parityResponses{};
	std::vector <t_integer> lookBackIdx{};
	t_integer parityCount{ 0 };
	t_integer testCount{ 0 };
	std::vector <t_integer> scramblingIdx{};
	//t_integer seed{ 129237 };
	std::vector<t_integer> dataOnHold{};
	t_integer correctParitiesInLoopBack{ 0 };
	t_integer correctedInLoopBack{ 0 };
	t_integer correctedBitsInLoopBack{ 0 };
	t_integer correctedBitsInPass{ 0 };
	t_integer minimumNumberOfPartitions{ 10 };
	t_integer correctedInLookBack{ 0 };
	t_integer correctParitiesInLookBack{ 0 };
	t_integer correctedBitsInLookBack{ 0 };
	unsigned int lookBackPass{ 0 };
	std::vector <std::vector <t_integer>> lookBackPassesIdx;
	std::vector <std::vector <t_integer>> keyIdx;
	std::vector <std::vector <t_integer>> reverseKeyIdx;
	std::vector <std::vector <t_integer>> keys;
	t_real q{ 0.73 };
	int leftoverBits{ 0 }; // There may be a difference between the number of processed bits and the number of received bits
	std::vector <t_integer> paritiesMsgOutput{};
	bool finishedSendingParitiesMsg{ true };
	bool sendingParitiesMsg{ false };
	std::vector <t_integer> paritiesMsgInput{};
	bool finishedReceivingParitiesMsg{ true };
	bool receivingParitiesMsg{ false };
	t_integer numberOfParitiesToReceive{ 0 };
	bool permutationsNotCalculated{ true };

	// State variables for the LDPC method ###################################################################################

	Eigen::SparseMatrix<t_binary> sparseParityCheckMatrix;

	Eigen::Matrix<t_binary, Eigen::Dynamic, 1> sindrome;
	size_t sindromeSize{ 0 };
	Eigen::Index sindrome_posOut{ 0 };

	Eigen::Matrix<t_binary, Eigen::Dynamic, 1> receivedData;
	Eigen::Index posIn{ 0 };
	Eigen::Index posOut{ 0 };



	// Private methods
//	t_integer getParity(std::vector<t_integer> &scrambledKeyVector, const t_integer startPosition, const t_integer endPosition, t_integer& count, vector<t_integer>& originalKey, const vector<t_integer>& originalKeyIdx);
	t_integer getParity(std::vector<t_integer> &keyVector, const t_integer startPosition, const t_integer endPosition, t_integer& count, std::vector<std::vector<t_integer>>& keyVectors, const std::vector<std::vector<t_integer>>& keyIdxVectors, const std::vector<std::vector<t_integer>>& reverseKeyIdxVectors, const unsigned int pass);
	bool updateStartAndEndPositions(const t_integer& aliceParityResponse, t_integer& startPosition, t_integer& endPosition, const t_integer windowSize, const t_integer totalSearchSize);
	t_integer correctBit(t_integer bitValue);
	bool outputKey(std::vector<Signal*> outputSignals, std::vector<t_integer> &correctedKey);
//	bool generatePermutationsIdx(const vector <t_integer>& originalKey, vector <t_integer>& scrambledKey, vector <t_integer>& scrambledKeyIdx);
	bool generatePermutationsIdx_testing(const t_seed seed, const std::vector <t_integer>& originalKey, std::vector <t_integer>& scrambledKey, std::vector <t_integer>& scrambledKeyIdx, std::vector <t_integer>& newReverseScrambledKeyIdx);
	t_integer ber2partitionSize(t_real qValue, t_real BER);
//	bool scrambleKeyWithMatrix(const t_integer M, const t_integer N, t_integer& keyIdx, const vector<t_integer>& ORIGINAL_KEY, vector<t_integer>& scrambledKey, vector<t_integer>& bitsIdx);

};

#endif // !ERROR_CORRECTION_H_
