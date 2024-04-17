#ifndef DV_POLARIZATION_PHYSICAL_LAYER_H
#define DV_POLARIZATION_PHYSICAL_LAYER_H

#include "netxpto_20200819.h"


class DvQkdPolarizationPhysicalLayer: public Block {

public:
	/*Public Methods*/
	DvQkdPolarizationPhysicalLayer(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	t_void initialize(t_void);
	t_bool runBlock(t_void);

	t_void setQBer(t_real ber) { qBer = ber; }
	t_void setProbabilityOfBothClick(t_real pOfBothClick) { probabilityOfDoubleClick = pOfBothClick; }
	t_void setProbabilityOfNoClick(t_real pOfNoClick) { probabilityOfNoClick = pOfNoClick; }
	t_void setNoClickValue(t_integer ncn) { noClickValue = ncn; }
	t_void setDoubleClickValue(t_integer dcn) { doubleClickValue = dcn; }
	t_void setDeadtime(double dt) { deadtime_s = dt; }

	t_void setDetectorEfficiency(t_real def) { detectorEfficiency = def; }								// percentage
	t_void setFiberAttenuation(t_real fatt) { fiberAttenuation_dB_per_km = fatt; }							// dB/km
	t_void setFiberLength(t_real fl) { fiberLength_km = fl; };												// km
	t_void setInsertionLosses(t_real il) { insertionLosses_dB = il; }									// dB
	t_void setNumberOfPhotonsPerInputPulse(t_real nPhotons) { numberOfPhotonsPerInputPulse = nPhotons; }	// statistical average


private:

	/******************************************************************/
	/***********************Input Parameters***************************/
	/******************************************************************/
	
	t_real qBer{ 0 };
	t_real probabilityOfDoubleClick{ 0 };
	t_integer doubleClickValue{ 2 };
	t_integer noClickValue{ 3 };
	t_real deadtime_s{ 1E-5 }; 
	t_real detectorEfficiency{ 0.25 };
	t_real fiberAttenuation_dB_per_km{ 0.2 };
	t_real fiberLength_km{ 50 };			
	t_real insertionLosses_dB{ 2 };			
	t_real numberOfPhotonsPerInputPulse{ 0.1 };	
	t_seed seed{ 0 };

	/******************************************************************/
	/***********************State Variables***************************/
	/******************************************************************/
	t_real symbolPeriod_s{ 0 };
	t_real elapsedDeadtime_s{ 0 };
	t_bool onDeadtime{ false };
	t_real averageNumberOfReceivedPhotons{ 0 };
	t_real probabilityOfNoClick{ 0 };
	
	/******************************************************************/
	/***********************Private Methods***************************/
	/******************************************************************/

	netxpto::UniformRandomRealNumbers uniformeRandomNumberBetweenZeroAndOne{ seed, 0, 1 };
	//std::default_random_engine poissonGenerator{ seed + 1 };

};

#endif // !MESSAGE_PROCESSOR_BOB_H

