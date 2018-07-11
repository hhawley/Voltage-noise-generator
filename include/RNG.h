/*
*	Name: Static Random Number Generator
*	Description: manages RNG with C++ functions
*/

#pragma once

#include <random>
#include <memory>

using namespace std;
	
typedef unique_ptr<uniform_real_distribution<double>> Real_RNG;
typedef unique_ptr<uniform_int_distribution<int>> Int_RNG;
typedef unique_ptr<normal_distribution<double>> Real_Normal_RNG;
	
class RNG {
private:

	static unsigned int _seed;
	static mt19937 _RNGBase;
	static Real_RNG _floatGen;
	static Int_RNG _intGen;
	static Real_Normal_RNG _normalFloatGen;
 
public:

	static void Initialize();
	static void Clear();
	
	static int getInt();
	static int getInt(const int&);
	static int getInt(const int&, const int&);
	static void setMaxValue(const int&);
	
	static double getDouble();
	static double getDouble(const double&);
	static double getDouble(const double&, const double&);

	static double getDoubleFloat();
	static void setSigma(const double& sigma);
	
	static unsigned int getSeed();

protected:
	
};
	
	
