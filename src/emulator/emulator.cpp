#include "netxpto_20200819.h"
#include "save_ascii_20200819.h"
#include "ms_windows_console_output_common_20200819.h"
#include "add_20200819.h"
#include "binary_source_20200819.h";


void initializeBlocks(auto blocks)
{
    for (auto block : blocks )
    {
        block->initialize(); 
    }
}

void runBlocks(auto blocks)
{
    bool alive;
    do 
    {
        alive = false;
        for (auto block : blocks )
        {
            alive = block->runBlock() | alive;
        }
    } while(alive);
}

void terminateBlocks(auto blocks)
{
    for (auto block : blocks )
    {
        block->terminate(); 
    }
}

int main(int argc, char* argv[])
{

    auto start = std::chrono::high_resolution_clock::now();
    double QBER = 0.1;
    int SEED = 0;

    Binary raw_signal = { "aux1.sgn", 1024 };

    BinarySource binary_source {{}, {&raw_signal}};
    binary_source.setMode(BinarySourceMode::Random);
    binary_source.setProbabilityOfZero(0.5);

    Binary raw_signal2 = { "aux2.sgn", 1024 };

    BinarySource binary_source2 {{}, {&raw_signal2}};
    binary_source2.setMode(BinarySourceMode::Random);
    binary_source2.setSeed( SEED );
    binary_source2.setProbabilityOfZero( QBER );

    Binary raw_signal3 = { "aux.sgn", 1024 };
    Add adder{{&raw_signal, &raw_signal2}, {&raw_signal3}};
    
    SaveAscii save_ascii_emulator {{&raw_signal3}, {}};
    save_ascii_emulator.setAsciiFolderName("generated_keys");
    save_ascii_emulator.setAsciiFileName("raw_key");
    save_ascii_emulator.setAsciiFileNameTailNumber("0");

    binary_source.initialize();
    binary_source2.initialize();
    adder.initialize();
    save_ascii_emulator.initialize();

    binary_source.runBlock();
    binary_source2.runBlock();
    adder.runBlock();
    save_ascii_emulator.runBlock();

    binary_source.terminate();
    binary_source2.terminate();
    adder.terminate();
    save_ascii_emulator.terminate();

    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    return 0;
}