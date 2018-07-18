#pragma once

#include <string>

class Options {
public:

	static void ReadParameters(int argc, char const *argv[]);

	static const std::string InputFileName();
	static const std::string InputFileType();
	static const std::string OutputFileName();
	static const std::string OutputType();
	static const int dBFilter();

private:
	static std::string _intputFile;
	static std::string _inputType;
	
	static std::string _outputFile;
	static std::string _outputType;

	static int _dbFilter;

};