#ifndef DV_QKD_TX_BASIS_RECONCILIATION_H_
#define DV_QKD_TX_BASIS_RECONCILIATION_H_

#include "netxpto_20200819.h"

class DvQkdTxBasesReconciliation : public Block {

public:

	//##############################################################################################################

	DvQkdTxBasesReconciliation(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	double getSiftedKeyRate(void) const { return totalNumberOfSiftedKeys/totalTime; };

	//##############################################################################################################

private:
	
	// Input Parameters ############################################################################################
	int dataInBufferLength{ 15000 };
	int basesInBufferLength{ 15000 };

	// State Variables #############################################################################################
	CircularBuffer<t_binary> DataIn{ dataInBufferLength };
	CircularBuffer<t_binary> BasesIn{ basesInBufferLength };

	double totalNumberOfSiftedKeys{ 0.0 };
	double totalTime{ 0.0 };

	//##############################################################################################################

};

#endif 
