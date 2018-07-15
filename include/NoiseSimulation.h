#pragma once

#include <memory>

#include "ListMath.h"

typedef std::unique_ptr<List2D> List2D_ptr;

class NoiseSimulation {

public:
	NoiseSimulation();
	~NoiseSimulation();

	void Init(List2D&);
	List2D* GenNoiseWaveFormSpeed();
	List2D* GenNoiseWaveFormPrecision();
	List2D* GenNoiseWaveForm(const int&, const unsigned int& freq_threshold=20);

	const double getFilterReduction();
	void setFilterReduction(const double&);
	void setFilterReductiondB(const double&);

private:
	List2D_ptr _amplitud_spectra;
	double* _output_signal;

	double _dt;
	double _total_time;
	unsigned int _num_points;

	double _max_amplitude;
	double _max_frequency;
	unsigned int _num_freqs;

	double _minimum_filter_reduction;

	void __to_amplitude_spectra(List2D&);
};