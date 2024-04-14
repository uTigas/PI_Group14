#include "netxpto_20200819.h"
#include "save_ascii_20200819.h"
#include "ms_windows_console_output_common_20200819.h"
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

    Binary raw_signal = { "aux.sgn", 1024 };

    SaveAscii save_ascii_emulator {{&raw_signal}, {}};
    save_ascii_emulator.setAsciiFolderName("generated_keys");
    save_ascii_emulator.setAsciiFileName("raw_key");
    save_ascii_emulator.setAsciiFileNameTailNumber("0");
    save_ascii_emulator.setAsciiFileNameTailNumberModulos(0);

    BinarySource binary_source {{}, {&raw_signal}};
    binary_source.setMode(BinarySourceMode::Random);
    //binary_source.setSeed(0);
    //binary_source.setBitPeriod(10);
    //binary_source.setNumberOfBits(1024);

    auto blocks = std::array<Block*, 2>{(Block*)&save_ascii_emulator, (Block*)&binary_source};

    initializeBlocks(blocks);
    runBlocks(blocks);
    terminateBlocks(blocks);

    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    return 0;
}