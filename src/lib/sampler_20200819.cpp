#include "sampler_20200819.h"

using namespace std;

void Sampler::initialize(void){

	//setFirstTime(false);

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setNumberOfValuesToBeSkipped(int(inputSignals[0]->getNumberOfValuesToBeSkipped()/inputSignals[0]->getSamplesPerSymbol()));

}


bool Sampler::runBlock(void) {

	//outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSymbolPeriod());
	auto numberOfInputSignals = getNumberOfInputSignals();
	
	bool alive{ false };

	// Sampler with only one input signal (internal clock), only works with an integer number of samplesPerSynbol
	if (numberOfInputSignals == 1) {
		int ready = inputSignals[0]->ready();
		if (getSamplesToSkip() > 0) {
			if (getSamplesToSkip() > ready) {

				//cout << "samplesPerSymbol>bufferLength" << endl;

				int aux3 = inputSignals[0]->getInPosition();
				int aux4 = inputSignals[0]->getOutPosition();

				//cout << "inPos=" << aux3 << endl;
				//cout << "outPos=" << aux4 << endl;

				inputSignals[0]->setOutPosition(inputSignals[0]->getInPosition());
				inputSignals[0]->setBufferEmpty(true); //declare that the buffer is empty

				int aux2 = inputSignals[0]->getOutPosition();
				//cout << "outPos_corr=" << aux2 << endl;

				setSamplesToSkip(getSamplesToSkip() - ready);
				int aux = getSamplesToSkip();
				//cout << aux << endl;

			}
			else {
				int process = min(ready, getSamplesToSkip());
				for (int k = 0; k < process; k++) {
					t_real in;
					inputSignals[0]->bufferGet(&in);
				}
				setSamplesToSkip(0);
				//cout << aux1 << endl;

				int aux5 = inputSignals[0]->getOutPosition();
				//cout << aux5 << endl;

				alive = true;

			}
		}

		int aux6 = inputSignals[0]->getOutPosition();
		//cout << "out position for sampling=" << aux6 << endl;

		ready = inputSignals[0]->ready();
		int space = outputSignals[0]->space();
		int process = min(ready, space);

		if (process == 0) return false;

		int samplesPerSymbol = (int)inputSignals[0]->getSamplesPerSymbol();
		for (int k = 0; k < process; k++) {
			t_real in;
			inputSignals[0]->bufferGet(&in);
			//cout << in << endl;
			//cout << inputSignals[0]->getOutPosition() << endl;
			if (count % samplesPerSymbol == 0) {
				outputSignals[0]->bufferPut((t_real)in);
			}
			count++;
		}

	}
	//Sampler with external clock
	else {

		int ready = inputSignals[0]->ready();
		int space = outputSignals[0]->space();
		int process = min(ready, space);

		if (process <= 0) return false;

		t_real inClock;
		t_real inSignal;

		for (int k = 0; k < process; k++) {

			inputSignals[1]->bufferGet(&inClock);
			inputSignals[0]->bufferGet(&inSignal);

			if (inClock == 1.0) {

				outputSignals[0]->bufferPut((t_real)inSignal);
			}

		}
		
		return true;
	}

	return alive;
}
