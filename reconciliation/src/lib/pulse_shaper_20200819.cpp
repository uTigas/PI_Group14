# include "pulse_shaper_20200819.h"


void raisedCosine(std::vector<t_real> &impulseResponse, int impulseResponseLength, double rollOffFactor, double samplingPeriod, double symbolPeriod, bool passiveFilterMode);
void rootRaisedCosine(std::vector<t_real> &impulseResponse, int impulseResponseLength, double rollOffFactor, double samplingPeriod, double symbolPeriod, bool passiveFilterMode, bool normalizeEnergy);
void gaussian(std::vector<t_real>& impulseResponse, int impulseResponseLength, double BTs, double samplingPeriod, double symbolPeriod, bool passiveFilterMode);
void square(std::vector<t_real> &impulseResponse, int impulseResponseLength, double samplingPeriod, double symbolPeriod);
void hyperbolicSecant(std::vector<t_real>& impulseResponse, int impulseResponseLength, double pulsePower, double samplingPeriod, double pulseWidth_s, bool passiveFilterMode);
void noFilter(std::vector<t_real>& impulseResponse, int impulseResponseLength, double samplingPeriod, double symbolPeriod);


void PulseShaper::initialize(void) {

	double samplingPeriod= inputSignals[0]->getSamplingPeriod();
	double symbolPeriod= inputSignals[0]->getSymbolPeriod();

	setImpulseResponseLength((int)floor(getImpulseResponseTimeLength_symbolPeriods() * symbolPeriod / samplingPeriod));
	impulseResponse.resize(getImpulseResponseLength());

	switch (getFilterType()) {

		case pulse_shaper_filter_type::RaisedCosine:
			raisedCosine(impulseResponse, getImpulseResponseLength(), rollOffFactor, samplingPeriod, symbolPeriod, passiveFilterMode);
			setBeginningOfImpulseResponseLength((int)(getImpulseResponseLength()/2));
			break;
		case pulse_shaper_filter_type::RootRaisedCosine:
			rootRaisedCosine(impulseResponse, getImpulseResponseLength(), rollOffFactor, samplingPeriod, symbolPeriod, passiveFilterMode, rrcNormalizeEnergy);
			setBeginningOfImpulseResponseLength((int)(getImpulseResponseLength()/2));
			break;
		case pulse_shaper_filter_type::Gaussian:
			gaussian(impulseResponse, getImpulseResponseLength(), getBTs(), samplingPeriod, pulseWidth_s, passiveFilterMode);
			setBeginningOfImpulseResponseLength((int)(getImpulseResponseLength()/2));
			break;
		case pulse_shaper_filter_type::Square:
			square(impulseResponse, getImpulseResponseLength(), samplingPeriod, symbolPeriod);
			setBeginningOfImpulseResponseLength((int)(getImpulseResponseLength()/2));
			break;
		case pulse_shaper_filter_type::HyperbolicSecant:
			hyperbolicSecant(impulseResponse, getImpulseResponseLength(), pulsePower_W, samplingPeriod, pulseWidth_s, passiveFilterMode);
			break;
		case pulse_shaper_filter_type::NoFilter:
			noFilter(impulseResponse, getImpulseResponseLength(), samplingPeriod, symbolPeriod);
			setBeginningOfImpulseResponseLength((int)(getImpulseResponseLength()/2));
			break;
	};

	//setSeeBeginningOfImpulseResponse(!getRemoveFilterDelay());

	initializeFIR_Filter();
}


void raisedCosine(std::vector<t_real> &impulseResponse, int impulseResponseLength, double rollOffFactor, double samplingPeriod, double symbolPeriod, bool passiveFilterMode) {
	double sinc;
	double gain{ 0 };
	for (int i = 0; i < impulseResponseLength; i++) {
		t_real t = -impulseResponseLength / 2 * samplingPeriod + i * samplingPeriod;
		if (t != 0) {
			sinc = sin(PI * t / symbolPeriod) / (PI * t / symbolPeriod);
		}
		else {
			sinc = 1;
		}
		impulseResponse[i] = sinc*cos(rollOffFactor*PI*t / symbolPeriod) / (1 - (4.0 * rollOffFactor * rollOffFactor * t * t) / (symbolPeriod * symbolPeriod));
		gain = gain + impulseResponse[i];
	};
	if (passiveFilterMode)
	{
		for (int i = 0; i < impulseResponseLength; i++)
		{
			impulseResponse[i] = impulseResponse[i] / gain;
		}
	}
};

void rootRaisedCosine(std::vector<t_real> &impulseResponse, int impulseResponseLength, double rollOffFactor, double samplingPeriod, double symbolPeriod, bool passiveFilterMode, bool normalizeEnergy) 
{

	t_real samplesPerSymbol = symbolPeriod / samplingPeriod;
	t_real irTimeLength     = impulseResponseLength / samplesPerSymbol;
	t_real stepIncreament   = irTimeLength/impulseResponseLength;

	double gain{ 0 };
	for (int i = 0; i < impulseResponseLength; i++) {
		t_real t = -impulseResponseLength / 2 * samplingPeriod + i * samplingPeriod;
		t_real t_sym = -irTimeLength/2 + i * stepIncreament;

		t_real alpha = PI * t / symbolPeriod;
		if (0 == t_sym) {
			impulseResponse[i] = (1 / symbolPeriod)*(1 + rollOffFactor * ((4 / PI) - 1));
		}
		else if ( abs(4*rollOffFactor*t_sym)-1==0 ) {
			impulseResponse[i] = (rollOffFactor/(symbolPeriod*sqrt(2))) * ( (1+(2/PI))*sin(PI/(4*rollOffFactor))  + (1-(2/PI))*cos(PI/(4*rollOffFactor))   );
		}
		else {
			impulseResponse[i] = sin((alpha)*(1 - rollOffFactor)) + ((4 * rollOffFactor*t / symbolPeriod)*cos((alpha)*(1 + rollOffFactor)));
			impulseResponse[i] = (1 / symbolPeriod)* impulseResponse[i] / (alpha * (1 - ((4 * rollOffFactor * t / symbolPeriod) * (4 * rollOffFactor * t / symbolPeriod))));
		}
	}

	for (int i = 0; i < impulseResponseLength; i++)
	{
		if (normalizeEnergy) {
			impulseResponse[i] = impulseResponse[i] * samplingPeriod;       // Energy == 1
		}
		else {
			impulseResponse[i] = impulseResponse[i] * symbolPeriod;         // Amplitude == 1
		}


		gain = gain + impulseResponse[i];
	}

	if (passiveFilterMode)
	{
		for (int i = 0; i < impulseResponseLength; i++)
		{
			impulseResponse[i] = impulseResponse[i] / gain;
		}
	}
};

void gaussian(std::vector<t_real>& impulseResponse, int impulseResponseLength, double BTs, double samplingPeriod, double pulseWidth_s, bool passiveFilterMode) {

	double gauss;
	double gain{ 0 };
	t_real Ts = pulseWidth_s;
	t_real T = samplingPeriod;
	t_real a = (sqrt(log(2) / 2) / BTs);

	for (int i = 0; i < impulseResponseLength; i++)
	{
		t_real t = -impulseResponseLength / 2 * samplingPeriod + i * samplingPeriod;
		gauss = (sqrt(PI)/(a))*(exp(-(PI * t / (a * Ts)) * (PI * t / (a*Ts))));
		impulseResponse[i] = gauss;
		gain = gain + impulseResponse[i];
	};


	if (passiveFilterMode)
	{
		for (int i = 0; i < impulseResponseLength; i++)
		{
			impulseResponse[i] = impulseResponse[i] / gain;
		}
	}
};

void square(std::vector<t_real> &impulseResponse, int impulseResponseLength, double samplingPeriod, double symbolPeriod) {

	int samplesPerSymbol = (int) (symbolPeriod / samplingPeriod);

	for (int k = 0; k < samplesPerSymbol; k++) {
		impulseResponse[k] = 1.0;
	};

	for (int k = samplesPerSymbol; k < impulseResponseLength; k++) {
		impulseResponse[k] = 0.0;
	};
};




void hyperbolicSecant(std::vector<t_real>& impulseResponse, int impulseResponseLength, double pulsePower, double samplingPeriod, double pulseWidth_s, bool passiveFilterMode) {
	double sech;
	double To = pulseWidth_s;
	double Po = pulsePower;
	double gain{ 0 };

	for (int i = 0; i < impulseResponseLength; i++) {
		t_real t = -impulseResponseLength / 2 * samplingPeriod + i * samplingPeriod;
		sech = sqrt(Po) * (1.0 / cosh(t / To));
		impulseResponse[i] = sech;
		gain = gain + impulseResponse[i];
	};
	if (passiveFilterMode)
	{
		for (int i = 0; i < impulseResponseLength; i++)
		{
			impulseResponse[i] = impulseResponse[i] / gain;
		}
	}
};



void noFilter(std::vector<t_real>& impulseResponse, int impulseResponseLength, double samplingPeriod, double symbolPeriod) {

	impulseResponse[0] = 1.0;
	for (int k = 1; k < impulseResponseLength; k++)
		impulseResponse[k] = 0.0;

};
