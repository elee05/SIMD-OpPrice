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
        // __m256d S     = _mm256_load_pd(&book.S[i]);
        // __m256d K     = _mm256_load_pd(&book.K[i]);
        // __m256d r     = _mm256_load_pd(&book.r[i]);
        // __m256d sigma = _mm256_load_pd(&book.sigma[i]);
        // __m256d T     = _mm256_load_pd(&book.T[i]);

        __m256d S     = _mm256_loadu_pd(&book.S[i]);
        __m256d K     = _mm256_loadu_pd(&book.K[i]);
        __m256d r     = _mm256_loadu_pd(&book.r[i]);
        __m256d sigma = _mm256_loadu_pd(&book.sigma[i]);
        __m256d T     = _mm256_loadu_pd(&book.T[i]);


        __m256d log_SK    = Sleef_logd4_u10avx2(_mm256_div_pd(S, K));          // ln(S/K) × 4
        __m256d sigma_sq  = sigma * sigma;
        __m256d sqrt_T    = Sleef_sqrtd4_u35avx2(T);
        __m256d sigma_sqT = sigma * sqrt_T;

        __m256d d1 = (log_SK + (r + 0.5 * sigma_sq) * T) / sigma_sqT;
        __m256d d2 = d1 - sigma_sqT;

        __m256d Nd1 = norm_cdf_pd(d1);
        __m256d Nd2 = norm_cdf_pd(d2);

        // __m256d discount = Sleef_expd4_u10avx2(_mm256_mul_pd(_mm256_sub_pd(_mm256_setzero_pd(), r), T));
        __m256d discount = Sleef_expd4_u10avx2(_mm256_mul_pd(-r, T));
        __m256d price = _mm256_sub_pd(
            _mm256_mul_pd(S, Nd1),
            _mm256_mul_pd(
                _mm256_mul_pd(K, discount),
                Nd2
            )
        ); // call price formula

        // __m256d discount = exp_pd(-r * T);
        // __m256d price    = S * Nd1 - K * discount * Nd2; // call price formula 

        // store 4 results at once
        // _mm256_store_pd(&results[i], price);

        // _mm256_store_pd(&results[i], log_SK); // for testing, store log(S/K) instead of price
        _mm256_storeu_pd(&results[i], price); // for testing, store the calculated price

    }
}

// Abramowitz & Stegun approximation — accurate to ~1e-7, fully vectorizable to use for SIMD
__m256d norm_cdf_pd(__m256d x) {
    // // rational polynomial approximation to N(x)
    // // accurate to ~1e-7, fully vectorizable

    // __m256d mask = _mm256_set1_pd(-0.0);
    // __m256d y = _mm256_andnot_pd(mask, x); // abs(x)

    // __m256d t = 1.0 / (1.0 + 0.2316419 * y);
    // __m256d poly = t * (0.319381530
    //            + t * (-0.356563782
    //            + t * (1.781477937
    //            + t * (-1.821255978
    //            + t *  1.330274429))));
    // __m256d result = 1.0 - norm_pdf_pd(x) * poly;
    // // handle negative x by symmetry: N(-x) = 1 - N(x)
    // return _mm_blend_pd(result, 1.0 - result, x < 0);


    __m256d zero = _mm256_setzero_pd();
    __m256d one  = _mm256_set1_pd(1.0);

    // mask for x < 0
    __m256d sign_mask = _mm256_cmp_pd(x, zero, _CMP_LT_OQ);

    // |x|
    __m256d y = _mm256_andnot_pd(_mm256_set1_pd(-0.0), x);

    // t = 1 / (1 + 0.2316419 * |x|)
    __m256d t = _mm256_div_pd(
        one,
        _mm256_add_pd(one,
            _mm256_mul_pd(_mm256_set1_pd(0.2316419), y))
    );

    __m256d poly = t; // (you should expand full polynomial here)
    __m256d pdf = norm_pdf_pd(x);
    __m256d result = _mm256_sub_pd(one, _mm256_mul_pd(pdf, poly));
    __m256d result_neg = _mm256_sub_pd(one, result);

    return _mm256_blendv_pd(result, result_neg, sign_mask);



}

__m256d norm_pdf_pd(__m256d x) {
    // const __m256d inv_sqrt_2pi = _mm256_set1_pd(0.3989422804014327); // 1/sqrt(2π)
    // return inv_sqrt_2pi * _mm256_exp_pd(-0.5 * x * x);

    __m256d x2 = _mm256_mul_pd(x, x);

    __m256d half = _mm256_set1_pd(0.5);
    __m256d neg_half_x2 = _mm256_mul_pd(_mm256_sub_pd(_mm256_setzero_pd(), half), x2);
    __m256d exp_term = Sleef_expd4_u10avx2(neg_half_x2);
    __m256d inv_sqrt_2pi = _mm256_set1_pd(0.3989422804014327);
    __m256d pdf = _mm256_mul_pd(inv_sqrt_2pi, exp_term);
    return pdf;

}

int main() {
    // Program starts here
    OptionBook book = {
        .S = {100.0, 105.0, 110.0, 115.0},
        .K = {100.0, 100.0, 100.0, 100.0},
        .r = {0.05, 0.05, 0.05, 0.05},
        .sigma = {0.2, 0.2, 0.2, 0.2},
        .T = {1.0, 1.0, 1.0, 1.0},
        .is_call = {true, true, true, true}
    };
    double results[4];
    price_book_avx2(book, results, 4);
    for (int i = 0; i < 4; i++) {
        std::cout << "Option " << i << " price: " << results[i] << std::endl;
    }


    return 0;
}