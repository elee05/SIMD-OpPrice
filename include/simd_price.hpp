#ifndef OPTIONBOOK_HPP
#define OPTIONBOOK_HPP

#include <vector>
#include <cmath>

// Structure of Arrays — SIMD-friendly
struct OptionBook {
    std::vector<double> S;
    std::vector<double> K;
    std::vector<double> r;
    std::vector<double> sigma;
    std::vector<double> T;
    std::vector<bool>   is_call;
};

void price_book_avx2(
    const OptionBook& book,
    double* results,
    int n
);

// Memory looks like:
// S S S S S S S S ... | K K K K K K K K ... | σ σ σ σ σ σ σ σ ...

#endif // OPTIONBOOK_HPP