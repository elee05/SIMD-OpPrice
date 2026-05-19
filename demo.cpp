#include "simd_price.hpp"
#include <iostream>
#include <numeric>
#include <cmath>
#include <vector>
#include "Option.hpp"
#include <sleef.h>

#include <immintrin.h>



int main() {
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
        std::cout << "Option " << i << " S: " << book.S[i] <<
        " K: " << book.K[i] << " r: " << book.r[i] << " sigma: " << book.sigma[i] <<
         " T: " << book.T[i] <<   " price: " << results[i] << std::endl;
    }
    


    return 0;
}