
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
#include "NoiseSimulation.h"
#include <time.h>
#include <iostream>
#include <memory>

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

	time(&timer0);
	Generator.Init(data);

	std::unique_ptr<List2D> waveForm;
	bool done_finding = false;
	unsigned int iters = 0, num = 0;
	double max = 0.0;

	do {
		waveForm.reset( Generator.GenNoiseWaveFormSpeed() );
		num = waveForm->get_Length();

		max = ListMath::Max(waveForm->get_X(), num);


		if(max >= 3*3.4681e-3) {
			std:: cout << "Rare event with a max of: " << max << std::endl;
		}

		if(max>= 5*3.4681e-3) {
			done_finding = true;
		}

		iters++;
		if(iters % 10 == 0) {
			std::cout << "Numer of iterations so far: " << iters << std::endl;
		}

	// std::unique_ptr<List2D> waveForm ( Generator.GenNoiseWaveFormSpeed() );
	// std::unique_ptr<List2D> waveForm ( Generator.GenNoiseWaveFormPrecision() );
	} while(!done_finding);

	std::cout << "Finished simulation" << std::endl;
	time(&timer1);
	double secs = difftime(timer1, timer0);
	std::cout << "Lasted: " << secs << " seconds." << std::endl;

	DataFrame<double> output_df;
	std::string cols[2] = {"voltage", "time"};
	if(!output_df.from_array(waveForm->get_X(), cols, waveForm->get_Length(), 2)) {
		// output_df.print();
		std::cout << "Failed to copy array" << std::endl;
	}

	// output_df.print();
	output_df.to_csv( Options::OutputFileName() , ',', 6);
	RNG::Clear();


	return 0;
}