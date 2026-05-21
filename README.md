# SIMD(Same Instruction Multiple Data Computation) to Speed up Option Price Calculation 

## Relvant Files
- benchmark.cpp
- demo.cpp

## Important Libraries
- AVX2 for SIMD
  - sleef
  - immintrin
- OpenMP parallization across cores
  - omp

## Running(Linux)
 ```g++ -O3 -march=native -mavx2 -mfma     -Iinclude -I/usr/local/include     src/simd_price.cpp benchmark.cpp -o benchmark     -L/usr/local/lib -lsleef``` \
 The goal of this project is to use SIMD to 4x the throughput of option pricing. The AVX2 library allows basic operations to be executed on 4 elements held in a vector at a time. I also use sleef to do the same for more complicated operations like taking logs. This command compares my own standard implementation of black scholes pricing calulation to one that uses the AVX2 and sleef libraries.  


 The OpenMP library can then be used to run multiple threads of SIMD across cores. Theoretical 64x
 Example:

 ```
 Workload: N = 1000000 options

Timing (best of 20 runs after warmup):
  Scalar:      63395699 ns total     63.40 ns/option     15.77 M options/sec
  AVX2 (1 core):     21272902 ns total     21.27 ns/option     47.01 M options/sec
  AVX2 + OpenMP:      4746179 ns total      4.75 ns/option    210.70 M options/sec

  AVX2  vs Scalar:          2.98x
  OpenMP vs Scalar:        13.36x
  OpenMP vs single-core:    4.48x  (ideal: 8x)

Accuracy vs scalar reference:
  AVX2   max abs err: 1.485e+02   max rel err: 1.485e+14
  OpenMP max abs err: 1.485e+02   max rel err: 1.485e+14
  ``` 

 
 - just for calls right now
 - a negative value corresponds to a worthless option
 - Compile and run demo.cpp to see example calculations and option parameters.

 ## Background
 - https://en.wikipedia.org/wiki/Single_instruction,_multiple_data
 - https://en.wikipedia.org/wiki/Advanced_Vector_Extensions
  

