# ifndef PROGRAM_INCLUDE_IQ_MODULATOR_H_
# define PROGRAM_INCLUDE_IQ_MODULATOR_H_

# include "netxpto_20200819.h"


// Implements a IQ modulator.
class IQModulator : public Block {

public:

	/* Methods */
	IQModulator() {};
	IQModulator(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);
	void terminate(void);

	void setVPi(t_real vPi_) { vPi = vPi_; }
	t_real getVPi() { return vPi; }

private:

	/* Input Parameters */

	t_real vPi{ PI };
 
};

# endif // PROGRAM_INCLUDE_netxpto_H_


