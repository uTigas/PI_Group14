# ifndef DISCRETE_TO_CONTINUOUS_TIME_H_
# define DISCRETE_TO_CONTINUOUS_TIME_H_

# include "netxpto_20200819.h"

class DiscreteToContinuousTime : public Block {

public:

	//################################################################################################################################

	DiscreteToContinuousTime(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//################################################################################################################################
		
	void setNumberOfSamplesPerSymbol(int nSamplesPerSymbol){ numberOfSamplesPerSymbol = nSamplesPerSymbol; };
	int const getNumberOfSamplesPerSymbol(void){ return numberOfSamplesPerSymbol; };

	//################################################################################################################################

private:

	/* input parameters */
	int numberOfSamplesPerSymbol{ 8 };

	/* state variables */
	int index{ 0 };

	//int contador{ 0 };

};

#endif
