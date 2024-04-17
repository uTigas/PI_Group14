#ifndef CV_QOKD_LDPC_TX_ERROR_CORRECTION_H_
#define CV_QOKD_LDPC_TX_ERROR_CORRECTION_H_

#include "netxpto_20200819.h"
#include "dv_qkd_message_processor_common_20200819.h"
#include "hex.h"
#include "sha.h"
#include "cryptlib.h"
#include "files.h"

//#ifdef __linux__ 
//#include "../Eigen/Sparse" // required for the Makefile
//#endif


class CvQokdLdpcTxSindromeReconciliation : public Block
{

public:

	//##############################################################################################################

	CvQokdLdpcTxSindromeReconciliation(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	enum class t_role { Tx, Rx, Undefined };
	enum class t_method { Cascade, LDPC };
	enum class t_mode { DirectReconciliation, InverseReconciliation, Undefined };

	//##############################################################################################################

	void setBypassErrorCorrection(t_bool bErrorCorrection) { bypassErrorCorrection = bErrorCorrection; }

	void setErrorCorrectionRole(t_role eCorrectionRole) { errorCorrectionRole = eCorrectionRole; }
	t_role getErrorCorrectionRole(void) const { return errorCorrectionRole; }

	void setErrorCorrectionMode(t_mode eCorrectionMode) { errorCorrectionMode = eCorrectionMode; }
	t_mode getErrorCorrectionMode(void) const { return errorCorrectionMode; }

	void setParityCheckMatrixFileName(t_string pCheckMatrixFileName) { parityCheckMatrixFileName = pCheckMatrixFileName; }
	t_string getParityCheckMatrixFileName(void) { return parityCheckMatrixFileName; }

	void setParityCheckMatrixFolderName(t_string pCheckMatrixFolderName) { parityCheckMatrixFolderName = pCheckMatrixFolderName; }
	t_string getParityCheckMatrixFolderName(void) { return parityCheckMatrixFolderName; }

	void setLdpcErrorCorrectionAlgorithMaxIterations(t_integer lErrorCorrectionAlgorithMaxIterations) { ldpcErrorCorrectionAlgorithMaxIterations = lErrorCorrectionAlgorithMaxIterations; };
	t_integer getLdpcErrorCorrectionAlgorithMaxIterations() const { return ldpcErrorCorrectionAlgorithMaxIterations; };

	void setCodeLength(Eigen::Index cLength) { codeLength = cLength; }
	Eigen::Index getCodeLength(void) const { return codeLength; }

	void setCodeRank(Eigen::Index cRank) { codeRank = cRank; }
	Eigen::Index getCodeRank(void) const { return codeRank; }

	void setKeyType(int keyType)  { sym = keyType; }

	void setNumberOfNonZerosValuesInSparseParityCheckMatrix(Eigen::Index  nOfNonZerosValuesInSparseParityCheckMatrix) { numberOfNonZerosValuesInSparseParityCheckMatrix = nOfNonZerosValuesInSparseParityCheckMatrix; }
	Eigen::Index getNumberOfNonZerosValuesInSparseParityCheckMatrix(void) const { return  numberOfNonZerosValuesInSparseParityCheckMatrix; }

	double getTotalNumberOfInputBits(void) { return totalNumberOfInputBits; }

	double getTotalNumberOfOutputBits(void) { return totalNumberOfOutputBits; }

	unsigned long int getNumberOfCalculatedSindromes() const { return numberOfCalculatedSindromes; }

	unsigned long int getNumberOfValidatedPairs() const { return numberOfValidatedPairs; }

	unsigned long int getNumberOfDiscardedPairs() const { return numberOfDiscardedPairs; }

	unsigned long int getNumberOfSavedPairs() const { return numberOfSavedPairs; }

	double getPercentageOfDiscardedPairs() const { return ((double)numberOfDiscardedPairs / (double)numberOfValidatedPairs * 100); }
	
	t_integer getNumberOfDataBitsPerBitFillingPeriod() const { return numberOfDataBitsPerBitFillingPeriod; }
	void setNumberOfDataBitsPerBitFillingPeriod(t_integer nOfDataBitsPerBitFillingPeriod) { numberOfDataBitsPerBitFillingPeriod = nOfDataBitsPerBitFillingPeriod; }

	t_integer getNumberOfDummyBitsPerBitFillingPeriod() const { return numberOfDummyBitsPerBitFillingPeriod; }
	void setNumberOfDummyBitsPerBitFillingPeriod(t_integer nOfDummyBitsPerBitFillingPeriod) { numberOfDummyBitsPerBitFillingPeriod = nOfDummyBitsPerBitFillingPeriod; }

private:

	// Input Parameters #####################################################################################################

	CvQokdLdpcTxSindromeReconciliation::t_role errorCorrectionRole{ t_role::Tx };
	CvQokdLdpcTxSindromeReconciliation::t_method errorCorrectionMethod{ t_method::LDPC };
	CvQokdLdpcTxSindromeReconciliation::t_mode errorCorrectionMode{ t_mode::DirectReconciliation };

	t_bool bypassErrorCorrection{ false };
	
	t_string parityCheckMatrixFileName{ "sparseParityCheckMatrix.txt" };
	t_string parityCheckMatrixFolderName{ "." };
	t_integer ldpcErrorCorrectionAlgorithMaxIterations{ 5 };
	Eigen::Index codeLength{ 1944 };
	Eigen::Index codeRank{ 972 };
	Eigen::Index numberOfNonZerosValuesInSparseParityCheckMatrix{ 6885 };

	t_real p_f{ 0.1 };

	t_integer numberOfDataBitsPerBitFillingPeriod{ 0 };
	t_integer numberOfDummyBitsPerBitFillingPeriod{ 0 };

	// State Variables ###############################################################################

	int process_block{ 0 };

	int sym{ 0 }; // 0 -> generate oblivious keys, 1 -> generate symmetric keys
	int indx{ 0 };

	t_real totalNumberOfInputBits{ 0.0 };
	t_real totalNumberOfOutputBits{ 0.0 };

	unsigned long int numberOfCalculatedSindromes{ 0 };

	unsigned long int numberOfValidatedPairs{ 0 };
	unsigned long int numberOfDiscardedPairs{ 0 };
	unsigned long int numberOfSavedPairs{ 0 };

	t_bool sindromeMatch{ false };
	
	CircularBuffer<t_binary> DataIn{ 0 };
	CircularBuffer<t_binary> DataOut{ 0 };
	CircularBuffer<t_binary> DataSymOut{ 0 };
	CircularBuffer<t_binary> SindromeIn{ 0 };
	CircularBuffer<t_binary> SindromeOut{ 0 };
	CircularBuffer<t_message> HashOut{ 0 };
	
	// State variables for the LDPC method ###################################################################################

	t_bool startSendingSindrome{ false };

	t_bool startedSendingSindrome_0{ false };
	t_bool startedSendingHash_0{ false };

	t_bool startedSendingSindrome_1{ false };
	t_bool startedSendingHash_1{ false };

	Eigen::SparseMatrix<t_binary> sparseParityCheckMatrix;
	Eigen::SparseMatrix<t_binary> sparseParityCheckMatrixTranspose;

	Eigen::Index calculatedSindromeSize{ 0 };

	Eigen::Matrix<t_binary, Eigen::Dynamic, 1> calculatedSindrome_0;
	Eigen::Index calculatedSindrome_0_posIn{ 0 };
	Eigen::Index calculatedSindrome_0_posOut{ 0 };
	t_bool calculatedSindrome_0_newData{ false };

	Eigen::Matrix<t_binary, Eigen::Dynamic, 1> calculatedSindrome_1;
	Eigen::Index calculatedSindrome_1_posIn{ 0 };
	Eigen::Index calculatedSindrome_1_posOut{ 0 };
	t_bool calculatedSindrome_1_newData{ false };

	t_bool calculatedSindrome_newData{ false };

	Eigen::Matrix<t_binary, Eigen::Dynamic, 1>* pt_calculatedSindrome{ &calculatedSindrome_0 };
	Eigen::Index* pt_calculatedSindromeSize{ &calculatedSindromeSize };
	Eigen::Index* pt_calculatedSindrome_posIn{ &calculatedSindrome_0_posIn };
	Eigen::Index* pt_calculatedSindrome_posOut{ &calculatedSindrome_0_posOut };
	t_bool* pt_calculatedSindrome_newData{ &calculatedSindrome_0_newData };

	std::string calculatedHash{ "0123456789ABCDEF0123" };

	Eigen::Matrix<t_binary, Eigen::Dynamic, 1> receivedSindrome;
	Eigen::Index receivedSindromeSize{ 0 };
	Eigen::Index receivedSindrome_posIn{ 0 };
	Eigen::Index receivedSindrome_posOut{ 0 };
	t_bool receivedSindrome_newData{ false };

	Eigen::Matrix<t_binary, Eigen::Dynamic, 1> receivedData_0;
	Eigen::Index receivedDataSize_0{ 0 };
	Eigen::Index receivedData_0_posIn{ 0 };
	Eigen::Index receivedData_0_posOut{ 0 };
	t_bool receivedData_0_newData{ false };

	Eigen::Matrix<t_binary, Eigen::Dynamic, 1> receivedData_1;
	Eigen::Index receivedDataSize_1{ 0 };
	Eigen::Index receivedData_1_posIn{ 0 };
	Eigen::Index receivedData_1_posOut{ 0 };
	t_bool receivedData_1_newData{ false };

	t_bool receivedData_newData{ false };

	Eigen::Matrix<t_binary, Eigen::Dynamic, 1>* pt_receivedData{ &receivedData_0 };
	Eigen::Index* pt_receivedDataSize{ &receivedDataSize_0 };
	Eigen::Index* pt_receivedData_posIn{ &receivedData_0_posIn };
	Eigen::Index* pt_receivedData_posOut{ &receivedData_0_posOut };
	t_bool* pt_receivedData_newData{ &receivedData_0_newData };

	Eigen::Matrix<t_real, Eigen::Dynamic, 1> prioriMessageProbabilities;
	
	Eigen::SparseMatrix<t_real> M;
	Eigen::SparseMatrix<t_real> E;

	typedef Eigen::Triplet<t_real> T;
	std::vector<T> tripletList;

	// Bit filling ###########################################################################################################

	t_integer inDataBitFillingCounter_0{ 0 };
	t_integer outDataBitFillingCounter_0{ 0 };

	t_integer inDataBitFillingCounter_1{ 0 };
	t_integer outDataBitFillingCounter_1{ 0 };

	t_integer* pt_inDataBitFillingCounter{ &inDataBitFillingCounter_0 };
	t_integer* pt_outDataBitFillingCounter{ &outDataBitFillingCounter_0 };


	t_integer bitFillingPeriod{ numberOfDataBitsPerBitFillingPeriod + numberOfDummyBitsPerBitFillingPeriod };

	// Methods ###############################################################################################################

	t_bool errorCorrectionLDPCAlgorithm(Eigen::Matrix<t_binary, Eigen::Dynamic, 1>* pt_calculatedSindrome, Eigen::Matrix<t_binary, Eigen::Dynamic, 1>* pt_receivedData);
	void loadSparseParityCheckMatrix(void);

};

#endif
