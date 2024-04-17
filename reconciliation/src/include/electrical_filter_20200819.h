# ifndef ELECTRICAL_FILTER_H_
# define ELECTRICAL_FILTER_H_

# include "netxpto_20200819.h"


enum class electrical_filter_type { LowPass, Defined, Unitary, NoFilter, LowPassResistorCapacitor, LowPassButterworth3rdOrder};

class ElectricalFilter : public FIR_Filter {

public:

	ElectricalFilter() :FIR_Filter() {};
	ElectricalFilter(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :FIR_Filter(InputSig, OutputSig) {};

	void initialize(void);
	void lowPass(std::vector<t_real> &impulseResponse, int impulseResponseLength, double cutoffFrequency, double samplingPeriod, double symbolPeriod);
	//void highPass(std::vector<t_real>& impulseResponse, int impulseResponseLength, double cutoffFrequency, double samplingPeriod, double symbolPeriod);
	void lowPassResistorCapacitor(std::vector<t_real>& impulseResponse, int impulseResponseLength, double cutoffFrequency, double samplingPeriod, double symbolPeriod);
	void lowPassButterworth3rdOrder(std::vector<t_real>& impulseResponse, int impulseResponseLength, double cutoffFrequency, double samplingPeriod, double symbolPeriod);

	void setImpulseResponseTimeLength(int impResponseTimeLength) { impulseResponseTimeLength = impResponseTimeLength; };
	int const getImpulseResponseTimeLength(void) { return impulseResponseTimeLength; };

	void setCutoffFrequency(double cOffFrequency) { cutoffFrequency = cOffFrequency; };
	double const getCutoffFrequency() { return cutoffFrequency; };

	void setFilterType(electrical_filter_type fType) { filterType = fType; };
	electrical_filter_type const getFilterType(void) { return filterType; };

	void setImpulseResponse(std::vector<t_real> ir) { impulseResponse = ir; };
	std::vector<t_real> getImpulseResponse(void) { return impulseResponse; };

private:

	electrical_filter_type filterType{ electrical_filter_type::LowPass };
	WindowType windowType{ WindowType::Hann };
	int impulseResponseTimeLength{ 128 };

	double cutoffFrequency{ 1E9 }; //hertz
	std::vector<double> window;

};
#endif