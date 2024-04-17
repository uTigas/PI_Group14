#ifndef FORK_H_
#define FORK_H_

#include "netxpto_20200819.h"

class Fork: public Block {

public:

	Fork(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

};


#endif // ! ALICE_PROCESS_INCOMING_DATA_H_