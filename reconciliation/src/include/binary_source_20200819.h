# ifndef BINARY_SOURCE_H_
# define BINARY_SOURCE_H_

# include "netxpto_20200819.h"

enum class BinarySourceMode 
{ 
	PseudoRandom,
	Random,
	DeterministicCyclic,
	DeterministicAppendZeros,
	AsciiFileCyclic,
	AsciiFileAppendZeros
};

BinarySourceMode stoBinarySourceMode(std::string);

class BinarySource : public Block {

public:

	//##############################################################################################################

	BinarySource(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};


	void initialize(void);
	void terminate(void);
	bool runBlock(void);

	//##############################################################################################################

	void setBitPeriod(double bPeriod);
	double getBitPeriod(void) const { return bitPeriod; };

	void setNumberOfBits(long int nOfBits) { numberOfBits = nOfBits; };
	long int getNumberOfBits(void) const { return numberOfBits; };

	void setMode(BinarySourceMode m) { mode = m; }
	BinarySourceMode getMode(void) const { return mode; };

	void setPatternLength(int pLength) { patternLength = pLength; };
	int getPatternLength(void) const { return patternLength; };

	void setSeed(long int s) { seed = s; };
	long int getSeed(void) const { return seed; };

	void setProbabilityOfZero(double pZero) { probabilityOfZero = pZero; };
	double getProbabilityOfZero(void) const { return probabilityOfZero; };

	void setBitStream(std::string bStream) { bitStream = bStream; };
	std::string getBitStream(void) const { return bitStream; };

	void setAsciiFileName(std::string aFileName) { asciiFileName = aFileName; }
	std::string getAsciiFileName() { return asciiFileName; }

	//###############################################################################################################

private:

	//# Input Parameters ############################################################################################

	double bitPeriod{ 1e-11 };
	long int numberOfBits{ -1 };

	BinarySourceMode mode{ BinarySourceMode::PseudoRandom };

	int patternLength{ 7 };

	long int seed{ (long int)std::chrono::system_clock::now().time_since_epoch().count() };
	double probabilityOfZero{ 0.5 };

	std::string bitStream{ "01000" };

	std::string asciiFileName{ "ascii_file.txt" };
	
	//# State Variables #############################################################################################

	std::vector<int> acumul{};		// used to generate the PRBS sequence
	int posBitStream{ 0 };
	std::ifstream asciiFile;

	 //###############################################################################################################

	 void runPseudoRandom(long int process);
	 void runRandom(long int process);
	 void runDeterministicCyclic(long int process);
	 void runDeterministicAppendZeros(long int process);
	 void runAsciiFileCyclic(long int process);
	 void runAsciiFileAppendZeros(long int process);

	 //###############################################################################################################

};

# endif


