# ifndef PROGRAM_INCLUDE_IDEAL_AMPLIFIER_H_
# define PROGRAM_INCLUDE_IDEAL_AMPLIFIER_H_

# include "netxpto_20200819.h"


class IdealAmplifier : public Block {

public:


	IdealAmplifier() {};
	IdealAmplifier(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	void setGain(double g) { gain = g; }
	double getGain() { return gain; }

private:

	/* Input Parameters */
	double gain{ 1e4 };


};

#endif // !PROGRAM_INCLUDE_IDEAL_AMPLIFIER_H_
