#include "dv_qkd_message_processor_transmitter_20200819.h"

void MessageProcessorTransmitter::initialize(void) {}

bool MessageProcessorTransmitter::runBlock(void)
{
	bool alive = false;

	// Bob input signals: {&BasisToSend, &ParityToSend, &PermutationsToSend, &UnusedSignal, &BERToAlice, &ToggleBERChange}
	int ready0 = inputSignals[0]->ready();
	int ready1 = inputSignals[1]->ready();
	int ready2{ 0 };
	int ready3{ 0 };
	int ready4{ 0 };
	int ready5{ 0 };
	int ready;
	ready = std::max(ready0, ready1);

	if (inputSignals.size() >= 3) {
		ready2 = inputSignals[2]->ready();
		ready = std::max(ready, ready2);

	}
	if (inputSignals.size() >= 4) {
		ready3 = inputSignals[3]->ready();
		ready = std::max(ready, ready3);
	}
	if (inputSignals.size() >= 5) {
		ready4 = inputSignals[4]->ready();
		ready = std::max(ready, ready4);
	}
	if (inputSignals.size() >= 6) {
		ready5 = inputSignals[5]->ready();
		ready = std::max(ready, ready5);
	}

	int space = outputSignals[0]->space();

	if (0 == ready || 0 == space) return alive;


	// Basis reconciliation message
	if ((ready0 > 0) && (outputSignals[0]->space() > 0)) {
		t_integer startIdx{ 0 };
		if (!basisReconStarted) {
			inputSignals[0]->bufferGet(&basisReconLength);
			basisReconStarted = true;
			startIdx = 1;
		}

		t_real basisReconData;
		for (auto k = startIdx; k < ready0; ++k) {
			inputSignals[0]->bufferGet(&basisReconData);
			basisReconVector.push_back((t_integer)basisReconData);
		}

		if (basisReconVector.size() == basisReconLength) {
			t_message_type msgType = "BasisReconciliationMsg";
			std::string messageText = "";

			for (unsigned int w = 0; w < basisReconVector.size(); ++w) {
				messageText.append(std::to_string(basisReconVector[w]));
			}

			t_message messageBasisRecon;
			messageBasisRecon.setMessageType(msgType);
			messageBasisRecon.setMessageDataLength(std::to_string(messageText.size()));
			messageBasisRecon.setMessageData(messageText);

			outputSignals[0]->bufferPut(messageBasisRecon);
			basisReconVector.erase(basisReconVector.begin(), basisReconVector.end());
			basisReconStarted = false;

		}
		alive = true;
	}


	// Error correction - Parities 
	if ((ready1 > 0) && (outputSignals[0]->space() > 0)) {
		t_integer startIdx{ 0 };
		if (!ErrCorrParityStarted) {
			inputSignals[1]->bufferGet(&ErrCorrParityLength);
			ErrCorrParityStarted = true;
			startIdx = 1;
		}

		t_real paritiesData;
		for (auto k = startIdx; k < ready1; ++k) {
			inputSignals[1]->bufferGet(&paritiesData);
			ErrCorrParityVector.push_back((t_integer)paritiesData);
		}

		if (ErrCorrParityVector.size() == ErrCorrParityLength) {
			t_message_type msgType = "ErrorCorrectionMsg";
			std::string messageText = "";

			for (unsigned int w = 0; w < ErrCorrParityVector.size(); ++w) {
				messageText.append(std::to_string(ErrCorrParityVector[w]));
			}

			t_message errCorrParityMsg;
			errCorrParityMsg.setMessageType(msgType);
			errCorrParityMsg.setMessageData(messageText);

			outputSignals[0]->bufferPut(errCorrParityMsg);
			ErrCorrParityVector.erase(ErrCorrParityVector.begin(), ErrCorrParityVector.end());
			ErrCorrParityStarted = false;

		}
		alive = true;
	}

	// Error Correction Permutations
	if ((ready2 > 0) && (outputSignals[0]->space() > 0)) {
		t_integer startIdx{ 0 };
		if (!ErrCorrPermStarted) {
			inputSignals[2]->bufferGet(&ErrCorrPermLength);
			ErrCorrPermStarted = true;
			startIdx = 1;
		}

		t_real permutationsIn;
		for (auto k = startIdx; k < ready2; ++k) {
			inputSignals[2]->bufferGet(&permutationsIn);
			ErrCorrPermVector.push_back((t_integer)permutationsIn);
		}

		if (ErrCorrPermVector.size() == ErrCorrPermLength) {
			t_message_type msgType = "ErrorCorrectionPermutationsMsg";
			std::string messageText = "";


			for (unsigned int w = 0; w < ErrCorrPermVector.size(); ++w) {
				messageText.append(std::to_string(ErrCorrPermVector[w]));
				messageText.append("#");
			}


			t_message messageErrorCorrection;
			messageErrorCorrection.setMessageType(msgType);
			messageErrorCorrection.setMessageData(messageText);

			outputSignals[0]->bufferPut(messageErrorCorrection);
			ErrCorrPermVector.erase(ErrCorrPermVector.begin(), ErrCorrPermVector.end());
			ErrCorrPermStarted = false;

		}
		alive = true;
	}

	// Transmit privacy amplification hashing seeds
	if ((ready3 > 0) && (outputSignals[0]->space() > 0)) {
		t_integer startIdx{ 0 };
		if (!privacySeedsStarted) {
			inputSignals[3]->bufferGet(&privacySeedsLength);
			privacySeedsStarted = true;
			startIdx = 1;
		}

		t_real paramEstData;
		for (auto k = startIdx; k < ready3; ++k) {
			inputSignals[3]->bufferGet(&paramEstData);
			privacySeedsBVector.push_back((t_integer)paramEstData);
		}

		if (privacySeedsBVector.size() == privacySeedsLength) {
			t_message_type msgType = "PrivacyAmplificationSeedsMsg";
			std::string messageText = "";

			for (unsigned int w = 0; w < privacySeedsBVector.size(); ++w) {
				messageText.append(std::to_string(privacySeedsBVector[w]));
				messageText.append("#");
			}


			t_message messagePASeeds;
			messagePASeeds.setMessageType(msgType);
			messagePASeeds.setMessageData(messageText);

			outputSignals[0]->bufferPut(messagePASeeds);
			privacySeedsBVector.erase(privacySeedsBVector.begin(), privacySeedsBVector.end());
			privacySeedsStarted = false;

		}
		alive = true;
	}

	//////////////////////////////////////////////////
	// Parameter estimation seed and bits

	if ((ready4 > 0) && (outputSignals[0]->space() > 0)) {
		t_integer startIdx{ 0 };
		t_message_type msgType = "ParameterEstimationSeedAndBitsMsg";
		if (!ParamEstStarted) {
			inputSignals[4]->bufferGet(&paramEstLength);
			ParamEstStarted = true;
			startIdx = 1;
		}

		t_real paramEstData;
		for (auto k = startIdx; k < ready4; ++k) {
			inputSignals[4]->bufferGet(&paramEstData);
			paramEstVector.push_back((t_integer)paramEstData);
		}

		if (paramEstVector.size() == paramEstLength) {

			std::string messageText = "";

			messageText.append(std::to_string(paramEstVector[0]));
			messageText.append("#");
			int start = 1;
			for (unsigned int w = start; w < paramEstVector.size(); ++w) {
				messageText.append(std::to_string(paramEstVector[w]));
			}


			t_message messageParamEst;
			messageParamEst.setMessageType(msgType);
			messageParamEst.setMessageDataLength(std::to_string(messageText.size()));
			messageParamEst.setMessageData(messageText);

			outputSignals[0]->bufferPut(messageParamEst);
			paramEstVector.erase(paramEstVector.begin(), paramEstVector.end());
			ParamEstStarted = false;

		}
		alive = true;
	}

	// Change BER value
	if ((ready5 > 0) && (outputSignals[0]->space() > 0)) {
		t_integer startIdx{ 0 };
		if (!ErrCorrBerStarted) {
			inputSignals[5]->bufferGet(&ErrCorrBerLength);
			ErrCorrBerStarted = true;
			startIdx = 1;
		}

		t_real berChangeIn;
		for (auto k = startIdx; k < ready5; ++k) {
			inputSignals[5]->bufferGet(&berChangeIn);
			ErrCorrBerVector.push_back((t_integer)berChangeIn);
		}

		if (ErrCorrBerVector.size() == ErrCorrBerLength) {
			t_message_type msgType = "ToggleBERChangeMsg";
			std::string messageText = "";

			for (unsigned int w = 0; w < ErrCorrBerVector.size(); ++w) {
				messageText.append(std::to_string(ErrCorrBerVector[w]));
			}

			t_message messageTBC;
			messageTBC.setMessageType(msgType);
			messageTBC.setMessageData(messageText);

			outputSignals[0]->bufferPut(messageTBC);
			ErrCorrBerVector.erase(ErrCorrBerVector.begin(), ErrCorrBerVector.end());
			ErrCorrBerStarted = false;
		}
		alive = true;
	}


	return alive;
}
