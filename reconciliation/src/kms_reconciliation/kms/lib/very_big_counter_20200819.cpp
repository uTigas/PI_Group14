#include "very_big_counter_20200819.h"

void VeryBigCounter::increment(void) {
//	m_numberVector[0]++;
	lowestPart++;
	if (lowestPart >= containerSize) {
		lowestPart = lowestPart-containerSize;
		for (auto i = 1; i <= m_numberVector.size(); i++) {
			if (i == m_numberVector.size()) {
				m_numberVector.push_back(1);
				return;
			}
			else {
				m_numberVector[i]++;
				if (m_numberVector[i] == containerSize) {
					m_numberVector[i] = 0;
				}
				else {
					return;
				}
			}
		}
	}
}

double VeryBigCounter::getDouble(void) {
	double number = 0;
	m_numberVector[0] = lowestPart;

	for (auto i = 0; i < m_numberVector.size(); i++) {
		number = number + m_numberVector[i] * pow(containerSize,i);
	}
	return number;
}

void VeryBigCounter::sum(const int& t_secondNumber) // sums numbers up to the int limit
{
	m_numberVector[0] = lowestPart;
	int carryOver = 0;
	
	int64_t tmpNum;

	tmpNum = static_cast<int64_t>(m_numberVector[0]) + static_cast<int64_t>(t_secondNumber);

	m_numberVector[0] = (tmpNum % containerSize);
	lowestPart = m_numberVector[0];
	carryOver = static_cast<int>(tmpNum / static_cast<int64_t>(containerSize));
	if (m_numberVector.size() >= 2) {
		m_numberVector[1] = m_numberVector[1] + carryOver;
	}
	else {
		m_numberVector.push_back(carryOver);
	}
	if (m_numberVector[1] >= containerSize) {
		m_numberVector[1] = m_numberVector[1] - containerSize;
		for (auto i = 2; i <= m_numberVector.size(); i++) {
			if (i == m_numberVector.size()) {
				m_numberVector.push_back(1);
				return;
			} else {
				m_numberVector[i]++;
				if (m_numberVector[i] == containerSize) {
					m_numberVector[i] = 0;
				}
				else {
					return;
				}
			}
		}
	}

}




/*	//Lacks verification, not really needed
std::vector<int> VeryBigCounter::convertToVector(const int64_t t_number)
{
	// Assuming initial input isn't bigger than an int64_t supports
	std::vector<int> numberVector{};
	int currentRemainder = 0;
	int64_t currentNumber = t_number;
	int64_t currentContainerSize;
	int64_t level = 1;

	bool deconstructingNumber = true;

	while (deconstructingNumber) {
		int64_t currentContainerSize = pow(containerSize, level);
		currentRemainder = currentNumber % currentContainerSize;
		numberVector.push_back(currentRemainder);
		currentNumber = currentNumber - currentRemainder;
		0 == currentNumber ? deconstructingNumber = false : level++;
	}
	return numberVector;
}
*/

/*	// Not finished
void VeryBigCounter::sum(const int64_t t_secondNumber) {
	std::vector<int> vectorSecondNumber = convertToVector(t_secondNumber);
	int div = 0;
	int rem = 0;
	for (auto i = 0; i < m_numberVector.size(); i++) {
		m_numberVector[i] = m_numberVector[i] + vectorSecondNumber[i];
		div = floor(m_numberVector[i] / containerSize);
		rem = m_numberVector[i] % containerSize;
		if (div > 0) {
			if (i + (size_t)1 == m_numberVector.size()) {
				m_numberVector.push_back(divpow(containerSize, level));
			}
			else {
				m_numberVector[i + (size_t)1] = m_numberVector[i + (size_t)1] + div;
			}
		}
		m_numberVector[i] = rem;
	}
}*/
