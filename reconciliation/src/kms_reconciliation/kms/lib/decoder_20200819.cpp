# include "decoder_20200819.h"

void Decoder::initialize(void) {

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod()/log2(m));
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSymbolPeriod() / log2(m));
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	samplesToSkip = inputSignals[0]->getNumberOfValuesToBeSkipped();
	
}

void Decoder::setM(int mValue) {

	m = mValue;

	iqAmplitudes.resize(m);
	switch (m)
	{
		case 2:
			iqAmplitudes = { {0.0, 0.0 }, {1.0, 0.0} };
			break;
		case 4:
			iqAmplitudes = { { 1.0, 1.0 }, { -1.0, 1.0 }, { -1.0, -1.0 }, { 1.0, -1.0 } };
			break;
		case 16:
			iqAmplitudes = { { -3.0, -3.0 },{ -3.0, -1.0 },{ -3.0, 3.0 },{ -3.0, 1.0 },{ -1.0, -3.0 },{ -1.0, -1.0 },{ -1.0, 3.0 },{ -1.0, 1.0 },{ 3.0, -3.0 },{ 3.0, -1.0 }, { 3.0, 3.0 },{ 3.0, 1.0 },{ 1.0, -3.0 },{ 1.0, -1.0 },{ 1.0, 3.0 },{ 1.0, 1.0 } };
	};
};

void Decoder::setIqAmplitudes(std::vector<t_complex> iqAmplitudesValues) {
	m = (int) iqAmplitudesValues.size();
	iqAmplitudes.resize(m);
	iqAmplitudes = iqAmplitudesValues;
};

bool Decoder::runBlock(void) 
{

	bool alive{ false };

	int ready0 = inputSignals[0]->ready();
	int ready1 = inputSignals[1]->ready();
	int ready = std::min(ready0, ready1);

	if (samplesToSkip > 0)
	{
		auto aux = std::min(samplesToSkip, ready);
		alive = aux ? true : false;
		for (auto k = 0; k < aux; k++)
		{
			inputSignals[0]->bufferGet();
			inputSignals[1]->bufferGet();
		}
		samplesToSkip -= aux;
		ready -= aux;
		if (samplesToSkip) return alive;
	}


	int space0 = outputSignals[0]->space();
	int proc = (int)floor(space0 / log2(m));

	int process = std::min(ready, proc);

	if (process == 0)
		return alive;
	else
		alive = true;

	t_real in1, in2;
	double d, minimum;
	
	for (int k = 0; k < process; k++) {
		
		inputSignals[0]->bufferGet(&in1);
		inputSignals[1]->bufferGet(&in2);

		std::complex<double> s_in(scalingFactor*in1, scalingFactor*in2);
		minimum = norm(s_in - iqAmplitudes[0]);
		int aux{ 0 };
		for (int l = 1; l < m; l++) {
			d = norm(s_in - iqAmplitudes[l]);
			if (d < minimum) {
				minimum = d;
				aux = l;
			}
		}
		// Makes the bits swtich order: example 01 is writen as 10
		/*for (int n = 0; n < log2(m); n++) {

			t_binary s_out = aux % 2; //returns the remainder of the division
			outputSignals[0]->bufferPut(s_out);
			aux = aux / 2; //returns the quocient of the division
		}*/
		t_binary s_out;
		std::vector<int> v((int) log2(m),0);
			/*for (int n = 0; n < log2(m); n++) {

			if (n > 0) { s_out = aux % 2; }
			
			else { s_out = aux / 2; }

			outputSignals[0]->bufferPut(s_out);
			aux = aux % 2; 
		}*/

		for (int n = 0; n < log2(m); n++) { 
			
			v[n] = aux % 2;
			aux = aux / 2;
			
			 }

		auto S = size(v);

		for (auto k = 0; k < S; k++) {
			
			s_out = v[(S-1)-k];
			outputSignals[0]->bufferPut(s_out);
			
			 }

	}

	return alive;
}