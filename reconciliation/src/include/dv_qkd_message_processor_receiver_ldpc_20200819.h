#ifndef MESSAGE_PROCESSOR_RECEIVER_H_
#define MESSAGE_PROCESSOR_RECEIVER_H_

#include "netxpto_20200819.h"
#include "dv_qkd_message_processor_common_20200819.h"

class MessageProcessorReceiver : public Block {
public:
	MessageProcessorReceiver(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);

	bool runBlock(void);

private:
	
	// Private methods
	//t_message_data getMessageData(const t_message& msg, t_message_data_length dataLength);
	//t_message_data_length getMessageDataLength(const t_message& msg);
	//t_message_type getMessageType(const t_message& msg);

	// Input Parameters ##################################################################################
	t_integer sindromeSize{ 972 };

	// State Variables ###################################################################################

	std::vector<t_message> storedMessages{};
	
	// Basis Reconciliation
	std::vector<t_integer> basisIn{};
	bool basisReconRcvStarted{ false };

	// Parameter Estimation
	std::vector<t_integer> paramEstIn{};
	bool paramEstRcvStarted{ false };

	// Error correction - Parities
	std::vector<t_integer> parityIn{};
	bool errCorrParitiesStarted{ false };

	// Error correction - Sindrome
	std::vector<t_binary> sindrome{};
	t_integer sindrome_PosOut{ 0 };

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
