
// go/create bin directory:
// WIN:
// cmake -G "MinGW Makefiles" ../
// mingw32-make.exe
// LINUX:
// cmake ../
// make

#include "DataFrame.h"
#include "RNG.h"
#include "ListMath.h"
#include "ListStatistics.h"
#include "NoiseSimulation.h"
#include "Options.h"

#include <time.h>
#include <iostream>
#include <memory>
#include <array>

int main(int argc, char const *argv[])
{
	NoiseSimulation Generator;
	DataFrame<double> df;
	time_t timer0, timer1;

	Options::ReadParameters(argc, argv);

	df.from_csv( Options::InputFileName() );

	
	// df.print();
	std::cout << "Finished reading data file with " 
		<< df.getLength() << " points." << std::endl;

	std::string first_col = df.getCols()[0];
	List2D data(df[first_col], df.getLength());

	Generator.Init(data);

	std::unique_ptr<List2D> waveForm;
	unsigned int iters = 0;
	DataFrame<double> output_df;
	std::string cols[2] = {"voltage", "time"};


	const unsigned int num_samples = 100;
	std::array<double, num_samples> mean_array;
	std::array<double, num_samples> mean_error_array;
	std::array<double, num_samples> sigma_array;
	std::array<double, num_samples> sigma_error_array;

	// Constants
	double t_1998_99 = 2.32653;
	double x2_1998_99 = 20512.8;
	double x2_1998_01 = 19482.7;

	time(&timer0);
	for(iters=0; iters < num_samples; iters++) {
		waveForm.reset( Generator.GenNoiseWaveForm(-80) );
		
		if(iters % 10 == 0) {
			std::cout << "Numer of iterations so far: " << iters << std::endl;
		}

	// std::unique_ptr<List2D> waveForm ( Generator.GenNoiseWaveFormSpeed() );
	// std::unique_ptr<List2D> waveForm ( Generator.GenNoiseWaveFormPrecision() );
	
		double v = waveForm->get_Length() - 1;
		double mean = ListStatistics::meanList(*waveForm);
		double std= ListStatistics::stdList(*waveForm);
		double mean_error = t_1998_99*std/sqrt(v+1);
		double std_error_01 = std*std*v/x2_1998_99;
		double std_error_02 = std*std*v/x2_1998_01;

		double std_error = sqrt(fabs(std_error_02 - std_error_01)/2.0);

		mean_array[iters] = mean;
		mean_error_array[iters] = mean_error;
		sigma_array[iters] = std;
		sigma_error_array[iters] = std_error;
		

		
	}

			if(!output_df.from_array(waveForm->get_X(), cols, waveForm->get_Length(), 2)) {
			// output_df.print();
			std::cout << "Failed to copy array" << std::endl;
		} else {
			std::string out = std::to_string(iters) + "_precision_" + Options::OutputFileName();
			output_df.to_csv( out , ',', 6);
		}

	time(&timer1);
	double secs = difftime(timer1, timer0);
	std::cout << "Lasted: " << secs << " seconds." << std::endl;

	double final_mean = 0.0, final_std = 0.0;
	double final_mean_error = 0.0, final_std_error = 0.0;
	for(unsigned int i = 0; i < num_samples; i++) {
		final_mean += mean_array[i];
		final_std += sigma_array[i];
		final_mean_error += mean_error_array[i]*mean_error_array[i];
		final_std_error += sigma_error_array[i]*sigma_error_array[i];
	}

	final_mean /= num_samples;
	final_mean_error /= num_samples;
	final_mean_error = sqrt(final_mean_error);
	std::cout << final_mean << " +- " << final_mean_error << std::endl;

	final_std /= num_samples;
	final_std_error /= num_samples;
	final_std_error = sqrt(final_std_error);
	std::cout << final_std << " +- " << final_std_error << std::endl;

// Precision:
// Lasted: 1265 seconds.
// 3.72674e-006 +- 5.69962e-005
// 0.00346285 +- 0.000556078
// Fast:
// Lasted: 95 seconds.
// 4.46481e-005 +- 5.37575e-005
// 0.00326514 +- 0.00052448
// Mixed: (-80 dB)
// Lasted: 440 seconds.
// -1.58278e-005 +- 5.72119e-005
// 0.00347545 +- 0.000558182
	
	std::cout << "Finished" << std::endl;
	RNG::Clear();


	return 0;
}