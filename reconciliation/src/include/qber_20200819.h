# ifndef QBER_H
# define QBER_H

# include "netxpto_20200819.h"
# include "ms_windows_console_output_common_20200819.h"
enum class sync_mode { On_BobAhead, On_AliceAhead, Off };

class QBer : public Block {
public:
	//##############################################################################################################

	QBer(std::vector<Signal*> InputSig, std::vector<Signal*> OutputSig) : Block(InputSig, OutputSig) {}; 

	void initialize(void);
	bool runBlock(void);

	void setSyncMode(sync_mode sMode) { syncMode = sMode; }
	sync_mode const getSyncMode(void) { return syncMode; }

	void setBerLimit(double bLimit) { berLimit = bLimit; }
	double const getBerLimit(void) { return berLimit; }

	void setBufferSize(int bSize) { bufferSize = bSize; }
	int const getBufferSize(void) { return bufferSize; }

	void setSaveReport(bool sReport) { midReport = sReport; };

	void setNumberOfSamplesToProcess(long int nOfSamplesToProcess) { numberOfSamplesToProcess = nOfSamplesToProcess; };
	long int getNumberOfSamplesToProcess() const { return numberOfSamplesToProcess; };

	//##############################################################################################################
	//###############################################################################################################
	double alpha = 0.05;
	int limit = 0;

private:
	//################################State variables################################################################
	long int numberOfSamplesToProcess{ -1 };
	long int numberOfProcessedSamples{ 0 };
	double z{ 0 };
	int firstPass{ 1 };
	double coincidences = 0.0;
	double receivedBits = 0.0;
	int posDataSeq{ 0 };
	int n{ 0 };
	int outputs{ 0 };
	double noClicks{ 0.0 };
	double doubleClicks{ 0.0 };
	double noClicksRate{ 0.0 };
	double doubleClicksRate{ 0.0 };
	double noClicksPercentage{ 0.0 };
	double doubleClicksPercentage{ 0.0 };
	double coincidences_difbasis{ 0.0 };
	int m{ 0 };
	int totalReceivedBits{ 0 };
	std::vector <int> windowBuffer;
	int window{ 1000 };
	int inBuffer{ 0 };
	bool firstFull{ true };
	double QBER{ 0.0 };
	double QBER_all{ 0.0 };
	
	
	bool emptyArray{ true };
	int cBit{ 0 };
	int numberSyncLossReport = 0;
	int inBufferSync{ 0 };
	double coincidencesSync = 0.0;
	bool discard = false;
	bool nSync = false; // if it is not synchronized, this bool is true 
	double berLimit = 15; // value of BER (in percentage) under which the sequences are assumed to be synchronized
	int bufferSize = 1000; // number of bits used to test synchronization status
	sync_mode syncMode{ sync_mode::Off }; // flag to set functioning mode

	bool midReport{ false };
	const char* folder = "reports";
	bool createFolder{ true };

};

#endif 

