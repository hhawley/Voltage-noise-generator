cmake_minimum_required(VERSION 2.6)
project(NOISEGEN)

include_directories(include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O3 -std=c++11 -fopenmp")

set(SOURCE_FILES debug_main.cpp
src/ListMath.cpp
src/ListStatistics.cpp
src/NoiseSimulation.cpp
src/RNG.cpp


include/DataFrame.h
)

add_executable(NOISEGEN ${SOURCE_FILES})
SET(CMAKE_EXE_LINKER_FLAGS "-lpthread")