#ifndef VERY_BIG_COUNTER_H_
#define VERY_BIG_COUNTER_H_

#include <vector>
#include <math.h>

class VeryBigCounter {
	//	Keeps count of very large integer numbers without overflowing.
	//	Represents any supported number as a vector of integers, allowing to
	//	keep track of very small increments without overflowing or losing precision.
	//	Allows incrementing, summing small numbers (int only) and retrieving the total
	//	number as a double.

public:
	// Constructor - Always starts empty
	VeryBigCounter(void) :
		m_numberVector({ 0 })
	{};

	// Public methods
	void increment(void);
	double getDouble(void);
	void sum(const int& t_secondNumber);
	void operator++() { this->increment(); }
	// Auxiliary methods
//	std::vector<int> convertToVector(const int64_t t_number);

private:
	// Private methods

	// State variables
	std::vector<int> m_numberVector;
	int lowestPart{ 0 };
	
	// Fixed variables
	int containerSize{ 1'000'000 };	// Chosen due to base 10.
									// Setting to highest possible int improves efficiency.

};

#endif // !VERY_BIG_COUNTER_H_

