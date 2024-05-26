# ifndef DECODER_H_
# define DECODER_H_

# include "netxpto_20200819.h"


// Implements a IQ Decoder.
class Decoder : public Block {

public:

	/* Methods */
	Decoder() {};
	Decoder(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	void setM(int mValue);
	int getM() { return m; };

	void setScalingFactor(t_real sFactor) { scalingFactor = sFactor; }

	void setIqAmplitudes(std::vector<t_complex> iqAmplitudesValues);
	std::vector<t_complex> getIqAmplitudes() { return iqAmplitudes; }

	/*void setIqValues(vector<t_complex> iq) { iqValues = iq; };
	vector<t_complex> getIqValues() { return iqValues; }*/

private:

	/* Input Parameters */

	t_integer m{ 4 };

	std::vector<t_complex> iqAmplitudes{ { 1.0, 1.0 },{ -1.0, 1.0 },{ -1.0, -1.0 },{ 1.0, -1.0 } };

	t_integer samplesToSkip{0};

	/* State Variables */

	t_real scalingFactor{ 1/(20E-4) };


};

# endif
