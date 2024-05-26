# ifndef OPTICAL_FILTER_H_
# define OPTICAL_FILTER_H_

# include "netxpto_20200819.h"

# include "complex_to_real_20200819.h"
# include "electrical_filter_20200819.h" 
# include "real_to_complex_20200819.h"

// #########################################################################################################
// This block currently implements an optical filter. Currently, it is only possible to have the filter
// centered on the carrier frequency of the signal. Right now this is assumed automatically by default.
// #########################################################################################################

class OpticalFilter : public SuperBlock 
{

public:

	//##############################################################################################################
	
	OpticalFilter(std::initializer_list<Signal*> inputSignal, std::initializer_list<Signal*> outputSignal);

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	void setBandwidth_Hz(t_real bw) { bandwidth_Hz = bw; ElectricalFilter_1_.setCutoffFrequency(bw / 2); ElectricalFilter_2_.setCutoffFrequency(bw / 2); };
	t_real getBandwidth_Hz(void) { return bandwidth_Hz; };

	void setFilterType(electrical_filter_type ft) { filterType = ft; };
	electrical_filter_type getFilterType(void) { return filterType; }

	void setImpulseResponseTimeLength(int irtl) 
	{
		impulseResponseTimeLength = irtl;
		ElectricalFilter_1_.setImpulseResponseTimeLength(impulseResponseTimeLength);
		ElectricalFilter_2_.setImpulseResponseTimeLength(impulseResponseTimeLength);
	}

	int getImpulseResponseTimeLength(void) { return impulseResponseTimeLength; };

private:

	// Input Parameters ################################################################################

	t_real bandwidth_Hz{ 100e9 };
	electrical_filter_type filterType{ electrical_filter_type::LowPass };
	int impulseResponseTimeLength{ 16 };

	// ##################################################################################################

	// Signals Declaration and Inicialization ###########################################################
	
	OpticalSignal OpticalSignal_In{ "OpticalSignal_In.sgn" };
	TimeContinuousAmplitudeContinuousReal ComplexToReal_Out_Re{ "ComplexToReal_Out_Re.sgn" }; // Real Input
	TimeContinuousAmplitudeContinuousReal ComplexToReal_Out_Im{ "ComplexToReal_Out_Im.sgn" }; // Imag Input
	TimeContinuousAmplitudeContinuousReal ElectricalFilter_1_Out{ "ElectricalFilter_1_Out.sgn" }; // Real Filtered Output
	TimeContinuousAmplitudeContinuousReal ElectricalFilter_2_Out{ "ElectricalFilter_2_Out.sgn" }; // Imag Filtered Output
	OpticalSignal OpticalSignal_Out{ "OpticalSignal_Out.sgn" };

	// ##################################################################################################

	// Blocks Declaration and Inicialization ############################################################
	
	ComplexToReal ComplexToReal_{ {&OpticalSignal_In}, {&ComplexToReal_Out_Re, &ComplexToReal_Out_Im} };
	ElectricalFilter ElectricalFilter_1_{ {&ComplexToReal_Out_Re},{&ElectricalFilter_1_Out} };
	ElectricalFilter ElectricalFilter_2_{ {&ComplexToReal_Out_Im},{&ElectricalFilter_2_Out} };
	RealToComplex RealToComplex_{ {&ElectricalFilter_1_Out, &ElectricalFilter_2_Out}, {&OpticalSignal_Out} };

	// ##################################################################################################

};

#endif
