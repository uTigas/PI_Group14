#include "dv_qkd_error_correction_20200819.h"

void ErrorCorrection::initialize(void) 
{
	int maxInData = std::min(inputSignals[0]->getBufferLength(), inputSignals[1]->getBufferLength());
	int maxOutData = std::min(outputSignals[0]->getBufferLength(), outputSignals[1]->getBufferLength());
	maxData = std::min(maxOutData, maxInData);

	dataOnHold.reserve(1000000);
}

bool ErrorCorrection::runBlock(void)
{
	bool alive{ false };

	if ((int)keys.size() < numPasses) 
	{
		keys.resize(numPasses, {});
		for (auto k = 0; k < (int)keys.size(); ++k) 
		{
			keys[k].reserve(maximumSyndromeSize);
		}
	}

	int readyData = inputSignals[0]->ready();

	t_binary dataFromBuffer;

	inputBits = inputBits + readyData;

	for (auto k = 0; k < readyData; ++k) 
	{
		inputSignals[0]->bufferGet(&dataFromBuffer);
		dataOnHold.push_back((t_integer)dataFromBuffer);
	}

	if (!errorCorrectionStatus && keys[0].size() > 0) 
	{
		outputKey({ outputSignals[0] }, keys[0]);
		if (keys[0].size() > 0)
			return true;
	}

	if (paritiesMsgOutput.size() > 0) 
	{
		finishedSendingParitiesMsg = MessageProcessors::sendToMsgProc(paritiesMsgOutput, *outputSignals[1], sendingParitiesMsg);
		alive = true;
		if (!finishedSendingParitiesMsg) return alive;
	}

	if (errorCorrectionRole == ErrorCorrection::t_role::Alice) 
	{

		
		//		t_binary keyIn{ 0 };
		std::vector <t_integer> bobParities;

		if (!errorCorrectionStatus) {
			correctedInLookBack = 0;
			if ((int)lookBackPassesIdx.size() < numPasses) {
				lookBackPassesIdx.resize(numPasses, {});
			}
			if ((int)keyIdx.size() < numPasses) {
				keyIdx.resize(numPasses, {});
			}
			if ((int)reverseKeyIdx.size() < numPasses) {
				reverseKeyIdx.resize(numPasses, {});
			}
			if ((int)keys.size() < numPasses) {
				keys.resize(numPasses, {});
			}

			t_binary changeBER = 0;
			t_integer changeBerReady = inputSignals[4]->ready();
			if (changeBerReady > 0) {
				std::vector<t_integer> changeBerVector{};
				bool haveBer{ false };
				t_integer numVals{ 0 };
				MessageProcessors::recvFromMsgProc(changeBerVector, *inputSignals[4], haveBer, numVals);
				changeBER = (t_binary)changeBerVector[0];
			}
			if (inputSignals[3]->ready() >= 2 && changeBER == 1) {
				t_real realNumBits;
				inputSignals[3]->bufferGet(&BER);
				inputSignals[3]->bufferGet(&realNumBits);
				numBits = (int)realNumBits;
				partitionSize = ber2partitionSize(q, BER);
			}
			/*
			if (inputSignals[3]->ready() >= 2 && alreadyUsedBER) {
				t_real realNumBits;
				inputSignals[3]->bufferGet(&BER);
				inputSignals[3]->bufferGet(&realNumBits);
				numBits = (int)realNumBits;
				alreadyUsedBER = false;
				partitionSize = floor(q / max(BER, 0.005));
			}
			*/
			if (BER == 2) return alive;

			if (partitionSize * minimumNumberOfPartitions > minimumSyndromeSize) {
				partitionSize = (int)floor(minimumSyndromeSize / minimumNumberOfPartitions);
			}
			// Start Error Correction
//			int ready0 = inputSignals[0]->ready();
			int ready0 = (int)dataOnHold.size();
			int ready1 = (int)inputSignals[1]->ready()-1;

			//			int ready = ready1;

//			int space = outputSignals[1]->space();
			int process = ready1;

			currentPartitionSize = partitionSize;

			//			if (process <= 0) return alive;
			if ((ready0 < minimumSyndromeSize) || (0 >= process) || ((int64_t)ready0 < (int64_t)ready1 * (int64_t)currentPartitionSize)) return alive;
			//			if ((ready0 < maxData) || (0 >= process) || (ready0 < (ready1 - 1) * currentPartitionSize)) return alive;

						//numberOfPartitions = (t_integer)ceil((t_real)ready0 / partitionSize);
			
//			if (numberOfPartitions > space) return alive;
			testCount++;
			currentPass = 1;

			// Clear old vectors
			keys[0].erase(keys[0].begin(), keys[0].end());
			startPositions.erase(startPositions.begin(), startPositions.end());
			endPositions.erase(endPositions.begin(), endPositions.end());
			totalSearchSpace.erase(totalSearchSpace.begin(), totalSearchSpace.end());
			//			lookBackIdx.erase(lookBackIdx.begin(), lookBackIdx.end());
			parityResponses.erase(parityResponses.begin(), parityResponses.end());
			for (unsigned int k = 0; k < lookBackPassesIdx.size(); ++k) {
				lookBackPassesIdx[k].erase(lookBackPassesIdx[k].begin(), lookBackPassesIdx[k].end());
				keyIdx[k].erase(keyIdx[k].begin(), keyIdx[k].end());
				reverseKeyIdx[k].erase(reverseKeyIdx[k].begin(), reverseKeyIdx[k].end());
				keys[k].erase(keys[k].begin(), keys[k].end());
			}


			if (ready1 > 0) {
				finishedReceivingParitiesMsg = MessageProcessors::recvFromMsgProc(paritiesMsgInput, * inputSignals[1], receivingParitiesMsg, numberOfParitiesToReceive);
				alive = true;
				if (!finishedReceivingParitiesMsg) return alive;
			}

			
			bobParities.assign(paritiesMsgInput.begin(), paritiesMsgInput.end());
			paritiesMsgInput.erase(paritiesMsgInput.begin(), paritiesMsgInput.end());
			numberOfPartitions = (int)bobParities.size();

			t_integer dataToRead = (int)bobParities.size() * currentPartitionSize;

			for (auto k = 0; k < dataToRead; ++k) {
				t_binary keyIn{ 0 };
				keyIn = (t_binary)dataOnHold[0];
				//				inputSignals[0]->bufferGet(&keyIn);
				keys[0].push_back((t_integer)keyIn);
				keyIdx[currentPass - 1].push_back(k);
				reverseKeyIdx[currentPass - 1].push_back(k);
				dataOnHold.erase(dataOnHold.begin());
			}

			parities = {};
			int count{ 0 };
			for (auto w = 0; w < (int)numberOfPartitions; w++) {
				startPositions.push_back(partitionSize * w);
				endPositions.push_back((t_integer)std::min(((t_real)partitionSize * w + (t_real)partitionSize), (t_real)ready0));
				//				endPositions.push_back(partitionSize * w + std::min(ceil((t_real)partitionSize / 2.0), (t_real)process));
				t_real leftover = (t_real)ready0 - ((t_real)partitionSize * w);
				totalSearchSpace.push_back(std::min(partitionSize, (t_integer)leftover));

				int parity = 0;

				parity = getParity(keys[currentPass - 1], startPositions[w], endPositions[w], parityCount, keys, keyIdx, reverseKeyIdx, currentPass - 1);
				parities.push_back(parity);


				if (parity == bobParities[w]) {
			
					paritiesMsgOutput.push_back(2);
					parityResponses.push_back((t_integer)2);
					count++;
				}
				else {
					paritiesMsgOutput.push_back(0);
					parityResponses.push_back((t_integer)0);
					count++;
				}
			}

			/*
			alreadyUsedBER = true;
			*/
			errorCorrectionStatus = true;
			alive = true;

		}
		else 
		{
			// If first pass has already been done
			int readyEC = inputSignals[1]->ready()-1;

			//			int spaceKeyOutput = outputSignals[0]->space();
//			int spaceEC = outputSignals[1]->space();
			//			int space = std::min(spaceKeyOutput, spaceEC);
			bool noErrors = true;

			if ((readyEC <= 0)) return alive;

			parities.erase(parities.begin(), parities.end());
			if (currentPass == 1) {

				correctedBitsInPass = 0;
				correctedBitsInLookBack = 0;

				if (readyEC > 0) {
					finishedReceivingParitiesMsg = MessageProcessors::recvFromMsgProc(paritiesMsgInput, *inputSignals[1], receivingParitiesMsg, numberOfParitiesToReceive);
					alive = true;
					if (!finishedReceivingParitiesMsg) return alive;
				}

				bobParities.assign(paritiesMsgInput.begin(), paritiesMsgInput.end());
				paritiesMsgInput.erase(paritiesMsgInput.begin(), paritiesMsgInput.end());

				for (int w = 0; w < (int)bobParities.size(); ++w) {
					// Update testing partition
					int oldSz = endPositions[w] - startPositions[w];
					updateStartAndEndPositions(parityResponses[w], startPositions[w], endPositions[w], partitionSize, totalSearchSpace[w]);
					if (oldSz < partitionSize) {
						if (1 == parityResponses[w]) {
							totalSearchSpace[w] = totalSearchSpace[w] - oldSz;
						}
						else if (0 == parityResponses[w]) {
							totalSearchSpace[w] = oldSz;
						}
					}
					if (totalSearchSpace[w] == 0) {
						std::cout << "Error";
					}
					int parity;

					// Calculate parities or correct error
					if (2 == bobParities[w]) {
						parities.push_back(2);
						paritiesMsgOutput.push_back(2);
						parityResponses[w] = (2);
					}
					else {
						parity = getParity(keys[currentPass - 1], startPositions[w], endPositions[w], parityCount, keys, keyIdx, reverseKeyIdx, currentPass - 1);
						if (parity == bobParities[w]) {
							parities.push_back(parity);
							paritiesMsgOutput.push_back(1);
							parityResponses[w] = (1);
							noErrors = false;
						}
						else {
							parities.push_back(parity);
							paritiesMsgOutput.push_back(0);
							parityResponses[w] = (0);
							noErrors = false;
						}
					}
				}
				if (noErrors) {
					// Current Pass should be complete
					if (numPasses == 1) {
						//						for (unsigned int i = 0; i < keys[0].size(); ++i) {
						//							outputSignals[0]->bufferPut((t_binary)keys[0][i]);
						//							
						//						}
						outputKey({ outputSignals[0] }, keys[0]);
						currentPass = 0;
						errorCorrectionStatus = false;
						currentPassStarted = false;
					}
					else {
						currentPass++;
						currentPassStarted = false;
					}
				}
			}
			else {
				if (doingLookBack && !lookBackStarted) {


					startPositions.erase(startPositions.begin(), startPositions.end());
					endPositions.erase(endPositions.begin(), endPositions.end());
					totalSearchSpace.erase(totalSearchSpace.begin(), totalSearchSpace.end());
					parities.erase(parities.begin(), parities.end());
					parityResponses.erase(parityResponses.begin(), parityResponses.end());

					// Update indexes for the proper key
					for (unsigned int x = 0; x < lookBackPassesIdx[lookBackPass].size(); ++x) {
						lookBackPassesIdx[lookBackPass][x] = reverseKeyIdx[lookBackPass][lookBackPassesIdx[lookBackPass][x]];
					}

					// Sort lookback idx to filter repeated partitions
					std::sort(lookBackPassesIdx[lookBackPass].begin(), lookBackPassesIdx[lookBackPass].end());
					
					t_integer process = (t_integer)keys[0].size();
					numberOfPartitions = (t_integer)lookBackPassesIdx[lookBackPass].size();

					if (readyEC > 0) {
						finishedReceivingParitiesMsg = MessageProcessors::recvFromMsgProc(paritiesMsgInput, *inputSignals[1], receivingParitiesMsg, numberOfParitiesToReceive);
						alive = true;
						if (!finishedReceivingParitiesMsg) return alive;
					}

					bobParities.assign(paritiesMsgInput.begin(), paritiesMsgInput.end());
					paritiesMsgInput.erase(paritiesMsgInput.begin(), paritiesMsgInput.end());

					partitionSizeLB = partitionSize;
					for (unsigned int k = 0; k < lookBackPass; ++k) {
						if (doubleWindowSize && partitionSizeLB * 2 <= minimumSyndromeSize / minimumNumberOfPartitions) {
							partitionSizeLB = partitionSizeLB * 2;
						}
					}

					parities = {};

					std::vector<t_integer> startPositions2{};
					for (auto w = 0; w < (int)numberOfPartitions; w++) {
						t_integer startPos = (t_integer)(floor(t_real(lookBackPassesIdx[lookBackPass][w]) / partitionSizeLB) * partitionSizeLB);
						startPositions2.push_back(startPos);
					}
					std::vector<t_integer> startPositions3{};
					startPositions3.assign(startPositions2.begin(), startPositions2.end());
					auto last = std::unique(startPositions3.begin(), startPositions3.end());
					startPositions3.erase(last, startPositions3.end());

					for (auto w = 0; w < (int)numberOfPartitions; w++) {
						t_integer startPos = (t_integer)(floor(t_real(lookBackPassesIdx[lookBackPass][w]) / partitionSizeLB) * partitionSizeLB);
						if ((startPositions.empty()) || (startPositions.back() != startPos)) {
							startPositions.push_back(startPos);
							endPositions.push_back((t_integer)std::min(((t_real)startPos + (t_real)partitionSizeLB), (t_real)process));
							//	endPositions.push_back(partitionSize * w + std::min(ceil((t_real)partitionSize / 2.0), (t_real)process));
							t_real leftover = (t_real)process - ((t_real)startPos);
							totalSearchSpace.push_back(std::min(partitionSizeLB, (t_integer)leftover));

							int parity = 0;

							parity = getParity(keys[lookBackPass], startPositions.back(), endPositions.back(), parityCount, keys, keyIdx, reverseKeyIdx, lookBackPass);
							parities.push_back(parity);

							if (parity == bobParities[0]) {
								paritiesMsgOutput.push_back(2);
								parityResponses.push_back((t_integer)2);
							}
							else {
								paritiesMsgOutput.push_back(0);
								parityResponses.push_back((t_integer)0);
							}
							bobParities.erase(bobParities.begin());
						}
					}
					lookBackPassesIdx[lookBackPass].erase(lookBackPassesIdx[lookBackPass].begin(), lookBackPassesIdx[lookBackPass].end());
					alive = true;
					lookBackStarted = true;
				}
				else if (doingLookBack && lookBackStarted) {

					if (readyEC > 0) {
						finishedReceivingParitiesMsg = MessageProcessors::recvFromMsgProc(paritiesMsgInput, *inputSignals[1], receivingParitiesMsg, numberOfParitiesToReceive);
						alive = true;
						if (!finishedReceivingParitiesMsg) return alive;
					}

					bobParities.assign(paritiesMsgInput.begin(), paritiesMsgInput.end());
					paritiesMsgInput.erase(paritiesMsgInput.begin(), paritiesMsgInput.end());

					for (int w = 0; w < (int)bobParities.size(); ++w) {
						
						// Update testing partition
						int oldSz = endPositions[w] - startPositions[w];
						updateStartAndEndPositions(parityResponses[w], startPositions[w], endPositions[w], partitionSizeLB, totalSearchSpace[w]);
						if (oldSz < partitionSizeLB) {
							if (1 == parityResponses[w]) {
								totalSearchSpace[w] = totalSearchSpace[w] - oldSz;
							}
							else if (0 == parityResponses[w]) {
								totalSearchSpace[w] = oldSz;
							}
						}
						if (totalSearchSpace[w] == 0) {
							std::cout << "Error";
						}

						int parity;

						// Calculate parities or correct error
						if (2 == bobParities[w]) {
							parities.push_back(2);
							paritiesMsgOutput.push_back(2);
							parityResponses[w] = (2);
						}
						else if (5 == bobParities[w]) {
							correctedInLookBack++;
							for (unsigned int k = 0; k < currentPass - 1; ++k) {
								if (k != lookBackPass) {
									lookBackPassesIdx[k].push_back(keyIdx[lookBackPass][startPositions[w]]);

								}
							}
							parities.push_back(2);
							paritiesMsgOutput.push_back(2);
							parityResponses[w] = (2);

						}
						else {
							parity = getParity(keys[lookBackPass], startPositions[w], endPositions[w], parityCount, keys, keyIdx, reverseKeyIdx, lookBackPass);
							if (parity == bobParities[w]) {
								parities.push_back(parity);
								paritiesMsgOutput.push_back(1);
								parityResponses[w] = (1);
								noErrors = false;
							}
							else {
								parities.push_back(parity);
								paritiesMsgOutput.push_back(0);
								parityResponses[w] = (0);
								noErrors = false;
							}
						}

					}

					if (noErrors) {

						bool errorsRemaining = false;

						for (auto k = 0; k < (int)lookBackPassesIdx.size(); ++k) {
							if (!lookBackPassesIdx[k].empty()) {
								lookBackPass = k;
								errorsRemaining = true;
								break;
							}
						}

						if (!errorsRemaining) {
							// Trigger lookback
							lookBackPass = 0;
							lookBackDone = true;
							doingLookBack = false;
							if (((int)currentPass >= numPasses)) {

								//for (unsigned int i = 0; i < keys[0].size(); ++i) {
								//	outputSignals[0]->bufferPut((t_binary)keys[0][i]);
								//}
								outputKey({ outputSignals[0] }, keys[0]);
								currentPass = 0;
								errorCorrectionStatus = false;
								currentPassStarted = false;
							}
							else {
								currentPass++;
								currentPassStarted = false;
							}
						}
						else {
							lookBackStarted = false;
						}
					}
				}
				else if (!currentPassStarted) {
					correctedBitsInPass = 0;
					correctedBitsInLookBack = 0;
					int readyPerm = inputSignals[2]->ready();
					if (( readyPerm == 0 ) &&  permutationsNotCalculated) return alive;

					if (doubleWindowSize && partitionSize * pow(2, currentPass - 1) <= minimumSyndromeSize / minimumNumberOfPartitions) {

						currentPartitionSize = lround(partitionSize * pow(2, currentPass - 1));
					}

					startPositions.erase(startPositions.begin(), startPositions.end());
					endPositions.erase(endPositions.begin(), endPositions.end());
					totalSearchSpace.erase(totalSearchSpace.begin(), totalSearchSpace.end());
					parities.erase(parities.begin(), parities.end());
					parityResponses.erase(parityResponses.begin(), parityResponses.end());

					// Scramble key
					keys[currentPass - 1] = {};
					keyIdx[currentPass - 1] = {};
					reverseKeyIdx[currentPass - 1] = {};

					if (readyEC > 0) {
						finishedReceivingParitiesMsg = MessageProcessors::recvFromMsgProc(paritiesMsgInput, *inputSignals[1], receivingParitiesMsg, numberOfParitiesToReceive);
						alive = true;
						if (!finishedReceivingParitiesMsg) return alive;
					}

					// Scramble key (new method)
					std::vector <t_integer> seedsVector;
					t_integer numVals{ 0 };
					bool haveSeed = false;
					MessageProcessors::recvFromMsgProc(seedsVector, *inputSignals[2], haveSeed, numVals);
					t_integer seed = (t_integer)seedsVector[0];
					seedsVector.erase(seedsVector.begin());
					if (permutationsNotCalculated) {
						generatePermutationsIdx_testing(seed, keys[0], keys[currentPass - 1], keyIdx[currentPass - 1], reverseKeyIdx[currentPass - 1]);
						permutationsNotCalculated = false;
					}
					t_integer process = (t_integer)keys[currentPass - 1].size();
					numberOfPartitions = (t_integer)ceil((t_real)process / currentPartitionSize);



					bobParities.assign(paritiesMsgInput.begin(), paritiesMsgInput.end());
					paritiesMsgInput.erase(paritiesMsgInput.begin(), paritiesMsgInput.end());

					parities = {};

					for (auto w = 0; w < (int)numberOfPartitions; w++) {
						startPositions.push_back(currentPartitionSize * w);
						endPositions.push_back((t_integer)std::min(((t_real)currentPartitionSize * w + (t_real)currentPartitionSize), (t_real)process));
						t_real leftover = (t_real)process - ((t_real)currentPartitionSize * w);
						totalSearchSpace.push_back(std::min(currentPartitionSize, (t_integer)leftover));

						int parity = 0;

						parity = getParity(keys[currentPass - 1], startPositions[w], endPositions[w], parityCount, keys, keyIdx, reverseKeyIdx, currentPass - 1);
						parities.push_back(parity);

						if (parity == bobParities[w]) {
							paritiesMsgOutput.push_back(2);
							parityResponses.push_back((t_integer)2);
						}
						else {
							paritiesMsgOutput.push_back(0);
							parityResponses.push_back((t_integer)0);
						}
					}

					currentPassStarted = true;
					errorCorrectionStatus = true;
					permutationsNotCalculated = true;
					alive = true;
				}
				else {

				if (readyEC > 0) {
					finishedReceivingParitiesMsg = MessageProcessors::recvFromMsgProc(paritiesMsgInput, *inputSignals[1], receivingParitiesMsg, numberOfParitiesToReceive);
					alive = true;
					if (!finishedReceivingParitiesMsg) return alive;
				}

				bobParities.assign(paritiesMsgInput.begin(), paritiesMsgInput.end());
				paritiesMsgInput.erase(paritiesMsgInput.begin(), paritiesMsgInput.end());

					for (int w = 0; w < (int)bobParities.size(); ++w) {
				
						// Update testing partition
						int oldSz = endPositions[w] - startPositions[w];
						updateStartAndEndPositions(parityResponses[w], startPositions[w], endPositions[w], currentPartitionSize, totalSearchSpace[w]);
						if (oldSz < currentPartitionSize) {
							if (1 == parityResponses[w]) {
								totalSearchSpace[w] = totalSearchSpace[w] - oldSz;
							}
							else if (0 == parityResponses[w]) {
								totalSearchSpace[w] = oldSz;
							}
						}
						if (totalSearchSpace[w] == 0) {
							std::cout << "Error";
						}
						int parity;

						// Calculate parities or correct error
						if (2 == bobParities[w]) {
							parities.push_back(2);
							paritiesMsgOutput.push_back(2);
							parityResponses[w] = (2);
						}
						else if (5 == bobParities[w]) {
							parities.push_back(2);
							for (unsigned int k = 0; k < currentPass - 1; ++k) {
								lookBackPassesIdx[k].push_back(keyIdx[currentPass - 1][startPositions[w]]);
							}
							lookBackDone = false;
							paritiesMsgOutput.push_back(2);
							parityResponses[w] = (2);
						}
						else {
							parity = getParity(keys[currentPass - 1], startPositions[w], endPositions[w], parityCount, keys, keyIdx, reverseKeyIdx, currentPass - 1);
							if (parity == bobParities[w]) {
								parities.push_back(parity);
								paritiesMsgOutput.push_back(1);
								parityResponses[w] = (1);
								noErrors = false;
							}

							else {
								parities.push_back(parity);
								paritiesMsgOutput.push_back(0);
								parityResponses[w] = (0);
								noErrors = false;
							}
						}

					}

					if (noErrors) {
						// Trigger lookback
						if (((int)currentPass >= numPasses) && ((lookBackDone) || (lookBackPassesIdx[0].size() == 0))) {
							outputKey({ outputSignals[0] }, keys[0]);
							currentPass = 0;
							errorCorrectionStatus = false;
							currentPassStarted = false;
						}
						else {
							if ((lookBackDone) || (lookBackPassesIdx[0].size() == 0)) {
								currentPass++;
								currentPassStarted = false;
							}
							else {
								doingLookBack = true;
								lookBackStarted = false;
							}
						}
					}
				}

			}

			alive = true;

		}

		alive = true;
	}
	else if (errorCorrectionRole == ErrorCorrection::t_role::Bob) {

		//		t_binary keyIn{ 0 };
		std::vector <t_integer> aliceParities;

		if (!errorCorrectionStatus) {
			correctedInLookBack = 0;

			correctedBitsInPass = 0;
			correctedBitsInLookBack = 0;

			if (inputSignals[2]->ready() >= 2) {
				t_real realNumBits;
				inputSignals[2]->bufferGet(&BER);
				inputSignals[2]->bufferGet(&realNumBits);
				numBits = (int)realNumBits;
				partitionSize = ber2partitionSize(q, BER);
				std::vector<t_integer> berChange{ 1 };
				bool sentBerChange = false;
				MessageProcessors::sendToMsgProc(berChange, *outputSignals[3], sentBerChange);
//				outputSignals[3]->bufferPut((t_binary)1);
			}
			/*
			if (inputSignals[2]->ready() >= 2 && alreadyUsedBER) {
				t_real realNumBits;
				inputSignals[2]->bufferGet(&BER);
				inputSignals[2]->bufferGet(&realNumBits);
				numBits = (int)realNumBits;
				alreadyUsedBER = false;
				partitionSize = floor(q / max(BER, 0.005));
			}
			*/
			if (BER == 2) return alive;

			if (partitionSize * minimumNumberOfPartitions > minimumSyndromeSize) {
				partitionSize = (int)floor(minimumSyndromeSize / minimumNumberOfPartitions);
			}

			// Start Basis Reconciliation
			int ready = std::min((t_integer)dataOnHold.size(), maximumSyndromeSize);
			//			int ready = inputSignals[0]->ready();
			int ready1 = inputSignals[1]->ready()-1;
			int space = outputSignals[1]->space();
			int process = std::min((t_integer)floor(ready / partitionSize), space);

			//			if (process <= 0) return alive;
			if (ready < minimumSyndromeSize || process == 0) return alive;

			currentPartitionSize = partitionSize;
			numberOfPartitions = (t_integer)floor((t_real)ready / partitionSize);

//			if (numberOfPartitions > space) return alive;


			currentPass = 1;
			testCount++;

			// Clear old vectors
			keys[0].erase(keys[0].begin(), keys[0].end());
			startPositions.erase(startPositions.begin(), startPositions.end());
			endPositions.erase(endPositions.begin(), endPositions.end());
			totalSearchSpace.erase(totalSearchSpace.begin(), totalSearchSpace.end());
			parities.erase(parities.begin(), parities.end());
			//			lookBackIdx.erase(lookBackIdx.begin(), lookBackIdx.end());
			//			lookBackPassesIdx.erase(lookBackPassesIdx.begin(), lookBackPassesIdx.end());

			for (auto k = 0; k < (int)lookBackPassesIdx.size(); ++k) {
				lookBackPassesIdx[k].erase(lookBackPassesIdx[k].begin(), lookBackPassesIdx[k].end());
				keyIdx[k].erase(keyIdx[k].begin(), keyIdx[k].end());
				reverseKeyIdx[k].erase(reverseKeyIdx[k].begin(), reverseKeyIdx[k].end());
				keys[k].erase(keys[k].begin(), keys[k].end());
			}

			if ((int)lookBackPassesIdx.size() < numPasses) {
				lookBackPassesIdx.resize(numPasses, {});
			}
			if ((int)keyIdx.size() < numPasses) {
				keyIdx.resize(numPasses, {});
			}
			if ((int)reverseKeyIdx.size() < numPasses) {
				reverseKeyIdx.resize(numPasses, {});
			}
			if ((int)keys.size() < numPasses) {
				keys.resize(numPasses, {});
			}
//			bool restart = false;
			//			t_real dataIn{ 0 };


			if (ready1 > 0) {
				finishedReceivingParitiesMsg = MessageProcessors::recvFromMsgProc(paritiesMsgInput, *inputSignals[1], receivingParitiesMsg, numberOfParitiesToReceive);
				alive = true;
				if (!finishedReceivingParitiesMsg) return alive;
			}

			paritiesMsgInput.erase(paritiesMsgInput.begin(), paritiesMsgInput.end());

			process = numberOfPartitions * partitionSize;
			t_binary keyIn{ 0 };
			for (auto k = 0; k < process; ++k) {
				keyIn = (t_binary)dataOnHold[0];
				//				inputSignals[0]->bufferGet(&keyIn);
				t_integer keyInInt = (t_integer)keyIn;
				if ((keyInInt != 0) && (keyInInt != 1)) {
					std::cout << "Problem.\n";
					//					keyInInt = 1;
				}
				keys[0].push_back(keyInInt);
				keyIdx[currentPass - 1].push_back(k);
				reverseKeyIdx[currentPass - 1].push_back(k);
				dataOnHold.erase(dataOnHold.begin());
			}

			parities = {};

			for (auto w = 0; w < numberOfPartitions; w++) {
				startPositions.push_back(partitionSize * w);
				endPositions.push_back((t_integer)std::min(((t_real)partitionSize * w + (t_real)partitionSize), (t_real)process));
				//			endPositions.push_back(partitionSize * w + std::min(ceil((t_real)partitionSize / 2.0), (t_real)process));
				t_real leftover = (t_real)process - ((t_real)partitionSize * w);
				totalSearchSpace.push_back(std::min(partitionSize, (t_integer)leftover));

				int parity = getParity(keys[currentPass - 1], startPositions[w], endPositions[w], parityCount, keys, keyIdx, reverseKeyIdx, currentPass - 1);
				parities.push_back(parity);

				paritiesMsgOutput.push_back(parity);
			}
			/*
			leftoverBits = numBits + leftoverBits - (int)keys[0].size();
			alreadyUsedBER = true;*/

			currentPassStarted = true;
			errorCorrectionStatus = true;
			alive = true;
		}
		else {
			int readyEC = inputSignals[1]->ready()-1;

// 			int spaceKeyOutput = outputSignals[0]->space();
//			int spaceEC = outputSignals[1]->space();
			// int space = std::min(spaceKeyOutput, spaceEC);
			bool noErrors = true;

			if ((readyEC <= 0) ) return alive;

			parities.erase(parities.begin(), parities.end());

			if (currentPass == 1) {
				doingLookBack = false;

				if (readyEC > 0) {
					finishedReceivingParitiesMsg = MessageProcessors::recvFromMsgProc(paritiesMsgInput, *inputSignals[1], receivingParitiesMsg, numberOfParitiesToReceive);
					alive = true;
					if (!finishedReceivingParitiesMsg) return alive;
				}

				aliceParities.assign(paritiesMsgInput.begin(), paritiesMsgInput.end());
				paritiesMsgInput.erase(paritiesMsgInput.begin(), paritiesMsgInput.end());

				for (int w = 0; w < (int)aliceParities.size(); ++w) {
					t_integer dataIn = aliceParities[w];

					// Update testing partition
					int oldSz = endPositions[w] - startPositions[w];
					updateStartAndEndPositions(aliceParities[w], startPositions[w], endPositions[w], partitionSize, totalSearchSpace[w]);
					if (1 == dataIn) {
						totalSearchSpace[w] = totalSearchSpace[w] - oldSz;
						if (totalSearchSpace[w] == 0) {
							std::cout << "Error";
						}
					}
					else if (0 == dataIn) {
						totalSearchSpace[w] = oldSz;
						if (totalSearchSpace[w] == 0) {
							std::cout << "Error";
						}
					}
					if (totalSearchSpace[w] == 0) {
						std::cout << "Error";
					}

					// Calculate parities or correct error
					if (2 == dataIn) {
						parities.push_back((t_integer)2);
						paritiesMsgOutput.push_back(2);
					}
					else if (totalSearchSpace[w] == 1) {
						correctedBitsInPass++;
						keys[0][startPositions[w]] = correctBit(keys[0][startPositions[w]]);
						parities.push_back((t_integer)2);
						paritiesMsgOutput.push_back(2);
					}
					else {
						int parity = getParity(keys[currentPass - 1], startPositions[w], endPositions[w], parityCount, keys, keyIdx, reverseKeyIdx, currentPass - 1);
						parities.push_back(parity);
						paritiesMsgOutput.push_back(parity);
						noErrors = false;
					}
				}
				if (noErrors) {
					// Current Pass should be complete
					if (numPasses == 1) {
						outputKey({ outputSignals[0] }, keys[0]);
						currentPass = 0;
						errorCorrectionStatus = false;
						currentPassStarted = false;
					}
					else {
						currentPass++;
						currentPassStarted = false;
					}
				}
			}
			else {
				if (doingLookBack && !lookBackStarted) {
					////////////////////////////////////////////////////////////////////////////
					////////////////////////////////////////////////////////////////////////////
					correctParitiesInLookBack = 0;

//					for (int i = 0; i < readyEC; ++i) {
//						inputSignals[1]->bufferGet(&dataIn);
//					}

					if (readyEC > 0) {
						finishedReceivingParitiesMsg = MessageProcessors::recvFromMsgProc(paritiesMsgInput, *inputSignals[1], receivingParitiesMsg, numberOfParitiesToReceive);
						alive = true;
						if (!finishedReceivingParitiesMsg) return alive;
					}

					paritiesMsgInput.erase(paritiesMsgInput.begin(), paritiesMsgInput.end());

					// Update indexes for the proper key
					for (auto x = 0; x < (int)lookBackPassesIdx[lookBackPass].size(); ++x) {
						lookBackPassesIdx[lookBackPass][x] = reverseKeyIdx[lookBackPass][lookBackPassesIdx[lookBackPass][x]];
					}

					//  Sort lookback idx in order to avoid repeat partitions
					std::sort(lookBackPassesIdx[lookBackPass].begin(), lookBackPassesIdx[lookBackPass].end());

					startPositions.erase(startPositions.begin(), startPositions.end());
					endPositions.erase(endPositions.begin(), endPositions.end());
					totalSearchSpace.erase(totalSearchSpace.begin(), totalSearchSpace.end());
					parities.erase(parities.begin(), parities.end());

					t_integer process = (t_integer)keys[lookBackPass].size();
					numberOfPartitions = (t_integer)lookBackPassesIdx[lookBackPass].size();

					partitionSizeLB = partitionSize;
					for (unsigned int k = 0; k < lookBackPass; ++k) {
						if (doubleWindowSize && partitionSizeLB * 2 <= minimumSyndromeSize / minimumNumberOfPartitions) {
							partitionSizeLB = partitionSizeLB * 2;
						}
					}

					parities = {};

					for (auto w = 0; w < numberOfPartitions; w++) {
						t_real startPos = (floor(t_real(lookBackPassesIdx[lookBackPass][w]) / partitionSizeLB) * partitionSizeLB);
						if ((startPositions.empty()) || (startPositions.back() != startPos)) {
							startPositions.push_back((t_integer)startPos);
							endPositions.push_back((t_integer)std::min(((t_real)startPos + (t_real)partitionSizeLB), (t_real)process));
							if (endPositions.back() > 512) {
								std::cout << "";
							}
							//			endPositions.push_back(partitionSize * w + std::min(ceil((t_real)partitionSize / 2.0), (t_real)process));
							t_real leftover = process - startPos;
							totalSearchSpace.push_back(std::min(partitionSizeLB, (t_integer)leftover));

							int parity = getParity(keys[lookBackPass], startPositions.back(), endPositions.back(), parityCount, keys, keyIdx, reverseKeyIdx, lookBackPass);
							parities.push_back(parity);
							paritiesMsgOutput.push_back(parity);
						}
					}
					lookBackPassesIdx[lookBackPass].erase(lookBackPassesIdx[lookBackPass].begin(), lookBackPassesIdx[lookBackPass].end());
					lookBackStarted = true;

				}
				else if (doingLookBack && lookBackStarted) {
					if (lookBackPass > 0) {
						std::cout << "";
					}

					if (readyEC > 0) {
						finishedReceivingParitiesMsg = MessageProcessors::recvFromMsgProc(paritiesMsgInput, *inputSignals[1], receivingParitiesMsg, numberOfParitiesToReceive);
						alive = true;
						if (!finishedReceivingParitiesMsg) return alive;
					}

					aliceParities.assign(paritiesMsgInput.begin(), paritiesMsgInput.end());
					paritiesMsgInput.erase(paritiesMsgInput.begin(), paritiesMsgInput.end());

					for (int w = 0; w < (int)aliceParities.size(); ++w) {
						t_integer dataIn = aliceParities[w];

						// Update testing partition
						int oldSz = endPositions[w] - startPositions[w];
						updateStartAndEndPositions(aliceParities[w], startPositions[w], endPositions[w], partitionSizeLB, totalSearchSpace[w]);
						if (1 == dataIn) {
							totalSearchSpace[w] = totalSearchSpace[w] - oldSz;
						}
						else if (0 == dataIn) {
							totalSearchSpace[w] = oldSz;
						}

						// Calculate parities or correct error
						if (2 == dataIn) {
							parities.push_back((t_integer)2);
							paritiesMsgOutput.push_back(2);
							correctParitiesInLookBack++;
						}
						else if (totalSearchSpace[w] == 1) {

							correctedBitsInLookBack++;

							t_integer correctBitValue = correctBit(keys[lookBackPass][startPositions[w]]);
							correctedInLookBack++;


							for (unsigned int k = 0; k < currentPass - 1; ++k) {
								keys[k][reverseKeyIdx[k][keyIdx[lookBackPass][startPositions[w]]]] = correctBitValue;
							}


							for (unsigned int k = 0; k < currentPass - 1; ++k) {
								if (k != lookBackPass) {
									lookBackPassesIdx[k].push_back(keyIdx[lookBackPass][startPositions[w]]);

								}
							}
							parities.push_back((t_integer)2);
							paritiesMsgOutput.push_back(5);
						}
						else {
							if (lookBackPass > 0) {
								std::cout << "";
							}
							int parity = getParity(keys[lookBackPass], startPositions[w], endPositions[w], parityCount, keys, keyIdx, reverseKeyIdx, lookBackPass);
							parities.push_back(parity);
							paritiesMsgOutput.push_back(parity);
							noErrors = false;
						}
					}
					if (noErrors) {
						bool errorsRemaining = false;

						for (auto k = 0; k < (int)lookBackPassesIdx.size(); ++k) {
							if (!lookBackPassesIdx[k].empty()) {
								lookBackPass = k;
								errorsRemaining = true;
								break;
							}
						}


						if (!errorsRemaining) {
							if ((currentPass > 1) && ((correctedBitsInLookBack + correctedBitsInPass) % 2 > 0)) {
								std::cout << "";
							}
							lookBackPass = 0;
							lookBackDone = true;
							doingLookBack = false;
							// Trigger lookback
							if (((int)currentPass >= numPasses)) {
								//							for (unsigned int i = 0; i < keys[0].size(); ++i) {
								//								outputSignals[0]->bufferPut((t_binary)keys[0][i]);
								//							}
								outputKey({ outputSignals[0] }, keys[0]);
								currentPass = 0;
								errorCorrectionStatus = false;
								currentPassStarted = false;
							}
							else {

								currentPass++;
								currentPassStarted = false;
							}
						}
						else {
							lookBackStarted = false;
						}
					}
				}

				////////////////////////////////////////////////////////////////////////////
				////////////////////////////////////////////////////////////////////////////

				else if (!currentPassStarted) {
					correctedBitsInPass = 0;
					correctedBitsInLookBack = 0;
					doingLookBack = false;
					

					int emptyPerm = outputSignals[2]->getBufferEmpty();
					if (!emptyPerm) return alive;


					if (doubleWindowSize && partitionSize * pow(2, currentPass - 1) <= minimumSyndromeSize / minimumNumberOfPartitions) {
						currentPartitionSize = lround(partitionSize * pow(2, currentPass-1));
					}

					if (readyEC > 0) {
						finishedReceivingParitiesMsg = MessageProcessors::recvFromMsgProc(paritiesMsgInput, *inputSignals[1], receivingParitiesMsg, numberOfParitiesToReceive);
						alive = true;
						if (!finishedReceivingParitiesMsg) return alive;
					}
					currentPassStarted = true;
					paritiesMsgInput.erase(paritiesMsgInput.begin(), paritiesMsgInput.end());

					startPositions.erase(startPositions.begin(), startPositions.end());
					endPositions.erase(endPositions.begin(), endPositions.end());
					totalSearchSpace.erase(totalSearchSpace.begin(), totalSearchSpace.end());
					parities.erase(parities.begin(), parities.end());
					//					lookBackIdx.erase(lookBackIdx.begin(), lookBackIdx.end());

										// Scramble keys[0] and keys[0]Idx
					keys[currentPass - 1] = {};
					keyIdx[currentPass - 1] = {};
					reverseKeyIdx[currentPass - 1] = {};

					//t_integer keyIdx = 0;
					//t_integer matrixIdx = currentPass - 1;
					//scrambleKeyWithMatrix(ms[matrixIdx], ns[matrixIdx], keyIdx, keys[0], scrambledKey, keyIdx[currentPass-1]);

					// Scramble key (new method) -> outputs to scrambledKey and keyIdx[currentPass-1]
					std::random_device rd;
					t_integer seed = rd();
					generatePermutationsIdx_testing(seed, keys[0], keys[currentPass - 1], keyIdx[currentPass - 1], reverseKeyIdx[currentPass - 1]);

					/*					for (auto k = 0; k < keyIdx[currentPass-1].size(); k++) {
											outputSignals[2]->bufferPut((t_real)keyIdx[currentPass-1][k]);
										}
					*/
					bool sentSeed = false;
					std::vector <t_integer> seedVector{seed};
					MessageProcessors::sendToMsgProc(seedVector, *outputSignals[2], sentSeed);
					


					t_integer process = (t_integer)keys[currentPass - 1].size();
					numberOfPartitions = (t_integer)ceil((t_real)process / currentPartitionSize);

					parities = {};

					for (auto w = 0; w < numberOfPartitions; w++) {
						startPositions.push_back(currentPartitionSize * w);
						endPositions.push_back((t_integer)std::min(((t_real)currentPartitionSize * w + (t_real)currentPartitionSize), (t_real)process));
						if (endPositions[w] > 512) {
							std::cout << "";
						}
						//			endPositions.push_back(partitionSize * w + std::min(ceil((t_real)partitionSize / 2.0), (t_real)process));
						t_real leftover = (t_real)process - ((t_real)currentPartitionSize * w);
						totalSearchSpace.push_back(std::min(currentPartitionSize, (t_integer)leftover));

						int parity = getParity(keys[currentPass - 1], startPositions[w], endPositions[w], parityCount, keys, keyIdx, reverseKeyIdx, currentPass - 1);
						parities.push_back(parity);
						paritiesMsgOutput.push_back(parity);
					}


					currentPassStarted = true;
					errorCorrectionStatus = true;
					alive = true;

				}
				else {
				
				if (readyEC > 0) {
					finishedReceivingParitiesMsg = MessageProcessors::recvFromMsgProc(paritiesMsgInput, *inputSignals[1], receivingParitiesMsg, numberOfParitiesToReceive);
					alive = true;
					if (!finishedReceivingParitiesMsg) return alive;
				}

				aliceParities.assign(paritiesMsgInput.begin(), paritiesMsgInput.end());
				paritiesMsgInput.erase(paritiesMsgInput.begin(), paritiesMsgInput.end());


					for (int w = 0; w < (int)aliceParities.size(); ++w) {
						t_integer dataIn = aliceParities[w];

						// Update testing partition
						int oldSz = endPositions[w] - startPositions[w];
						updateStartAndEndPositions(aliceParities[w], startPositions[w], endPositions[w], currentPartitionSize, totalSearchSpace[w]);
						if (1 == dataIn) {
							totalSearchSpace[w] = totalSearchSpace[w] - oldSz;
						}
						else if (0 == dataIn) {
							totalSearchSpace[w] = oldSz;
						}

						// Calculate parities or correct error
						if (2 == dataIn) {
							parities.push_back((t_integer)2);
							paritiesMsgOutput.push_back(2);
						}
						else if (totalSearchSpace[w] == 1) {
							correctedBitsInPass++;
							t_integer correctBitValue = correctBit(keys[currentPass - 1][startPositions[w]]);
							keys[currentPass - 1][startPositions[w]] = correctBitValue;
							for (unsigned int k = 0; k < currentPass - 1; ++k) {
								lookBackPassesIdx[k].push_back(keyIdx[currentPass - 1][startPositions[w]]);
								keys[k][reverseKeyIdx[k][keyIdx[currentPass - 1][startPositions[w]]]] = correctBitValue;
							}

							parities.push_back((t_integer)2);
							lookBackDone = false;
							paritiesMsgOutput.push_back(5);
						}
						else {
							int parity = getParity(keys[currentPass - 1], startPositions[w], endPositions[w], parityCount, keys, keyIdx, reverseKeyIdx, currentPass - 1);
							parities.push_back(parity);
							paritiesMsgOutput.push_back(parity);
							noErrors = false;
						}
					}

					//					lookBackDone = true;

					if (noErrors) {
						// Trigger lookback
						if (((int)currentPass >= numPasses) && ((lookBackDone) || (lookBackPassesIdx[0].size() == 0))) {
							outputKey({ outputSignals[0] }, keys[0]);
							currentPass = 0;
							errorCorrectionStatus = false;
							currentPassStarted = false;
						}
						else {
							if ((lookBackDone) || (lookBackPassesIdx[0].size() == 0)) {
								currentPass++;
								currentPassStarted = false;
							}
							else if (lookBackPassesIdx[0].size() > 0) {


								doingLookBack = true;
								lookBackStarted = false;
								alive = true;
							}

						}


					}
				}
			}

			alive = true;
		}

	}
	else if (errorCorrectionRole == ErrorCorrection::t_role::Undefined)
	{
		std::cout << "Undefined role of the error correction block.";
	}
	else 
	{
		std::cout << "Undefined behavior on the error correction block.";
	}


	finishedSendingParitiesMsg = MessageProcessors::sendToMsgProc(paritiesMsgOutput, *outputSignals[1], sendingParitiesMsg);

	return alive;
}

t_integer ErrorCorrection::getParity(std::vector<t_integer>& keyVector, const t_integer startPosition, const t_integer endPosition, t_integer& count, std::vector<std::vector<t_integer>>& keyVectors, const std::vector<std::vector<t_integer>>& keyIdxVectors, const std::vector<std::vector<t_integer>>& reverseKeyIdxVectors, const unsigned int pass) {
	t_integer parity = 0;
	bool alreadyDiscarded{ false };
	for (auto k = endPosition - 1; k >= startPosition; k--) {
		if ((!alreadyDiscarded) && (keyVector[k] < 7)) {
			// Mark bits to be discarded, replace 1 with 7 and 0 with 8
			alreadyDiscarded = true;
			switch (keyVector[k]) {
			case 0:
				keyVector[k] = 8;
				for (unsigned int m = 0; m < keyVectors.size(); ++m) {
					if ((m != pass) && (!keyVectors[m].empty())) {
						keyVectors[m][reverseKeyIdxVectors[m][keyIdxVectors[pass][k]]] = 8;
					}
				}
				break;
			case 1:
				keyVector[k] = 7;
				for (unsigned int m = 0; m < keyVectors.size(); ++m) {
					if ((m != pass) && (!keyVectors[m].empty())) {
						keyVectors[m][reverseKeyIdxVectors[m][keyIdxVectors[pass][k]]] = 7;
					}
				}
				break;
			}

		}
		parity += keyVector[k];
	}
	parity = parity % 2;
	count++;
	measuredParities++;
	if (parity == 1) return 1;
	return 0;
}

bool ErrorCorrection::updateStartAndEndPositions(const t_integer& aliceParityResponse, t_integer& startPosition, t_integer& endPosition, const t_integer windowSize, const t_integer totalSearchSize) {
	int oldStart = startPosition;
	int oldEnd = endPosition;
	int winSz = oldEnd - oldStart;
	bool status{ false };

	if (totalSearchSize == endPosition - startPosition) {
		endPosition = (t_integer)ceil(((t_real)startPosition + (t_real)endPosition) / 2.0);
		status = true;
	}
	else if (startPosition == endPosition) {
		status = true;
	}
	else if (aliceParityResponse == 2) {
		status = true;
	}
	else if (aliceParityResponse == 1) {
		startPosition = endPosition;
		endPosition = (t_integer)ceil(((t_real)startPosition + ceil(((t_real)totalSearchSize - winSz) / 2.0)));
		if ((endPosition < startPosition) || (endPosition > startPosition + windowSize)) {
			std::cout << "Error!";
		}
		status = true;
	}
	else {
		endPosition = (t_integer)ceil(((t_real)startPosition + (t_real)endPosition) / 2.0);
		if ((endPosition < startPosition) || (endPosition > startPosition + windowSize)) {
			std::cout << "Error!";
		}
		status = true;
	}
	if ((startPosition >= 512) || (endPosition > 512)) {
		std::cout << "";
	}
	return status;
}

t_integer ErrorCorrection::correctBit(t_integer bitValue) {
	switch (bitValue)
	{
	case 1:
		return 0;
		break;
	case 0:
		return 1;
		break;
		// 7 and 8 are values which keep the numbers odd or even, but indicate that these bits are to be discarded
	case 7:
		return 8;
		break;
	case 8:
		return 7;
		break;
	default:
		std::cout << "Stuff...";
		return 10;
		break;
	}
}

bool ErrorCorrection::outputKey(std::vector<Signal*> outputSignals_f, std::vector<t_integer>& correctedKey) {
	bool alive = false;
	t_integer process = (t_integer)correctedKey.size();
	for (auto k = 0; k < (int)outputSignals_f.size(); ++k) {
		process = std::min(process, outputSignals_f[k]->space());
	}

	t_integer keyVal;
	for (auto k = 0; k < process; ++k) {
		keyVal = correctedKey[0];
		if ((keyVal == 1) || (keyVal == 0)) {
			for (auto m = 0; m < (int)outputSignals_f.size(); ++m) {
				outputSignals_f[m]->bufferPut((t_binary)keyVal);
				outputBits++;
			}
		}/*
		else if (keyVal == 8)  {
			for (auto m = 0; m < outputSignals_f.size(); ++m) {
				outputSignals_f[m]->bufferPut((t_binary)8);
			}
		}
		else if (keyVal == 7) {
			for (auto m = 0; m < outputSignals_f.size(); ++m) {
				outputSignals_f[m]->bufferPut((t_binary)7);
			}
		}*/
		else if ((keyVal != 1) && (keyVal != 0) && (keyVal != 8) && (keyVal != 7)) {
			std::cout << "Error!";
		}
		correctedKey.erase(correctedKey.begin());
	}


	alive = true;

	return alive;

}

bool ErrorCorrection::generatePermutationsIdx_testing(const t_seed seed, const std::vector <t_integer>& originalKey, std::vector <t_integer>& newScrambledKey, std::vector <t_integer>& newScrambledKeyIdx, std::vector <t_integer>& newReverseScrambledKeyIdx) {
	bool alive = false;

	netxpto::UniformRandomRealNumbers uniformeRandomNumberBetweenZeroAndOne{ seed, 0, 1 };

	t_integer randIdx{ 0 };
	t_real randNum{ 0 };
	std::vector <t_integer> idx{};

	for (auto k = 0; k < (int)originalKey.size(); ++k) {
		idx.push_back(k);
	}

	int iterationLength = (int)idx.size();
	for (auto k = 0; k < iterationLength; ++k) {
		randNum = uniformeRandomNumberBetweenZeroAndOne() * ((t_real)idx.size() - 1);
		randIdx = (int)round(randNum);
		newScrambledKey.push_back(originalKey[idx[randIdx]]);
		newScrambledKeyIdx.push_back(idx[randIdx]);
		idx.erase(idx.begin() + randIdx);
	}

	newReverseScrambledKeyIdx.resize(newScrambledKeyIdx.size());

	for (auto k = 0; k < (int)newReverseScrambledKeyIdx.size(); ++k) {
		newReverseScrambledKeyIdx[newScrambledKeyIdx[k]] = k;
	}

	alive = true;
	return alive;
}
t_integer ErrorCorrection::ber2partitionSize(t_real qValue, t_real BERValue) {
	t_integer pSz = (int)floor(qValue / std::max(BERValue, minimumBER));
//	t_integer pSz = (int)pow(2,ceil(log2(qValue / max(BERValue, minimumBER))-0.5));
	return pSz;
}
