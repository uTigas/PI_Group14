# ifndef PULSE_SHAPER_H_
# define PULSE_SHAPER_H_

# include "netxpto_20200819.h"


enum class pulse_shaper_filter_type { RaisedCosine, Gaussian, Square, RootRaisedCosine, HyperbolicSecant, NoFilter};

/* Raised-cosine filter FIR implementation. */
class PulseShaper : public FIR_Filter{

public:

	//#########################################################################################################################

	PulseShaper(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :FIR_Filter(InputSig, OutputSig) {};

	void initialize(void);

	//#########################################################################################################################

	void setImpulseResponseTimeLength_symbolPeriods(int iResponseTimeLength_symbolPeriods){ impulseResponseTimeLength_symbolPeriods = iResponseTimeLength_symbolPeriods; };
	int const getImpulseResponseTimeLength_symbolPeriods(void) { return impulseResponseTimeLength_symbolPeriods; };

	void  setFilterType(pulse_shaper_filter_type fType) { filterType = fType; };
	pulse_shaper_filter_type const getFilterType(void){ return filterType; };

	void setRollOffFactor(double rOffFactor){ rollOffFactor = rOffFactor; };
	double const getRollOffFactor(){ return rollOffFactor; };

	void setPulseWidth_s(double pWidth_s) { pulseWidth_s = pWidth_s; };
	double const getPulseWidth_s() { return pulseWidth_s; };

	void setPulseWidth_ps(double pWidth_ps) { pulseWidth_s = pWidth_ps * 1E-12; };
	double const getPulseWidth_ps() { return pulseWidth_s * 1E12; };

	void setPassiveFilterMode(bool pFilterMode){ passiveFilterMode = pFilterMode; };
	bool const getPassiveFilterMode() { return passiveFilterMode; };

	void setRrcNormalizeEnergy(bool ne) { rrcNormalizeEnergy = ne; };
	bool getRrcNormalizeEnergy(void) { return rrcNormalizeEnergy; };

	void setBTs(double bandwidthSymbolTimeProduct) { BTs = bandwidthSymbolTimeProduct; };
	double const getBTs() { return BTs; };

	//void setRemoveFilterDelay(bool sBeginning) { removeFilterDelay = sBeginning; };
	//bool const getRemoveFilterDelay() { return removeFilterDelay; };


	//#########################################################################################################################

private:

	/* Input Parameters */
	pulse_shaper_filter_type filterType{ pulse_shaper_filter_type::NoFilter };

	int impulseResponseTimeLength_symbolPeriods{ 0 };

	double pulseWidth_s{ 50e-12 };

	double rollOffFactor{ 0.9 };						// Roll-off factor (roll) for the raised-cosine filter

	double timeDelay{ 0 };

	double pulsePower_W{ 1.0 };

	bool passiveFilterMode{ false };

	bool rrcNormalizeEnergy{ false };                   // Set filter energy to 1, otherwise amplitude == 1

	double BTs{ 0.25 };									// Bandwidth-symbol time product of gaussian filter

	bool removeFilterDelay{ false };

};

# endif


