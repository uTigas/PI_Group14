#include "save_ascii_2024.h"
#include "netxpto_20200819.h"
#include <typeinfo>
#include <iostream>
#include <memory>
#include "ms_windows_console_output_common_20200819.h"
#include "key_generator.h"
#include "cv_qokd_ldpc_multi_machine_sdf.h"
#include "convert_b64.h"


int main(){
    

    DvQkdLdpcInputParameters param = DvQkdLdpcInputParameters();
    param.setInputParametersFileName("input_parameters_RECON_EMULATOR.txt");
    param.readSystemInputParameters();

    Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{ Signal::t_header_type::fullHeader };

    Binary key_generated_RX{"Rx_key.sgn",36,hType,sWriteMode};
    Binary key_generated_TX{"Tx_key.sgn",36,hType,sWriteMode};
    Binary key_base64RX{"b64_RX_key.sgn",36,hType,sWriteMode};
    Binary key_base64TX{"b64_TX_key.sgn",36,hType,sWriteMode};

    KeyGenerator KEY_GENERATOR{{},{&key_generated_RX,&key_generated_TX}};
    KEY_GENERATOR.setKey_type(param.keyType);
    KEY_GENERATOR.setseed((unsigned int)param.seed);

    if(param.fileType == 0){    // ASCII

        SaveAscii saveRX{{&key_generated_RX},{}};
        saveRX.setFile_type(param.fileType);
        saveRX.setAsciiFolderName("../generated_keys_RX");
        saveRX.setAsciiFileName(param.rxFileName);
        saveRX.setAsciiFileNameExtension("dat");
        saveRX.setAsciiFileNameTailNumber(param.cntFirstVal);
        saveRX.setAsciiFileNameTailNumberModulos(0);

        SaveAscii saveTX{{&key_generated_TX},{}};
        saveTX.setFile_type(param.fileType);
        saveTX.setAsciiFolderName("../generated_keys_TX");
        saveTX.setAsciiFileName(param.txFileName);
        saveTX.setAsciiFileNameExtension("dat");
        saveTX.setAsciiFileNameTailNumber(param.cntFirstVal);
        saveTX.setAsciiFileNameTailNumberModulos(0);

        System System_
            {
                {
                &KEY_GENERATOR,
                &saveRX,
                &saveTX,
                }
            };

        System_.run();
        System_.terminate();

    } else if (param.fileType == 1){    // B64

        SaveAscii saveRX{{&key_base64RX},{}};
        saveRX.setFile_type(param.fileType);
        saveRX.setAsciiFolderName("../generated_keys_RX");
        saveRX.setAsciiFileName(param.rxFileName);
        saveRX.setAsciiFileNameExtension("b64");
        saveRX.setAsciiFileNameTailNumber(param.cntFirstVal);
        saveRX.setAsciiFileNameTailNumberModulos(0);

        SaveAscii saveTX{{&key_base64TX},{}};
        saveTX.setFile_type(param.fileType);
        saveTX.setAsciiFolderName("../generated_keys_TX");
        saveTX.setAsciiFileName(param.txFileName);
        saveTX.setAsciiFileNameExtension("b64");
        saveTX.setAsciiFileNameTailNumber(param.cntFirstVal);
        saveTX.setAsciiFileNameTailNumberModulos(0);

        ConvertB64 b64_converter{{&key_generated_TX, &key_generated_RX},{&key_base64TX, &key_base64RX}};

        System System_
            {
                {
                &KEY_GENERATOR,
                &b64_converter,
                &saveRX,
                &saveTX,
                }
            };
        
        System_.run();
        System_.terminate();
    } else {
        std::cout << "ERROR RECON_EMULATOR MAIN: Bad File Type" << std::endl;
    }

    return 0;
}
