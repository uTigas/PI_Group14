//#include <algorithm>
//#include <complex>
//#include <fstream>

#include "save_ascii_20200819.h"


void SaveAscii::initialize(void)
{
	aux_asciiFileName = aux_asciiFileName + "_" + asciiFileName + "." + asciiFileNameExtension;

	if (!std::filesystem::is_directory(asciiFolderName) || !std::filesystem::exists(asciiFolderName)) 
	{
		std::filesystem::create_directory(asciiFolderName);
	}
	else
	{
		// Note: this code looks like it's currently launching an std::out_of_range exception (DONT USE)
		if (!replacePreviousKeys)
		{
			t_string extension = ".dat";
			size_t pos;
			t_integer max_index = 0;
			t_string file_name;

			for (const auto& entry : std::filesystem::directory_iterator(asciiFolderName)) {
				file_name = std::string(entry.path().u8string());
				pos = 0;

				file_name.erase(0, asciiFolderName.length() + 1);

				if (!(file_name == "aux_file_tx_key.dat" || file_name == "aux_file_rx_key.dat")) {
					file_name = file_name.substr(6, file_name.length());

					pos = file_name.find(extension);

					file_name = file_name.erase(pos, extension.length());

					t_integer file_number = stoi(file_name);

					if (file_number > max_index)
						max_index = file_number;
				}
				
			}

			asciiFileNameTailNumber = std::to_string(max_index + 1);
		}
	}
	
}


bool SaveAscii::runBlock(void)
{
	t_string asciiFileName_;

	if (asciiFileNameTailNumber.size() == 0)
	{
		asciiFileName_ = asciiFileName;
		endFile = -1;
	}
	else
	{
		asciiFileName_ = asciiFileName + asciiFileNameTailNumber + "." + asciiFileNameExtension;
	}

	std::ofstream outFile;
	if (getFirstTime())
	{
		remove(aux_asciiFileName.c_str());
		setFirstTime(false);
	}
	outFile.open(asciiFolderName + "/" + aux_asciiFileName, std::ofstream::app);

	if (!outFile) {
		std::cout << "ERROR: save_ascii.cpp (can't create output file!)" << "\n";
		return false;
	}

	int ready = inputSignals[0]->ready();
		
	switch (delimiterType)
	{
		case delimiter_type::ConcatenatedValues:
		{
			auto process = ready;
			if (endFile > 0) process = std::min(process, endFile - outPosition);

			for (auto k = 0; k < process; k++)
			{
				t_binary val{ 0 };
				inputSignals[0]->bufferGet(&val);
				if (val == 0) outFile.put('0');
				if (val == 1) outFile.put('1');
				column++;
				if (column == endLine)
				{
					column = 0;
					
					if (insertEndLine) {
						outFile.put('\n');
					}
				}
			}
			outPosition = outPosition + process;

			if (outPosition == endFile)
			{
				outFile.close();
				t_string aux_str_new = asciiFolderName + "/" + asciiFileName_;
				t_string aux_str_old = asciiFolderName + "/" + aux_asciiFileName;
				remove(aux_str_new.c_str());
				auto result = rename(aux_str_old.c_str(), aux_str_new.c_str());

				if (getVerboseMode())
				{
					if (result == 0)
						std::cout << "File successfully renamed: " << aux_asciiFileName << " => " << asciiFileName_ << std::endl;
					else
						std::cout << "Error when trying to renamed: " << aux_asciiFileName << " => " << asciiFileName_ << std::endl;
				}

				outPosition = 0;

				t_integer tailNumber = stoi(asciiFileNameTailNumber);
				if (asciiFileNameTailNumberModulos == 0)
					tailNumber = tailNumber + 1;
				else
					tailNumber = (tailNumber + 1) % asciiFileNameTailNumberModulos;
				asciiFileNameTailNumber = std::to_string(tailNumber);
			}
			break;
		}
		/*
		case delimiter_type::CommaSeperatedValues:
		{

			if (space == 0)
			{
				inFile.close();
				return false;
			}
			if (getFirstTime())
			{
				setFirstTime(false);
				t_string s;
				for (unsigned long int i = 0; i < startLine - 1; i++) getline(inFile, s);
			}
			

			switch (dataType)
			{
		
			case signal_value_type::t_binary:
				{
					inFile.seekg(position);
					if (sTypeOut != signal_value_type::t_binary)
					{
						std::cout << "ERROR: load_ascii.cpp (signal type mismatch!)" << "\n";
						inFile.close();
						return false;
					}
					t_string line;
					t_string in;
					t_binary out;
					for (int i = 0; i < space; i++)
					{
						getline(inFile, in);

						std::stringstream inLine(line);
						if (line == "" || inFile.eof())
						{
							inFile.close();
							return false;
						}

						for (unsigned long int j = 0; j < startColumn; j++) getline(inLine, in, ',');

						out = 1;
						if (in == "0") out = 0;

						outputSignals[0]->bufferPut(out);
					}
					position = inFile.tellg();
					break;
				}

				case signal_value_type::t_real:
				{
					inFile.seekg(position);
					if (sTypeOut != signal_value_type::t_real)
					{
						std::cout << "ERROR: load_ascii.cpp (signal type mismatch!)" << "\n";
						inFile.close();
						return false;
					}
					t_string line;
					t_string in;
					t_real out;
					for (int i = 0; i < space; i++)
					{
						getline(inFile, line);
						std::stringstream inLine(line);
						if (line == "" || inFile.eof()) 
						{
							inFile.close();
							return false;
						}

						for (unsigned long int j = 0; j < startColumn; j++) getline(inLine, in, ',');

						out = stod(in);
						outputSignals[0]->bufferPut(out * auxMultiply);
					}

					position = inFile.tellg();
					break;
				}

				case signal_value_type::t_complex:
				{
					if (sTypeOut != signal_value_type::t_complex)
					{
						std::cout << "ERROR: load_ascii.cpp (signal type mismatch!)" << "\n";
						inFile.close();
						return false;
					}
					t_string line;
					for (unsigned long int i = 0; i < auxPosition; i++) getline(inFile, line);
					t_string in;
					double real;
					double imag;
					for (int i = 0; i < space; i++)
					{
						getline(inFile, line);
						std::stringstream inLine(line);

						if (line == "" || inFile.eof())
						{
							inFile.close();
							return false;
						}

						for (unsigned long int j = 0; j < startColumn - 1; j++) getline(inLine, in, ',');

						for (int j = 0; j < 2; j++)
						{
							getline(inLine, in, ',');
							if (j == 0) real = stod(in);
							else imag = stod(in);
						}
						t_complex out(real, imag);
						outputSignals[0]->bufferPut(out);
					}
					auxPosition += space;
					break;
				}
				case signal_value_type::t_complex_xy:
				{
					if (sTypeOut != signal_value_type::t_complex_xy)
					{
						std::cout << "ERROR: load_ascii.cpp (signal type mismatch!)" << "\n";
						inFile.close();
						return false;
					}
					t_string line;
					for (unsigned long int i = 0; i < auxPosition; i++) getline(inFile, line);
					t_string in;
					double realX;
					double imagX;
					double realY;
					double imagY;
					for (int i = 0; i < space; i++)
					{
						getline(inFile, line);
						std::stringstream inLine(line);
						if (line == "" || inFile.eof())
						{
							inFile.close();
							return false;
						}

						for (unsigned long int j = 0; j < startColumn - 1; j++) getline(inLine, in, ',');

						for (int j = 0; j < 4; j++)
						{
							getline(inLine, in, ',');
							if (j == 0)      realX = stod(in);
							else if (j == 1) imagX = stod(in);
							else if (j == 2) realY = stod(in);
							else if (j == 3) imagY = stod(in);
						}

						t_complex X(realX, imagX);
						t_complex Y(realY, imagY);
						t_complex_xy out = { X, Y };
						outputSignals[0]->bufferPut(out);
					}
					auxPosition += space;
					break;
				}
			}
		break;
		}
		*/
	}
	if (outFile.is_open()) 
		outFile.close();
	return true;
}
