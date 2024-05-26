#ifndef ALICE_PROCESS_INCOMING_DATA_H_
#define ALICE_PROCESS_INCOMING_DATA_H_

#include "netxpto_20200819.h"

class AliceIncomingDataProcessor: public Block {

public:

	AliceIncomingDataProcessor(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

};


#endif // ! ALICE_PROCESS_INCOMING_DATA_H_