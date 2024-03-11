#ifndef DV_QKD_TX_INCOMING_DATA_PROCESSOR_H_
#define DV_QKD_TX_INCOMING_DATA_PROCESSOR_H_

#include "netxpto_20200819.h"

class DvQkdRxIncomingDataProcessor : public Block {

public:

	DvQkdRxIncomingDataProcessor(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

private:
	// Input parameters
//	t_real controlBitNumber{ 5 };


	// state variables
	t_real numberOfSamples{ 0 };
	t_real oldNumberOfSamples{ 0 };
};


#endif // !BOB_PROCESS_INCOMING_DATA_H_