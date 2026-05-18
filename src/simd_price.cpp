#include "simd_price.hpp"
#include <iostream>
#include <numeric>
#include <cmath>
#include <vector>

#include <immintrin.h>

void price_book_avx2(
    const OptionBook& book,
    double* results,
    int n
) {
    for (int i = 0; i < n; i += 4) {
        // load 4 options worth of data simultaneously
        __m256d S     = _mm256_load_pd(&book.S[i]);
        __m256d K     = _mm256_load_pd(&book.K[i]);
        __m256d r     = _mm256_load_pd(&book.r[i]);
        __m256d sigma = _mm256_load_pd(&book.sigma[i]);
        __m256d T     = _mm256_load_pd(&book.T[i]);

    }
}

int main() {
    // Program starts here
    return 0;
}