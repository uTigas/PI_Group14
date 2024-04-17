# ifndef SAMPLER_H_
# define SAMPLER_H_

# include "netxpto_20200819.h"


// Takes a sampling of the input electrical signal
class Sampler : public Block {


	bool repeatedPass{ false };
	int samplesToSkip{ 0 };
	int count = 0;
	bool aux1{ false };


public:

	double outputOpticalPower{ 1e-3 };

	void initialize(void);
	bool runBlock(void);

	void setSamplesToSkip(int sToSkip) { samplesToSkip = sToSkip; }
	int const getSamplesToSkip(void) { return samplesToSkip; }
	Sampler() {};
	Sampler(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	//void setSamplingRate(int Sampling) { sampling = Sampling; }
	//void setDelay(int Delay) { delay = Delay; }
	void setOutputOpticalPower(double outOpticalPower) { outputOpticalPower = outOpticalPower; }
	void setOutputOpticalPower_dBm(double outOpticalPower_dBm) { outputOpticalPower = 1e-3*pow(10, outOpticalPower_dBm / 10); }

};


#endif // !PROGRAM_INCLUDE_SAMPLER_H_
