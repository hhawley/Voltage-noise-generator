#pragma once

#include <ios>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <memory>

#include <sys/stat.h>


template <class T>
class DataFrame {
protected:
	unsigned int _num_cols;
	std::string* _cols;
	unsigned int _length;
	T* _data;

	bool _copy;

private:
	unsigned int __read_num_rows(char*, unsigned int);
	unsigned int __read_num_cols(char*, unsigned int);



public:
	DataFrame();
	DataFrame(const std::string&, const char& del);
	~DataFrame();

	bool from_csv(const std::string&, const char& del = ',');
	bool to_csv(const std::string&, const char& del = ',', const int& precision = 9);

	bool from_array(T*, std::string*, const unsigned int&, const unsigned int&);

	void print();

	const unsigned int getNumCols();
	const unsigned int getLength();
	const std::string* getCols();

	T* operator[](const std::string&);

};

template <class T>
const unsigned int DataFrame<T>::getNumCols() {
	return _num_cols;
}

template <class T>
const unsigned int DataFrame<T>::getLength() {
	return _length;
}

template <class T>
const std::string* DataFrame<T>::getCols() {
	return _cols;
}

template <class T>
DataFrame<T>::DataFrame() : _num_cols(0), _cols(nullptr), _length(0), _data(nullptr), _copy(false) {

}

template <class T>
DataFrame<T>::DataFrame(const std::string& path, const char& del) : _num_cols(0), _cols(nullptr), _length(0), _data(nullptr), _copy(false) {
	from_csv(path, del);
}

template <class T>
DataFrame<T>::~DataFrame() {

	if(!_copy) {
		if(_cols != nullptr) {
			delete [] _cols;
		}
	
		if(_data != nullptr) {
			delete[] _data;
		}
	}
}

// Optimizations break this function creating undefined behavior
template <class T>
bool __attribute__((optimize("O0"))) DataFrame<T>::from_csv(const std::string& path, const char& del) {

	struct stat stat_buf;
	int rc = stat(path.c_str(), &stat_buf);

	if(rc != 0) {
		return false;
	}

	unsigned int size = stat_buf.st_size;

	char* buffer = new char[size + 1];
	buffer[size] = '\0';

	std::ifstream _input_file(path, std::ifstream::binary | std::ifstream::in);

	if(!_input_file.is_open()) {
		throw "Failed to read file";
	}
	
	_input_file.read(buffer, size);
	_input_file.close();

	unsigned int rows = __read_num_rows(buffer, size);
	unsigned int cols = __read_num_cols(buffer, size);

	_length = rows;
	_num_cols = cols;

	_cols = new std::string[_num_cols];
	_data = new T[_num_cols*_length];

	unsigned int index = 0, curr_elem = 0, curr_col = 0, start_index=0;
	bool ar = true;
	while(index < size) {
		if(buffer[index] != '\n') {
			if(buffer[index] == del) {
				char* last_word = &buffer[start_index];

				if(start_index != index) {
					std::string val_s(last_word, index-start_index);

					if(!ar) {
						double val_d = stod(val_s);
						T val_t = static_cast<T>(val_d);

						_data[curr_col*_length + (curr_elem-1)] = val_t;
					}
				}

				start_index = index+1;
				curr_col++;
			} else if(ar) {
				_cols[curr_col] += buffer[index];
			}

			
		} else {
			char* last_word = &buffer[start_index];
			std::string val_s(last_word, index-start_index);

			if( start_index != index) {
				std::string val_s(last_word, index-start_index);

				if(!ar) {
					double val_d = stod(val_s);
					T val_t = static_cast<T>(val_d);

					_data[curr_col*_length + (curr_elem-1)] = val_t;

				}
			}			

			curr_elem++;
			curr_col = 0;
			ar = false;
			start_index = index+1;
		}

		index++;
		
	}

	char* last_word = &buffer[start_index];

	if(start_index != size) {
		std::string val_s(last_word, size-start_index);

		if(!ar) {
			double val_d = stod(val_s);
			T val_t = static_cast<T>(val_d);

			_data[curr_col*_length + (curr_elem-1)] = val_t;

		}
	}

	delete [] buffer;

	return true;

}

template <class T>
bool __attribute__((optimize("O0"))) DataFrame<T>::to_csv(const std::string& name, const char& del, const int& precision) {
	if(_num_cols <= 0){
		return false;
	}

	std::ofstream output_file(name);

	if(output_file.fail()) {
		return false;
	}

	// Filesize total worst case scenario:
	// Is a float/double with precision named "precision"
	// Each num is 6 + precision long,
	// Times the length and columns
	// Plus the number of del which is number of columns - 1 
	// Plus the \n for each line which is equal to the length
	int numCharsIfNumber = 6 + precision; // The constant is from one point, 3 exponents digits, the e and +.
	long int fileSize = _length * _num_cols * numCharsIfNumber + (_num_cols - 1) + _length;

	if(fileSize > 2e9) {
		throw "File too big";
	}
	
	output_file << std::scientific << std::setprecision(precision);
	std::string _buffer;

	for(unsigned int col = 0; col < _num_cols; col++) {

		if(col == _num_cols - 1) {
			_buffer += _cols[col] + "\n";
			
		} else if(col < _num_cols - 1) {
			_buffer += _cols[col] + del;
		}
	}
	
	for(unsigned int row = 0; row < _length; row++) {
		for(unsigned int col = 0; col < _num_cols; col++) {

			std::ostringstream tmp_stream;
			tmp_stream << std::scientific << std::setprecision(precision);
			tmp_stream << _data[col*_length + row];

			if(col == _num_cols - 1) {
				_buffer += tmp_stream.str() + "\n";
				
			} else if(col < _num_cols - 1) {
				_buffer += tmp_stream.str() + del;
			}

			

			// If file size bigger than 100MB save directly file.
			if(_buffer.size() >= 100e6) {
				output_file << _buffer;
				_buffer.clear();
			}

		}
	}

	if(_buffer.size() <= 100e6) {
		output_file << _buffer;
		
	}


	output_file.close();
	return true;
}

template <class T>
bool DataFrame<T>::from_array(T* data, std::string* colsNames, const unsigned int& length, const unsigned int& cols) {

	_copy = true;
	if(data == nullptr || length == 0 || cols == 0) {
		return false;
	}

	_cols = colsNames;
	_data = data;
	_length = length;
	_num_cols = cols;
	return true;

}

template <class T>
T* DataFrame<T>::operator[](const std::string& col) {
	for(unsigned int x =0; x < _num_cols; x++) {
		if(_cols[x] == col) {
			return &_data[x*_length];
		}
	}

	return nullptr;
}

template <class T>
unsigned int DataFrame<T>::__read_num_rows(char* buffer, unsigned int size) {

	unsigned int rows = 1;
	for (unsigned int i = 0; i < size; ++i)
	{	
		if(buffer[i] == '\n') {
			rows++;
		}
	}

	// First row doesnt count, its the columns names
	return rows-1;
}

template <class T>
unsigned int DataFrame<T>::__read_num_cols(char* buffer, unsigned int size) {

	unsigned int cols = 1;
	for(unsigned int i = 0; i < size; ++i) {

		if(buffer[i] == '\n') {
			return cols;
		} else if(buffer[i] == ',') {
			cols++;
		}

	}

	return cols;

}

template <class T>
void DataFrame<T>::print() {

	for(unsigned int col = 0; col < _num_cols; col++) {
		if(col == _num_cols - 1) {
			std::cout << _cols[col] << std::endl;
		} else if(col < _num_cols) {
			std::cout << _cols[col] << ", ";
		}
	}

	if(_length < 10) {
		
		for(unsigned int row = 0; row < _length; row++) {
			for(unsigned int col = 0; col < _num_cols; col++) {
	
				if(col == _num_cols - 1) {
					std::cout << _data[col*_length + row] << std::endl;  
				} else if(col < _num_cols - 1) {
					std::cout << _data[col*_length + row] << ", "; 
				}
	
			}
		}
	
		std::cout << std::endl;
	} else {
		for(unsigned int row = 0; row < 10; row++) {
			for(unsigned int col = 0; col < _num_cols; col++) {
	
				if(col == _num_cols - 1) {
					std::cout << _data[col*_length + row] << std::endl;  
				} else if(col < _num_cols - 1) {
					std::cout << _data[col*_length + row] << ", "; 
				}
	
			}
		}

		for(unsigned int row = _length-10; row < _length; row++) {
			for(unsigned int col = 0; col < _num_cols; col++) {
	
				if(col == _num_cols - 1) {
					std::cout << _data[col*_length + row] << std::endl;  
				} else if(col < _num_cols - 1) {
					std::cout << _data[col*_length + row] << ", "; 
				}
	
			}
		}
	}

}

