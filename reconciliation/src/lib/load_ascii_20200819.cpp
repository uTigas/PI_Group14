//#include <algorithm>
//#include <complex>
//#include <fstream>

#include "load_ascii_20200819.h"


void LoadAscii::initialize(void){

	outputSignals[0]->setSymbolPeriod(symbolPeriod);
	outputSignals[0]->setSamplingPeriod(samplingPeriod);
    lastTailNumber = stoi(asciiFileNameTailNumber);
    beforelastTailNumber = lastTailNumber;
}


bool LoadAscii::runBlock(void)
{
	t_string asciiFileName_;

	if(beforelastTailNumber == 0 && lastTailNumber == 1 && stoi(asciiFileNameTailNumber) == 2 && !cycleModulos)
		return true;

	if (asciiFileNameTailNumber.size() == 0)
	{
		asciiFileName_ = asciiFileName;
	}
	else
	{
		asciiFileName_ = asciiFileName + asciiFileNameTailNumber + asciiFileNameExtension;
	}

	std::ifstream inFile;
	inFile.open(asciiFileName_);

	if (!inFile) 
	{
		fileOpened = false;
		if (getVerboseMode())
		{
			std::cout << "File: " << asciiFileName_ << " was not opened." << "\n";
		}
		return false;
	}
	fileOpened = true;	

	int space = outputSignals[0]->space();
	signal_value_type sTypeOut = outputSignals[0]->getValueType();
    /*
	if (stoi(asciiFileNameTailNumber) > asciiFileNameTailNumberModulos && !cycleModulos) {
		while (ready && space)
		{
			char in = inFile.get();
			ready--;
			if (in == '0' || in == '1')
			{
				space--;
				numberOfLoadedValues++;
				if (in == '0')
					outputSignals[0]->bufferPut((t_binary)0);
				if (in == '1')
					outputSignals[0]->bufferPut((t_binary)1);
			}
		}
		inFile.close();
		return true;
	}*/
	
	switch (delimiterType)
	{
		case delimiter_type::ConcatenatedValues:
		{
			if (getFirstTime())
			{
				setFirstTime(false);
				inFile.seekg(0, inFile.end);
				sizeOfFile = (int)(inFile.tellg());
				ready = sizeOfFile;
				inFile.seekg(0, inFile.beg);
			}
			else
			{
				inFile.seekg(std::streampos (sizeOfFile - ready));
			}

			//int process = std::min(ready, space);
			if (ready == 0)
			{
				inFile.close();
				if (asciiFileNameTailNumber.size()==0)	return false;

				t_integer tailNumber = stoi(asciiFileNameTailNumber);
				if (asciiFileNameTailNumberModulos == 0) // || !cycleModulos
					tailNumber = tailNumber + 1;
				else{
                    beforelastTailNumber = lastTailNumber;
					lastTailNumber = tailNumber;
					tailNumber = (tailNumber + 1) % asciiFileNameTailNumberModulos;
				}
				asciiFileNameTailNumber = std::to_string(tailNumber);
				setFirstTime(true);

				return true;
			}

			int process{ 0 };

			switch (dataType)
			{
				case signal_value_type::t_binary:
				{
					while (ready && space)
					{
						char in = inFile.get();
						ready--;
						if (in == '0' || in == '1')
						{
							space--;
							numberOfLoadedValues++;
							if (in == '0')
								outputSignals[0]->bufferPut((t_binary)0);
							if (in == '1')
								outputSignals[0]->bufferPut((t_binary)1);
						}
					}
					break;
				}
				default:
					std::cout << "ERROR: load_ascii.cpp (this is not a good way to save non-binary ascii)" << "\n";
					break;
			}

			break;
		}

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
	}
	inFile.close();
	return true;
}
