#ifndef DV_QKD_LDPC_RX_MESSAGE_PROCESSOR_RECEIVER_H_
#define DV_QKD_LDPC_RX_MESSAGE_PROCESSOR_RECEIVER_H_

#include "netxpto_20200819.h"
#include "dv_qkd_message_processor_common_20200819.h"

class DvQkdLdpcRxMessageProcessorReceiver : public Block
{
public:
	DvQkdLdpcRxMessageProcessorReceiver(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);

	bool runBlock(void);

private:
	
	// Input Parameters ##################################################################################

	// State Variables ###################################################################################

	std::vector<t_message> storedMessages{};
	
	// Basis Reconciliation
	t_integer messageReconciliationMaximumDataLength{ 4096 };
	CircularBuffer<t_binary> BasesFrom{ messageReconciliationMaximumDataLength };

	// Parameter Estimation
	t_integer messageParameterEstimationMaximumDataLength{ 100 };

	t_integer numberOfProcessedBits{ -1 };

	CircularBuffer<t_integer> SeedFrom{ 10 };
	CircularBuffer<t_integer> RatioFrom{ 10 };
	CircularBuffer<t_integer> NumberOfBitsPerEstimationBlockFrom{ 10 };
	CircularBuffer<t_binary> DataFrom{ 10*(size_t) messageParameterEstimationMaximumDataLength };

	// Error correction - Parities
	std::vector<t_integer> parityIn{};
	bool errCorrParitiesStarted{ false };

	// Sindrome
	t_integer messageSindromeMaximumDataLength{ 5000 };
	CircularBuffer<t_binary> Sindrome{ messageSindromeMaximumDataLength };

	// Error correction - Permutations
	std::vector<t_integer> permutationsIn{};
	bool errCorrPermStarted{ false };

	// Error correction - BER
	std::vector<t_integer> errCorrBerIn{};
	bool errCorrBerStarted{ false };

	// Privacy amplification seeds
	std::vector<t_integer> privacySeedsIn{};
	bool privacySeedsStarted{ false };

	bool outputReceivedData(std::vector <t_integer>& dataVector, Signal& outputSignal, bool &started);

};
#endif // !MESSAGE_PROCESSOR_RECEIVER_H_
