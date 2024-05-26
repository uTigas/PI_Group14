
# define PROGRAM_LOAD_ASCII_H_

# include "netxpto_20200819.h"


class LoadAscii : public Block {

public:

	LoadAscii(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	enum class delimiter_type { CommaSeperatedValues, ConcatenatedValues };

	void setSamplingPeriod(double sPeriod) { samplingPeriod = sPeriod; }
	double const getSamplingPeriod(void) { return samplingPeriod; }

	void setSymbolPeriod(double sPeriod) { symbolPeriod = sPeriod; }
	double const getSymbolPeriod(void) { return symbolPeriod; }

	void setDataType(signal_value_type dType) { dataType = dType; }
	signal_value_type const getDataType(void) { return dataType; }

	void setDelimiterType(delimiter_type delType) { delimiterType = delType; }
	delimiter_type const getDelimiterType(void) { return delimiterType; }

	void setAsciiFileName(t_string aFileName) { asciiFileName = aFileName; }
	t_string const getAsciiFileName(void) { return asciiFileName; }

	void setCycleModulos(bool cycle) { cycleModulos = cycle; }
	bool getCycleModulos() { return cycleModulos; }

    void setProvider(std::string Provider) { provider = Provider; }
	std::string getProvider() { return provider; }

	void setAsciiFileNameExtension(t_string aFileNameExtension) { asciiFileNameExtension = aFileNameExtension; }
	t_string const getAsciiFileNameExtension(void) { return asciiFileNameExtension; }

	void setAsciiFileNameTailNumber(t_string aFileNameTailNumber) { asciiFileNameTailNumber = aFileNameTailNumber; }
	t_string const getAsciiFileNameTailNumber(void) { return asciiFileNameTailNumber; }

	void setAsciiFileNameTailNumberModulos(t_integer aFileNameTailNumberModulos) { asciiFileNameTailNumberModulos = aFileNameTailNumberModulos; }
	t_integer const getAsciiFileNameTailNumberModulos(void) { return asciiFileNameTailNumberModulos; }

	std::string getAsciiFileFullName(void) const {
		return (asciiFileName + asciiFileNameTailNumber + asciiFileNameExtension);
	}

	void setSartColumn(int sColumn) { startColumn = sColumn; }
	int const getStartColumn(void) { return startColumn; }
	
	void setSartLine(int sLine) { startLine = sLine; }
	int const getStartLine(void) { return startLine; }

	void setMultiplyValue(double aMultiply) { auxMultiply = aMultiply; }
	double const getMultiplyValue(void) { return auxMultiply; }

	unsigned long int getNumberOfLoadedValues(void) const { return numberOfLoadedValues; }

private:

	std::streampos position = 0;
	t_bool fileOpened{ false };

	//unsigned long int index = 0;

	int sizeOfFile{ 0 };
	int ready{ 0 };
	double samplingPeriod{ 1 };
	double symbolPeriod{ 1 };
	double auxMultiply = 1;
	signal_value_type dataType{ signal_value_type::t_binary };
	delimiter_type delimiterType{ delimiter_type::ConcatenatedValues };

	bool cycleModulos = true; // mandates if the program should read files on loop (module)
    t_integer beforelastTailNumber{ 0 };
	t_integer lastTailNumber{ 0 };
	t_string asciiFileName{ "InputFile" };
	t_string asciiFileNameExtension{ ".dat" };
	t_string asciiFileNameTailNumber{ "" };
	t_integer asciiFileNameTailNumberModulos{ 0 };
    std::string provider{ "default" };
    t_binary keyType;



	unsigned long int startColumn = 1;
	unsigned long int startLine = 1;
	unsigned long int auxPosition = 0;

	unsigned long int numberOfLoadedValues{ 0 };

};