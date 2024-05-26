//#include <algorithm>
//#include <complex>
//#include <fstream>

#include "save_ascii_2024.h"
#include <vector>
#include <string>
#include <iostream>
const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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
	std::string binaryBuffer;
		
	switch (delimiterType)
	{
		case delimiter_type::ConcatenatedValues:
		{
			auto process = ready;
			if(!insertIndex){
				if (endFile > 0) process = std::min(process, endFile - outPosition);
			}

			switch (File_type)
			{
			case ASCII:

				if(insertIndex){
					for (auto k = 0; k < process; k++)
					{
						t_binary val{ 0 };
						inputSignals[0]->bufferGet(&val);
						if (val == 0) outFile.put('0');
						if (val == 1) outFile.put('1');
						column++;
					}
					if(inputSignals[1]->ready()){
						t_message id;
						inputSignals[1]->bufferGet(&id);
						outFile << " [index]: " << id.getMessageData() << "\n";
						//int length = (" [index]: " + id.getMessageData()).length();
						outPosition = outPosition + process;
					}
				} else {
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
				}
				
				if (outPosition >= endFile)
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
			
			case B64:

				if(insertIndex){
					for (auto k = 0; k < process; k++)
					{
						t_binary val{ 0 };
						inputSignals[0]->bufferGet(&val);
						outFile.put(static_cast<char>(val));
						column++;
					}
					if(inputSignals[1]->ready()){
						t_message id;
						inputSignals[1]->bufferGet(&id);
						outFile << " [index]: " << id.getMessageData() << "\n";
						//int length = (" [index]: " + id.getMessageData()).length();
						outPosition = outPosition + process;
					}
				} else {
					for (auto k = 0; k < process; k++)
					{
						t_binary val{ 0 };
						inputSignals[0]->bufferGet(&val);
						outFile.put(static_cast<char>(val));
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
				}

				if (outPosition >= endFile)
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
			default:
			{
				break;
			}
		}
			
		
	}
	if (outFile.is_open()) 
		outFile.close();
	return true;
	}
}
