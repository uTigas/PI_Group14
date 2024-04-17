# ifndef OPTICAL_HYBRID_H_
# define OPTICAL_HYBRID_H_

# include "netxpto_20200819.h"


// Implements a Balanced BeamSplitter
class OpticalHybrid : public Block {

public:

	//##############################################################################################################

	OpticalHybrid(std::initializer_list<Signal*> InputSig, std::initializer_list<Signal*> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

private:

	t_complex div = 1 / 2;
	t_complex unit = 1;

	std::complex<t_real> imaginary=sqrt(-1);
	std::array<std::complex<double>, 8> matrix = { { 1 / 2, 1 / 2, 1 / 2, -1 / 2, 1 / 2, sqrt(-1) / 2, 1 / 2, -sqrt(-1) / 2 } };
	
	void setTransferMatrix(std::array<std::complex<double>, 8> TransferMatrix) { matrix = TransferMatrix; }
	std::array<std::complex<double>, 8> const getTransferMatrix(void) { return matrix; }

};


#endif // !PROGRAM_INCLUDE_OPTICAL_HYBRID_H_
