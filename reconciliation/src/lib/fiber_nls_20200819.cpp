# include <math.h> 
# include "..\include\fiber_nls_20191209.h"


//using namespace std;
int factorial(int num);


void FiberNls::initialize(void) {

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	//outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved()); 
	
	/* preliminary calculations for parameter conversion */
	t_real L = fiberLength * 1e3;								 // Fiber length (km --> m) 
	t_real D = 1e-6*dispersionCoeficient;						 // Group - velocity dispersion parameter (ps/km/nm --> s/m^2)
	t_real S = 1e3*dispersionSlope;								 // Dispersion slope (ps/km/nm^2 --> s/m^3)
	t_real att = log(10)*attenuationCoeficient / (10*pow(10,3)); // Attenuation coefficient (dB/km to linear value per meter)
	t_real c = SPEED_OF_LIGHT;									 // speed of light (m/s)
	t_real lambda = centralWavelength * 1e-9;					 // central wavelength (nm --> m)
	t_real fc = c / lambda;										 // central frequency (Hz)
	t_real numberOfSteps = L / stepLength;				    	 // number of steps to cover full length of fiber
	int length = 2 * inputSignals[0]->getBufferLength();         // set the linear operator transfer dunction length (2*bufferLength)

	/* calculation of beta2, beta3, beta4 */
	t_real beta2 = -D * lambda*lambda / (2 * PI*c);                                              // calculate beta2 value
	t_real beta3; 
	if (useBeta3) {beta3=(lambda*lambda/((2*PI*c)*(2*PI*c)))*((lambda*lambda)*S + 2*lambda*D);}  // calculate beta3 value (if specified)
	else { beta3 = 0.0; }
	t_real beta4; 
	if (useBeta4) { beta4 = -pow(lambda, 5) / (pow(2 * PI*c, 3)) * 6 * S; }                      // calculate beta4 value (if specified)
	else { beta4 = 0.0; }

	/* angular frequency vector */
	t_real dt = inputSignals[0]->getSamplingPeriod(); // sampling period
	t_real fs = 1 / dt;								  // samplinf frequency	
	int    nt = length;                               // length of the signal block

	std::vector<int> itegerVec(nt);                  
	for (int i = 0; i < (nt / 2); i++)               // fill vector for the positive frequency (i.e. for nt = 1024, it fills (0:1:511))
		itegerVec[i] = i;
	for (int i = 0; i < nt / 2; i++)
		itegerVec[nt / 2 + i] = -nt / 2 + i;         // fill vector for the negative frequency (i.e. for nt = 1024, it fills (-512:1:0))

	vector<t_complex> w(nt);
	for (int i = 0; i < nt; i++)
		w[i] = 2 * PI*itegerVec[i] / (dt*(double)nt);


	/* Calculate the attenuation + dispersion operator --> operatorD */
	vector<t_real> operatorD_real(nt);
	vector<t_real> operatorD_imag(nt);
	vector<t_complex> operatorD(nt);

	for (int i = 0; i < nt; i++){
		operatorD_real[i] = -att / 2 + beta3 * pow(w[i].real(), 2) / 6.0;                             // real-part of operator D
		operatorD_imag[i] = -beta2 * pow(w[i].real(), 2) / 2.0 - beta4 * pow(w[i].real(), 4) / 24.0;  // imag-part of operator D
	}
	for (int i = 0; i < nt; i++){
		operatorD[i] = { operatorD_real[i] , operatorD_imag[i] };                                     // operator D
	}

	/* calculate the linear effect  --> exp(i*operator D) */
	vector<t_complex> exponentail_D_half(nt);
	for (int i = 0; i < nt; i++)
		exponentail_D_half[i] = exp(operatorD[i] * stepLength / (double)2);       // calculate linear effect --> exp(i*operator D)
	
	linear_effect_half_step = exponentail_D_half;

	/*ofstream b2;
	b2.open("b2.txt");
	b2.precision(15);
	for (int i = 0; i < operatorD_imag.size(); i++)
	{
		b2 << operatorD_imag[i] << endl;
	}
	b2.close();

	ofstream a2;
	a2.open("a2.txt");
	a2.precision(15);
	for (int i = 0; i < operatorD_real.size(); i++)
	{
		a2 << operatorD_real[i] << endl;
	}
	a2.close();*/

}

bool FiberNls::runBlock(void) {

	int ready = inputSignals[0]->ready();
	int space = outputSignals[0]->space();
	int process = min(ready, space);
	if (process == 0) return false;

	/* get the input signal for as a vector*/
	int block_size = linear_effect_half_step.size();

	vector<t_complex> currentCopy(process);   // Get the Input signal
	t_complex input;
	for (int i = 0; i < process; i++) {	
		inputSignals[0]->bufferGet(&input);
		currentCopy.at(i) = input;
	}

	if (K > 0)
	{

	//////////////////////// DATA BLOCK LENGTH ADJUSTMENT : START ////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////
	/* this part deals especially last transmitted signal which may or may not be same as bufferLength !
	Since our transfer function length is 2*bufferLength, it is necessary to make data length equal to
	2*bufferLength by concatenating the data. */
		if (previousCopy.size() != currentCopy.size()){
			vector<t_complex> currentCopyAux;   // it'll concatenate the currentCopy integer number of time --> currentCopyAux (output)
			while (currentCopyAux.size() <= previousCopy.size()){
				for (size_t i = 0; i < currentCopy.size(); i++){
					currentCopyAux.push_back(currentCopy[i]);
				}
			}

			vector<t_complex> cc(previousCopy.size());  // it'll truncate the currentCopyAux to the previousCopy size --> cc (output)
			for (size_t i = 0; i < previousCopy.size(); i++){
				cc[i] = currentCopyAux[i];
			}
			currentCopy = cc;
		} 

		vector<t_complex> inputSignal;
		inputSignal.reserve(previousCopy.size() + currentCopy.size());
		inputSignal.insert(inputSignal.end(), previousCopy.begin(), previousCopy.end());
		inputSignal.insert(inputSignal.end(), currentCopy.begin(), currentCopy.end());

		//////////////////////// DATA BLOCK LENGTH ADJUSTMENT : END //////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////

		/* initial variables for SSFM */
		vector<t_complex> utime = inputSignal;			 // reference copy of the input
		vector<t_complex> ufft = ifft(utime);			 // fft of the input data
		vector<t_complex> utime_save;					 // export data in-between transmission
		t_real zTotal  = fiberLength*1e3;				 // total length of fiber (m)
		t_real walkedz = 0;								 // incicates distance travelled in while loop
		t_real dz      = stepLength;					 // step length for the ssfm
		t_real saveDistanceInterval = saveInterval*1e3;  // save data after traveling given distance (m)
		t_real saveFlag = 0.0;							 // it's a flag to save data

		/* create directory to save data */
		string folder{ "signals/fiber/" };
		if (L == 1) {
			namespace fs = std::filesystem;
			fs::path dir = "signals/";
			//fs::create_directories( dir/"iteratonsOut1/2");
			std::uintmax_t n = fs::remove_all(dir / "fiber");
		}
		if (L == 1) {
			if (!filesystem::is_directory(folder) || !filesystem::exists(folder)) {
				filesystem::create_directory(folder);
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////// SSMF START ////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		vector<t_complex> uHalf(block_size); // frequency-domain data
		vector<t_complex> uhalf(block_size); // time-domain data
		vector<t_complex> nonlinear_effect(block_size);

		while (walkedz < zTotal)
		{
			// att + dispersion till length dz/2 (half of step length)
			for (int i = 0; i < block_size; i++)
				uHalf[i] = linear_effect_half_step[i] * ufft[i];      // apply linear effect in frequency-domain (1)
			uhalf = fft(uHalf);                                       // ifft

			for (int j = 0; j < block_size; j++)
				nonlinear_effect[j] = exp(complex<double>(0, -nlCoeff * (pow(abs(uhalf[j]), 2))*dz ));   // calculate nonlinearity effect --> exp(-i*operator N)
			for (int k = 0; k < block_size; k++)
				uhalf[k] = uhalf[k] * nonlinear_effect[k];              // apply nonlinear effect in time-domain (2)

			/*ofstream nlReal;
			nlReal.open("nlReal.txt");
			nlReal.precision(15);
			for (int i = 0; i < nonlinear_effect.size(); i++)
			{
				nlReal << nonlinear_effect[i].real() << endl;
			}
			nlReal.close();

			ofstream nlImag;
			nlImag.open("nlImag.txt");
			nlImag.precision(15);
			for (int i = 0; i < nonlinear_effect.size(); i++)
			{
				nlImag << nonlinear_effect[i].imag() << endl;
			}
			nlImag.close();*/

			uHalf = ifft(uhalf);                                     // fft
			for (int l = 0; l < block_size; l++)
				uHalf[l] = linear_effect_half_step[l] * uHalf[l];    // apply linear effect in frequency-domain (3)
			ufft = uHalf;                                            // for next iteration
			utime_save = fft(uHalf);								 // for saving data
			walkedz = walkedz + dz;									 // next step in ssfm
			saveFlag = saveFlag + dz;							     // data save flag  in ssfm

			///////////////////////////////////////////// SAVE SSMF DATA : START //////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/* comment this part if you don't want to save data in between transmission */
			string distance_string;
			stringstream dd;
			dd << std::setprecision(4) <<((walkedz)/1e3);
			distance_string = dd.str();
			ofstream outRef;
			ofstream outTravelled;

			if (walkedz == dz) { // save reference signal
				outRef.open(folder + "signal_after_" + "0" + "km.txt", std::ios::app);
				outRef.precision(15);

				for (int i = 0; i < process; i++) {
					outRef << inputSignal[block_size/4+i].real() << endl;
					outRef << inputSignal[block_size/4+i].imag() << endl;
				}
				outRef.close();
			}

			if (saveFlag == saveDistanceInterval){ // save signal after travelling specified distance interval
				outTravelled.open(folder + "signal_after_" + distance_string + "km.txt", std::ios::app);
				outTravelled.precision(15);

				for (int i = 0; i < process; i++) {
					outTravelled << utime_save[block_size/4+i].real() << endl;
					outTravelled << utime_save[block_size/4+i].imag() << endl;
				}
				outTravelled.close();
				saveFlag = 0.0;
			}

			///////////////////////////////////////////// SAVE SSMF DATA : END ///////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		fft(ufft);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////// SSMF END ////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		for (int i = 0; i < process; i++){
			outputSignals[0]->bufferPut(ufft[block_size/4 + i]);
		}
	}
		
	previousCopy = currentCopy;
	K = K + 1;
	L = L + 1;
	return true;
}

int factorial(int num)
{
	int fact=1;
	for (int i = num; i > 0; i--)
		fact = fact * i;
	return fact;
};

vector<t_real> freq_time(int N, double samplingPeriod)
{
	t_real dt = samplingPeriod;
	t_real fs = 1 / dt;
	vector<t_real> t(N);
	vector<t_real> f(N);

	for (int i = 0; i < N; i++){
		if (i == 0){ t[i] = 0; }
		else { t[i] = t[i - 1] + dt;}
	}

	t_real df = 1 / (dt*(double)N);

	for (int i = 0; i < N; i++){
		if (i == 0) { f[i] = -fs/2; }
		else { f[i] = f[i - 1] + df; }
	}

	return f;

}
