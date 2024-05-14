#include "netxpto_20200819.h"
#include "save_ascii_20200819.h"
#include "ms_windows_console_output_common_20200819.h"
#include "add_20200819.h"
#include "binary_source_20200819.h"
#include "qkd_emulator.h"
#include "m_qam_mapper_20200819.h"
#include "real_to_complex_20200819.h"
#include "white_noise_20200819.h"
#include "clone.h"

int main_dv(){
    auto start = std::chrono::high_resolution_clock::now();

    QkdEmulatorInputParameters param = QkdEmulatorInputParameters();
    param.setInputParametersFileName("input_parameters.txt");
    param.readSystemInputParameters();

    double QBER = param.qber / 100.0 ;
    int SEED = param.seed;

    Binary raw_signal = { "aux1.sgn", param.numberOfValuesPerFile };

    BinarySource binary_source {{}, {&raw_signal}};
    binary_source.setMode(BinarySourceMode::Random);
    binary_source.setSeed( SEED );
    binary_source.setProbabilityOfZero(0.5);

    Binary raw_signal2 = { "aux2.sgn", param.numberOfValuesPerFile };

    BinarySource binary_source2 {{}, {&raw_signal2}};
    binary_source2.setMode(BinarySourceMode::Random);
    binary_source2.setSeed( SEED );
    binary_source2.setProbabilityOfZero( 1-QBER );

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

int main_cv(){

    auto start = std::chrono::high_resolution_clock::now();

    QkdEmulatorInputParameters param = QkdEmulatorInputParameters();
    param.setInputParametersFileName("input_parameters.txt");
    param.readSystemInputParameters();

    int SEED = param.seed;
    double noisePower = 0.25;

    Binary raw_signal = { "aux1.sgn", param.numberOfValuesPerFile };

    BinarySource binary_source {{}, {&raw_signal}};
    binary_source.setMode(BinarySourceMode::Random);
    binary_source.setSeed( SEED );
    binary_source.setProbabilityOfZero(0.5);

    TimeContinuousAmplitudeContinuousReal real_sgn = { "aux2.sgn", param.numberOfValuesPerFile };
    TimeContinuousAmplitudeContinuousReal img_sgn = { "aux3.sgn", param.numberOfValuesPerFile };

    TimeContinuousAmplitudeContinuousReal real_sgn_clone = { "aux4.sgn", param.numberOfValuesPerFile };
    TimeContinuousAmplitudeContinuousReal img_sgn_clone = { "aux5.sgn", param.numberOfValuesPerFile };
    TimeContinuousAmplitudeContinuousReal real_sgn_clone2 = { "aux6.sgn", param.numberOfValuesPerFile };
    TimeContinuousAmplitudeContinuousReal img_sgn_clone2 = { "aux7.sgn", param.numberOfValuesPerFile };
    
    MQAMMapper mapper{{&raw_signal}, {&real_sgn, &img_sgn}};
    mapper.setM(4);

    Clone clone{{&real_sgn}, {&real_sgn_clone, &real_sgn_clone2}};
    Clone clone2{{&img_sgn}, {&img_sgn_clone, &img_sgn_clone2}};

    TimeContinuousAmplitudeContinuousReal noise_sgn = { "aux8.sgn", param.numberOfValuesPerFile };
    TimeContinuousAmplitudeContinuousReal noise_sgn2 = { "aux9.sgn", param.numberOfValuesPerFile };

    WhiteNoise white_noise{{}, {&noise_sgn}};
    white_noise.setNoisePower(noisePower);
    white_noise.setSeed(SEED);
    
    WhiteNoise white_noise2{{}, {&noise_sgn2}};
    white_noise2.setNoisePower(noisePower);
    white_noise2.setSeed(SEED+1);

    TimeContinuousAmplitudeContinuousReal simulated_real_sgn = { "aux10.sgn", param.numberOfValuesPerFile };
    TimeContinuousAmplitudeContinuousReal simulated_img_sgn = { "aux11.sgn", param.numberOfValuesPerFile };

    Add adder{{&real_sgn_clone, &noise_sgn}, {&simulated_real_sgn}};
    Add adder2{{&img_sgn_clone, &noise_sgn2}, {&simulated_img_sgn}};

    TimeContinuousAmplitudeContinuousComplex simulated_sgn = { "aux12.sgn", param.numberOfValuesPerFile };

    RealToComplex real_to_complex{{&simulated_real_sgn, &simulated_img_sgn}, {&simulated_sgn}};

    SaveAscii save_ascii_rx {{&simulated_sgn}, {}};
    save_ascii_rx.setAsciiFolderName("generated_keys");
    save_ascii_rx.setAsciiFileName(param.rxFilesName);
    save_ascii_rx.setAsciiFileNameTailNumber("0");
    
    TimeContinuousAmplitudeContinuousComplex sgn_clone = { "aux13.sgn", param.numberOfValuesPerFile };

    RealToComplex real_to_complex2{{&real_sgn_clone2, &img_sgn_clone2}, {&sgn_clone}};

    SaveAscii save_ascii_tx {{&sgn_clone}, {}};
    save_ascii_tx.setAsciiFolderName("generated_keys");
    save_ascii_tx.setAsciiFileName(param.txFilesName);
    save_ascii_tx.setAsciiFileNameTailNumber("0");

    MainSystem mainSystem 
    {
        {
            &binary_source,
            &mapper,
            &clone,
            &clone2,
            &white_noise,
            &white_noise2,
            &adder,
            &adder2,
            &real_to_complex,
            &save_ascii_rx,
            &real_to_complex2,
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

int main(int argc, char* argv[])
{
    main_cv();
}