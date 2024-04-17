#include "electrical_filter_20200819.h"

void ElectricalFilter::initialize(void) {

	double samplingPeriod = inputSignals[0]->getSamplingPeriod();
	double symbolPeriod = inputSignals[0]->getSymbolPeriod();
	/*double samplingPeriod = 1 / (50 * cutoffFrequency);
	double symbolPeriod = inputSignals[0]->getSymbolPeriod() / inputSignals[0]->getSamplingPeriod() * samplingPeriod;*/

	outputSignals[0]->setSymbolPeriod(symbolPeriod);
	outputSignals[0]->setSamplingPeriod(samplingPeriod);
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped());


	if (filterType == electrical_filter_type::NoFilter)
		setImpulseResponseLength(1);
	else
		setImpulseResponseLength((int)floor(impulseResponseTimeLength * symbolPeriod / samplingPeriod));

	impulseResponse.resize(getImpulseResponseLength());

	switch (getFilterType()) {

	case electrical_filter_type::LowPass:
		lowPass(impulseResponse, getImpulseResponseLength(), cutoffFrequency, samplingPeriod, symbolPeriod);
		break;
	case electrical_filter_type::LowPassResistorCapacitor:
		lowPassResistorCapacitor(impulseResponse, getImpulseResponseLength(), cutoffFrequency, samplingPeriod, symbolPeriod);
	case electrical_filter_type::LowPassButterworth3rdOrder:
		lowPassButterworth3rdOrder(impulseResponse, getImpulseResponseLength(), cutoffFrequency, samplingPeriod, symbolPeriod);
		break;
	case electrical_filter_type::Defined:
		break;
	case electrical_filter_type::Unitary:
		cutoffFrequency = 0;
//		impulseResponseLength = symbolPeriod / samplingPeriod;
//		impulseResponse.resize(impulseResponseLength);
		//impulseResponse[0] = 1;
		for (int i = 1; i < getImpulseResponseLength(); i++) {
			impulseResponse[i] = 0;
		}
		impulseResponse[(const unsigned int) round(getImpulseResponseLength() /2)] = 1;
		break;
	case electrical_filter_type::NoFilter:
			impulseResponse[0] = 1;
			for (int i = 1; i < getImpulseResponseLength(); i++) {
				impulseResponse[i] = 0;
			}
		break;
	};

	initializeFIR_Filter();

	/*
	outputSignals[0]->setSymbolPeriod(symbolPeriod);
	outputSignals[0]->setSamplingPeriod(samplingPeriod);
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped()+getImpulseResponseLength()/2);
	*/
};

void ElectricalFilter::lowPass(std::vector<t_real> &impulseResponse, int impulseResponseLength, double cutoffFrequency, double samplingPeriod, double symbolPeriod) {
	double sinc;
	double gain{ 0 };

	window = getWindow(windowType, impulseResponseLength);

	for (int i = 0; i < impulseResponseLength; i++) {
		t_real t = -impulseResponseLength / 2 * samplingPeriod + i * samplingPeriod;
		if (t != 0) {
			sinc = sin(2 * PI * cutoffFrequency * t) / (2 * PI * cutoffFrequency * t);
		}
		else {
			sinc = 1;
		}
		impulseResponse[i] = sinc * 2 * cutoffFrequency;
		impulseResponse[i] = impulseResponse[i]*window[i];
		gain = gain + impulseResponse[i];
	};

	for (int i = 0; i < impulseResponseLength; i++)
	{
		impulseResponse[i] = impulseResponse[i] / gain;
	}
};

// Low Pass Resistor-Capacitor Filter -- Tiago Fernandes
void ElectricalFilter::lowPassResistorCapacitor(std::vector<t_real>& impulseResponse, int impulseResponseLength, double cutoffFrequency, double samplingPeriod, double symbolPeriod)
{
	double gain{ 1 };
	double cutoffFrequency_rad = 2 * PI * cutoffFrequency;

	//std::vector<t_real> time(impulseResponseLength);

	for (int i = 0; i < impulseResponseLength; i++) {
		t_real t = -impulseResponseLength / 2 * samplingPeriod + i * samplingPeriod;
		
		//time[i] = t;
		
		if (t >= 0) {
			impulseResponse[i] = cutoffFrequency_rad * exp(-cutoffFrequency_rad * t);
		}
		else {
			impulseResponse[i] = 0;
		}
		gain = gain + impulseResponse[i];			//normalize energy
		//if (t == 0) gain = impulseResponse[i];		//get 1 at t=0;
	}
	for (int i = 0; i < impulseResponseLength; i++)
	{
		impulseResponse[i] = impulseResponse[i] / gain;
	}


	//std::stringstream ss;
	////ss << std::fixed << std::setprecision(0) << cutoffFrequency;
	//ss << std::setprecision(1) << std::scientific <<  cutoffFrequency;
	//std::string mystring = ss.str();
	////std::string mystring = "";
	//std::string fileName = "rcIR" + mystring + ".txt";
	//std::ofstream rcIR(fileName);
	//rcIR.precision(15);
	//for (size_t i = 0; i < impulseResponse.size(); i++)
	//{
	//	rcIR << time[i] << " " << impulseResponse[i] << "\n";
	//}
	//rcIR.close();
}

// Low Pass 3rd order Butterworth Filter -- Tiago Fernandes
void ElectricalFilter::lowPassButterworth3rdOrder(std::vector<t_real>& impulseResponse, int impulseResponseLength, double cutoffFrequency, double samplingPeriod, double symbolPeriod)
{
	double cutoffFrequency_rad = 2 * PI * cutoffFrequency;
	double gain{ 0 };

	//std::vector<t_real> time(impulseResponseLength);

	for (int i = 0; i < impulseResponseLength; i++) {
		t_real t = -impulseResponseLength / 2 * samplingPeriod + i * samplingPeriod;

		//time[i] = t;

		if (t>= 0) {
			impulseResponse[i] = cutoffFrequency_rad * exp(-t * cutoffFrequency_rad) - 2.0/3.0 * sqrt(3) * cutoffFrequency_rad * exp(-t * cutoffFrequency_rad / 2) * cos(t * cutoffFrequency_rad / 2 * sqrt(3) + PI / 6);
		}
		else {
			impulseResponse[i] = 0;
		}
		
		//if (impulseResponse[i] > gain) gain = impulseResponse[i];			// gives wrong results
		gain = gain + impulseResponse[i];
	};

	for (int i = 0; i < impulseResponseLength; i++)
	{
		impulseResponse[i] = impulseResponse[i] / gain;
	}


	/*std::ofstream rcIR("rcIR.txt");
	rcIR.precision(15);
	for (size_t i = 0; i < impulseResponse.size(); i++)
	{
		rcIR << time[i] << " " << impulseResponse[i] << "\n";
	}
	rcIR.close();*/
}