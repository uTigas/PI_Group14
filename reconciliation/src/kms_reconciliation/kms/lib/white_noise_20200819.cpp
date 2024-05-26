# include "white_noise_20200819.h"

void WhiteNoise::initialize(void){

	//firstTime = false;
	outputSignals[0]->setSamplingPeriod(samplingPeriod);
	outputSignals[0]->setSymbolPeriod(symbolPeriod);
		 
	noisePower = unilateralNoiseSpectralDensity_W_per_Hz * 1 / samplingPeriod;

	switch (getSeedType()) {
	case SeedType::RandomDevice:
		{
			// Resorts to 624 32-bit random ints from a random device to
			// initialize each generator.

			std::generate_n(seed_data.data(), seed_data.size(), std::ref(randomDevice));
			std::seed_seq seq1(begin(seed_data), end(seed_data));
			generator1.seed(seq1);

			std::generate_n(seed_data.data(), seed_data.size(), std::ref(randomDevice));
			std::seed_seq seq2(begin(seed_data), end(seed_data));
			generator2.seed(seq2);

			std::generate_n(seed_data.data(), seed_data.size(), std::ref(randomDevice));
			std::seed_seq seq3(begin(seed_data), end(seed_data));
			generator3.seed(seq3);

			std::generate_n(seed_data.data(), seed_data.size(), std::ref(randomDevice));
			std::seed_seq seq4(begin(seed_data), end(seed_data));
			generator4.seed(seq4);

			break;
		}
		case SeedType::DefaultDeterministic:
		{
			// Initial default seed values, decided by human randomness.
			// The generator needs to be seeded even by default,
			// otherwise generators from this block may end up being correlated.
			// Not recommended when using several noise blocks.

			std::seed_seq seq1{ 17171717 };
			generator1.seed(seq1);

			std::seed_seq seq2{ 27315883 };
			generator2.seed(seq2);

			std::seed_seq seq3{ 4387621 };
			generator3.seed(seq3);

			std::seed_seq seq4{ 91827631 };
			generator4.seed(seq4);

			break;
		}
		case SeedType::SingleSelected:
		{
			// Initial implementation for user determined seeding,
			// using sequential values to avoid the need to input 4 seeds.
			// The sequences should not be correlated, although quality
			// of the noise may not be great due to lack of entropy in
			// initializing the generators.

			std::seed_seq seq1{seed};
			generator1.seed(seq1); 

			std::seed_seq seq2{seed+1};
			generator2.seed(seq2);

			std::seed_seq seq3{seed+2};
			generator3.seed(seq3);

			std::seed_seq seq4{seed+3};
			generator4.seed(seq4);
			break;
		}
};
	
//	outputSignals[0]->setSamplingPeriod(samplingPeriod);
//	outputSignals[0]->setSymbolPeriod(symbolPeriod);

}


bool WhiteNoise::runBlock(void){

	int space = outputSignals[0]->space();
	
	int process = space;

	if (process == 0) return false;
	
	signal_value_type sType = outputSignals[0]->getValueType();
	switch (sType)
	{
	case signal_value_type::t_real:
	{
					  std::normal_distribution<double> distributionNoise(0, 1);
					  for (int i = 0; i < process; i++)
					  {
						  t_real output = 0;
						  if (noisePower!=0)
						  {
							  t_real noise = distributionNoise(generator1);
							  //output = noise*sqrt(noiseVariance);
							  output = noise * sqrt(noisePower);
						  }
						  outputSignals[0]->bufferPut(output);
					  }
					  break;
	}
	case signal_value_type::t_complex:
	{
						 std::normal_distribution<double> distributionNoiseI(0, 1);
						 std::normal_distribution<double> distributionNoiseQ(0, 1);
						 for (int i = 0; i < process; i++) {

							 t_complex output(0, 0);

							 if (noisePower != 0)
							 {
								 t_real noiseI = distributionNoiseI(generator1);
								 t_real noiseQ = distributionNoiseQ(generator2);
								 output = t_complex(noiseI*std::sqrt(noisePower/2), noiseQ*std::sqrt(noisePower/2));
							 }

							 outputSignals[0]->bufferPut(output);
						 }

						 break;
	}
	case signal_value_type::t_complex_xy:
	{
						   std::normal_distribution<double> distributionNoiseIX(0, 1);
						   std::normal_distribution<double> distributionNoiseQX(0, 1);
						   std::normal_distribution<double> distributionNoiseIY(0, 1);
						   std::normal_distribution<double> distributionNoiseQY(0, 1);

						   for (int i = 0; i < process; i++) {

							   t_complex_xy output = { 0, 0 };

							   if (noisePower != 0)
							   {
								   t_real noiseIX = distributionNoiseIX(generator1);
								   t_real noiseQX = distributionNoiseQX(generator2);
								   output.x = t_complex(noiseIX*(std::sqrt(noisePower / 2)), noiseQX*(std::sqrt(noisePower / 2)));
								   t_real noiseIY = distributionNoiseIY(generator3);
								   t_real noiseQY = distributionNoiseQY(generator4);
								   output.y = t_complex(noiseIY*std::sqrt(noisePower / 2), noiseQY*std::sqrt(noisePower / 2));

							   }

							   outputSignals[0]->bufferPut(output);
						   }
						   break;
	}
	}
	
	return true;
}
