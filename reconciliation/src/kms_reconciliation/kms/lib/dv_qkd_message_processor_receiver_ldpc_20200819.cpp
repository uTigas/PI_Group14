#include "dv_qkd_message_processor_receiver_ldpc_20200819.h"

void MessageProcessorReceiver::initialize(void) 
{
	sindrome.resize(sindromeSize);
	sindrome_PosOut = sindromeSize;
}

bool MessageProcessorReceiver::runBlock(void)
{
	bool alive = false;

	int ready = inputSignals[0]->ready();
	
	for (auto k = 0; k < ready; ++k) 
	{
			t_message messageIn;
			inputSignals[0]->bufferGet(&messageIn);

			std::string mType = messageIn.getMessageType();
			std::string mDataLength = messageIn.getMessageDataLength();
			size_t dataLength = stoi(mDataLength);
			std::string mDataString = messageIn.getMessageData();
			//size_t dataLength = mDataString.size();
			//std::vector <int> mDataVector;
			std::string num{ "" };

			//t_message_data mData;


			if (mType == "BasisReconciliationMsg")
			{
				for (auto k = 0; k < dataLength; k++)
				{
					t_binary value = (t_binary)(mDataString.at(k) - '0');
					outputSignals[0]->bufferPut((t_binary)(value));
				}
				alive = true;

			}
			else if (mType == "SindromeMsg") 
			{
				if (sindrome_PosOut == sindromeSize)
				{
					for (int k = 0; k < sindromeSize; k++)
						sindrome[k] = (t_binary)(mDataString.at(k) - '0');

					sindrome_PosOut = 0;
					alive = true;
				}

				if (sindrome_PosOut < sindromeSize)
				{	
					auto space = outputSignals[1]->space();
					auto process = std::min(space, sindromeSize - sindrome_PosOut);
					if (process > 0) alive = true;
					for (auto k = 0; k < process; k++)
						outputSignals[1]->bufferPut((t_binary) sindrome[(size_t) sindrome_PosOut + k]);

					sindrome_PosOut = sindrome_PosOut + process;
				}
			}
			else if (mType == "ErrorCorrectionPermutationsMsg") {

				for (int w = 0; w < dataLength; w++) {
					std::string data;
					data = mDataString.at(w);
					if (data == "#") {
						permutationsIn.push_back(stoi(num));
						num = "";
					}
					else {
						num.append(data);
					}
				}

				alive = true;
			}
			else if (mType == "PrivacyAmplificationSeedsMsg") {

				for (int w = 0; w < dataLength; w++) {
					std::string data;
					data = mDataString.at(w);
					if (data == "#") {
						privacySeedsIn.push_back(stoi(num));
						num = "";
					}
					else {
						num.append(data);
					}
				}

				alive = true;
			}
			else if (mType == "ParameterEstimationSeedAndBitsMsg") {
				bool gotSeed = false;

				for (int w = 0; w < dataLength; w++) {
					std::string data;
					data = mDataString.at(w);
					if (!gotSeed) {
						if (data == "#") {
							paramEstIn.push_back(stoi(num));
							num = "";
							gotSeed = true;
						}
						else {
							num.append(data);
						}
					}
					else {
						char datac = mDataString.at(w);
						if (datac == '1') {
							paramEstIn.push_back(1);
						}
						else if (datac == '0') {
							paramEstIn.push_back(0);
						}
						else {
							std::cout << "Error during parameter estimation message reception.\n";
						}
					}
				}

				alive = true;
			}
			else if (mType == "ToggleBERChangeMsg") {

				for (int w = 0; w < dataLength; w++) {
					char data = mDataString.at(k);

					if (data == '1') {
						errCorrBerIn.push_back(1);
					}
					else if (data == '0') {
						errCorrBerIn.push_back(0);
					}
					else if (data == '2') {
						errCorrBerIn.push_back(2);
					}
					else if (data == '3') {
						errCorrBerIn.push_back(3);
					}
					else if (data == '4') {
						errCorrBerIn.push_back(3);
					}
					else if (data == '5') {
						errCorrBerIn.push_back(5);
					}
					else if (data == '-') {
						char aux_data = mDataString.at((unsigned _int64)w + 1);
						if (aux_data == '2') {
							errCorrBerIn.push_back(-2);
							w++;
						}
						else if (aux_data == '1') {
							errCorrBerIn.push_back(-1);
							w++;
						}
					}

				}

				alive = true;
			}
		}
	

	alive = outputReceivedData(basisIn, *outputSignals[0], basisReconRcvStarted) || alive;
	//if (outputSignals.size() >= 2) alive = outputReceivedData(parityIn, *outputSignals[1], errCorrParitiesStarted) || alive;
	if (outputSignals.size() >= 3) alive = outputReceivedData(permutationsIn, *outputSignals[2], errCorrPermStarted) || alive;
	if (outputSignals.size() >= 4) alive = outputReceivedData(privacySeedsIn, *outputSignals[3], privacySeedsStarted) || alive;
	if (outputSignals.size() >= 5) alive = outputReceivedData(paramEstIn, *outputSignals[4], privacySeedsStarted) || alive;
	if (outputSignals.size() >= 6) alive = outputReceivedData(errCorrBerIn, *outputSignals[5], paramEstRcvStarted) || alive;

	return alive;
}

bool MessageProcessorReceiver::outputReceivedData(std::vector<t_integer>& dataVector, Signal& outputSignal, bool& started)
{
	bool alive = false;
	int space = outputSignal.space();

	if ((dataVector.size() > 0) && (space > 0)) {

		if (!started) {
			outputSignal.bufferPut((t_real)dataVector.size());
			started = true;
			space--;
		}

		int process = std::min(space, (int)dataVector.size());

		for (auto m = 0; m < process; ++m) {
			outputSignal.bufferPut((t_real)dataVector[m]);
		}
		dataVector.erase(dataVector.begin(), dataVector.begin() + process);
		if (0 == dataVector.size()) started = false;
		alive = true;

	}

	return alive;
}