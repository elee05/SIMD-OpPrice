#include "simd_price.hpp"
#include <iostream>
#include <numeric>
#include <cmath>
#include <vector>
#include "Option.hpp"
#include <sleef.h>

#include <immintrin.h>

void price_book_avx2(
    const OptionBook& book,
    double* results,
    int n
) {
    for (int i = 0; i < n; i += 4) {
        // load 4 options' parameters into AVX registers per loop and run the pricing logic in parallel
        __m256d S     = _mm256_load_pd(&book.S[i]);
        __m256d K     = _mm256_load_pd(&book.K[i]);
        __m256d r     = _mm256_load_pd(&book.r[i]);
        __m256d sigma = _mm256_load_pd(&book.sigma[i]);
        __m256d T     = _mm256_load_pd(&book.T[i]);

        __m256d log_SK    = Sleef_logf8_u10(x)(S / K);          // ln(S/K) × 4
        __m256d sigma_sq  = sigma * sigma;
        __m256d sqrt_T    = Sleef_sqrtf8_u10(x)(T);
        __m256d sigma_sqT = sigma * sqrt_T;

        __m256d d1 = (log_SK + (r + 0.5 * sigma_sq) * T) / sigma_sqT;
        __m256d d2 = d1 - sigma_sqT;

        __m256d Nd1 = norm_cdf_pd(d1);
        __m256d Nd2 = norm_cdf_pd(d2);

        __m256d discount = exp_pd(-r * T);
        __m256d price    = S * Nd1 - K * discount * Nd2; // call price formula 

        // store 4 results at once
        _mm256_store_pd(&results[i], price);

    }
}

int main() {
    // Program starts here
    return 0;
}