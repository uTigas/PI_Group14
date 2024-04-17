#ifndef BASIS_RECONCILIATION_H_
#define BASIS_RECONCILIATION_H_

#include "netxpto_20200819.h"


enum class basis_reconciliation_role{ Alice, Bob, Undefined};

class BasisReconciliation : public Block {

public:

	BasisReconciliation(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};

	void initializer(void);
	bool runBlock(void);

	void setBasisReconciliationRole(basis_reconciliation_role role) { basisReconciliationRole = role; }
	void setDoubleClickNumber(t_integer dcn) { doubleClickNumber = dcn; }
	void setNoClickNumber(t_integer ncn) { noClickNumber = ncn; }

	int64_t getReceivedQubits(void) { return receivedQubits; }
	int64_t getReconciledBits(void) { return reconciledBits; }

	int64_t getNumberOfNoClicks(void) { return noClicks; }
	int64_t getNumberOfDoubleClicks(void) { return doubleClicks; }

private:
	// Input Variables
	basis_reconciliation_role basisReconciliationRole{ basis_reconciliation_role::Undefined };
	t_integer doubleClickNumber{ 2 };
	t_integer noClickNumber{ 3 };

	// State Variables
	bool bobBasisReconStatus{ false };
	std::vector<t_integer> storedData = {};
	int64_t receivedQubits{ 0 };
	int64_t reconciledBits{ 0 };
	std::vector<t_integer> msgOutputData{};
	std::vector<t_integer> msgInputData{};
	bool startedSending{ false };
	bool finishedSending{ false };


	bool startedReceiving{ false };
	bool finishedReceiving{ false };
	t_real numberOfBasisToReceive{ 0 };

	bool sendToMsg(std::vector <t_integer> &data, Signal &signalToMsg, bool &started);

	int64_t noClicks{ 0 };
	int64_t doubleClicks{ 0 };

};

#endif // !BASIS_RECONCILIATION_H_
