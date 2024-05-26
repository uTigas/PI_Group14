# ifndef SINK_H_
# define SINK_H_

# include "netxpto_20200819.h"

class Sink : public Block {

public:

	/* Methods */

	Sink() {};
	Sink(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig);

	bool runBlock(void);

	void setNumberOfSamplesToProcess(long int nOfSamplesToProcess) { numberOfSamplesToProcess = nOfSamplesToProcess; };
	long int getNumberOfSamplesToProcess() const { return numberOfSamplesToProcess; };

	void setDisplayNumberOfSamples(bool opt) { displayNumberOfSamples = opt; };
	bool getDisplayNumberOfSamples() const { return displayNumberOfSamples; };

	void setDisplayNumberOfProcessedSamples(bool opt) { displayNumberOfProcessedSamples = opt; };
	bool getDisplayNumberOfProcessedSamples() const { return displayNumberOfProcessedSamples; };

private:

	/* Input Parameters */

	long int numberOfSamplesToProcess{ -1 };
	long int numberOfProcessedSamples{ 0 };
	bool displayNumberOfSamples{ false };
	bool displayNumberOfProcessedSamples{ false };

};

#endif
