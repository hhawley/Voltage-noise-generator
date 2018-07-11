#include "RNG.h"

#include <chrono>

unsigned int RNG::_seed = chrono::system_clock::now().time_since_epoch().count();

mt19937 RNG::_RNGBase;
Real_RNG RNG::_floatGen = nullptr;
Int_RNG RNG::_intGen = nullptr;
Real_Normal_RNG RNG::_normalFloatGen = nullptr;

void RNG::Initialize() {
	
	_RNGBase.seed(_seed);
	_floatGen.reset(new uniform_real_distribution<double>(0.0, 1.0));
	_intGen.reset(new uniform_int_distribution<int>(0, 10000000));
	_normalFloatGen.reset(new normal_distribution<double>(0, 0.2));
	
}

void RNG::Clear() {
	_floatGen.release();
	_intGen.release();
	_normalFloatGen.release();
}

void RNG::setMaxValue(const int& max) {
	uniform_int_distribution<int>::param_type params(0, max);
	_intGen->param(params);
}

int RNG::getInt() {
	
	return _intGen->operator()(_RNGBase);
	
}

int RNG::getInt(const int& max) {
	
	return (max / 100) * _intGen->operator()(_RNGBase);
	
}

int RNG::getInt(const int& min, const int& max) {
	
	return (max / 100) * _intGen->operator()(_RNGBase) + min;
	
}

double RNG::getDouble() {
	
	return _floatGen->operator()(_RNGBase);
	
}

double RNG::getDouble(const double& max) {
	
	return (max / 100.0) * _floatGen->operator()(_RNGBase);
	
}

double RNG::getDouble(const double& min, const double& max) {
	
	return (max / 100.0) * _floatGen->operator()(_RNGBase) + min;
	
}

unsigned int RNG::getSeed() {
	
	return _seed;
	
}

double RNG::getDoubleFloat() {
	return _normalFloatGen->operator()(_RNGBase);
}

void RNG::setSigma(const double& sigma) {
	// _normalFloatGen.reset(new normal_distribution<double>(0, sigma));
	normal_distribution<double>::param_type params(0.0, sigma);
	_normalFloatGen->param(params);
	// _normalFloatGen->reset();
}