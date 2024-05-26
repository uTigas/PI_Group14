# include "qber_20200819.h"
#include <direct.h>
void QBer::initialize(void) {
	for (auto k : outputSignals) {
		k->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
		k->setSamplingPeriod(inputSignals[0]->getSamplingPeriod() / n);
		k->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	}


	if (window > 0) {
		for (auto i = 0; i < window; i++)
			windowBuffer.push_back(0);
	}
}

bool QBer::runBlock(void) {
	/* Computing z */ // This code converges in below 10 steps, exactness chosen in order to achieve this rapid convergence
	limit++;
	if (firstPass)
	{
		firstPass = 0;
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

	}

	int ready0 = inputSignals[0]->ready();
	int ready1 = inputSignals[1]->ready();
	int ready2 = inputSignals[2]->ready();
	int ready3 = inputSignals[3]->ready();
	int ready = min(min(min(ready0, ready1), ready2), ready3);

	int space = outputSignals[0]->space();

	int process = min(ready, space);

	if (numberOfSamplesToProcess == 0 || process <=0) return false;

	

	for (auto k = 0; k < process; k++) { // process all demands contained in the buffer
		t_binary dataAlice{ 0 };
		t_binary dataBob{ 0 };
		t_binary basisAlice{ 0 };
		t_binary basisBob{ 0 };
		inputSignals[0]->bufferGet(&dataAlice); 
		inputSignals[1]->bufferGet(&dataBob);
		inputSignals[2]->bufferGet(&basisAlice);
		inputSignals[3]->bufferGet(&basisBob); 

		if (syncMode != sync_mode::Off && nSync)
		{
			
			if (discard)
			{
				discard = false;
				switch (syncMode)
				{
				case sync_mode::On_BobAhead:
					inputSignals[1]->bufferGet(&dataBob);
					ready1--;
					process = min(ready1, space);
					break;
				case sync_mode::On_AliceAhead:
					inputSignals[0]->bufferGet(&dataAlice);
					ready0--;
					process = min(ready0, space);
					break;
				default:
					std::cout << "QBER syncing routine: No direction given to sync!\n";
					break;

				}

			}

			if ((dataBob == 0) || (dataBob == 1))
			{
				inBufferSync++;
				if (dataBob == dataAlice) {
					outputSignals[0]->bufferPut((t_binary)2);
					coincidencesSync++;
				}
			}
			if (inBufferSync == bufferSize)
			{
				inBufferSync = 0;
				QBER = (1 - coincidencesSync / bufferSize) * 100;
				std::cout << QBER << std::endl;
				if (QBER > 100 - berLimit) {
					std::cout << "Cables are crossed yo!\n";

				}
				if (QBER < berLimit) {
					nSync = false; // if the QBER is bellow the limit, the system is synchronized
					std::cout << "Synchronization established! \n";
				}
				else discard = true;

				coincidencesSync = 0.0;
			}
		}
		else
		{
			// window represents the number of bits used to calculate the QBER
				if (dataBob == 5.0) { // Control bit
					outputSignals[0]->bufferPut((t_binary)dataBob);
					cBit++;

				}
				else {
					if (cBit > 1) { // If it is not a control bit and there were previously registred control bits
						for (auto i = 0; i < windowBuffer.size(); i++) {
							windowBuffer[i] = 0;
						}
					}
					cBit = 0; // resets control bits

					double samplingPeriod_ = inputSignals[0]->getSamplingPeriod();

					inputSignals[0]->bufferGet(&dataAlice);
					inputSignals[1]->bufferGet(&dataBob);
					inputSignals[2]->bufferGet(&basisAlice);
					inputSignals[3]->bufferGet(&basisBob);

					

					//bool fillArray{ false };
					if (basisAlice == basisBob) {
						if (dataBob == 3) { // 00
							t_binary out = 3;
							outputSignals[0]->bufferPut((t_binary)out);
							noClicks++;
							outputs++;
						}
						else if (dataBob == 2) { // 11
							t_binary out = 2;
							outputSignals[0]->bufferPut((t_binary)out);
							doubleClicks++;
							outputs++;
						}
						else if (dataBob == dataAlice){
							t_binary out = 0;
							outputSignals[0]->bufferPut((t_binary)out);
							m++;
							coincidences++;
							totalReceivedBits++;
							receivedBits++;
							outputs++;
							
						}
						else {
							t_binary out = 1;
							outputSignals[0]->bufferPut((t_binary)out);
							totalReceivedBits++;
							receivedBits++;
							m++;
							outputs++;

							
						}
						
					}
					else if (basisAlice != basisBob) {
						if (dataBob == 3) { // 00
							t_binary out = 3;
							outputSignals[0]->bufferPut((t_binary)out);
							noClicks++;
							outputs++;
						}
						else if (dataBob == 2) { // 11
							t_binary out = 2;
							outputSignals[0]->bufferPut((t_binary)out);
							doubleClicks++;
							outputs++;
						}
						else if (dataBob == dataAlice) {
							t_binary out = 0;
							outputSignals[0]->bufferPut((t_binary)out);
							coincidences_difbasis++;
							totalReceivedBits++;
							m++;
							outputs++;
						}
						else {
							t_binary out = 1;
							outputSignals[0]->bufferPut((t_binary)out);
							totalReceivedBits++;
							m++;
							outputs++;
						}
						
					}


					if (m == window) {
						// Same basis between Alice and Bob

						QBER = (receivedBits - coincidences) / (receivedBits);
						/* Calculating bounds */
						double UpperBound = QBER + 1 / sqrt(receivedBits) * z * sqrt(QBER * (1 - QBER)) + 1 / (3 * receivedBits) * (2 * z * z * (1 / 2 - QBER) + (2 - QBER));
						double LowerBound = QBER - 1 / sqrt(receivedBits) * z * sqrt(QBER * (1 - QBER)) + 1 / (3 * receivedBits) * (2 * z * z * (1 / 2 - QBER) - (1 + QBER));

						// Same and different basis between Alice and Bob

						QBER_all = (totalReceivedBits - receivedBits - coincidences_difbasis) / (totalReceivedBits - receivedBits);

						/* Calculating bounds */
						double UpperBound_all = QBER_all + 1 / sqrt(totalReceivedBits) * z * sqrt(QBER_all * (1 - QBER_all)) + 1 / (3 * totalReceivedBits) * (2 * z * z * (1 / 2 - QBER_all) + (2 - QBER_all));
						double LowerBound_all = QBER_all - 1 / sqrt(totalReceivedBits) * z * sqrt(QBER_all * (1 - QBER_all)) + 1 / (3 * totalReceivedBits) * (2 * z * z * (1 / 2 - QBER_all) - (1 + QBER_all));


						noClicksRate = noClicks / (outputs * samplingPeriod_);
						doubleClicksRate = doubleClicks / (outputs * samplingPeriod_);

						noClicksPercentage = 100 * noClicks / outputs;
						doubleClicksPercentage = 100 * doubleClicks / outputs;

						ConsoleOutputCommon::moveCursorTopLeft();
						ConsoleOutputCommon::clrLine(); printf("Results - same basis between Alice and Bob \n");
						ConsoleOutputCommon::clrLine(); printf("Estimated QBER:					%.5f %%\n", 100 * QBER);
						ConsoleOutputCommon::clrLine(); printf("QBER Upper Bound:				%.5f %%\n", UpperBound * 100);
						ConsoleOutputCommon::clrLine(); printf("QBER Lower Bound:				%.5f %%\n", LowerBound * 100);
						ConsoleOutputCommon::clrLine(); printf("Processed Bits:					%.f \n", receivedBits);

						ConsoleOutputCommon::clrLine(); printf("#####################################################\n");

						ConsoleOutputCommon::clrLine(); printf("Results - different basis between Alice and Bob \n");
						ConsoleOutputCommon::clrLine(); printf("Estimated QBER:					%.5f %%\n", 100 * QBER_all);
						ConsoleOutputCommon::clrLine(); printf("QBER Upper Bound:				%.5f %%\n", UpperBound_all * 100);
						ConsoleOutputCommon::clrLine(); printf("QBER Lower Bound:				%.5f %%\n", LowerBound_all * 100);
						ConsoleOutputCommon::clrLine(); printf("Processed Bits:					%.f \n", totalReceivedBits - receivedBits);

						ConsoleOutputCommon::clrLine(); printf("#####################################################\n");

						ConsoleOutputCommon::clrLine(); printf("Double Clicks Percentage :			%.5f %%\n", doubleClicksPercentage);
						ConsoleOutputCommon::clrLine(); printf("Double Clicks Rate:				%.5f /s\n", doubleClicksRate );
						ConsoleOutputCommon::clrLine(); printf("No Clicks Percentage :				%.5f %%\n", noClicksPercentage);
						ConsoleOutputCommon::clrLine(); printf("No Clicks Rate:					%.5f /s\n", noClicksRate );


						ConsoleOutputCommon::clrRemainingScreen();

						if (midReport) {
							
							if (createFolder) {
								_mkdir(folder);
								createFolder = false;
							}

							std::string filename;
							std::ostringstream oss;
							

							if (nSync && syncMode != sync_mode::Off) {
								numberSyncLossReport++;
								oss << "midreportSyncLoss" << numberSyncLossReport << ".txt";
								filename = oss.str();
							}
							else
							{
								n++;
								oss << "midreport" << n << ".txt";
								filename = oss.str();
							}

							
							/* Outputting a .txt report*/
													
							std::ofstream myfile("reports/" + filename); 

							if (nSync) myfile << "### SYNCHRONIZATION LOST! ###\n";
							myfile << "Results - same basis between Alice and Bob" << "\n";
							myfile << "Estimated QBER = " << 100 * QBER << " %\n";
							myfile << "Upper Bound = " << UpperBound * 100 << " %\n";
							myfile << "Lower Bound = " << LowerBound * 100 << " %\n";
							myfile << "Number of received bits = " << receivedBits << "\n";
							myfile << "#####################################################\n";
							myfile << "Results - different basis between Alice and Bob" << "\n";
							myfile << "Estimated QBER = " << 100 * QBER_all << " %\n";
							myfile << "Upper Bound = " << UpperBound_all * 100 << " %\n";
							myfile << "Lower Bound = " << LowerBound_all * 100 << " %\n";
							myfile << "Number of received bits = " << totalReceivedBits - receivedBits << "\n";
							myfile << "#####################################################\n";
							myfile << "Double Clicks Percentage = " << doubleClicksPercentage << " %\n";
							myfile << "Double Clicks Rate = " << doubleClicksRate << " /s\n";
							myfile << "No Clicks Percentage = " << noClicksPercentage << " %\n";
							myfile << "No Clicks Rate = " << noClicksRate << " /s\n";
							

							myfile.close();
						}
						m = 0;
						receivedBits = 0;
						coincidences = 0;
						totalReceivedBits = 0;
						coincidences_difbasis = 0;
						outputs = 0;
						noClicks = 0;
						doubleClicks = 0;
						return false;



					}
					

				}

			
		}
	

		
	}
	
	numberOfProcessedSamples = numberOfProcessedSamples + process;
	numberOfSamplesToProcess = numberOfSamplesToProcess - process;

	

	return true;
}
