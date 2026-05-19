# Calculating Option Prices Using Same Instruction Multiple Data Computation

## Relvant Files
- benchmark.cpp
- demo.cpp

## Important Libraries
- sleef.h
- immintrin.h

## Running(Linux)
 ```g++ -O3 -march=native -mavx2 -mfma     -Iinclude -I/usr/local/include     src/simd_price.cpp benchmark.cpp -o benchmark     -L/usr/local/lib -lsleef``` \
 The goal of this project is to use SIMD to 4x the throughput of option pricing. The AVX2 library allows basic operations to be executed on 4 elements held in a vector at a time. I also use sleef to do the same for more complicated operations like taking logs. This command compares my own standard implementation of black scholes pricing calulation to one that uses the AVX2 and sleef libraries. 
 - just for calls right now
 - Compile and run demo.cpp to see example calculations and option parameters.

 ## Background
 - https://en.wikipedia.org/wiki/Single_instruction,_multiple_data
 - https://en.wikipedia.org/wiki/Advanced_Vector_Extensions
  

