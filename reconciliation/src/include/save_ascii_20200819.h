# ifndef PROGRAM_SAVE_ASCII_H_
# define PROGRAM_SAVE_ASCII_H_

# include "netxpto_20200819.h"



class SaveAscii : public Block {

public:

	enum class delimiter_type { CommaSeperatedValues, ConcatenatedValues };

	SaveAscii(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	void setDataType(signal_value_type dType) { dataType = dType; }
	signal_value_type const getDataType(void) { return dataType; }

	void setDelimiterType(delimiter_type delType) { delimiterType = delType; }
	delimiter_type const getDelimiterType(void) { return delimiterType; }

	void setInsertEndLine(t_bool option) { insertEndLine = option; }
	t_bool const getInsertEndLine(void) { return insertEndLine; }

	void setReplacePreviousKeys(t_bool option) { replacePreviousKeys = option; }
	t_bool const getReplacePreviousKeys(void) { return replacePreviousKeys; }
	
	void setAsciiFileName(t_string aFileName) { asciiFileName = aFileName; }
	t_string const getAsciiFileName(void) { return asciiFileName; }

	void setAsciiFileNameExtension(t_string aFileNameExtension) { asciiFileNameExtension = aFileNameExtension; }
	t_string const getAsciiFileNameExtension(void) { return asciiFileNameExtension; }

	void setAsciiFileNameTailNumber(t_string aFileNameTailNumber) { asciiFileNameTailNumber = aFileNameTailNumber; }
	t_string const getAsciiFileNameTailNumber(void) { return asciiFileNameTailNumber; }

	void setAsciiFileNameTailNumberModulos(t_integer aFileNameTailNumberModulos) { asciiFileNameTailNumberModulos = aFileNameTailNumberModulos; }
	t_integer const getAsciiFileNameTailNumberModulos(void) { return asciiFileNameTailNumberModulos; }

	void setAsciiFolderName(t_string aFolderName) { asciiFolderName = aFolderName; }
	t_string const getAsciiFolderName(void) { return asciiFolderName; }

private:

	signal_value_type dataType{ signal_value_type::t_binary };
	delimiter_type delimiterType{ delimiter_type::ConcatenatedValues };

	t_bool replacePreviousKeys{ true };
	t_bool insertEndLine{ true };

	t_string aux_asciiFileName{ "aux_file" };
	t_string aux_asciiFileNameExtension{ "dat" };
	t_string asciiFolderName{ "." };
	t_string asciiFileName{ "OutputFile" };
	t_string asciiFileNameExtension{ "dat" };
	t_string asciiFileNameTailNumber{ "0" };

	t_integer asciiFileNameTailNumberModulos{ 10 };

	t_integer column{ 0 };
	t_integer endLine{ 200 };
	t_integer endFile{ 11664 };  // 6 * 1944
	t_integer outPosition = 0;

};


# endif