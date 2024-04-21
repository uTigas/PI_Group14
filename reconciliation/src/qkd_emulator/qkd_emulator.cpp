#include "netxpto_20200819.h"
#include "save_ascii_20200819.h"
#include "ms_windows_console_output_common_20200819.h"
#include "add_20200819.h"
#include "binary_source_20200819.h"
#include "qkd_emulator.h"

int main(int argc, char* argv[])
{

    auto start = std::chrono::high_resolution_clock::now();

    QkdEmulatorInputParameters param = QkdEmulatorInputParameters();
    param.setInputParametersFileName("input_parameters.txt");
    param.readSystemInputParameters();

    double QBER = param.qber / 100.0 ;
    int SEED = param.seed;

    Binary raw_signal = { "aux1.sgn", param.numberOfValuesPerFile };

    BinarySource binary_source {{}, {&raw_signal}};
    binary_source.setMode(BinarySourceMode::Random);
    binary_source.setProbabilityOfZero(1-QBER);

    Binary raw_signal2 = { "aux2.sgn", param.numberOfValuesPerFile };

    BinarySource binary_source2 {{}, {&raw_signal2}};
    binary_source2.setMode(BinarySourceMode::Random);
    binary_source2.setSeed( SEED );
    binary_source2.setProbabilityOfZero( QBER );

    Binary raw_signal3 = { "aux.sgn", param.numberOfValuesPerFile };
    Add adder{{&raw_signal, &raw_signal2}, {&raw_signal3}};
    
    SaveAscii save_ascii_rx {{&raw_signal3}, {}};
    save_ascii_rx.setAsciiFolderName("generated_keys");
    save_ascii_rx.setAsciiFileName(param.rxFilesName);
    save_ascii_rx.setAsciiFileNameTailNumber("0");

    SaveAscii save_ascii_tx {{&raw_signal}, {}};
    save_ascii_tx.setAsciiFolderName("generated_keys");
    save_ascii_tx.setAsciiFileName(param.txFilesName);
    save_ascii_tx.setAsciiFileNameTailNumber("0");

    MainSystem mainSystem 
    {
        {
            &binary_source,
            &binary_source2,
            &adder,
            &save_ascii_rx,
            &save_ascii_tx
        }
    };

    mainSystem.setLogValue(false);

    mainSystem.run();
    mainSystem.terminate();

    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    return 0;
}