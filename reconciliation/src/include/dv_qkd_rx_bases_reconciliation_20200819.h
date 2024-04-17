#ifndef DV_QKD_RX_BASES_RECONCILIATION_H_
#define DV_QKD_RX_BASES_RECONCILIATION_H_

#include "netxpto_20200819.h"

class DvQkdRxBasesReconciliation : public Block {

public:

	//##############################################################################################################

	DvQkdRxBasesReconciliation(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

private:

	// Input Parameters ############################################################################################
	int dataInBufferLength{ 10000 };
	int basesInBufferLength{ 10000 };

	// State Variables #############################################################################################
	CircularBuffer<t_binary> DataIn{ dataInBufferLength };
	CircularBuffer<t_binary> BasesIn{ basesInBufferLength };

	//##############################################################################################################

};

#endif 
