#include "ListMath.h"

double ListMath::Max(double* list, unsigned int size) {
	double maxVal = 0.0;
	for(unsigned int i = 0; i < size; i++){
		if(list[i] > maxVal) {
			maxVal = list[i];
		}
	}

	return maxVal;
}

double ListMath::Min(double* list, unsigned int size) {
	double minVal = 0.0;
	for(unsigned int i = 0; i < size; i++){
		if(list[i] < minVal) {
			minVal = list[i];
		}
	}

	return minVal;
}

List2D* ListMath::int2dSquare(List2D& list) {

	List2D* outputList = new List2D(list.get_Length()-1);
	outputList->clear();
	for(unsigned int i = 0; i < list.get_Length()-1; i++) {

		double& x0 = list.get_X()[i];
		double& x1 = list.get_X()[i+1];
		double& y = list.get_Y()[i];

		outputList->get_Y()[i] = (x1-x0)*y;
		outputList->get_X()[i] = (x1+x0)/2.0; 

	}

	return outputList;

}

List2D* ListMath::int2dTrapezoidal(List2D& list) {

	List2D* outputList = new List2D(list.get_Length()-1);
	outputList->clear();
	
	#pragma omp parallel for
	for(unsigned int i = 0; i < list.get_Length()-1; i++) {

		double& x0 = list.get_X()[i];
		double& y0 = list.get_Y()[i];
		double& x1 = list.get_X()[i+1];
		double& y1 = list.get_Y()[i+1];

		outputList->get_Y()[i] = (x1-x0)*(y1+y0)/2.0;
		outputList->get_X()[i] = (x1+x0)/2.0; 

	}

	return outputList;

}

double ListMath::intMonteCarlo(List2D& list) {

	throw "Not implemented, yet";
	return 0.0;

}

List2D* ListMath::fft(List2D& list) {

	List2D* output_list_ptr = nullptr;
	throw "Not implemented, yet";
	return output_list_ptr;

}

List2D* ListMath::invFft(List2D& list) {

	List2D* output_list_ptr = nullptr;
	throw "Not implemented, yet";
	return output_list_ptr;

}



void ListMath::addSineGeneral(double* in, const unsigned int& numpts, const double& dt, 
		const double& amplitude, const double& w, const double& phase, const unsigned int& freq_selc) {

	unsigned int time_divs_freqs = static_cast<unsigned int>(Pi2() / (dt*w));

	if(time_divs_freqs > freq_selc) {
		double* sineWaveTable = new double[time_divs_freqs];

		#pragma omp parallel for
		for(unsigned int t = 0; t < time_divs_freqs; t++) {
			sineWaveTable[t] = amplitude*sin(w*(t*dt)+phase);
		}

		#pragma omp parallel for
		for(unsigned int t = 0; t < numpts; t++) {
			in[t] += sineWaveTable[t % time_divs_freqs];
		}

		delete [] sineWaveTable;

	} else {
		#pragma omp parallel for
		for(unsigned int t = 0; t < numpts; t++) {
			in[t] += amplitude*sin(w*(t*dt)+phase);
		}
	}

}

void ListMath::addSineFast(double* in, const unsigned int& numpts, const double& dt, 
		const double& amplitude, const double& w, const double& phase) {

	unsigned int time_divs_freqs = static_cast<unsigned int>(Pi2() / (dt*w));
	double* sineWaveTable = new double[time_divs_freqs];

	#pragma omp parallel for
	for(unsigned int t = 0; t < time_divs_freqs; t++) {
		sineWaveTable[t] = amplitude*sin(w*(t*dt)+phase);
	}

	#pragma omp parallel for
	for(unsigned int t = 0; t < numpts; t++) {
		in[t] += sineWaveTable[t % time_divs_freqs];
	}

	delete [] sineWaveTable;


}

void ListMath::addSinePrecision(double* in, const unsigned int& numpts, const double& dt, 
		const double& amplitude, const double& w, const double& phase) {

	#pragma omp parallel for
	for(unsigned int t = 0; t < numpts; t++) {
		in[t] += amplitude*sin(w*(t*dt)+phase);
	}
}


// Combines sine Fast and sine Precision. It just does some previous calculations
// to estimate which one is better
double* ListMath::sineGeneral(const unsigned int& numpts, const double& dt, 
	const double& amplitude, const double& w, const double& phase) {

	unsigned int time_divs_freqs = static_cast<unsigned int>(Pi2() / (dt*w));

	double* sineWave = nullptr;
	if(time_divs_freqs > 20) {
		sineWave = sineFast(numpts, dt, amplitude, w, phase);
	} else {
		sineWave = sinePrecision(numpts, dt, amplitude, w, phase);
	}

	return sineWave;

}

List2D* ListMath::sineListGeneral(const unsigned int& numpts, const double& dt, 
	const double& amplitude, const double& w, const double& phase) {

	unsigned int time_divs_freqs = static_cast<unsigned int>(1.0 / (dt*w));

	List2D* sineWave = nullptr;
	if(time_divs_freqs > 20) {
		sineWave = sineListFast(numpts, dt, amplitude, w, phase);
	} else {
		sineWave = sineListPrecision(numpts, dt, amplitude, w, phase);
	}

	return sineWave;

}

// sine Fast works by creating a table of one period of the sinewave
// then copying it to the output array.
// It is not precise (if numpts isnt exactly a multiple of the period points,
// there is some errors at the end of the sinewave), but it is really fast.
double* ListMath::sineFast(const unsigned int& numpts, const double& dt, 
	const double& amplitude, const double& w, const double& phase) {

	unsigned int time_divs_freqs = static_cast<unsigned int>(Pi2() / (dt*w));
	double* sineWaveTable = new double[time_divs_freqs];

	#pragma omp parallel for
	for(unsigned int t = 0; t < time_divs_freqs; t++) {
		sineWaveTable[t] = amplitude*sin(w*(t*dt)+phase);
	}

	double* sineWave = new double[numpts];
	#pragma omp parallel for
	for(unsigned int t = 0; t < numpts; t++) {
		sineWave[t] = sineWaveTable[t % time_divs_freqs];
	}

	delete [] sineWaveTable;
	return sineWave;

}

List2D* ListMath::sineListFast(const unsigned int& numpts, const double& dt, 
	const double& amplitude, const double& w, const double& phase) {

	unsigned int time_divs_freqs = static_cast<unsigned int>(Pi2() / (dt*w));
	double* sineWaveTable = new double[time_divs_freqs];

	for(unsigned int t = 0; t < time_divs_freqs; t++) {
		sineWaveTable[t] = amplitude*sin(w*(t*dt)+phase);
	}

	List2D* sineWave = new List2D(numpts);
	for(unsigned int t = 0; t < numpts; t++) {
		sineWave->get_X()[t] = sineWaveTable[t % time_divs_freqs];
		sineWave->get_Y()[t] = t*dt;
	}

	delete [] sineWaveTable;
	return sineWave;

}

// Contrary to sine Fast, it just calculates the sine for all points.
// Much slower, but no errors
double* ListMath::sinePrecision(const unsigned int& numpts, const double& dt, 
	const double& amplitude, const double& w, const double& phase) {

	double* sineWave = new double[numpts];

	#pragma omp parallel for
	for(unsigned int t = 0; t < numpts; t++) {
		sineWave[t] = amplitude*sin(w*(t*dt)+phase);
	}

	return sineWave;

}

List2D* ListMath::sineListPrecision(const unsigned int& numpts, const double& dt, 
	const double& amplitude, const double& w, const double& phase) {

	List2D* sineWave = new List2D(numpts);

	for(unsigned int t = 0; t < numpts; t++) {
		sineWave->get_Y()[t] = amplitude*sin(w*(t*dt)+phase);
		sineWave->get_X()[t] = t*dt;
	}

	return sineWave;

}

