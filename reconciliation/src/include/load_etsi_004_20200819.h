# ifndef PROGRAM_LOAD_ETSI_004_H_
# define PROGRAM_LOAD_ETSI_004_H_

# include "netxpto_20200819.h"

# include "etsi_qkd_004.hpp"

class LoadEtsi004 : public Block {

public:

	LoadEtsi004(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	void setSamplingPeriod(double sPeriod) { samplingPeriod = sPeriod; }
	double const getSamplingPeriod(void) { return samplingPeriod; }

	void setSymbolPeriod(double sPeriod) { symbolPeriod = sPeriod; }
	double const getSymbolPeriod(void) { return symbolPeriod; }

	// ETSI

	void setPort(t_integer newPort) { etsi_port = newPort; }
	t_integer const getPort(void) { return etsi_port; }

	void setKeyStreamId(std::string ksid) { key_stream_id = ksid; }
	t_string const getKeyStreamId(void) { return key_stream_id; }

	void setNode1UUID(std::string uuid) { node1_uuid = uuid; }
	t_string const getNode1UUID(void) { return node1_uuid; }

	void setNode2UUID(std::string uuid) { node2_uuid = uuid; }
	t_string const getNode2UUID(void) { return node2_uuid; }

	void setApp(std::string newApp) { app = newApp; }
	t_string const getApp(void) { return app; }

	void setAddress(std::string address) { etsi_address = address; }
	t_string const getAddress(void) { return etsi_address; }

	void setKeySource(t_string source) { keySource = source; }
	t_string const getKeySource(void) { return keySource; }

	void setSaveKeys(t_bool saveBool) { saveKeys = saveBool; }
	t_bool const getSaveKeys(void) { return saveKeys; }

	// ASCII

	enum class delimiter_type { CommaSeperatedValues, ConcatenatedValues };

	void setDataType(signal_value_type dType) { dataType = dType; }
	signal_value_type const getDataType(void) { return dataType; }

	void setDelimiterType(delimiter_type delType) { delimiterType = delType; }
	delimiter_type const getDelimiterType(void) { return delimiterType; }

	void setAsciiFileName(t_string aFileName) { asciiFileName = aFileName; }
	t_string const getAsciiFileName(void) { return asciiFileName; }

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

	double samplingPeriod{ 1 };
	double symbolPeriod{ 1 };

	t_string keySource = "";

	// ETSI

	etsi_qkd_004::LKMSAddress address = { "127.0.0.1", 24031 };
	t_string key_stream_id = "";
	t_bool saveKeys = false;

	t_integer etsi_port = 24030;
	t_string etsi_address = "127.0.0.1";
	t_string app = std::string("AtoB");
	t_string node1_uuid = std::string("aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa");
	t_string node2_uuid = std::string("cccccccc-cccc-cccc-cccc-cccccccccccc");
	t_bool closed_connection = false;

	t_string raw = std::string("raw://");

	unsigned int CHUNK_SIZE{ 128 };  // Number of key bytes retrieved per GET_KEY
	unsigned int CHUNK_NUMBER{ 1 };
	unsigned int MIN_BPS{ 128 };

	unsigned int INDEX_NUMBER{ 0 };

	etsi_qkd_004::QoS qos{ CHUNK_SIZE, MIN_BPS, MIN_BPS, 0, 0, 150000, 0, "" };

	etsi_qkd_004::KeyBuffer cumulative_key_buffer;

	// ASCII

	std::streampos position = 0;
	t_bool fileOpened{ false };

	int sizeOfFile{ 0 };
	int ready{ 0 };
	double auxMultiply = 1;
	signal_value_type dataType{ signal_value_type::t_binary };
	delimiter_type delimiterType{ delimiter_type::ConcatenatedValues };

	t_string asciiFileName{ "InputFile" };
	t_string asciiFileNameExtension{ ".dat" };
	t_string asciiFileNameTailNumber{ "" };
	t_integer asciiFileNameTailNumberModulos{ 0 };

	unsigned long int startColumn = 1;
	unsigned long int startLine = 1;
	unsigned long int auxPosition = 0;

	unsigned long int numberOfLoadedValues{ 0 };

};


# endif