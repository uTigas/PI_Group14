#include "dv_qkd_privacy_amplification_20200819.h"
#include <bitset>

constexpr int64_t HASH_BIT_SET_SIZE{ 256 };

void PrivacyAmplification::initialize(void)
{
	vectorHashedBits.reserve(HASH_BIT_SET_SIZE);
	vectorBitsToHash.reserve(HASH_BIT_SET_SIZE);
	bitsOnHold.reserve(MAX_BUFFER_LENGTH);
}

bool PrivacyAmplification::runBlock(void)
{
	bool alive = false;

	if (bypassPrivacyAmplification)
	{
		auto ready = inputSignals[0]->ready();
		auto space = outputSignals[0]->space();
		auto process = std::min(ready, space);
		totalNumberOfInputBits += process;
		totalNumberOfOutputBits += process;
		for (; process--;)
		{
			t_binary val{ 0 };
			inputSignals[0]->bufferGet(&val);
			outputSignals[0]->bufferPut(val);
		}

		for (auto k = 1; k < inputSignals.size(); k++)
		{
			auto process = inputSignals[k]->ready();
			for (; process--;)
				inputSignals[k]->bufferGet();
		}

		return alive;
	}

	// This block should use a seeded hash function instead of the default one,
	// with the seeds being updated on regular intervals. Message signals are in place to do that,
	// and roles are implemented to define a transmitter and a receiver for the seeds. It should
	// be easy to implement when a proper hash function is chosen



	std::vector<t_binary> outputKey{};
	std::vector<t_binary> inputKey{};
//	int numberOfBits{ 0 };
	t_binary keyIn;

	int space = outputSignals[0]->space();
	int ready = inputSignals[0]->ready();

	/*for (auto k = 0; k < ready; ++k) {
		inputSignals[0]->bufferGet(&keyIn);
		bitsOnHold.push_back(keyIn);
	}*/
//	if (paRole == privacy_amplification_role::SeedTransmitter) {
/*		bool seedsOnHold = !outputSignals[1]->getBufferEmpty();
//		if (seedsOnHold) return alive;
//	} else if (paRole == privacy_amplification_role::SeedReceiver){
	/*	int seedsReady = inputSignals[1]->ready();
//	} else {
//		cout << "No role defined in privacy amplification.\n";
//	}*/

//	int process = std::min(space, (t_integer)bitsOnHold.size());
	int process = std::min(space, ready);
	int totalBlocks = (int)floor(process / HASH_BIT_SET_SIZE);
	process = totalBlocks * HASH_BIT_SET_SIZE;
	if (process == 0) return alive;
	
	// UniformRandomRealNumbers<> uniformeRandomNumberBetweenZeroAndOne{ seed, 0, 1 };

	totalNumberOfInputBits += process;
	for (auto k = 0; k < process; ++k) {
		inputSignals[0]->bufferGet(&keyIn);
//		keyIn = bitsOnHold[0];
//		bitsOnHold.erase(bitsOnHold.begin());
		vectorBitsToHash.push_back(keyIn);
		inputKey.push_back(keyIn);
		if (vectorBitsToHash.size() == HASH_BIT_SET_SIZE) {
//			vectorHashedBits = hashTheBits(vectorBitsToHash);
			vectorHashedBits = vectorBitsToHash;
			outputKey.insert(outputKey.end(), vectorHashedBits.begin(), vectorHashedBits.end());
			vectorBitsToHash = {};
			vectorHashedBits = {};
		}
	}
		
	int bitsToIgnore = (int)(2*round((t_real)HASH_BIT_SET_SIZE * ber) + securityParameter);
	//bitsToIgnore=0;
	int bitsToSend = HASH_BIT_SET_SIZE - bitsToIgnore;

	for (auto m = 0; m < totalBlocks; ++m) {
		totalNumberOfOutputBits += bitsToSend;
		for (auto k = 0; k < bitsToSend; ++k) 
		{
			if (bypassHashing) 
				outputSignals[0]->bufferPut(inputKey[(m * HASH_BIT_SET_SIZE) + k]);
			else 
				outputSignals[0]->bufferPut(outputKey[(m * HASH_BIT_SET_SIZE) + k]);
		}
	}

//	for (auto k = 0; k < bitsToSend; ++k) {
//		outputSignals[0]->bufferPut(outputKey[k]);
//	}

	alive = true;

	return alive; 
}
template<std::size_t N>
std::bitset<N> PrivacyAmplification::hashFn(std::bitset<N> bitsToHash) {
	std::hash<std::bitset<N>> hash_fn;

	std::bitset< N > hashedBits = hash_fn(bitsToHash);
	
	return hashedBits;
}

std:: vector<t_binary> PrivacyAmplification::hashTheBits(const std::vector<t_binary>& vectorBitsToHash_f) {
	std::string bitString{ "" };
	std::vector<t_binary> vectorHashedBits_f;

	for (unsigned int  k = 0; k < vectorBitsToHash_f.size(); ++k) {
		bitString = bitString + std::to_string(vectorBitsToHash_f[k]);
	}

	std::bitset<HASH_BIT_SET_SIZE> bitsToHash_f(bitString);
	std::bitset< HASH_BIT_SET_SIZE > hashedBits_f = hashFn(bitsToHash_f);
	for (unsigned int  m = 0; m < hashedBits_f.size(); ++m) {
		(vectorHashedBits_f).push_back(hashedBits_f[m]);
	}

	return vectorHashedBits_f;
}
