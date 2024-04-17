//#include <algorithm>
//#include <complex>
//#include <fstream>

#include "load_etsi_004_20200819.h"

static volatile bool close_flag = false;

void get_close_key() {
	char key = 'a';

	while (key != 'q') {
		key = getchar();
	}

	close_flag = true;
}

void LoadEtsi004::initialize(void){
	outputSignals[0]->setSymbolPeriod(symbolPeriod);
	outputSignals[0]->setSamplingPeriod(samplingPeriod);

	if (keySource == "etsi") {
		address.port = etsi_port;
		address.ip = etsi_address;

		etsi_qkd_004::URI source = raw + app + "_1@" + node1_uuid;
		etsi_qkd_004::URI destination = raw + app + "_2@" + node2_uuid;

		// ----- OPEN_CONNECT -----
		auto oc_response = etsi_qkd_004::open_connect(address, source, destination, qos);
		key_stream_id = oc_response.key_stream_id;
		if (getVerboseMode()) std::cout << "OPEN CONNECT STATUS: " << oc_response.status << std::endl;

		std::thread th1(get_close_key);
		th1.detach();
	}
}

void print_key_buffer(const etsi_qkd_004::KeyBuffer& key_buffer, const std::string& name = "KEY BUFFER")
{
	std::cout << name << ": [ ";
	std::for_each(key_buffer.cbegin(), key_buffer.cend(), [](const unsigned char& c) { std::cout << int(c) << " "; });
	std::cout << "]" << std::endl;
}


bool LoadEtsi004::runBlock(void)
{
	if (keySource == "etsi") {
		if (getVerboseMode())
		{
			std::cout << "Using address: " << address.ip << ':' << address.port << std::endl;
		}

		if (close_flag) {
			if (closed_connection) {
				return false;
			}
			else {
				auto cl_response = etsi_qkd_004::close(address, key_stream_id);
				std::cout << "CLOSE STATUS: " << cl_response.status << std::endl;
				closed_connection = true;
				return false;
			}
		}
		else {
			auto alive{ false };

			auto space = outputSignals[0]->space();

			CHUNK_SIZE = space / 8;

			if (CHUNK_SIZE > 0)
			{
				auto gk_response = etsi_qkd_004::get_key(address, key_stream_id, INDEX_NUMBER);

				if (gk_response.status == etsi_qkd_004::Status::SUCCESSFUL)
				{
					if (getVerboseMode()) print_key_buffer(gk_response.key_buffer);

					while (gk_response.key_buffer.size() && outputSignals[0]->space() > 8) {
						t_binary key_byte = (t_binary)gk_response.key_buffer.back();
						gk_response.key_buffer.pop_back();

						for (int i = 0; i < 8; i++) {
							outputSignals[0]->bufferPut((t_binary)(key_byte & 0x1));
							key_byte = key_byte >> 1;
						}
					}

					alive = true;
					INDEX_NUMBER++;
				}
				else {
					if (getVerboseMode()) std::cout << "ERROR: Status " << gk_response.status << std::endl;
				}

			}

			return alive;
		}		
	}
	else if(keySource == "ascii") {
		t_string asciiFileName_;

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
				inFile.seekg(std::streampos(sizeOfFile - ready));
			}

			//int process = std::min(ready, space);
			if (ready == 0)
			{
				inFile.close();
				if (asciiFileNameTailNumber.size() == 0)	return false;

				t_integer tailNumber = stoi(asciiFileNameTailNumber);
				if (asciiFileNameTailNumberModulos == 0)
					tailNumber = tailNumber + 1;
				else
					tailNumber = (tailNumber + 1) % asciiFileNameTailNumberModulos;
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
}
