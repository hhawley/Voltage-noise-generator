#include "Options.h"

#include <exception>
#include <iostream>

std::string Options::_intputFile = "../Noise Spectras/noiseSpectra.csv";
std::string Options::_inputType = "csv";
std::string Options::_outputFile = "noise-fast-example.csv";;
std::string Options::_outputType = "csv";
int Options::_dbFilter = -80;

void Options::ReadParameters(int argc, char const *argv[]) {

	if(argc <= 1) {
		return;
	}

	bool error = false;
	std::string errMsg = "";
	for(int i = 1; i < argc; i += 2) {
		std::string option(argv[i]);

		error = i+1 >= argc;

		if(error){
			if(option == "-o") {
				errMsg = "Output option is not included"; 
			} else if (option == "-ot") {
				errMsg =  "Output extension is not included."; 
			}else if (option == "-i") {
				errMsg =  "Input option is not included"; 
			} else if(option == "-it") {
				errMsg = "Input extension is not included"; 
			}else if(option == "-f") {
				errMsg = "dB filter option was given with no option";
			}

			throw errMsg;
		} else {

			std::string option_param = argv[i+1];

			if(option == "-o") {
				_outputFile = option_param;
			} else if (option == "-ot") {
				_outputType = option_param;
			}else if (option == "-i") {
				_intputFile = option_param; 
			} else if(option == "-it") {
				_inputType = option_param;
			}else if(option == "-f") {

				int filterdB = std::stoi(option_param);

				if(filterdB >= 0) { throw "dB value cannot be bigger or equal than 0."; }
				_dbFilter = filterdB;
			}
		}
	}
}

const std::string Options::InputFileName() {
	return _intputFile;
}

const std::string Options::InputFileType() {
	return _inputType;
}

const std::string Options::OutputFileName() {
	return _outputFile;
}

const std::string Options::OutputType() {
	return _outputType;
}

const int Options::dBFilter() {
	return _dbFilter;
}
