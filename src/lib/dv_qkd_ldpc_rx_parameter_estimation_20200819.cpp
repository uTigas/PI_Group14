#include "dv_qkd_ldpc_rx_parameter_estimation_20200819.h"
clock_t init_start, partial_end;


void DvQkdLdpcRxParameterEstimation::initialize(void) 
{
	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	
	init_start = clock();     // Timer Start
	
	
	if (outputSignals.size() >= 6)
	{
		outputSignals[5]->setSymbolPeriod(numberOfBitsPerEstimationBlock * inputSignals[0]->getSymbolPeriod());
		outputSignals[5]->setSamplingPeriod(numberOfBitsPerEstimationBlock * inputSignals[0]->getSamplingPeriod());
		outputSignals[5]->setFirstValueToBeSaved(numberOfBitsPerEstimationBlock * inputSignals[0]->getFirstValueToBeSaved());
	}

	if (!bypassParameterEstimation)
	{
		dataInToBerEstimation.resize(inputSignals[0]->getBufferLength());
		dataFromTx.resize(inputSignals[4]->getBufferLength());
		dataToTx.resize(outputSignals[4]->getBufferLength());

		double x1 = -2;
		double x2 = 2;
		double x3 = x2 - (erf(x2 / sqrt(2)) + 1 - alpha) * (x2 - x1) / (erf(x2 / sqrt(2)) - erf(x1 / sqrt(2)));
		double exacteness = 1e-15;

		while (abs(erf(x3 / sqrt(2)) + 1 - alpha) > exacteness)
		{
			x3 = x2 - (erf(x2 / sqrt(2)) + 1 - alpha) * (x2 - x1) / (erf(x2 / sqrt(2)) - erf(x1 / sqrt(2)));
			x1 = x2;
			x2 = x3;
		}

		z = -x3;

		if (writeBerList && getVerboseMode())
		{
			if (!std::filesystem::is_directory(outputFolderName) || !std::filesystem::exists(outputFolderName))
			{
				std::filesystem::create_directories(outputFolderName);
			}
			std::ofstream myfile;
			myfile.open(outputFolderName + bersListFileName);
			myfile << "BER \t\t Upper Bound \t\t Lower Bound\n";

			myfile.close();
		}
	}
}

bool DvQkdLdpcRxParameterEstimation::runBlock(void)
{
	t_bool alive{ false };

	if (bypassParameterEstimation)
	{

		auto ready = inputSignals[0]->ready();
		auto space = outputSignals[0]->space();
		auto process = std::min(ready, space);
		for (auto k = 0; k < process; k++)
		{
			t_binary dataIn{ 0 };
			inputSignals[0]->bufferGet((t_binary*)&dataIn);
			outputSignals[0]->bufferPut(dataIn);

			partial_end = clock();
			totalNumberOfInputBits++;

			// Speed Calculation
			transmitSpeed = totalNumberOfInputBits/(double(partial_end - init_start) / double(CLOCKS_PER_SEC));
		}

		for (auto k = 1; k <= 4; k++)
		{
			ready = inputSignals[k]->ready();
			process = ready;
			for (auto m = 0; m < process; k++)
				inputSignals[k]->bufferGet();
		}

		return alive;
	}

	auto ready = dataToTx.ready();
	auto space = outputSignals[4]->space();
	auto process = std::min(ready, space);
	if (process) alive = true;
	for (auto k = 0; k < process; k++)
		outputSignals[4]->bufferPut(dataToTx.bufferGet());

	ready = inputSignals[1]->ready();
	space = seed_buffer.space();
	process = std::min(ready, space);
	if (process) alive = true;
	for (auto k = 0; k < process; k++)
	{
		t_integer val{ 0 };
		inputSignals[1]->bufferGet((t_integer*)&val);
		seed_buffer.bufferPut(val);
	}

	ready = inputSignals[2]->ready();
	space = ratio_buffer.space();
	process = std::min(ready, space);
	if (process) alive = true;
	for (auto k = 0; k < process; k++)
	{
		t_integer val{ 0 };
		inputSignals[2]->bufferGet((t_integer*)&val);
		ratio_buffer.bufferPut(val);
	}

	ready = inputSignals[3]->ready();
	space = NumberOfBitsPerEstimationBlockFromTx.space();
	process = std::min(ready, space);
	if (process) alive = true;
	for (auto k = 0; k < process; k++)
	{
		t_integer val{ 0 };
		inputSignals[3]->bufferGet((t_integer*)&val);
		NumberOfBitsPerEstimationBlockFromTx.bufferPut(val);
	}

	ready = inputSignals[4]->ready();
	space = dataFromTx.space();
	process = std::min(ready, space);
	if (process) alive = true;
	for (auto k = 0; k < process; k++)
	{
		t_binary val{ 0 };
		inputSignals[4]->bufferGet((t_binary*)&val);
		dataFromTx.bufferPut(val);
	}

	if (role == t_role::slave)
	{
		ready = (size_t)inputSignals[0]->ready();
		space = (size_t)outputSignals[0]->space();

		process = std::min(ready, space);

		space = dataInToBerEstimation.space();
		space = std::min(space, dataToTx.space());

		while (process && space )
		{
			if (numberOfBitsSendToDataToBerEstimation < 0)  // -1 means renew seed
			{
				if (outputSignals[1]->space() && seed_buffer.ready() && outputSignals[2]->space() && ratio_buffer.ready() && outputSignals[3]->space() && NumberOfBitsPerEstimationBlockFromTx.ready())
				{
					auto seedFromTx = seed_buffer.bufferGet();
					rand_rx.setSeed(seedFromTx);

					ratio = ratio_buffer.bufferGet();
					numberOfBitsPerEstimationBlock = NumberOfBitsPerEstimationBlockFromTx.bufferGet();

					outputSignals[1]->bufferPut(seedFromTx);
					outputSignals[2]->bufferPut(ratio);
					outputSignals[3]->bufferPut(numberOfBitsPerEstimationBlock);

					numberOfBitsSendToDataToBerEstimation = 0;

					alive = true;
				}
				else
					break;
			}

			t_binary val{ 0 };
			inputSignals[0]->bufferGet((t_binary*)&val);
			totalNumberOfInputBits += 1;

			auto rv = rand_rx();
			if (rv < (1.0 / ratio))
			{
				dataInToBerEstimation.bufferPut(val);
				dataToTx.bufferPut(val);
				numberOfBitsSendToDataToBerEstimation++;
				if (numberOfBitsSendToDataToBerEstimation == numberOfBitsPerEstimationBlock)	numberOfBitsSendToDataToBerEstimation = -1;
				--space;
				alive = true;
			}
			else
			{
				outputSignals[0]->bufferPut(val);
				totalNumberOfOutputBits++;
				alive = true;
			}
			--process;
		}
	}

	process = std::min(dataInToBerEstimation.ready(), dataFromTx.ready());
	if (process) alive = true;
	for (auto k = 0; k < process; k++)
	{
		t_binary valDataIn = dataInToBerEstimation.bufferGet();
		t_binary valDataFromTx = dataFromTx.bufferGet();

		if (valDataIn == valDataFromTx)	coincidences++;

		numberOfProcessedBits++;

		if (numberOfProcessedBits == numberOfBitsPerEstimationBlock)
		{
			BER = 1.0 - (coincidences / (t_real)numberOfBitsPerEstimationBlock);
			BERUpperBound = BER + 1.0 / sqrt((t_real)numberOfBitsPerEstimationBlock) * z * sqrt(BER * (1.0 - BER)) + 1.0 / (3.0 * (t_real)numberOfBitsPerEstimationBlock) * (2.0 * z * z * (1.0 / 2.0 - BER) + (2.0 - BER));
			BERLowerBound = BER - 1.0 / sqrt((t_real)numberOfBitsPerEstimationBlock) * z * sqrt(BER * (1.0 - BER)) + 1.0 / (3.0 * (t_real)numberOfBitsPerEstimationBlock) * (2.0 * z * z * (1.0 / 2.0 - BER) - (1.0 + BER));
			BERLowerBound = std::min(BERLowerBound, BER);
			BERLowerBound = std::max(BERLowerBound, (t_real)0);

			numberOfEstimations += 1;

			berListUpdated = false;

			if (writeBerList && getVerboseMode())
			{
				std::ofstream myfile;
				myfile.open(outputFolderName + bersListFileName, std::ofstream::app);
				myfile << BER << "\t\t " << BERUpperBound << "\t\t " << BERLowerBound << '\n';
				myfile.close();
			}

			updateReportResults();

			numberOfProcessedBits = 0;
			coincidences = 0;
		}
	}

	ready = dataToTx.ready();
	space = outputSignals[4]->space();
	process = std::min(ready, space);
	if (process) alive = true;
	for (auto k = 0; k < process; k++)
		outputSignals[4]->bufferPut(dataToTx.bufferGet());


	if (((!reportUpdated) || !alive) &&getVerboseMode())
	{

		std::ofstream myfile;
		myfile.open(outputFolderName + reportOutputFileName);
		myfile << "Total Average BER=           " << averageBER << "\n";
		myfile << "Highest estimated BER=       " << highestBER << "\n";
		myfile << "Lowest estimated BER=        " << lowestBER << "\n";
		myfile << "Upper and lower confidence bounds for " << (1 - alpha) * 100 << "% confidence level \n";

		myfile << "Highest Upper Bound=         " << highestBERUpperBound << "\n";
		myfile << "Lowest Upper Bound=          " << lowestBERUpperBound << "\n";
		myfile << "Highest Lower Bound=         " << highestBERLowerBound << "\n";
		myfile << "Lowest Lower Bound=          " << lowestBERLowerBound << "\n";
		myfile << "Number of estimations=        " << numberOfEstimations << "\n";
		myfile << "Number of received bits=     " << totalNumberOfInputBits << "\n";
		myfile << "Number of sent bits=         " << totalNumberOfOutputBits << "\n";
		myfile << "Transmit Speed= 					" << transmitSpeed << "\n";

		myfile << "Number of Bits Per estimation Block= " << numberOfBitsPerEstimationBlock << "\n";
		myfile << "Bits used for estimation=    " << numberOfEstimations * numberOfBitsPerEstimationBlock << "\n";

		myfile.close();
		reportUpdated = false;

	}

	return alive;
}

void DvQkdLdpcRxParameterEstimation::updateReportResults(void) 
{
	averageBER = ((averageBER * ((int64_t)numberOfEstimations - 1)) + BER) / numberOfEstimations;
	highestBER = std::max(highestBER, BER);
	lowestBER = std::min(lowestBER, BER);

	highestBERUpperBound = std::max(highestBERUpperBound, BERUpperBound);
	lowestBERUpperBound = std::min(lowestBERUpperBound, std::max(BERUpperBound, (t_real)0));
	highestBERLowerBound = std::max(highestBERLowerBound, BERLowerBound);
	lowestBERLowerBound = std::min(lowestBERLowerBound, std::max(BERLowerBound, (t_real)0));

	reportUpdated = false;
}