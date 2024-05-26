# include "binary_source_20200819.h"

void BinarySource::initialize(void) 
{
	for (auto k : outputSignals)
	{
		k->setSymbolPeriod(bitPeriod);
		k->setSamplingPeriod(bitPeriod);
	}

	if ((mode == BinarySourceMode::AsciiFileAppendZeros) || (mode == BinarySourceMode::AsciiFileCyclic))
	{
		asciiFile.open(asciiFileName);
	}
	else if (mode == BinarySourceMode::Random)
	{
		netxpto::rng(seed);
	}
}

void BinarySource::terminate(void)
{
	if ((mode == BinarySourceMode::AsciiFileAppendZeros) || (mode == BinarySourceMode::AsciiFileCyclic))
	{
		asciiFile.close();
	}
}

bool BinarySource::runBlock(void) 
{
	long space = outputSignals[0]->space();
	for (auto k : outputSignals) {
		long int aux = k->space();
		space = std::min(space, aux);
	}

	long int process{ 0 };
	if (numberOfBits >= 0) {
		process = std::min(space, numberOfBits);
	}
	else {
		process = space;
	}

	if (process <= 0) return false;

	switch (mode) {
		case BinarySourceMode::PseudoRandom: 
			runPseudoRandom(process);
			break;
		case BinarySourceMode::Random:
			runRandom(process);
			break;
		case BinarySourceMode::DeterministicCyclic:
			runDeterministicCyclic(process);
			break;
		case BinarySourceMode::DeterministicAppendZeros:
			runDeterministicAppendZeros(process);
			break;
		case BinarySourceMode::AsciiFileCyclic:
			runAsciiFileCyclic(process);
			break;
		case BinarySourceMode::AsciiFileAppendZeros:
			runAsciiFileAppendZeros(process);
			break;
	}

	return true;
}

void BinarySource::setBitPeriod(double bPeriod) {

	bitPeriod = bPeriod;

	for (auto k : outputSignals) {
		k->setSymbolPeriod(bitPeriod);
		k->setSamplingPeriod(bitPeriod);
	}
}

void BinarySource::runPseudoRandom(long int process)
{
	if (acumul.size() == 0) {

		acumul.resize(33);

		for (int i = 0; i < 32; i += 2) {
			acumul[i] = 1;
			acumul[i + 1] = 0;
		}
		acumul[5] = 0;
		acumul[4] = 0;
		acumul[3] = 1;
		acumul[2] = 0;
		acumul[1] = 1;
		acumul[0] = 0;
	}

	int len = patternLength;

	std::vector<int>& ac = acumul;

	for (int k = 0; k < process; k++) {
		t_binary aux = (t_binary)ac[len];
		for (auto m = 0; m != getNumberOfOutputSignals(); ++m) {
			outputSignals[m]->bufferPut((t_binary)aux);
		}
		numberOfBits--;

		for (int i = len; i > 0; --i) ac[i] = ac[i - 1];

		switch (len) {
		case 1:
			ac[0] = ac[1];
			break;
		case 2:
			ac[0] = (ac[2] + ac[1]) % 2;
			break;
		case 3:
			ac[0] = (ac[3] + ac[1]) % 2;
			break;
		case 4:
			ac[0] = (ac[4] + ac[1]) % 2;
			break;
		case 5:
			ac[0] = (ac[5] + ac[2]) % 2;
			break;
		case 6:
			ac[0] = (ac[6] + ac[1]) % 2;
			break;
		case 7:
			ac[0] = (ac[7] + ac[1]) % 2;
			break;
		case 8:
			ac[0] = (ac[8] + ac[4] + ac[3] + ac[2]) % 2;
			break;
		case 9:
			ac[0] = (ac[9] + ac[4]) % 2;
			break;
		case 10:
			ac[0] = (ac[10] + ac[3]) % 2;
			break;
		case 11:
			ac[0] = (ac[11] + ac[2]) % 2;
			break;
		case 12:
			ac[0] = (ac[12] + ac[6] + ac[4] + ac[1]) % 2;
			break;
		case 13:
			ac[0] = (ac[13] + ac[4] + ac[3] + ac[1]) % 2;
			break;
		case 14:
			ac[0] = (ac[14] + ac[5] + ac[3] + ac[1]) % 2;
			break;
		case 15:
			ac[0] = (ac[15] + ac[1]) % 2;
			break;
		case 16:
			ac[0] = (ac[16] + ac[5] + ac[3] + ac[2]) % 2;
			break;
		case 17:
			ac[0] = (ac[17] + ac[3]) % 2;
			break;
		case 18:
			ac[0] = (ac[18] + ac[5] + ac[2] + ac[1]) % 2;
			break;
		case 19:
			ac[0] = (ac[19] + ac[5] + ac[2] + ac[1]) % 2;
			break;
		case 20:
			ac[0] = (ac[20] + ac[3]) % 2;
			break;
		case 21:
			ac[0] = (ac[21] + ac[2]) % 2;
			break;
		case 22:
			ac[0] = (ac[22] + ac[1]) % 2;
			break;
		case 23:
			ac[0] = (ac[23] + ac[5]) % 2;
			break;
		case 24:
			ac[0] = (ac[24] + ac[4] + ac[3] + ac[1]) % 2;
			break;
		case 25:
			ac[0] = (ac[25] + ac[3]) % 2;
			break;
		case 26:
			ac[0] = (ac[26] + ac[6] + ac[2] + ac[1]) % 2;
			break;
		case 27:
			ac[0] = (ac[27] + ac[5] + ac[2] + ac[1]) % 2;
			break;
		case 28:
			ac[0] = (ac[28] + ac[3]) % 2;
			break;
		case 29:
			ac[0] = (ac[29] + ac[2]) % 2;
			break;
		case 30:
			ac[0] = (ac[30] + ac[6] + ac[4] + ac[2]) % 2;
			break;
		case 31:
			ac[0] = (ac[31] + ac[3]) % 2;
			break;
		case 32:
			ac[0] = (ac[32] + ac[7] + ac[5] + ac[3] + ac[2] + ac[1]) % 2;
			break;
		}
	}

}

void BinarySource::runRandom(long int process) 
{

	for (int k = 0; k < process; k++) {
		t_binary aux;
		double number = netxpto::rand();
		if (number < probabilityOfZero) {
			aux = 0;
		}
		else {
			aux = 1;
		};
		for (auto m = 0; m < getNumberOfOutputSignals(); m++) {
			outputSignals[m]->bufferPut((t_binary)aux);
		}
		numberOfBits--;
	}
}

void BinarySource::runDeterministicCyclic(long int process) {

	std::vector<char> values(bitStream.begin(), bitStream.end());
	int valuesSize = (int)values.size();
	for (int k = 0; k < process; k++) {
		t_binary aux = (t_binary)(values[posBitStream++] - '0');
		for (auto m = 0; m != getNumberOfOutputSignals(); m++) {
			outputSignals[m]->bufferPut((t_binary)aux);
		}
		numberOfBits--;
		posBitStream = posBitStream % valuesSize;
	}

}

void BinarySource::runDeterministicAppendZeros(long int process) {
	std::vector<char> values(bitStream.begin(), bitStream.end());
	int valuesSize = (int)values.size();
	t_binary aux;
	for (int k = 0; k < process; k++) {
		if (posBitStream == valuesSize) {
			aux = 0;
		}
		else {
			aux = (t_binary)(values[posBitStream++] - '0');
		}
		for (auto m = 0; m != getNumberOfOutputSignals(); m++) {
			outputSignals[m]->bufferPut((t_binary)aux);
		}
		numberOfBits--;
	}
}

void BinarySource::runAsciiFileCyclic(long int process)
{

	if (asciiFile)
	{
		asciiFile.seekg(0, std::ios::end);
		long size = (long)asciiFile.tellg();

		long read = std::min(size, process);
		t_string str(size, ' ');
		asciiFile.seekg(0, std::ios::beg);
		asciiFile.read(&str[0], size);

		auto k = 0;
		while (k < process)
		{
			for (auto m = 0; m < read; m++)
			{
				for (auto n = 0; n != getNumberOfOutputSignals(); n++) {
					outputSignals[n]->bufferPut((t_binary)(str[m] - '0'));
				}
				numberOfBits--;
			}
			k = k + read;
			if (k < process)
			{
				asciiFile.close();
				asciiFile.open(asciiFileName);

				asciiFile.seekg(0, std::ios::end);
				size = (long)asciiFile.tellg();

				read = std::min(size, process-k);
				str.resize(size, ' ');
				asciiFile.seekg(0, std::ios::beg);
				asciiFile.read(&str[0], size);
			}
		}
	}
}


void BinarySource::runAsciiFileAppendZeros(long int process)
{

	if (asciiFile)
	{
		asciiFile.seekg(0, std::ios::end);
		const long size = (long) asciiFile.tellg();

		auto read = std::min(size, process);
		std::string str(size, ' ');
		asciiFile.seekg(0, std::ios::beg);
		asciiFile.read(&str[0], size);
		

		for (auto k = 0; k < read; k++) 
		{
			for (auto m = 0; m != getNumberOfOutputSignals(); m++) {
				outputSignals[m]->bufferPut((t_binary)(str[k]-'0'));
			}
			numberOfBits--;
		}

		for (auto k = read; k < process; k++)
		{

			for (auto m = 0; m != getNumberOfOutputSignals(); m++) {
				outputSignals[m]->bufferPut((t_binary) 0);
			}
			numberOfBits--;

		}
	}
}

BinarySourceMode stoBinarySourceMode(std::string str)
{
	if (str == "BinarySourceMode::PseudoRandom" || str == "PseudoRandom")
	{
		return BinarySourceMode::PseudoRandom;
	}
	else if (str == "BinarySourceMode::Random" || str == "Random")
	{
		return BinarySourceMode::Random;
	}
	else if (str == "BinarySourceMode::DeterministicCyclic" || str == "DeterministicCyclic")
	{
		return BinarySourceMode::DeterministicCyclic;
	}
	else if (str == "BinarySourceMode::DeterministicAppendZeros" || str == "DeterministicAppendZeros")
	{
		return BinarySourceMode::DeterministicAppendZeros;
	}
	else if (str == "BinarySourceMode::AsciiFileCyclic" || str == "AsciiFileCyclic")
	{
		return BinarySourceMode::AsciiFileCyclic;
	}
	else if (str=="BinarySourceMode::AsciiFileAppendZeros" || str == "AsciiFileAppendZeros")
	{
		return BinarySourceMode::AsciiFileAppendZeros;
	}
	else
		return BinarySourceMode::PseudoRandom;
}