#include "dv_qkd_ldpc_tx_message_processor_transmitter_20200819.h"

void DvQkdLdpcTxMessageProcessorTransmitter::initialize(void) 
{
	Sindrome.resize(messageSindromeMaximumDataLength);
	DataTo.resize(inputSignals[4]->getBufferLength());
}

bool DvQkdLdpcTxMessageProcessorTransmitter::runBlock(void)
{
	bool alive = false;

	auto ready_BasesAckTo = inputSignals[0]->ready();
	auto ready_SeedTo = inputSignals[1]->ready();
	auto ready_RatioTo = inputSignals[2]->ready();
	auto ready_NumberOfBitsPerEstimationBlock = inputSignals[3]->ready();
	auto ready_DataTo = inputSignals[4]->ready();
	auto ready_SindromeToSend = inputSignals[5]->ready();
	auto ready_HashToSend = inputSignals[6]->ready();
	
	// Basis reconciliation message
	if ((ready_BasesAckTo > 0))
	{
		auto space = BasesAckTo.space();
		auto process = std::min(ready_BasesAckTo, space);
		if (process) alive = true;
		for (auto k = 0; k < process; ++k)
		{
			t_binary basesAckTo{ 0 };
			inputSignals[0]->bufferGet(&basesAckTo);
			BasesAckTo.bufferPut(basesAckTo);
		}
	}
			
	if (outputSignals[0]->space() && BasesAckTo.ready() == messageReconciliationMaximumDataLength)
	{
		t_message_type msgType = "BasisReconciliationMsg";
		std::string messageText = "";

		for (auto k = 0; k < messageReconciliationMaximumDataLength; k++)
			messageText.append(std::to_string(BasesAckTo.bufferGet()));

		t_message messageBasisRecon;
		messageBasisRecon.setMessageType(msgType);
		messageBasisRecon.setMessageDataLength(std::to_string(messageText.size()));
		messageBasisRecon.setMessageData(messageText);

		outputSignals[0]->bufferPut(messageBasisRecon);

		alive = true;
	}

	// Parameter estimation (SeedTo, RatioTo, NumberOfBitsPerEstimationBlockTo, DataToRx)
	auto space = SeedTo.space(ready_SeedTo);
	auto process = std::min(ready_SeedTo, space);
	if (process)
	{
		t_integer seed{ 0 };
		inputSignals[1]->bufferGet(&seed);
		SeedTo.bufferPut(seed);
		alive = true;
	}

	space = RatioTo.space(ready_RatioTo);
	process = std::min(ready_RatioTo, space);
	if (process)
	{
		t_integer ratioTo{ 0 };
		inputSignals[2]->bufferGet(&ratioTo);
		RatioTo.bufferPut(ratioTo);
		alive = true;
	}

	space = NumberOfBitsPerEstimationBlock.space(ready_NumberOfBitsPerEstimationBlock);
	process = std::min(ready_NumberOfBitsPerEstimationBlock, space);
	if (process)
	{
		t_integer numberOfBitsPerEstimationBlock{ 0 };
		inputSignals[3]->bufferGet(&numberOfBitsPerEstimationBlock);
		NumberOfBitsPerEstimationBlock.bufferPut(numberOfBitsPerEstimationBlock);
		alive = true;
	}
	
	space = DataTo.space(ready_DataTo);
	process = std::min(ready_DataTo, space);
	for (auto k = 0; k < process; k++)
	{
		t_binary dataTo{ 0 };
		inputSignals[4]->bufferGet(&dataTo);
		DataTo.bufferPut(dataTo);
		alive = true;
	}

	if (outputSignals[0]->space() && SeedTo.ready() && RatioTo.ready() && NumberOfBitsPerEstimationBlock.ready() && DataTo.ready())
	{
		t_message_type msgType = "ParameterEstimationSeedAndBitsMsg";

		std::string messageText = "";
		messageText.append(std::to_string(SeedTo.bufferRead()));
		messageText.append("#");
		messageText.append(std::to_string(RatioTo.bufferRead()));
		messageText.append("#");
		auto numberOfBitsPerEstimationBlockFrom = NumberOfBitsPerEstimationBlock.bufferRead();
		messageText.append(std::to_string(numberOfBitsPerEstimationBlockFrom));
		messageText.append("#");
		auto process = std::min(DataTo.ready(), numberOfBitsPerEstimationBlockFrom - numberOfProcessedBits);
		process = std::min(messageParameterEstimationMaximumDataLength, process);
		for (auto k = 0; k < process; k++)
		{
			messageText.append(std::to_string(DataTo.bufferGet()));
		}

		numberOfProcessedBits = numberOfProcessedBits + process;
		if (numberOfProcessedBits == numberOfBitsPerEstimationBlockFrom)
		{
			SeedTo.bufferGet();
			RatioTo.bufferGet();
			NumberOfBitsPerEstimationBlock.bufferGet();

			numberOfProcessedBits = 0;
		}

		t_message messageParamEst;
		messageParamEst.setMessageType(msgType);
		messageParamEst.setMessageDataLength(std::to_string(messageText.size()));
		messageParamEst.setMessageData(messageText);

		outputSignals[0]->bufferPut(messageParamEst);

		alive = true;
	}


	// Sindrome 
	if (ready_SindromeToSend && outputSignals[0]->space())
	{
		
		auto space = Sindrome.space(ready_SindromeToSend);
		auto process = std::min(ready_SindromeToSend, space);
		for (;process--;) 
		{
			t_binary sindromeValue;
			inputSignals[5]->bufferGet(&sindromeValue);
			Sindrome.bufferPut(sindromeValue);
		}

		process = Sindrome.ready();
		if (process)
		{
			t_message sindromeMsg;
			sindromeMsg.setMessageType("SindromeMsg");

			std::string messageText = "";

			process = std::min(process, messageSindromeMaximumDataLength);
			for (auto k = 0; k < process; ++k)
				messageText.append(std::to_string(Sindrome.bufferGet()));

			if (ready_HashToSend)
			{
				sindromeMsg.setMessageType("SindromeHashMsg");

				t_message hashMessage;
				inputSignals[6]->bufferGet(&hashMessage);
				messageText.append(hashMessage.getMessageData());
			}

			sindromeMsg.setMessageDataLength(std::to_string(messageText.length()));
			sindromeMsg.setMessageData(messageText);

			outputSignals[0]->bufferPut(sindromeMsg);
		}

		alive = true;
	}

	return alive;
}
