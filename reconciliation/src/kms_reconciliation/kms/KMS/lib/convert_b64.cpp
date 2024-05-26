#include "convert_b64.h"


void ConvertB64::initialize(void){
      
}

bool ConvertB64::runBlock(void){

    // entram o sinal &key_generatedTX e &key_generatedRx
    // vão ser ambos convertidos para base64
    // o resultado é colocado em dois sinais correspondentes
    // &key_base64Tx e &key_base64Rx que vão ser usados no saveAscii

    readyTx = inputSignals[0]->ready();
    readyRx = inputSignals[1]->ready();

    // Encode TX keys
    for (auto k = 0; k < readyTx; k++){
        t_binary val{ 0 };
        inputSignals[0]->bufferGet(&val);
        binaryBufferTX.push_back(val ? '1' : '0');
        
        // Encode when binaryBuffer has at least 6 bits
        if (binaryBufferTX.length() >= 6) {
            std::string bufferToEncode = binaryBufferTX.substr(0, 6);
            std::cout << "Encoding Base64 for buffer: " << bufferToEncode << std::endl;
            std::string base64Encoded = hardcodedBase64Encode(bufferToEncode);
            std::cout << "Base64 Encoded String: " << base64Encoded << std::endl;
            binaryBufferTX = binaryBufferTX.substr(6);  // Adjust the binary buffer
            if (outputSignals[0]->space()){
                outputSignals[0]->bufferPut(static_cast<t_binary>(base64Encoded[0]));
            }
        }

    }

    if (!binaryBufferTX.empty()) {
        std::cout << "Encoding remaining Base64 for buffer: " << binaryBufferTX << std::endl;
        std::string base64Encoded = hardcodedBase64Encode(binaryBufferTX);
        std::cout << "Base64 Encoded String: " << base64Encoded << std::endl;
        if (outputSignals[0]->space()){
            outputSignals[0]->bufferPut(static_cast<t_binary>(base64Encoded[0]));
        }
        binaryBufferTX.clear();
    }

    // Encode RX keys
    for (auto k = 0; k < readyRx; k++){
        t_binary val{ 0 };
        inputSignals[1]->bufferGet(&val);
        binaryBufferRX.push_back(val ? '1' : '0');
        
        // Encode when binaryBuffer has at least 6 bits
        if (binaryBufferRX.length() >= 6) {
            std::string bufferToEncode = binaryBufferRX.substr(0, 6);
            std::cout << "Encoding Base64 for buffer: " << bufferToEncode << std::endl;
            std::string base64Encoded = hardcodedBase64Encode(bufferToEncode);
            std::cout << "Base64 Encoded String: " << base64Encoded << std::endl;
            binaryBufferRX = binaryBufferRX.substr(6);  // Adjust the binary buffer
            if (outputSignals[1]->space()){
                outputSignals[1]->bufferPut(static_cast<t_binary>(base64Encoded[0]));
            }
        }

    }

    if (!binaryBufferRX.empty()) {
        std::cout << "Encoding remaining Base64 for buffer: " << binaryBufferRX << std::endl;
        std::string base64Encoded = hardcodedBase64Encode(binaryBufferRX);
        std::cout << "Base64 Encoded String: " << base64Encoded << std::endl;
        if (outputSignals[1]->space()){
            outputSignals[1]->bufferPut(static_cast<t_binary>(base64Encoded[0]));
        }
        binaryBufferRX.clear();

    }

    

    return true;
}

std::string ConvertB64::hardcodedBase64Encode(const std::string& in) {

	std::cout << std::endl;
    std::string out;
    int val = 0;
    int valb = 0; 

    for (char c : in) {
        val = (val << 1) | (c - '0');
        valb += 1;
        if (valb >= 6) {
            out.push_back(base64_chars[(val >> (valb - 6))]);
            val &= ((1 << (valb - 6)) - 1);
            valb -= 6;
        }
    }

    if (valb > 0) { 
        out.push_back(base64_chars[val << (6 - valb)]); // Shift for valid bits
        int padding_needed = 4 - out.size() % 4;  // Correct padding calculation
    }

    return out;
}




				