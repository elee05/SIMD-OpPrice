# Calculating Option Prices Using Same Instruction Multiple Data Computation

## Relvant Files
- benchmark.cpp
- demo.cpp

## Important Libraries
- sleef.h
- immintrin.h

## Running(Linux)
 g++ -O3 -march=native -mavx2 -mfma     -Iinclude -I/usr/local/include     src/simd_price.cpp benchmark.cpp -o benchmark     -L/usr/local/lib -lsleef

