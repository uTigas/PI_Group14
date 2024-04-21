#include "netxpto_20200819.h"

/*
    Input Parameters:
    Key Type: Binary/Real
    File Type: ASCII/Base64
    Counter First Value: 0
    Counter Last Value: max_int
    Tx files name: raw [*_[bin_tx/real_tx_*.[dat/b64]
    Rx files name: raw [*_[bin_rx/real_rx_*.[dat/b64]
    Number of Values per File: 1024
    QBER: 40
    Seed: 0
*/

#define MAX_INT 2147483647

class MainSystem : public System
{

public:
	MainSystem(std::initializer_list<Block*> MainSystem) : System(MainSystem) {};

};

class QkdEmulatorInputParameters : public SystemInputParameters{
    public:
        
        t_string keyType{ "Binary" };
        t_string fileType{ "ASCII" };
        t_integer counterFirstValue{ 0 };
        t_integer counterLastValue{ MAX_INT };
        t_string txFilesName{ "bin_tx_0.dat" };
        t_string rxFilesName{ "bin_rx_0.dat" };
        t_integer numberOfValuesPerFile{ 1024 };
        t_integer qber{ 40 };
        t_integer seed{ 123456789 };

        t_string defaultInputFname{ "input_parameters_0.txt" };

        QkdEmulatorInputParameters() : SystemInputParameters() {
            initializeInputParameterMap();
        }

        QkdEmulatorInputParameters(int argc, char* argv[]) : SystemInputParameters(argc, argv) {
            setInputParametersFileName(defaultInputFname);
            initializeInputParameterMap();
            readSystemInputParameters();
        }

        void initializeInputParameterMap() {
            addInputParameter("keyType", &keyType);
            addInputParameter("fileType", &fileType);
            addInputParameter("counterFirstValue", &counterFirstValue);
            addInputParameter("counterLastValue", &counterLastValue);
            addInputParameter("txFilesName", &txFilesName);
            addInputParameter("rxFilesName", &rxFilesName);
            addInputParameter("numberOfValuesPerFile", &numberOfValuesPerFile);
            addInputParameter("qber", &qber);
            addInputParameter("seed", &seed);
        }
};