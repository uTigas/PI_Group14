#include "fork_20200819.h"

void Fork::initialize(void)
{
	auto multiplicity{ outputSignals.size() / inputSignals.size() };

	if (multiplicity % multiplicity != 0)
		std::cerr << "Fork error: the number of output signals must be a multiple of the number of input signals" << std::endl;

	auto numberOfInputSignals{ inputSignals.size() };
	for(auto k = 0; k < multiplicity; k++)
		for (auto m = 0; k < numberOfInputSignals; k++)
		{
			outputSignals[k * numberOfInputSignals + m]->setSymbolPeriod(inputSignals[m]->getSymbolPeriod());
			outputSignals[k * numberOfInputSignals + m]->setSamplingPeriod(inputSignals[m]->getSamplingPeriod());
			outputSignals[k * numberOfInputSignals + m]->setFirstValueToBeSaved(inputSignals[m]->getFirstValueToBeSaved());
		}

}

bool Fork::runBlock(void)
{
	bool alive{ true };

	t_index ready{ MAX_BUFFER_LENGTH };
	t_index space{ MAX_BUFFER_LENGTH };

	for (auto k : inputSignals) { ready = (ready < k->ready()) ? ready : k->ready(); }
	for (auto k : outputSignals) { space = (space < k->space()) ? space : k->space(); }

	t_index process = std::min(ready, space);
	if (process == 0) return false;

	auto numberOfInputSignals{ inputSignals.size() };
	auto multiplicity{ outputSignals.size() / inputSignals.size() };

	for (auto h = 0; h < process; h++)
		for (auto m = 0; m < numberOfInputSignals; m++)
		{
			signal_value_type sValueType = inputSignals[m]->getValueType();
			switch (sValueType)
			{
			case signal_value_type::t_binary:
			{
				t_binary value;
				inputSignals[m]->bufferGet(&value);
				for (auto k = 0; k < multiplicity; k++)
					outputSignals[k * numberOfInputSignals + m]->bufferPut(value);
				break;
			}
			/*
			case signal_value_type::t_char:
			{
				t_char value;
				inputSignals[m]->bufferGet(&value);
				for (auto k = 0; k < multiplicity; k++)
					outputSignals[k * numberOfInputSignals + m]->bufferPut(value);
				break;
			}
			*/
			case signal_value_type::t_complex:
			{
				t_complex value;
				inputSignals[m]->bufferGet(&value);
				for (auto k = 0; k < multiplicity; k++)
					outputSignals[k * numberOfInputSignals + m]->bufferPut(value);
				break;
			}
			case signal_value_type::t_complex_xy:
			{
				t_complex_xy value;
				inputSignals[m]->bufferGet(&value);
				for (auto k = 0; k < multiplicity; k++)
					outputSignals[k * numberOfInputSignals + m]->bufferPut(value);
				break;
			}
			/*
			case signal_value_type::t_integer:
			{
				t_integer value;
				inputSignals[m]->bufferGet(&value);
				for (auto k = 0; k < multiplicity; k++)
					outputSignals[k * numberOfInputSignals + m]->bufferPut(value);
				break;
			}
			*/
			case signal_value_type::t_message:
			{
				t_message value;
				inputSignals[m]->bufferGet(&value);
				for (auto k = 0; k < multiplicity; k++)
					outputSignals[k * numberOfInputSignals + m]->bufferPut(value);
				break;
			}
			case signal_value_type::t_photon:
			{
				t_photon value;
				inputSignals[m]->bufferGet(&value);
				for (auto k = 0; k < multiplicity; k++)
					outputSignals[k * numberOfInputSignals + m]->bufferPut(value);
				break;
			}
			/*
			case signal_value_type::t_photon_mp:
			{
				t_photon_mp value;
				inputSignals[m]->bufferGet(&value);
				for (auto k = 0; k < multiplicity; k++)
					outputSignals[k * numberOfInputSignals + m]->bufferPut(value);
				break;
			}
			case signal_value_type::t_photon_mp_xy:
			{
				t_photon_mp_xy value;
				inputSignals[m]->bufferGet(&value);
				for (auto k = 0; k < multiplicity; k++)
					outputSignals[k * numberOfInputSignals + m]->bufferPut(value);
				break;
			}
			*/
			case signal_value_type::t_raw:
			{
				t_raw value;
				inputSignals[m]->bufferGet(&value);
				for (auto k = 0; k < multiplicity; k++)
					outputSignals[k * numberOfInputSignals + m]->bufferPut(value);
				break;
			}
			case signal_value_type::t_real:
			{
				t_real value;
				inputSignals[m]->bufferGet(&value);
				for (auto k = 0; k < multiplicity; k++)
					outputSignals[k * numberOfInputSignals + m]->bufferPut(value);
				break;
			}
			default:
				std::cerr << "Fork error: value type not supported" << std::endl;
			}
		}

	return alive;
}
