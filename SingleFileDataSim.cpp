// g++ SingleFileDataSim.cpp RNG.cpp -O3 -lpthread -fopenmp -Wall -o FFTNoise.exe
//g++ ../SingleFileDataSim.cpp ../src/RNG.cpp -O3 -lpthread -fopenmp -Wall -o FFTNoise.exe -I ../include/

#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>

#include <fstream>
#include <vector>
#include <omp.h>
#include <memory>
#include <sstream>
#include <time.h>
#include "RNG.h"

#define pow2(x) x*x

const double signal_trig_val = 0.010;

double minFrequency = 1;
double maxFrequency = 10000;
double dt = 1/(100*maxFrequency) ;
double totalTime = 1/minFrequency;
unsigned int numPoints = int(totalTime/dt);
unsigned int numSamples = 1000000;

bool save = true;
int tries = 0;

double* voltageTimeArray;
// double pulsesTempArray[numSamples][3] = {0};

std::vector<double> voltages;
std::vector<double> frequencies;

std::ofstream outputFile;
std::ifstream inputFFT;

double getMax(double* data, unsigned int size) {
	double maxVal = 0.0;
	for(unsigned int i = 0; i < size; i++){
		if(data[i] > maxVal) {
			maxVal = data[i];
		}
	}

	return maxVal;
}

inline double sinsin(double w, double phase, double t) {
	return sin((w*t)+phase);
}

int main(int argc, char const *argv[])
{
	time_t timer0, timer1;
	RNG::Initialize();

	switch(argc) {
		case 2:
			if(argv[1][0] != 0) {
				inputFFT.open("../Noise Spectras/noiseSpectra.csv");
				outputFile.open(std::string(argv[1]));
			}
			break;

		case 3:
			if(argv[1][0] != 0) {
				outputFile.open(std::string(argv[1]));
			}
			if(argv[2][0] != 0) {
				inputFFT.open(std::string(argv[2]));
			}
			break;

		default:
			outputFile.open("../output/noise-sing-precision-example.csv");
			inputFFT.open("../Noise Spectras/noiseSpectra.csv", std::ios::binary | std::ios::in);
			break;
	}

	if(inputFFT.fail()) {
		std::cout << "Failed to open FFT file." << std::endl;
		return -1;
	}

	if(outputFile.fail()) {
		std::cout << "Failed to open output file." << std::endl;
	}

	double freq, powerValue;
	char del;
	int  i = 0;
	while(inputFFT >> freq >> del >> powerValue && del == ',') {
		voltages.push_back(powerValue);
		frequencies.push_back(freq);
		i++;
	}

	if(i == 0 || voltages.size() == 0) { 
		std::cout << "File contains no data points or incorrect format." << std::endl;
		return -1;
	 }

	std::cout << "Finished reading data file with " 
		<< frequencies.size()-1 << " points." << std::endl;


	time(&timer0);
	// Voltage vs time array
	
	unsigned int NPSSize = frequencies.size()-1;
	double* NPSVoltages = new double[NPSSize];
	double* NPSFreqs = new double[NPSSize];

	// This for loop is a kind of "integration" of the file which has units of
	// V / sqrt(Hz) in the y axis, and Hz in the x axis.
	#pragma omp parallel for
	for(unsigned int i=1; i < frequencies.size(); i++) {
		unsigned int j = i-1;
		freq = frequencies[i];

		// sigmaV^2 = 0.5*(v[n]^2+v[n-1]^2)*df
		// amplitude = sqrt(2 sigmaV^2) = sqrt((v[n]^2+v[n-1]^2*df))
		NPSVoltages[j] = sqrt((pow2(voltages[i])+pow2(voltages[j])*(freq-frequencies[j])));

		// w = (w1 + w2) / 2 = (2 pi f1 + 2 pi f2) / 2 = pi (f1 + f2)
		NPSFreqs[j] = M_PI*(freq + frequencies[j]);
	}

	auto maxNPSAmplitude = getMax(NPSVoltages, NPSSize);
	auto maxNPSFrequency = NPSFreqs[NPSSize - 1];
	std::cout << maxNPSAmplitude << std::endl;
	std::cout << maxNPSFrequency << std::endl;

		// Create variables necessary for the simulation/calculations
	dt = 2.0*M_PI/(2.0*NPSFreqs[NPSSize-1]);
	totalTime = 4.0*M_PI/NPSFreqs[0];
	numPoints = static_cast<int>(totalTime/dt);
	RNG::setMaxValue(numPoints);

	voltageTimeArray = new double[numPoints];

	std::cout << "Time jumps: " << dt << 's' << std::endl;
	std::cout << "Total time: " << totalTime << "s" << std::endl;
	std::cout << "Total number of points: " << numPoints << std::endl;

	double amplitude = 0.0, phase_shift = 0.0;
	do {
		#pragma omp parallel for
		for(unsigned int time_index = 0; time_index < numPoints; time_index++) {
			voltageTimeArray[time_index] = 0;
		}

		// This gets the amplitude from a normal distribution with sigma = NPSFreqs[i]
		// then saved in an array one period of the resulting waveform
		for(unsigned int i = 0; i < NPSSize; i++) {
			freq = NPSFreqs[i];


			RNG::setSigma(NPSVoltages[i]);
			amplitude = RNG::getDoubleFloat();
			phase_shift = M_PI_2*RNG::getDouble();

			unsigned int time_divs_freq = static_cast<unsigned int>(2.0*M_PI/(dt*freq));
			// If the number of time divisions required for the frequency is bigger than 0,
			// we should save one period of the sine wave to an array, and just repeat that
			// similar to a look-up table approach
			// otherwise, just calculate it for the time array/
			// if(time_divs_freq > 20) {

			// 	double* sineWaveBuffer = new double[time_divs_freq];
			// 	#pragma omp parallel for
			// 	for(unsigned int time_index = 0; time_index < time_divs_freq; time_index++) {
			// 		sineWaveBuffer[time_index] = amplitude*sinsin(freq, phase_shift, time_index*dt);
			// 		// sineWaveBuffer[time_index] = amplitude*sinsin(2*M_PI*10000, 0, time_index*dt);
			// 	}

			// 	#pragma omp parallel for
			// 	for(unsigned int time_index = 0; time_index < numPoints; time_index++) {
			// 		voltageTimeArray[time_index] += sineWaveBuffer[time_index % time_divs_freq];
			// 	}

			// 	delete [] sineWaveBuffer;

			// } else {
				#pragma omp parallel for
				for(unsigned int time_index = 0; time_index < numPoints; time_index++) {
					voltageTimeArray[time_index] += amplitude*sinsin(freq, phase_shift, time_index*dt);
				}
			// }

		}

		if(!save) {
			#pragma omp parallel for
			for(unsigned int i = 0; i < numPoints; i++ ){
				if(voltageTimeArray[i] > signal_trig_val) {
					save = true;
				}
			}

			tries++;

			if(tries % 10 == 0){
				std::cout << "Tries so far: " << tries << std::endl;
			}
		}

	} while(!save);

	time(&timer1);

	double secs = difftime(timer1, timer0);
	std::cout << "Lasted: " << secs << " seconds." << std::endl;

	std::string fileData = "time,voltage\n";

	for(unsigned int i = 0; i < numPoints; i++ ){
		std::ostringstream streamObj, timeObj;
		streamObj << voltageTimeArray[i];
		timeObj << i*dt;
		fileData += timeObj.str() + "," + streamObj.str() +  "\n";

		if(fileData.size() > 100e6) {
			outputFile << fileData;
			fileData.clear();
		}
	}

	outputFile << fileData;
	std::cout << "Finished writing to output file" << std::endl;

	delete [] voltageTimeArray;
	delete [] NPSVoltages;
	delete [] NPSFreqs;
	outputFile.close();
	inputFFT.close();
	RNG::Clear();
	return 0;
}
