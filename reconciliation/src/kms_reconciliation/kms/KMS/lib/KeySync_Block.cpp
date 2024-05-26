#include "KeySync_Block.h"
#include "key_sync.h"

void KeySyncBlock::initialize(void) {

}

bool KeySyncBlock::runBlock(void){
    
    bool alive = true;
    if (getTerminated()) return false;

    ready = inputSignals[1]->ready();
    
    std::cout << "KeySyncBlock running\n";

    if (ready == 0) {
        return alive;
    }
    else {
        inputSignals[1]->bufferRead(&m_index);
        std::cout << "Index: " << m_index.getMessageData() << std::endl;
    }
    return alive;
}