# include "sink_20200819.h"

Sink::Sink(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) 
{

  setNumberOfInputSignals((int) InputSig.size());
  setNumberOfOutputSignals((int) OutputSig.size());

  inputSignals = InputSig;
}

bool Sink::runBlock(void)
{

	int ready = inputSignals[0]->ready();

	int process;
	if (numberOfSamplesToProcess >= 0) {
		process = std::min((long int)ready, numberOfSamplesToProcess);
	}
	else {
		process = ready;
	}
	 
	if ((process == 0) || (numberOfSamplesToProcess==0)) return false;

	for (int i = 0; i<process; i++) (inputSignals[0])->bufferGet();

	numberOfProcessedSamples = numberOfProcessedSamples + process;
	numberOfSamplesToProcess = numberOfSamplesToProcess - process;
	if (displayNumberOfSamples) std::cout << "Number of samples to process: " << numberOfSamplesToProcess << "\n";
	if (displayNumberOfProcessedSamples) std::cout << "Number of processed samples: " << numberOfProcessedSamples << "\n";

	return true;
}
