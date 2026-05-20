#include <omp.h>
#include <vector>
#include <iostream>

#include <numeric>
#include <cmath>
#include <vector>
#include <immintrin.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <random>

int main(int argc, char** argv) {
    int N = (argc > 1) ? std::atoi(argv[1]) : 1'000'000;
    if (N % 4 != 0) N -= (N % 4);  // round down to multiple of 4

    std::vector<double> array(N, 0.0);
    for (int i = 0; i < N; i += 4) {
        __m256d vec  = _mm256_loadu_pd(array.data() + i);
    }
    // #pragma omp parallel for
    // for (int i = 0; i < N; i++) {
    //     array[i] += 5.0;
    // }
    std::cout<< "size of array: " << array.size() << std::endl;
    std::cout << "array[0] = " << array[0] << ", array[N-1] = " << array[N-1] << "\n";
    return 0;
}