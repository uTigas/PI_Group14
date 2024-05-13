#ifndef DV_QKD_LDPC_RX_ERROR_CORRECTION_H_
#define DV_QKD_LDPC_RX_ERROR_CORRECTION_H_

#include "netxpto_20200819.h"
#include "dv_qkd_message_processor_common_20200819.h"

#ifdef __linux__
#include "../Eigen/Sparse"
#endif


class DvQkdLdpcRxErrorCorrection : public Block
{

public:

	//##############################################################################################################

	DvQkdLdpcRxErrorCorrection(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

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

	void setNumberOfNonZerosValuesInSparseParityCheckMatrix(Eigen::Index  nOfNonZerosValuesInSparseParityCheckMatrix) { numberOfNonZerosValuesInSparseParityCheckMatrix = nOfNonZerosValuesInSparseParityCheckMatrix; }
	Eigen::Index getNumberOfNonZerosValuesInSparseParityCheckMatrix(void) const { return  numberOfNonZerosValuesInSparseParityCheckMatrix; }

	double getTotalNumberOfInputBits(void) { return totalNumberOfInputBits; }

	double getTotalNumberOfOutputBits(void) { return totalNumberOfOutputBits; }

	unsigned long int getNumberOfCalculatedSindromes() const { return numberOfCalculatedSindromes; }

	unsigned long int getNumberOfNoMatchedSindromes() const { return numberOfNoMatchedSindromes; }

	double getPercentageOfNoMatchedSindromes() const { return ((double) numberOfNoMatchedSindromes / (double) numberOfCalculatedSindromes * 100); }
	
	t_integer getNumberOfDataBitsPerBitFillingPeriod() const { return numberOfDataBitsPerBitFillingPeriod; }
	void setNumberOfDataBitsPerBitFillingPeriod(t_integer nOfDataBitsPerBitFillingPeriod) { numberOfDataBitsPerBitFillingPeriod = nOfDataBitsPerBitFillingPeriod; }

	t_integer getNumberOfDummyBitsPerBitFillingPeriod() const { return numberOfDummyBitsPerBitFillingPeriod; }
	void setNumberOfDummyBitsPerBitFillingPeriod(t_integer nOfDummyBitsPerBitFillingPeriod) { numberOfDummyBitsPerBitFillingPeriod = nOfDummyBitsPerBitFillingPeriod; }

private:

	// Input Parameters #####################################################################################################

	DvQkdLdpcRxErrorCorrection::t_role errorCorrectionRole{ t_role::Rx };
	DvQkdLdpcRxErrorCorrection::t_method errorCorrectionMethod{ t_method::LDPC };
	DvQkdLdpcRxErrorCorrection::t_mode errorCorrectionMode{ t_mode::InverseReconciliation };

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

	t_real totalNumberOfInputBits{ 0.0 };
	t_real totalNumberOfOutputBits{ 0.0 };

	unsigned long int numberOfCalculatedSindromes{ 0 };
	unsigned long int numberOfNoMatchedSindromes{ 0 };

	t_bool sindromeMatch{ false };
	
	CircularBuffer<t_binary> DataIn{ 0 };
	CircularBuffer<t_binary> DataOut{ 0 };
	CircularBuffer<t_binary> SindromeIn{ 0 };
	CircularBuffer<t_binary> SindromeOut{ 0 };
	
	// State variables for the LDPC method ###################################################################################

	t_bool startSendingSindrome{ false };

	Eigen::SparseMatrix<t_binary> sparseParityCheckMatrix;
	Eigen::SparseMatrix<t_binary> sparseParityCheckMatrixTranspose;

	Eigen::Matrix<t_binary, Eigen::Dynamic, 1> calculatedSindrome;
	Eigen::Index calculatedSindromeSize{ 0 };
	Eigen::Index calculatedSindrome_posIn{ 0 };
	Eigen::Index calculatedSindrome_posOut{ 0 };
	t_bool calculatedSindrome_newData{ false };

	Eigen::Matrix<t_binary, Eigen::Dynamic, 1> receivedSindrome;
	Eigen::Index receivedSindromeSize{ 0 };
	Eigen::Index receivedSindrome_posIn{ 0 };
	Eigen::Index receivedSindrome_posOut{ 0 };
	t_bool receivedSindrome_newData{ false };

	Eigen::Matrix<t_binary, Eigen::Dynamic, 1> receivedData;
	Eigen::Index receivedDataSize{ 0 };
	Eigen::Index receivedData_posIn{ 0 };
	Eigen::Index receivedData_posOut{ 0 };
	t_bool receivedData_newData{ false };

	Eigen::Matrix<t_real, Eigen::Dynamic, 1> prioriMessageProbabilities;
	
	Eigen::SparseMatrix<t_real> M;
	Eigen::SparseMatrix<t_real> E;

	typedef Eigen::Triplet<t_real> T;
	std::vector<T> tripletList;

	// Bit filling ###########################################################################################################

	t_integer inDataBitFillingCounter{ 0 };
	t_integer outDataBitFillingCounter{ 0 };
	t_integer bitFillingPeriod{ numberOfDataBitsPerBitFillingPeriod + numberOfDummyBitsPerBitFillingPeriod };

	// Methods ###############################################################################################################

	t_bool errorCorrectionLDPCAlgorithm(void);
	void loadSparseParityCheckMatrix(void);

};

#endif
