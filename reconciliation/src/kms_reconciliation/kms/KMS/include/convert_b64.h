# define PROGRAM_CONVERT_B64_H_

#include "netxpto_20200819.h"

class ConvertB64 : public Block {

    public:

    ConvertB64(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

    std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    void initialize(void);
    bool runBlock(void);

    std::string hardcodedBase64Encode (const std::string& in);
    

    private:

    t_integer readyTx{ 0 };
    t_integer readyRx{ 0 };
    std::string binaryBufferTX{ "" };
    std::string binaryBufferRX{ "" };
    
};