#include "dv_polarization_physical_layer_emulator_20200819.h"

t_void DvPolarizationPhysicalLayer::initialize(t_void)
{
	symbolPeriod_s = inputSignals[0]->getSymbolPeriod();

	double totalAttenuation = pow(10, (insertionLosses_dB + fiberLength_km * fiberAttenuation_dB_per_km) / 10);
	averageNumberOfReceivedPhotons = detectorEfficiency * numberOfPhotonsPerInputPulse / totalAttenuation;

	double probabilityOfClick = 1 - exp(-averageNumberOfReceivedPhotons);
	probabilityOfNoClick = 1 - probabilityOfClick; // Neither detector should click. (is this right?)
}


t_bool DvPolarizationPhysicalLayer::runBlock(t_void) 
{

	int ready0 = inputSignals[0]->ready();	// Tx data
	int ready1 = inputSignals[1]->ready();	// Tx basis
	int ready2 = inputSignals[2]->ready();	// Rx basis

	int ready = std::min(ready0, ready1);
	ready = std::min(ready, ready2);
	
	int space = outputSignals[0]->space();

	int process = std::min(ready, space);

	if (0 == process) return false;

	for (auto k = 0; k < process; k++) 
	{

		t_binary dataIn;
		inputSignals[0]->bufferGet(&dataIn);

		t_binary aliceBasis;
		inputSignals[1]->bufferGet(&aliceBasis);

		t_binary bobBasis;
		inputSignals[2]->bufferGet(&bobBasis);
			
		if (onDeadtime) 
		{
			elapsedDeadtime_s += symbolPeriod_s;
		}

		if (onDeadtime && (elapsedDeadtime_s > deadtime_s)) 
		{
			onDeadtime = false;
			elapsedDeadtime_s = 0;
		}

		if ((uniformeRandomNumberBetweenZeroAndOne() <= probabilityOfNoClick) || (onDeadtime))
		{
			outputSignals[0]->bufferPut((t_binary)noClickValue);
		}
		else if (uniformeRandomNumberBetweenZeroAndOne() <= probabilityOfDoubleClick) 
		{
			onDeadtime = true;
			outputSignals[0]->bufferPut((t_binary)doubleClickValue);
		} 
		else 
		{
			onDeadtime = true;
			if (uniformeRandomNumberBetweenZeroAndOne() <= qBer) 
			{
				switch ((t_binary)dataIn) {
				case 1:
					outputSignals[0]->bufferPut((t_binary)0);
					break;
				case 0:
					outputSignals[0]->bufferPut((t_binary)1);
					break;
				}
			}
			else
			{				
				if ((aliceBasis == bobBasis))
				{
					switch ((t_binary)dataIn)
					{
					case 1:
						outputSignals[0]->bufferPut((t_binary)1);
						break;
					case 0:
						outputSignals[0]->bufferPut((t_binary)0);
						break;
					default:
						std::cerr << "Error in dv_polarization_physical_layer_emulator!" << std::endl;
						break;
					}
				}
				else
					outputSignals[0]->bufferPut((t_binary)round(uniformeRandomNumberBetweenZeroAndOne()));
			}
		}
	}
	
	return true;
}
