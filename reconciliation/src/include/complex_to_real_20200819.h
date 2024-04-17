# ifndef COMPLEX_TO_REALH_
# define COMPLEX_TO_REAL_H_

# include "netxpto_20200819.h"


// Creates a complex signal from two real signals
class ComplexToReal : public Block {

public:

	/* Methods */
	ComplexToReal() {};
	ComplexToReal(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

};

# endif // PROGRAM_INCLUDE_REAL_TO_COMPLEX_H_
#pragma once
