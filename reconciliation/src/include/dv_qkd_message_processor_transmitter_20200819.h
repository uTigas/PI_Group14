#ifndef MESSAGE_PROCESSOR_TRANSMITTER_H_
#define MESSAGE_PROCESSOR_TRANSMITTER_H_

#include "netxpto_20200819.h"
#include "dv_qkd_message_processor_common_20200819.h"

class MessageProcessorTransmitter : public Block {
public:

	MessageProcessorTransmitter(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);

	bool runBlock(void);

	// Private methods

private:
	// State variables
	std::vector<t_message> storedMessages = {};
	
	// Basis reconciliation
	bool basisReconStarted{ false };
	t_real basisReconLength{ 0 };
	std::vector<t_integer> basisReconVector{};
	
	// Parameter estimation
	bool ParamEstStarted{ false };
	t_real paramEstLength{ 0 };
	std::vector<t_integer> paramEstVector{};
	
	// Error correction - Parities
	bool ErrCorrParityStarted{ false };
	t_real ErrCorrParityLength{ 0 };
	std::vector<t_integer> ErrCorrParityVector{};
	
	// Error correction - Permutation seeds
	bool ErrCorrPermStarted{ false };
	t_real ErrCorrPermLength{ 0 };
	std::vector<t_integer> ErrCorrPermVector{};

	// Error correction - BER change signal
	bool ErrCorrBerStarted{ false };
	t_real ErrCorrBerLength{ 0 };
	std::vector<t_integer> ErrCorrBerVector{};

	// Privacy amplification seeds
	std::vector<t_integer> privacySeedsBVector{};
	t_real privacySeedsLength{ 0 };
	bool privacySeedsStarted{ false };
};
#endif // !MESSAGE_PROCESSOR_TRANSMITTER_H_
