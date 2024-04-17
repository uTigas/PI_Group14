#include "dv_qkd_ldpc_rx_message_processor_transmitter_20200819.h"

void DvQkdLdpcRxMessageProcessorTransmitter::initialize(void) 
{
	Sindrome.resize(messageSindromeMaximumDataLength);
	DataTo.resize(inputSignals[4]->getBufferLength());
}

bool DvQkdLdpcRxMessageProcessorTransmitter::runBlock(void)
{
	bool alive = false;

	auto ready_BasesTo = inputSignals[0]->ready();
	auto ready_SeedTo = inputSignals[1]->ready();
	auto ready_RatioTo = inputSignals[2]->ready();
	auto ready_NumberOfBitsPerEstimationBlock = inputSignals[3]->ready();
	auto ready_DataTo = inputSignals[4]->ready();
	auto ready_SindromeToSend = inputSignals[5]->ready();

	// Basis reconciliation message
	auto space = BasesTo.space(ready_BasesTo);
	auto process = std::min(ready_BasesTo, space);
	if (process) alive = true;
	for (; process--;)
	{
		t_binary val{ 0 };
		inputSignals[0]->bufferGet(&val);
		BasesTo.bufferPut(val);
	}
	
	auto ready = BasesTo.ready();
	if (ready && outputSignals[0]->space()) 
	{
		t_message_type msgType = "BasisReconciliationMsg";
		std::string messageText = "";



		for (;ready--;) 
			messageText.append(std::to_string(BasesTo.bufferGet()));
	
		t_message messageBasisRecon;
		messageBasisRecon.setMessageType(msgType);
		messageBasisRecon.setMessageDataLength(std::to_string(messageText.size()));
		messageBasisRecon.setMessageData(messageText);

		outputSignals[0]->bufferPut(messageBasisRecon);

		alive = true;
	}

	// Parameter estimation (SeedTo, RatioTo, NumberOfBitsPerEstimationBlock, DataToRx)
	space = SeedTo.space(ready_SeedTo);
	process = std::min(ready_SeedTo, space);
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
			for (auto k=0; k < process; ++k)
				messageText.append(std::to_string(Sindrome.bufferGet()));
			
			sindromeMsg.setMessageDataLength(std::to_string(process));
			sindromeMsg.setMessageData(messageText);

			outputSignals[0]->bufferPut(sindromeMsg);
		}

		alive = true;
	}

	return alive;
}
