#include "ListStatistics.h"

double ListStatistics::meanList(List2D& list){

	double mean = 0.0;
	unsigned int n  =  list.get_Length();
	double* vals = list.get_X();

	#pragma omp parallel for reduction(+:mean)
	for(unsigned int i = 0; i < n; i++) {
		mean += vals[i];
	}

	mean /= n;
	return mean;

}

double ListStatistics::meanIntervals(List2D& list){
	throw "Not implemented, yet";
	return 0.0; 

}

double ListStatistics::stdList(List2D& list){
	double std = 0.0, plc_holder = 0.0;
	double mean = meanList(list);
	unsigned int n = list.get_Length();
	double* vals = list.get_X();

	#pragma omp parallel for reduction(+:std)
	for(unsigned int i = 0; i < n; i++) {
		plc_holder = (vals[i] - mean);
		std += plc_holder*plc_holder;
	}

	std /= n-1;
	return sqrt(std);

}

double ListStatistics::stdIntervals(List2D& list){

	throw "Not implemented, yet";
	return 0.0; 

}

double ListStatistics::cdf_standard_dist(const double& x, const double& mean, const double& std){

	double amp = std*sqrt(2.0);
	double plc_holder = (x-mean)/std;
	double t = plc_holder*plc_holder / 2;
	amp = 1/amp;

	return amp*erf(t)/2;

}

double ListStatistics::cdf_chisqr_dist(const double& x, const unsigned int& n){

	throw "Not implemented, yet";
	return 0.0; 
}

double  ListStatistics::cdf_tstudent_dist(const double& x, const unsigned int& n) {
	throw "Not implemented, yet";
	return 0.0; 
}

double ListStatistics::inv_cdf_standard_dist(const double& alpha, const double& mean, const double& std){

	double z = alpha*std*sqrt(ListMath::Pi2());
	double log_z = log(z);
	double q = sqrt(-2*log_z);

	return q*std+mean; 
}

double ListStatistics::inv_cdf_chisqr_dist(const double& alpa, const unsigned int& n){
	throw "Not implemented, yet";
	return 0.0; 
}

double ListStatistics::inv_cdf_tstudent_dist(const double& alpa, const unsigned int& n) {
	throw "Not implemented, yet";
	return 0.0; 
}