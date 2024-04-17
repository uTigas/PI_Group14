# include "fiber_20200819.h"

void Fiber::initialize(void) 
{

	if (saveSignalDuringPropagation)
	{
		if (!(std::filesystem::is_directory(getSignalsFolderName())))	
			std::filesystem::create_directory(getSignalsFolderName());

		if ((std::filesystem::is_directory(getSignalsFolderName() + "/" + saveSignalDuringPropagationFolder)))
			std::filesystem::remove_all(getSignalsFolderName() + "/" + saveSignalDuringPropagationFolder);

		std::filesystem::create_directory(getSignalsFolderName() + "/" + saveSignalDuringPropagationFolder);
	}
	
	/* calculation of beta2, beta3 */
	t_real lambda = inputSignals[0]->getCentralWavelength_m();
	t_real beta2 = - dispersion_s_per_m2 * lambda * lambda / (2 * PI * SPEED_OF_LIGHT);
	t_real beta3{ 0.0 };
	if (useBeta3)
		beta3 = pow(lambda, 4) / pow(2 * PI * SPEED_OF_LIGHT, 2) * dispersionSlope_s_per_m3 + 2 * pow(lambda, 3) * dispersion_s_per_m2 / pow(2 * PI * SPEED_OF_LIGHT, 2);
	
	t_real dt = inputSignals[0]->getSamplingPeriod(); // sampling period

	if (fiberLength_m == 0.0)
		fiberImpulseResponseLength = 1;
	else
	{
		fiberImpulseResponseLength = std::max((int)(fiberImpulseResponseSafeMarginParameter * std::abs(2 * PI * beta2 / (dt * dt) * stepLength_m)),1);

		printf("Fiber Impulse Response Length: %d\n", fiberImpulseResponseLength);
		printf("Overlap Save Block Length: %d\n", overlapSaveBlockLength);

		if (fiberImpulseResponseLength > overlapSaveBlockLength)
		{
			printf("Error: the overlap save block length must be increased\n");
			exit(EXIT_FAILURE);
		}
		else if (fiberImpulseResponseLength > overlapSaveBlockLength / 2)
		{
			printf("Warning: considerer to increase the overlap save block length\n");
		}

		std::vector<t_real> w(overlapSaveBlockLength);
		t_real dw = 2 * PI / (dt * overlapSaveBlockLength);
		for (int k = 0; k <= overlapSaveBlockLength / 2; k++)
			w[k] = k * dw;
		for (int k = 1; k < overlapSaveBlockLength / 2; k++)
			w[k + (int)(overlapSaveBlockLength / 2)] = -overlapSaveBlockLength / 2 * dw + k * dw;

		/* Calculate the linear operator */
		linearEffectHalfStep.resize(overlapSaveBlockLength);
		linearEffectFullStep.resize(overlapSaveBlockLength);
		//std::vector<double> phase(overlapSaveBlockLength); // just for debug
		for (int k = 0; k < overlapSaveBlockLength; k++)
		{
			auto amplitudeHalfStep = exp(-attenuation_per_m / 2 * stepLength_m / 2);
			auto amplitudeFullStep = exp(-attenuation_per_m / 2 * stepLength_m);
			auto phaseHalfStep = fmod((beta2 * pow(w[k], 2) / 2.0 + beta3 * pow(w[k], 3) / 6.0) * stepLength_m / 2, 2 * PI);
			auto phaseFullStep = fmod((beta2 * pow(w[k], 2) / 2.0 + beta3 * pow(w[k], 3) / 6.0) * stepLength_m, 2 * PI);
			linearEffectHalfStep[k] = { amplitudeHalfStep * cos(phaseHalfStep), amplitudeHalfStep * sin(phaseHalfStep) };
			linearEffectFullStep[k] = { amplitudeFullStep * cos(phaseFullStep), amplitudeFullStep * sin(phaseFullStep) };
			//phase[k] = phaseFullStep; // just for debug
		}


		if (debugMode)
		{
			saveComplexVector(linearEffectFullStep, saveSignalDuringPropagationFolder + "/fiberTransferFunctionFullStep.txt", 0, overlapSaveBlockLength);
			std::vector<t_complex> aux = linearEffectFullStep;
			std::vector<t_complex> fiberImpulseResponseFullfStep = ifft(aux);
			saveComplexVector(fiberImpulseResponseFullfStep, saveSignalDuringPropagationFolder + "/fiberImpulseResponseFullStep.txt", 0, overlapSaveBlockLength);
		}
	}

	signalTimeDomain.resize(overlapSaveBlockLength, { 0,0 });
	positionIn = fiberImpulseResponseLength-1;
	positionOut = -1;

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[0]->setNumberOfValuesToBeSkipped(inputSignals[0]->getNumberOfValuesToBeSkipped()+2*positionIn); //why the two? because it works..., ANP, 18/12/20220

}

bool Fiber::runBlock(void) 
{
	bool alive{ false };

	int ready = inputSignals[0]->ready();

	int process{ 0 };
	if (positionOut == -1)
		process = std::min(ready, overlapSaveBlockLength - positionIn);
	else
		process = std::min(ready, positionOut - positionIn);

	alive = (process>0);

	for (auto k=0; k < process; k++)
	{
		t_complex aux;
		inputSignals[0]->bufferGet(&aux);
		signalTimeDomain[positionIn] = aux;
		positionIn++;
	}

	if ((positionIn == overlapSaveBlockLength ) && (positionOut == -1))
	{
		t_real z{ 0 };
		t_real distanceSinceLastSave_m{ 0 };

		//if (saveSignalDuringPropagation)
		//	distanceSinceLastSave_m = saveComplexVector(signalTimeDomain, saveSignalDuringPropagationFolder + "/signal_after_" + std::to_string(int(z)) + "_m_time_domain.txt", 0, overlapSaveBlockLength);

		std::vector<t_complex> signalFrequencyDomain(overlapSaveBlockLength);

		if (fiberLength_m> 0 && useSSFM) // implements the SSFM
		{
			
			signalFrequencyDomain = fft(signalTimeDomain);
			std::transform(linearEffectHalfStep.begin(), linearEffectHalfStep.end(), signalFrequencyDomain.begin(), signalFrequencyDomain.begin(), std::multiplies<t_complex>());
			signalTimeDomain = ifft(signalFrequencyDomain);
			

			std::vector<t_complex> nonLinearEffect(overlapSaveBlockLength);
			for (auto k = 0; k < overlapSaveBlockLength; k++)
				nonLinearEffect[k] = exp(std::complex<double>(0, nonlinearCoefficient_per_W_per_m * (2 * pow(abs(signalTimeDomain[k]), 2)) * stepLength_m));
			std::transform(nonLinearEffect.begin(), nonLinearEffect.end(), signalTimeDomain.begin(), signalTimeDomain.begin(), std::multiplies<t_complex>());

			z = z + stepLength_m / 2;
			distanceSinceLastSave_m = distanceSinceLastSave_m + stepLength_m / 2;
			if (saveSignalDuringPropagation && saveDistanceInterval_m <= distanceSinceLastSave_m)
				distanceSinceLastSave_m = saveComplexVector(signalTimeDomain, saveSignalDuringPropagationFolder + "/signal_after_" + std::to_string(int(z)) + "_m_time_domain.txt", fiberImpulseResponseLength - 1, overlapSaveBlockLength);

			auto numberOfSteps{ (int)(fiberLength_m / stepLength_m) };
			for (auto k = 0; k < numberOfSteps - 1; k++)
			{
				signalFrequencyDomain = fft(signalTimeDomain);
				std::transform(linearEffectFullStep.begin(), linearEffectFullStep.end(), signalFrequencyDomain.begin(), signalFrequencyDomain.begin(), std::multiplies<t_complex>());
				signalTimeDomain = ifft(signalFrequencyDomain);

				for (auto k = 0; k < overlapSaveBlockLength; k++)
					nonLinearEffect[k] = exp(std::complex<double>(0, -nonlinearCoefficient_per_W_per_m * (2 * pow(abs(signalTimeDomain[k]), 2)) * stepLength_m));
				std::transform(nonLinearEffect.begin(), nonLinearEffect.end(), signalTimeDomain.begin(), signalTimeDomain.begin(), std::multiplies<t_complex>());

				z = z + stepLength_m;
				distanceSinceLastSave_m = distanceSinceLastSave_m + stepLength_m;
				if (saveSignalDuringPropagation && saveDistanceInterval_m <= distanceSinceLastSave_m)
					distanceSinceLastSave_m = saveComplexVector(signalTimeDomain, saveSignalDuringPropagationFolder + "/signal_after_" + std::to_string(int(z)) + "_m_time_domain.txt", fiberImpulseResponseLength - 1, overlapSaveBlockLength);
			}

			signalFrequencyDomain = fft(signalTimeDomain);
			std::transform(linearEffectHalfStep.begin(), linearEffectHalfStep.end(), signalFrequencyDomain.begin(), signalFrequencyDomain.begin(), std::multiplies<t_complex>());
			signalTimeDomain = ifft(signalFrequencyDomain);

			z = z + stepLength_m / 2;
			if (saveSignalDuringPropagation)
				saveDistanceInterval_m = saveComplexVector(signalTimeDomain, saveSignalDuringPropagationFolder + "/signal_after_" + std::to_string(int(z)) + "_m_time_domain.txt", fiberImpulseResponseLength - 1, overlapSaveBlockLength);

			distanceSinceLastSave_m = 0;
			positionIn = std::max((int)fiberImpulseResponseLength - 1, 0);
			positionOut = positionIn;

			signalTimeDomain.insert(signalTimeDomain.begin(), signalTimeDomain.end() - positionIn, signalTimeDomain.end());

			alive = true;
		}
		else if (fiberLength_m>0) // implements the linear operator
		{
			std::vector<t_complex> save((int)fiberImpulseResponseLength - 1);
			save.insert(save.begin(),signalTimeDomain.end()-((int)fiberImpulseResponseLength-1),signalTimeDomain.end());

			auto numberOfSteps{ (int)(fiberLength_m / stepLength_m) };
			for (auto k = 0; k < numberOfSteps; k++)
			{
				saveComplexVector(signalTimeDomain, saveSignalDuringPropagationFolder + "/signal_after_" + std::to_string(int(0)) + "_m_time_domain.txt", 0, overlapSaveBlockLength);
				std::vector<t_complex> aux = signalTimeDomain;
				signalFrequencyDomain = fft(aux);
				saveComplexVector(signalFrequencyDomain, saveSignalDuringPropagationFolder + "/signal_after_" + std::to_string(int(0)) + "_m_frequency_domain.txt", 0, overlapSaveBlockLength);

				std::transform(linearEffectFullStep.begin(), linearEffectFullStep.end(), signalFrequencyDomain.begin(), signalFrequencyDomain.begin(), std::multiplies<t_complex>());
				//for (auto m = 0; m < overlapSaveBlockLength; m++)
					//signalFrequencyDomain[m] *= linearEffectFullStep[m];

				z = z + stepLength_m;

				saveComplexVector(signalFrequencyDomain, saveSignalDuringPropagationFolder + "/signal_after_" + std::to_string(int(z)) + "_m_frequency_domain.txt", 0, overlapSaveBlockLength);
				aux = signalFrequencyDomain;
				signalTimeDomain = ifft(aux);

				distanceSinceLastSave_m = distanceSinceLastSave_m + stepLength_m;
				if (saveSignalDuringPropagation && saveDistanceInterval_m <= distanceSinceLastSave_m)
					distanceSinceLastSave_m = saveComplexVector(signalTimeDomain, saveSignalDuringPropagationFolder + "/signal_after_" + std::to_string(int(z)) + "_m_time_domain.txt", 0, overlapSaveBlockLength);
			}

			distanceSinceLastSave_m = 0;
			positionIn = fiberImpulseResponseLength - 1;
			positionOut = positionIn;

			signalTimeDomain.insert(signalTimeDomain.begin(), save.begin(), save.end());

			alive = true;
		}
		else  // it is a null block
		{
			positionIn = fiberImpulseResponseLength - 1;
			positionOut = positionIn;

			alive = true;
		}
	}

	if (positionOut > - 1)
	{
		int space = outputSignals[0]->space();

		process = std::min(space, overlapSaveBlockLength  - positionOut);
		alive = (process > 0);

		for (auto k=0; k < process ; k++, positionOut++)
			outputSignals[0]->bufferPut(signalTimeDomain[positionOut]);

		if (positionOut == overlapSaveBlockLength)
			positionOut = -1;
	}

	return alive;
}

void Fiber::terminate(void)
{

	int ready = inputSignals[0]->ready();

	if (ready == 0)
		for (; positionIn < overlapSaveBlockLength; positionIn++)
			signalTimeDomain[positionIn] = t_complex(0, 0);

	return;
}

	
t_real Fiber::saveRealVector(std::vector<t_real>& data, std::string str, int begin, int end)
{
	std::ofstream outRef;
	outRef.open(getSignalsFolderName() + "/" + str, std::ios::app);
	outRef.precision(15);

	for (int k = begin; k < end; k++)
	{
		outRef << data[k] << std::endl;
	}

	outRef.close();

	return 0;
}


t_real Fiber::saveComplexVector(std::vector<t_complex> &data, std::string str, int begin, int end)
{
	std::ofstream outRef;
	outRef.open(getSignalsFolderName() + "/" + str, std::ios::app);
	outRef.precision(15);

	for (int k = begin; k < end; k++)
	{
		outRef << data[k].real();
		if (data[k].imag() >= 0)
			outRef << "+";
		else
			outRef << "-";
		outRef << abs(data[k].imag()) << "i" << std::endl;
	}

	outRef.close();

	return 0;
}

