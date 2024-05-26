# ifndef CLONE_H_
# define CLONE_H_

# include "netxpto_20200819.h"

class  Clone: public Block {

public:

	//##############################################################################################################

	Clone(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

private:

};

#endif
