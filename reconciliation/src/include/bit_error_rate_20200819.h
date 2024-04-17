# ifndef BIT_ERROR_RATE_H_
# define BIT_ERROR_RATE_H_

# include "netxpto_20200819.h"


// Calculates the BER between two binary signals
class BitErrorRate : public Block {

	
public:

	BitErrorRate(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig):Block(InputSig,OutputSig){};
	
	void initialize(void);
	bool runBlock(void);

	double coincidences = 0;
	double receivedBits = 0;

	void setConfidence(double P) { alpha = 1-P; }
	double const getConfidence(void) { return 1 - alpha; }

	void setMidReportSize(int M) { m = M; }
	int const getMidReportSize(void) { return m; }
    
    void setLowestMinorant(double lMinorant) { lowestMinorant=lMinorant; }
    double getLowestMinorant(void) { return lowestMinorant; }

	void setSkipInput0(t_integer sInput0) { skipInput0 = sInput0; }

	void setSkipInput1(t_integer sInput1) { skipInput1 = sInput1; }

private:
    
    double lowestMinorant = 1e-10;
    
	double alpha = 0.05;
	double z{ 0.0 };
	int m = 0;
	int n = 0;

	t_integer skipInput0{ 0 };
	t_integer skipInput1{ 0 };
};


#endif // !PROGRAM_INCLUDE_BIT_ERROR_RATE_H_
