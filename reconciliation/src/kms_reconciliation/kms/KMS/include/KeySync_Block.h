#define PROGRAM_KEY_SYNC_BLOCK_H_

#include "netxpto_20200819.h"
#include "etsi_qkd_004.h"
#include "json.hpp"


class KeySyncBlock : public Block {

public: 

    KeySyncBlock(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

        void initialize(void);
        bool runBlock(void);

private:

    t_message m_index;
    t_integer ready;

};