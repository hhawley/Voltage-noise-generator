#pragma once

#include <string.h>
#include <cmath>

class List2D {
public:

	List2D(const unsigned int& length) : _length(length), _copy(false) {
		_vals = new double[2*_length];
	}

	List2D(double* vals, const unsigned int& length) : _length(length), _vals(vals), _copy(true) {}

	~List2D() {
		if(!_copy) {
			delete [] _vals;
		}
	}

	double* get_X() {
		return _vals;
	}

	double* get_Y() {
		return &_vals[_length];
	}

	const unsigned int get_Length() {
		return _length;
	}

	void clear() {
		memset(_vals, 0, 2*sizeof(double)*_length);
	}

private:

	unsigned int _length;
	double* _vals;
	bool _copy;
	
};

class ListMath {
public:

	// Math constants
	static constexpr double Pi() {
		return std::atan(1)*4;
	}

	static constexpr double Pi2() {
		return Pi()*2.0;
	}

	static constexpr double PiHalf() {
		return Pi()/2.0;
	}
	// End Math constants

	// Basic Math functions
	static double Max(double* list, unsigned int size);
	static double Min(double* list, unsigned int size);
	// End Basic

	// Integration functions
	static List2D* int2dSquare(List2D& list);
	static List2D* int2dTrapezoidal(List2D& list);

	static double intMonteCarlo(List2D& list);
	// End Integration

	// FFT related functions
	static List2D* fft(List2D& list);
	static List2D* invFft(List2D& list);
	// End FFT

	static void addSineGeneral(double*, const unsigned int& numpts, const double& dt, 
		const double& amplitude, const double& w, const double& phase);
	static double* sineGeneral(const unsigned int& numpts, const double& dt, 
		const double& amplitude, const double& w, const double& phase);
	static List2D* sineListGeneral(const unsigned int& numpts, const double& dt, 
		const double& amplitude, const double& w, const double& phase);
	static double* sineFast(const unsigned int& numpts, const double& dt, 
		const double& amplitude, const double& w, const double& phase);
	static List2D* sineListFast(const unsigned int& numpts, const double& dt, 
		const double& amplitude, const double& w, const double& phase);
	static double* sinePrecision(const unsigned int& numpts, const double& dt, 
		const double& amplitude, const double& w, const double& phase);
	static List2D* sineListPrecision(const unsigned int& numpts, const double& dt, 
		const double& amplitude, const double& w, const double& phase);

	// static T* derivate_2d(T* x, T* y);

private:

};