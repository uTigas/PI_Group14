#include "bit_error_rate_20200819.h"

void BitErrorRate::initialize(void){


	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());
}


bool BitErrorRate::runBlock(void)
{
	t_bool alive{ false };

	/* Computing z */ // This code converges in below 10 steps, exactness chosen in order to achieve this rapid convergence
	if (getFirstTime())
	{
		setFirstTime(false);
		double x1 = -2;
		double x2 = 2;
		double x3 = x2 - (erf(x2 / sqrt(2)) + 1 - alpha)*(x2 - x1) / (erf(x2 / sqrt(2)) - erf(x1 / sqrt(2)));
		double exacteness = 1e-15;

		while (abs(erf(x3 / sqrt(2)) + 1 - alpha)>exacteness)
		{
			x3 = x2 - (erf(x2 / sqrt(2)) + 1 - alpha)*(x2 - x1) / (erf(x2 / sqrt(2)) - erf(x1 / sqrt(2)));
			x1 = x2;
			x2 = x3;
		}

		z = -x3;

	}

	int ready0 = inputSignals[0]->ready();
	auto aux = std::min(skipInput0, ready0);
	alive = !alive ? aux > 0 : alive;
	for (auto k=0; k < aux; k++)
		inputSignals[0]->bufferGet();
	skipInput0 -= aux;
	ready0 -= aux;
	int ready1 = inputSignals[1]->ready();
	aux = std::min(skipInput1, ready1);
	alive = !alive ? aux > 0 : alive;
	for (auto k = 0; k < aux; k++)
		inputSignals[1]->bufferGet();
	skipInput1 -= aux;
	ready1 -= aux;
	int ready = std::min(ready0, ready1);
	int space = outputSignals[0]->space();

	int process = std::min(ready, space);
	alive = !alive ? process > 0 : alive;
	
	/* Outputting final report */

	if (process == 0 && getVerboseMode())
	{

		/* Calculating BER and bounds */

		double BER = (receivedBits - coincidences) / receivedBits;
        
        if (BER==0) {
           // cout << "ERRO: bit_error_rate.cpp (null BER, not enough samples)" << "\n";
        }

		double UpperBound = BER + 1 / sqrt(receivedBits) * z  * sqrt(BER*(1 - BER)) + 1 / (3 * receivedBits)*(2 * z * z * (1 / 2 - BER) + (2 - BER));
		double LowerBound = BER - 1 / sqrt(receivedBits) * z  * sqrt(BER*(1 - BER)) + 1 / (3 * receivedBits)*(2 * z * z * (1 / 2 - BER) - (1 + BER));

        if (LowerBound<lowestMinorant) {
            LowerBound=lowestMinorant;
        }
        
		/* Outputting a .txt report*/
		std::ofstream myfile;
		myfile.open("BER.txt");
		myfile << "BER= " << BER << "\n";
		myfile << "Upper and lower confidence bounds for " << (1 - alpha) * 100 << "% confidence level \n";
		myfile << "Upper Bound= " << UpperBound << "\n";
		myfile << "Lower Bound= " << LowerBound << "\n";
		myfile << "Number of received bits =" << receivedBits << "\n";
		myfile.close();
		return false;
	}



	for (long int i = 0; i < process; i++) {

		t_binary signalValue;
		inputSignals[0]->bufferGet(&signalValue);
		t_binary SignalValue;
		inputSignals[1]->bufferGet(&SignalValue);

		/* Outputting mid reports */
		if (m > 0)
		{
			if ((remainder(receivedBits, m) == 0) & (receivedBits > 0))
			{
				n++;

				std::ostringstream oss;
				oss << "midreport" << n << ".txt";
				std::string filename = oss.str();

				/* Calculating BER and bounds */

				double BER;
				BER = (receivedBits - coincidences) / receivedBits;

				double UpperBound = BER + 1 / sqrt(receivedBits) * z  * sqrt(BER*(1 - BER)) + 1 / (3 * receivedBits)*(2 * z * z * (1 / 2 - BER) + (2 - BER));
				double LowerBound = BER - 1 / sqrt(receivedBits) * z  * sqrt(BER*(1 - BER)) + 1 / (3 * receivedBits)*(2 * z * z * (1 / 2 - BER) - (1 + BER));

				/* Outputting a .txt report*/
				std::ofstream myfile;
				myfile.open(filename);
				myfile << "BER= " << BER << "\n";
				myfile << "Upper and lower confidence bounds for" << 1 - alpha << "confidence level \n";
				myfile << "Upper Bound= " << UpperBound << "\n";
				myfile << "Lower Bound= " << LowerBound << "\n";
				myfile << "Number of received bits =" << receivedBits << "\n";
				myfile.close();
			}
		}

		receivedBits++;
			if (signalValue == SignalValue)
			{
				coincidences++;
				outputSignals[0]->bufferPut((t_binary)0);
			}
			else
			{
				//std::cout << "received bits: " << receivedBits << "\n";
				outputSignals[0]->bufferPut((t_binary)1);
			}
	}

	return alive;
}


