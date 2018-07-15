#pragma once

#include "ListMath.h"

class ListStatistics {
public:
	// statistics functions
	static double meanList(List2D& list);
	static double meanIntervals(List2D& list);

	static double stdList(List2D& list);
	static double stdIntervals(List2D& list);

	static double cdf_standard_dist(const double& x, const double& mean, const double& std);
	static double cdf_chisqr_dist(const double& x, const unsigned int& n);
	static double cdf_tstudent_dist(const double& x, const unsigned int& n);

	static double inv_cdf_standard_dist(const double& alpha, const double& mean, const double& std);
	static double inv_cdf_chisqr_dist(const double& alpa, const unsigned int& n);
	static double inv_cdf_tstudent_dist(const double& alpa, const unsigned int& n);

	//end functions


};