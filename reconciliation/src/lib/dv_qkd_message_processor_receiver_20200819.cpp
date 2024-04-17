#include "dv_qkd_message_processor_receiver_20200819.h"

void MessageProcessorReceiver::initialize(void) {}

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
			//int dataLength = stoi(mDataLength);
			std::string mDataString = messageIn.getMessageData();
			size_t dataLength = mDataString.size();
			//std::vector <int> mDataVector;
			std::string num{ "" };

			//t_message_data mData;


			if (mType == "BasisReconciliationMsg") {

				for (int w = 0; w < dataLength; w++) {
					char data = mDataString.at(w);

					if (data == '1') {
						basisIn.push_back(1);
					}
					else if (data == '0') {
						basisIn.push_back(0);
					}
					else if (data == '2') {
						basisIn.push_back(2);
					}
					else if (data == '3') {
						basisIn.push_back(3);
					}
					else if (data == '4') {
						basisIn.push_back(3);
					}
					else if (data == '5') {
						basisIn.push_back(5);
					}
					else if (data == '-') {
						char aux_data = mDataString.at((unsigned _int64)w + 1);
						if (aux_data == '2') {
							basisIn.push_back(-2);
							w++;
						}
						else if (aux_data == '1') {
							basisIn.push_back(-1);
							w++;
						}
					}

				}

				alive = true;
			}
			else if (mType == "ErrorCorrectionMsg") {

				for (int w = 0; w < dataLength; w++) {
					char data = mDataString.at(w);

					if (data == '1') {
						parityIn.push_back(1);
					}
					else if (data == '0') {
						parityIn.push_back(0);
					}
					else if (data == '2') {
						parityIn.push_back(2);
					}
					else if (data == '3') {
						parityIn.push_back(3);
					}
					else if (data == '4') {
						parityIn.push_back(3); // Error here?
					}
					else if (data == '5') {
						parityIn.push_back(5);
					}
					else if (data == '-') {
						char aux_data = mDataString.at((unsigned _int64)w + 1);
						if (aux_data == '2') {
							parityIn.push_back(-2);
							w++;
						}
						else if (aux_data == '1') {
							parityIn.push_back(-1);
							w++;
						}
					}

				}

				alive = true;
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
	if (outputSignals.size() >= 2) alive = outputReceivedData(parityIn, *outputSignals[1], errCorrParitiesStarted) || alive;
	if (outputSignals.size() >= 3) alive = outputReceivedData(permutationsIn, *outputSignals[2], errCorrPermStarted) || alive;
	if (outputSignals.size() >= 4) alive = outputReceivedData(privacySeedsIn, *outputSignals[3], privacySeedsStarted) || alive;
	if (outputSignals.size() >= 5) alive = outputReceivedData(paramEstIn, *outputSignals[4], privacySeedsStarted) || alive;
	if (outputSignals.size() >= 6) alive = outputReceivedData(errCorrBerIn, *outputSignals[5], paramEstRcvStarted) || alive;

	/*// Output Parameter Estimation
	int spacePE = outputSignals[4]->space();
	if ((paramEstIn.size() > 0) && (spacePE > 0)) {

		if (!paramEstRcvStarted) {
			outputSignals[4]->bufferPut((t_real)paramEstIn.size());
			paramEstRcvStarted = true;
			spacePE--;
		}

		int process = std::min(spacePE, (int)paramEstIn.size());

		for (auto m = 0; m < process; ++m) {
			outputSignals[4]->bufferPut((t_real)paramEstIn[m]);
		}
		paramEstIn.erase(paramEstIn.begin(), paramEstIn.begin() + process);
		if (0 == paramEstIn.size()) paramEstRcvStarted = false;
		alive = true;
	}*/

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