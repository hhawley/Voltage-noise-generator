# Electrical Noise Generator

This project is a simple C++ program that allows to transform the noise spectral densities from a comma separated file, csv (or any char separated file), created from SPICE simulations (for example, LTSpice) to a voltage-time csv.

In this git everything is available to run the code in your client with no additional code or data. Using an example noise spectral density (NSD) found in `Noise Spectras` called `noiseSpectra.csv`.

To compile you can run on Linux:
```bash
mkdir bin
cd bin
cmake ../
make
```

On Windows, you'll need to install CMake and MinGW then run the CMD on the folder:
```bash
mkdir bin
cd bin
cmake -G "MinGW Makefiles" ../ 
mingw32-make.exe
```
Make sure MinGW is in the PATH variable.

## Usage
To run the program just type in the console:

    NOISEGEN [OUTPUT_PATH] [INPUT_PATH]

If no OUTPUT_PATH and INPUT_PATH was included, it will try to use the default NSD found in `Noise Spectras` and it will output a csv file named `noise-example.csv` where the executable was found.


## Test the output

There is an included python script, `Compare.py`, that reads the output voltage-time file, and the NSD and output the variance for both files. It will also estimate the NSD from the voltage-time file and graph it along side the original NSD. Finally, it also plots the voltage-time signal.

It requires `pandas`, `matplotlib`, and `numpy` to work.

Example of a graph output from the script:
![graph_example](Example_Graph_Output.png)

## Errors in the simulation

The simulation uses a sinewave table method which includes some errors which can be significant (higher than 1%) because of some non-linearities created from the sine wave table imperfections (it does not start where it ends). However, this is a good precision sacrifice as it can reduce the simulation time by 10. If you require very precise simulation, it is possible to change the source code to use the precise sinewave function found in `ListMath.cpp`.

There is an option found in `debug_main.cpp` found in `line 60` in a function called
`setFilterReductiondB`. It takes a dB value, and then calculates which would be the minimum amplitude of a sinewave allowed, essentially filtering these values from the simulation. In my example, I use a filter than removes any amplitudes lower than `-80dB` in respect to maximum value found in the NSD. It helps improving the simulation the higher it is (closer to 0), but decreases precision.

## TODO:

* Read and write to SQL/NoSQL databases.
* Optimize (use OpenCL, perhaps?)
* Add an option to choose between speed and precision.
