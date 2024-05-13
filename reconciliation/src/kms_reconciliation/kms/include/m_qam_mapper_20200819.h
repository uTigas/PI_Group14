# ifndef M_QAM_MAPPER_H_
# define M_QAM_MAPPER_H_

# include "netxpto_20200819.h"


class MQAMMapper : public Block {

public:

	//##############################################################################################################

	MQAMMapper(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	void setM(int mValue);		// m should be of the form m = 2^n, with n integer;
	void setIqAmplitudes(std::vector<std::vector<t_real>> iqAmplitudesValues);

	void setFirstTime(bool fTime) { firstTime = fTime; };  // if set to true the first output sample is going to be zero, to make the system causal
	bool getFirstTime() { return firstTime; };

	//##############################################################################################################

private:

	/* State Variables */

	t_integer auxBinaryValue{ 0 };
	t_integer auxSignalNumber{ 0 };
	bool firstTime{ true };

	/* Input Parameters */
	t_integer m{ 4 };
	std::vector<std::vector<t_real>> iqAmplitudes{ { 1,1 },{ -1,1 },{ 1,-1 },{ -1,-1 } };

};

#endif
