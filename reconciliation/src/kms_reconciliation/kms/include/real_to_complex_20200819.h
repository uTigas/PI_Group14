# ifndef PROGRAM_INCLUDE_REAL_TO_COMPLEX_H_
# define PROGRAM_INCLUDE_REAL_TO_COMPLEX_H_

# include "netxpto_20200819.h"


// Creates a complex signal from two real signals
class RealToComplex : public Block {

	/* State Variables */
	bool firstTime{ true };

 public:

	/* Methods */
	RealToComplex() {};
	RealToComplex(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :
		Block(InputSig, OutputSig){};

	void initialize(void);
	bool runBlock(void);

};

# endif // PROGRAM_INCLUDE_REAL_TO_COMPLEX_H_
