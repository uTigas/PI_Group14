#ifndef DV_QKD_LDPC_TX_MESSAGE_PROCESSOR_TRANSMITTER_H_
#define DV_QKD_LDPC_TX_MESSAGE_PROCESSOR_TRANSMITTER_H_

#include "netxpto_20200819.h"
#include "dv_qkd_message_processor_common_20200819.h"

class DvQkdLdpcTxMessageProcessorTransmitter : public Block 
{
public:

	DvQkdLdpcTxMessageProcessorTransmitter(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);

	bool runBlock(void);

private:

	// # Input Parameters #########################################################################

	// # State variables #########################################################################

	std::vector<t_message> storedMessages = {};
	
	// Basis reconciliation
	t_integer messageReconciliationMaximumDataLength{ 4096 };
	CircularBuffer<t_binary> BasesAckTo{ messageReconciliationMaximumDataLength };
	
	// Parameter estimation
	t_integer messageParameterEstimationMaximumDataLength{ 100 };

	t_integer numberOfProcessedBits{ 0 };

	CircularBuffer<t_integer> SeedTo{ 1 };
	CircularBuffer<t_integer> RatioTo{ 1 };
	CircularBuffer<t_integer> NumberOfBitsPerEstimationBlock{ 1 };
	CircularBuffer<t_binary> DataTo{ 0 };

	// Sindrome
	t_integer messageSindromeMaximumDataLength{ 5000 };
	CircularBuffer<t_binary> Sindrome{ messageSindromeMaximumDataLength };
	
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
