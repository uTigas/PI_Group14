#include "dv_qkd_ldpc_rx_message_processor_receiver_20200819.h"

void DvQkdLdpcRxMessageProcessorReceiver::initialize(void) 
{
	Sindrome.resize(outputSignals[5]->getBufferLength());
}

bool DvQkdLdpcRxMessageProcessorReceiver::runBlock(void)
{
	bool alive = false;

	if (getTerminated()) return false;

	auto ready = BasesFrom.ready();
	auto space = outputSignals[0]->space();
	auto process = std::min(ready, space);
	if (process) alive = true;
	for (; process--;)
		outputSignals[0]->bufferPut((t_binary)BasesFrom.bufferGet());

	ready = Sindrome.ready();
	space = outputSignals[5]->space();
	process = std::min(ready, space);
	if (process) alive = true;
	for (; process--;)
		outputSignals[5]->bufferPut((t_binary)Sindrome.bufferGet());

	ready = inputSignals[0]->ready();
	
	for (auto k = 0; k < ready; ++k)
	{
		t_message messageIn;
		inputSignals[0]->bufferRead(&messageIn);

		std::string mType = messageIn.getMessageType();
		std::string mDataLength = messageIn.getMessageDataLength();
		t_integer dataLength = stoi(mDataLength);
		std::string mDataString = messageIn.getMessageData();
		std::string num{ "" };

		if (mType == "BasisReconciliationMsg")
		{
			if (dataLength > messageReconciliationMaximumDataLength) std::cerr << "ERROR: BasesReconciliation  wrong messageReconciliationMaximumDataLength!!!!";

			auto space = BasesFrom.space(dataLength);
			if (space >= dataLength)
			{
				inputSignals[0]->bufferGet();
				for (auto k = 0; k < dataLength; k++)
				{
					t_binary value = (t_binary)(mDataString.at(k) - '0');
					BasesFrom.bufferPut((t_binary)(value));
				}
				alive = true;
			}
			else
			{
				t_message messageIn;
				inputSignals[0]->bufferGet(&messageIn);
				inputSignals[0]->bufferPut(messageIn);
			}
		}
		else if (mType == "ParameterEstimationSeedAndBitsMsg")
		{
			if (SeedFrom.space() && RatioFrom.space() && NumberOfBitsPerEstimationBlockFrom.space())
			{
				t_integer seedFrom{ 0 };
				t_integer ratioFrom{ 0 };
				t_integer numberOfBitsPerEstimationBlockFrom{ 0 };

				bool gotSeed{ false };
				bool gotRatio{ false };
				bool gotNumberOfBitsPerEstimationBlock{ false };

				int w{ 0 };
				while (!gotSeed || !gotRatio || !gotNumberOfBitsPerEstimationBlock)
				{
					std::string data;
					data = mDataString.at(w);
					if (!gotSeed)
					{
						if (data == "#")
						{
							seedFrom = stoi(num);
							num = "";
							gotSeed = true;
						}
						else
						{
							num.append(data);
						}
					}
					else if (!gotRatio)
					{
						if (data == "#")
						{
							ratioFrom = stoi(num);
							num = "";
							gotRatio = true;
						}
						else
						{
							num.append(data);
						}
					}
					else if (!gotNumberOfBitsPerEstimationBlock)
					{
						if (data == "#")
						{
							numberOfBitsPerEstimationBlockFrom = stoi(num);
							num = "";
							gotNumberOfBitsPerEstimationBlock = true;
						}
						else
						{
							num.append(data);
						}
					}
					++w;
				}
				auto space = DataFrom.space(dataLength - w);
				if (space >= dataLength - w)
				{
					inputSignals[0]->bufferGet(&messageIn);
					alive = true;

					if (numberOfProcessedBits == -1)
					{
						SeedFrom.bufferPut(seedFrom);
						RatioFrom.bufferPut(ratioFrom);
						NumberOfBitsPerEstimationBlockFrom.bufferPut(numberOfBitsPerEstimationBlockFrom);

						numberOfProcessedBits = 0;
					}

					for (auto k = w; k < dataLength; k++)
					{
						char datac = mDataString.at(k);
						if (datac == '1')
						{
							DataFrom.bufferPut(1);
						}
						else if (datac == '0')
						{
							DataFrom.bufferPut(0);
						}
						else
						{
							std::cout << "Error during parameter estimation message reception.\n";
						}
					}
					numberOfProcessedBits = numberOfProcessedBits + dataLength - w;
					if (numberOfProcessedBits == numberOfBitsPerEstimationBlockFrom) numberOfProcessedBits = -1;
				}
			}
			else
			{
				t_message messageIn;
				inputSignals[0]->bufferGet(&messageIn);
				inputSignals[0]->bufferPut(messageIn);
			}
		}
		else if (mType == "SindromeMsg")
		{
			if (dataLength > messageSindromeMaximumDataLength) std::cerr << "ERROR: Sindrome wrong messageSindromeMaximumDataLength!!!!";

			auto space = Sindrome.space(dataLength);
			if (space >= dataLength)
			{
				inputSignals[0]->bufferGet();
				for (auto k = 0; k < dataLength; k++)
				{
					t_binary value = (t_binary)(mDataString.at(k) - '0');
					Sindrome.bufferPut((t_binary)(value));
				}
				alive = true;
			}
			else
			{
				t_message messageIn;
				inputSignals[0]->bufferGet(&messageIn);
				inputSignals[0]->bufferPut(messageIn);
			}
		}
		else if (mType == "ErrorCorrectionPermutationsMsg")
		{
			inputSignals[0]->bufferGet();

			for (int w = 0; w < dataLength; w++)
			{
				std::string data;
				data = mDataString.at(w);
				if (data == "#")
				{
					permutationsIn.push_back(stoi(num));
					num = "";
				}
				else
				{
					num.append(data);
				}
			}
			alive = true;
		}
		else if (mType == "PrivacyAmplificationSeedsMsg")
		{
			inputSignals[0]->bufferGet();

			for (int w = 0; w < dataLength; w++)
			{
				std::string data;
				data = mDataString.at(w);
				if (data == "#") 
				{
					privacySeedsIn.push_back(stoi(num));
					num = "";
				}
				else 
				{
					num.append(data);
				}
			}
			alive = true;
		}
		else if (mType == "ToggleBERChangeMsg") 
		{
			inputSignals[0]->bufferGet();

			for (int w = 0; w < dataLength; w++)
			 {
				char data = mDataString.at(k);

				if (data == '1')
				{
					errCorrBerIn.push_back(1);
				}
				alive = true;
			}
		}
		alive = true;
	}
	
	ready = BasesFrom.ready();
	space = outputSignals[0]->space();
	process = std::min(ready, space);
	alive = process ? process : alive;
	for (; process--;)
		outputSignals[0]->bufferPut(BasesFrom.bufferGet());

	ready = SeedFrom.ready();
	space = outputSignals[1]->space();
	process = std::min(ready, space);
	alive = process ? process : alive;
	for (auto k = 0; k < process; k++)
		outputSignals[1]->bufferPut(SeedFrom.bufferGet());

	ready = RatioFrom.ready();
	space = outputSignals[2]->space();
	process = std::min(ready, space);
	alive = process ? process : alive;
	for (auto k = 0; k < process; k++)
		outputSignals[2]->bufferPut(RatioFrom.bufferGet());

	ready = NumberOfBitsPerEstimationBlockFrom.ready();
	space = outputSignals[3]->space();
	process = std::min(ready, space);
	alive = process ? process : alive;
	for (auto k = 0; k < process; k++)
		outputSignals[3]->bufferPut(NumberOfBitsPerEstimationBlockFrom.bufferGet());

	ready = DataFrom.ready();
	space = outputSignals[4]->space();
	process = std::min(ready, space);
	alive = process ? process : alive;
	for (auto k = 0; k < process; k++)
		outputSignals[4]->bufferPut(DataFrom.bufferGet());

	ready = Sindrome.ready();
	space = outputSignals[5]->space();
	process = std::min(ready, space);
	alive = process ? process : alive;
	for (auto k = 0; k < process; k++)
		outputSignals[5]->bufferPut(Sindrome.bufferGet());
	
	return alive;
}

bool DvQkdLdpcRxMessageProcessorReceiver::outputReceivedData(std::vector<t_integer>& dataVector, Signal& outputSignal, bool& started)
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
