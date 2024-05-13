#include "dv_qkd_basis_reconciliation_20200819.h"

// Many of the type conversions on this file are due to the lack of discrete signal types.
// Amplitude discrete signals are represented as t_real, and binary signals are unsigned integers.
void BasisReconciliation::initializer(void)
{
	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());

	outputSignals[1]->setSymbolPeriod(inputSignals[1]->getSymbolPeriod());
	outputSignals[1]->setSamplingPeriod(inputSignals[1]->getSamplingPeriod());
	outputSignals[1]->setFirstValueToBeSaved(inputSignals[1]->getFirstValueToBeSaved());

	storedData.reserve(MAX_BUFFER_LENGTH);
	msgOutputData.reserve(MAX_BUFFER_LENGTH);
	msgInputData.reserve(MAX_BUFFER_LENGTH);
}

bool BasisReconciliation::runBlock(void)
{
	bool alive{ false };

	if (basisReconciliationRole == basis_reconciliation_role::Alice) {

		if ((startedSending) && (msgOutputData.size() > 0)) {
			finishedSending = sendToMsg(msgOutputData, *outputSignals[2], startedSending);
			alive = true;
			if (!finishedSending) return alive;
		}



		t_binary dataInBinary{ 0 };
		t_real dataInReal{ 0 };
		t_binary basisIn{ 0 };
		t_real basisFromBob{ 0 };

		// Separate receiving from bob and processing/sending response
		int readyFromBob = inputSignals[2]->ready();

		if (readyFromBob > 0) {
			t_integer startIdx{ 0 };
			if (!startedReceiving) {
				inputSignals[2]->bufferGet(&numberOfBasisToReceive);
				startedReceiving = true;
				finishedReceiving = false;
				startIdx = 1;
			}
			for (auto k = startIdx; k < readyFromBob; ++k) {
				inputSignals[2]->bufferGet(&basisFromBob);
				msgInputData.push_back((t_integer)basisFromBob);
			}
			if (msgInputData.size() == numberOfBasisToReceive) {
				finishedReceiving = true;
				startedReceiving = false;
			}
			alive = true;
		}


		// Output and save to msgOutputData
		if ((finishedReceiving) && (msgInputData.size() > 0)) {
			readyFromBob = (t_integer)msgInputData.size();
			int ready = std::min(inputSignals[0]->ready(), inputSignals[1]->ready());
			int space = std::min(outputSignals[0]->space(), outputSignals[1]->space());
			int process = std::min(ready, space);
			process = std::min(process, readyFromBob);

			if (process <= 0) return alive;

			//		if (readyFromBob < process) return alive;


			if (signal_value_type::t_binary == inputSignals[0]->getValueType()) {
				for (auto k = 0; k < process; ++k) {
					inputSignals[0]->bufferGet(&dataInBinary);
					inputSignals[1]->bufferGet(&basisIn);
					basisFromBob = msgInputData[k];
					receivedQubits++;
					
					if (basisFromBob == noClickNumber) {
						noClicks++;
					}
					else if (basisFromBob == doubleClickNumber) {
						doubleClickNumber++;
					}

					if (basisFromBob == (t_binary)basisIn) {
						if (((t_binary)dataInBinary != 0) && ((t_binary)dataInBinary != 1)) {
							std::cout << "";
						}
						outputSignals[0]->bufferPut((t_binary)dataInBinary);
						outputSignals[1]->bufferPut((t_binary)dataInBinary);
						msgOutputData.push_back(1);
						reconciledBits++;
					}
					else {
						msgOutputData.push_back(0);
					}
				}
			}
			else if (signal_value_type::t_real == inputSignals[0]->getValueType()) {
				for (auto k = 0; k < process; ++k) {
					inputSignals[0]->bufferGet(&dataInReal);
					inputSignals[1]->bufferGet(&basisIn);
					basisFromBob = msgInputData[k];
					receivedQubits++;

					if (dataInReal == noClickNumber) {
						noClicks++;
					}
					else if (dataInReal == doubleClickNumber) {
						doubleClickNumber++;
					}

					if (basisFromBob == (t_binary)basisIn) {
						if (((t_binary)dataInReal != 0) && ((t_binary)dataInReal != 1)) {
							std::cout << "";
						}
						outputSignals[0]->bufferPut((t_binary)dataInReal);
						outputSignals[1]->bufferPut((t_binary)dataInReal);
						msgOutputData.push_back(1);
						reconciledBits++;
					}
					else {
						msgOutputData.push_back(0);
					}
				}
			}
			else {
				std::cout << "Invalid signal type.";
			}

			msgInputData.erase(msgInputData.begin(), msgInputData.begin() + process);
			alive = true;
		}

		// Send msg to Bob
		if (msgInputData.empty() && !startedSending && !msgOutputData.empty()) {
			finishedSending = sendToMsg(msgOutputData, *outputSignals[2], startedSending);
			alive = true;
		}


	}
	else if (basisReconciliationRole == basis_reconciliation_role::Bob) {
		t_binary dataInBinary{ 0 };
		t_real dataInReal{ 0 };
		t_binary basisIn{ 0 };
		t_real basisFromAlice{ 0 };

		if (msgOutputData.size() > 0) {
			finishedSending = sendToMsg(msgOutputData, *outputSignals[2], startedSending);
			if (!finishedSending) return true;
		}

		if (!bobBasisReconStatus) {
			// Start Basis Reconciliation
			int ready = std::min(inputSignals[0]->ready(), inputSignals[1]->ready());
			int minBufSz = (int)std::min(outputSignals[0]->getBufferLength() - 1, outputSignals[1]->getBufferLength() - 1);
			ready = std::min(ready, minBufSz);
			const t_integer constDoubleClickNumber = doubleClickNumber;
			const t_integer constNoClickNumber = noClickNumber;

			if (ready <= 0) return alive;
			//outputSignals[2]->bufferPut
			if (signal_value_type::t_binary == inputSignals[0]->getValueType()) {
				for (auto k = 0; k < ready; ++k) {
					inputSignals[0]->bufferGet(&dataInBinary);
					inputSignals[1]->bufferGet(&basisIn);
					if ((t_integer)dataInBinary == 0) {
						msgOutputData.push_back((t_integer)basisIn);
						storedData.push_back((t_integer)dataInBinary);
					}
					else if (1 == (t_integer)dataInBinary) {
						msgOutputData.push_back((t_integer)basisIn);
						storedData.push_back((t_integer)dataInBinary);
					}
					else if (constDoubleClickNumber == (int)dataInBinary) {
						msgOutputData.push_back((t_integer)dataInBinary);
						storedData.push_back((t_integer)dataInBinary);
						doubleClicks++;
					}
					else if (constNoClickNumber == (t_integer)dataInBinary) {
						msgOutputData.push_back((t_integer)dataInBinary);
						storedData.push_back((t_integer)dataInBinary);
						noClicks++;
					}
					else {
						std::cout << "Error. Invalid data.";
						break;
					}
				}
			}
			else if (signal_value_type::t_real == inputSignals[0]->getValueType()) {
				for (auto k = 0; k < ready; ++k) {
					inputSignals[0]->bufferGet(&dataInReal);
					inputSignals[1]->bufferGet(&basisIn);
					if ((t_integer)dataInReal == 0) {
						msgOutputData.push_back((t_integer)basisIn);
						storedData.push_back((t_integer)dataInReal);
					}
					else if (1 == (t_integer)dataInReal) {
						msgOutputData.push_back((t_integer)basisIn);
						storedData.push_back((t_integer)dataInReal);
					}
					else if (constDoubleClickNumber == dataInReal) {
						msgOutputData.push_back((t_integer)dataInReal);
						storedData.push_back((t_integer)dataInReal);
						doubleClicks++;
					}
					else if (constNoClickNumber == (t_integer)dataInReal) {
						msgOutputData.push_back((t_integer)dataInReal);
						storedData.push_back((t_integer)dataInReal);
						noClicks++;
					}
					else {
						std::cout << "Error. Invalid data.";
						break;
					}
				}
			}
			else {
				std::cout << "Invalid signal type.";
			}

			finishedSending = sendToMsg(msgOutputData, *outputSignals[2], startedSending);

			receivedQubits = receivedQubits + ready;
			bobBasisReconStatus = true;
			alive = true;
		}
		else {
			// Act on response sent by Alice
			int readyFromAlice = inputSignals[2]->ready();

			if (readyFromAlice > 0) {
				//				t_real basisFromAlice;
				t_integer startIdx = 0;
				if (!startedReceiving) {
					inputSignals[2]->bufferGet(&numberOfBasisToReceive);
					startedReceiving = true;
					finishedReceiving = false;
					startIdx++;
				}
				for (auto k = startIdx; k < readyFromAlice; ++k) {
					inputSignals[2]->bufferGet(&basisFromAlice);
					msgInputData.push_back((t_integer)basisFromAlice);
				}
				if (msgInputData.size() == numberOfBasisToReceive) {
					finishedReceiving = true;
					startedReceiving = false;
				}
				alive = true;
			}

			if ((finishedReceiving) && (msgInputData.size() > 0)) {
				int space = outputSignals[0]->space();
				space = std::min(space, outputSignals[1]->space());
				int process = std::min((int)msgInputData.size(), space);
				if (process <= 0) return alive;


				t_integer dataOut;
				for (auto k = 0; k < process; ++k) {
					basisFromAlice = msgInputData[k];
					switch ((t_integer)basisFromAlice) {
					case 1:
						dataOut = storedData[k];
						if (((dataOut != 0) && (dataOut != 1)) || (dataOut > 1)) {
							std::cout << "";
						}
						outputSignals[0]->bufferPut((t_binary)dataOut);
						outputSignals[1]->bufferPut((t_binary)dataOut);
						if (dataOut > 1) {
							std::cout << "This shouldn't happen";
						}
						reconciledBits++;
						break;
					case 0:
						break;
					default:
						std::cout << "Error. Invalid data.";
						break;
					}
				}
				alive = true;
				storedData.erase(storedData.begin(), storedData.begin() + process);
				msgInputData.erase(msgInputData.begin(), msgInputData.begin() + process);
				readyFromAlice = (int)msgInputData.size();
				if (0 == readyFromAlice) bobBasisReconStatus = false;
			}

		}

	}
	else if (basisReconciliationRole == basis_reconciliation_role::Undefined) {
		std::cout << "Undefined role of the basis reconcilliation block.";
	}
	else {
		std::cout << "Undefined behavior on basis reconcilliation block.";
	}

	return alive;
}

bool BasisReconciliation::sendToMsg(std::vector <t_integer>& data, Signal& signalToMsg, bool& started) {
	int space = signalToMsg.space();
	if (space <= 0) return false;

	if (!started) signalToMsg.bufferPut((t_real)data.size());

	if (space <= 0) return false;
	started = true;

	int ready = (int)data.size();
	space = signalToMsg.space();
	int process = std::min(ready, space);

	for (auto k = 0; k < process; ++k) {
		signalToMsg.bufferPut((t_real)data[k]);
	}
	data.erase(data.begin(), data.begin() + process);

	if (data.empty()) {
		started = false;
		return true;
	}
	else {
		return false;
	}
}
