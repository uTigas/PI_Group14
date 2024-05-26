#ifndef PRIVACY_AMPLIFICATION_H_
#define PRIVACY_AMPLIFICATION_H_

#include "netxpto_20200819.h"
enum class privacy_amplification_role{SeedTransmitter, SeedReceiver, Undefined};

class PrivacyAmplification : public Block {
public:
	PrivacyAmplification(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};
	void initialize(void);
	bool runBlock(void);

	void setRole(privacy_amplification_role role) { paRole = role; }
	void setBER(t_real berValue) { ber = berValue; }
	void setSystematicSecurityParameter(t_integer sp) { securityParameter = sp; }
	void setBypassPrivacyAmplification(bool mrb) { bypassHashing= mrb; }

	double getTotalNumberOfInputBits(void) { return totalNumberOfInputBits; }
	double getTotalNumberOfOutputBits(void) { return totalNumberOfOutputBits; }

private:
	// State variables
	std::vector<t_binary> vectorHashedBits{};
	std::vector<t_binary> vectorBitsToHash{};
	std::vector<t_binary> bitsOnHold{};
	double totalNumberOfInputBits{ 0 };
	double totalNumberOfOutputBits{ 0 };

	// Input Variables
	t_real ber{ 0.03 };
	privacy_amplification_role paRole{ privacy_amplification_role::Undefined };
	t_integer securityParameter{ 1 };
	bool bypassHashing{ true };
	t_bool bypassPrivacyAmplification{ true };

	// Private Methods 
	template<std::size_t N>
	std::bitset<N> hashFn(std::bitset<N> bitsToHash);
	std::vector<t_binary> hashTheBits(const std::vector<t_binary>& vectorBitsToHash);

};
#endif // !PRIVACY_AMPLIFICATION_H_
