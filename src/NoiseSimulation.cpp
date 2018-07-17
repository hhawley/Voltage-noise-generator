#include "NoiseSimulation.h"
#include "RNG.h"

#include <iostream>

NoiseSimulation::NoiseSimulation() : _output_signal(nullptr), _dt(0), _total_time(0), 
	_num_points(0), _max_amplitude(0), _max_frequency(0), _num_freqs(0), _minimum_filter_reduction(1e3) {

}

NoiseSimulation::~NoiseSimulation() {
	if(_output_signal != nullptr) {
		delete [] _output_signal;
	}
}

void NoiseSimulation::Init(List2D& volt_spectra) {

	RNG::Initialize();
	__to_amplitude_spectra(volt_spectra);

	_num_freqs = _amplitud_spectra->get_Length();
	_max_amplitude = ListMath::Max(_amplitud_spectra->get_Y(), 
		_num_freqs);

	_max_frequency = _amplitud_spectra->get_X()[_num_freqs - 1];

	_dt = ListMath::Pi2() / (2.0*_max_frequency);
	_total_time = ListMath::Pi2() / (_amplitud_spectra->get_X()[0]);
	_num_points = static_cast<unsigned int>(_total_time/_dt);

	_output_signal = new double[2*_num_points];
	RNG::setMaxValue(_num_points);

	std::cout << "Max freq: " << _max_frequency / ListMath::Pi2() << "Hz" << std::endl;
	std::cout << "Min freq: " << _amplitud_spectra->get_X()[0] / ListMath::Pi2() << "Hz" << std::endl;

	std::cout << "Time jumps: " << _dt << 's' << std::endl;
	std::cout << "Total time: " << _total_time << "s" << std::endl;
	std::cout << "Total number of points: " << _num_points << std::endl << std::endl;


}


void NoiseSimulation::__to_amplitude_spectra(List2D& volt_spectra) {

	List2D power_spectra(volt_spectra.get_Length());
	power_spectra.clear();
	// vrms[f] = volt_spectra
	// In order to simulate a noise voltage example we need to turn
	// the noise voltage spectra to a normal voltage spectra.
	// Then we get the contribution by frequency differential for each
	// frequency and sum all the resulting waveforms together.

	// First, we turn the noise spectra to a power noise spectra by squaring
	// each value in the list
	// p[f] = vrms[f]*vrms[f]
	// p[f] has units of [Sigma_V^2 / Hz]

	// Sigma_V can mean Vrms

	// Then, we integrate p[f] using the trapezoidal method to obtain Sigma_V[f]
	// Sigma_V[f'] = Sqrt{ 0.5*(p[f]+p[f-1])*([f] - [f-1]) }
	// where f' = ([f-1]+[f])/2

	// This is the RMS voltage for a sinewave with a frequency equal to f'
	// My formulas use Vp instead of Vrms, so we turn them to Vp using
	// Vp = Sqrt[2] * Vrms

	#pragma omp parallel for
	for(unsigned int i = 0; i < volt_spectra.get_Length(); i++) {
		double& vsqured = power_spectra.get_Y()[i];
		double& fprime = power_spectra.get_X()[i];

		double& v = volt_spectra.get_Y()[i];
		double& f = volt_spectra.get_X()[i];

		fprime = f;
		vsqured = v*v; 
	}

	_amplitud_spectra.reset( 
		ListMath::int2dTrapezoidal(power_spectra) );

	double vpp = 0.0;
	for(unsigned int i = 0; i < _amplitud_spectra->get_Length(); i++) {
		double& v = _amplitud_spectra->get_Y()[i];
		double& f = _amplitud_spectra->get_X()[i];

		vpp = sqrt(2*v);
		v = vpp;
		f = ListMath::Pi2()*f;
	}

}

List2D* NoiseSimulation::GenNoiseWaveForm(const int& dbFilter, const unsigned int& freq_threshold) {

	if(_output_signal == nullptr) {
		throw "Class hasnt been initialized";
	}

	std::cout << "Starting simulation..." << std::endl;
	setFilterReductiondB(dbFilter);
	memset(_output_signal, 0, 2*sizeof(double)*_num_points);

	double* w_freqs = _amplitud_spectra->get_X();
	double* amplitudes = _amplitud_spectra->get_Y();
	double ran_amp = 0.0, phase_shift = 0.0;

	#pragma omp parallel for reduction(+:_output_signal[:_num_points])
	for(unsigned int i = 0; i < _num_freqs; i++) {

		double& curr_w = w_freqs[i];
		double& curr_amp = amplitudes[i];

		if(curr_amp < _minimum_filter_reduction) {
			continue;
		}

		RNG::setSigma(curr_amp);
		ran_amp = abs(RNG::getDoubleFloat());

		if(ran_amp < _minimum_filter_reduction) {
			continue;
		}

		phase_shift = ListMath::Pi2()*RNG::getDouble();

		ListMath::addSineGeneral(_output_signal, _num_points, _dt, ran_amp, curr_w, phase_shift, freq_threshold);

	}

	#pragma omp parallel for
	for(unsigned int time_index = 0; time_index < _num_points; time_index++) {
		_output_signal[time_index + _num_points] = time_index*_dt;
	}

	List2D* _output_waveForm = new List2D(_output_signal, _num_points);
	return _output_waveForm;

}

List2D* NoiseSimulation::GenNoiseWaveFormSpeed() {
	if(_output_signal == nullptr) {
		throw "Class hasnt been initialized";
	}

	std::cout << "Starting fast as possible simulation..." << std::endl;
	setFilterReductiondB(-60);
	memset(_output_signal, 0, 2*sizeof(double)*_num_points);

	double* w_freqs = _amplitud_spectra->get_X();
	double* amplitudes = _amplitud_spectra->get_Y();
	double ran_amp = 0.0, phase_shift = 0.0;

	#pragma omp parallel for reduction(+:_output_signal[:_num_points])
	for(unsigned int i = 0; i < _num_freqs; i++) {

		double& curr_w = w_freqs[i];
		double& curr_amp = amplitudes[i];

		if(curr_amp < _minimum_filter_reduction) {
			continue;
		}

		RNG::setSigma(curr_amp);
		ran_amp = abs(RNG::getDoubleFloat());

		if(ran_amp < _minimum_filter_reduction) {
			continue;
		}

		phase_shift = ListMath::Pi2()*RNG::getDouble();

		ListMath::addSineFast(_output_signal, _num_points, _dt, curr_amp, curr_w, phase_shift);

	}

	#pragma omp parallel for
	for(unsigned int time_index = 0; time_index < _num_points; time_index++) {
		_output_signal[time_index + _num_points] = time_index*_dt;
	}

	List2D* _output_waveForm = new List2D(_output_signal, _num_points);
	return _output_waveForm;

}

List2D* NoiseSimulation::GenNoiseWaveFormPrecision() {
	if(_output_signal == nullptr) {
		throw "Class hasnt been initialized";
	}

	std::cout << "Starting precise as possible simulation..." << std::endl;
	memset(_output_signal, 0, 2*sizeof(double)*_num_points);

	double* w_freqs = _amplitud_spectra->get_X();
	double* amplitudes = _amplitud_spectra->get_Y();
	double ran_amp = 0.0, phase_shift = 0.0;

	#pragma omp parallel for reduction(+:_output_signal[:_num_points])
	for(unsigned int i = 0; i < _num_freqs; i++) {

		double& curr_w = w_freqs[i];
		double& curr_amp = amplitudes[i];

		RNG::setSigma(curr_amp);
		ran_amp = abs(RNG::getDoubleFloat());
		phase_shift = ListMath::Pi2()*RNG::getDouble();

		ListMath::addSinePrecision(_output_signal, _num_points, _dt, ran_amp, curr_w, phase_shift);

	}

	#pragma omp parallel for
	for(unsigned int time_index = 0; time_index < _num_points; time_index++) {
		_output_signal[time_index + _num_points] = time_index*_dt;
	}

	List2D* _output_waveForm = new List2D(_output_signal, _num_points);
	return _output_waveForm;

}




const double NoiseSimulation::getFilterReduction() {
	return _minimum_filter_reduction;
}

void NoiseSimulation::setFilterReduction(const double& fR) {
	_minimum_filter_reduction = fR;
}

void NoiseSimulation::setFilterReductiondB(const double& fRdB) {
	if(_output_signal == nullptr) {
		throw "Class hasnt been initialized";
	}

	_minimum_filter_reduction = exp(fRdB/20.0)*_max_amplitude;
	std::cout << "dB filter: " << _minimum_filter_reduction << std::endl;
	std::cout << "Max Amplitude: " << _max_amplitude << std::endl;
}
