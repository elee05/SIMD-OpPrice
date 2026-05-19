#include "simd_price.hpp"
#include "Option.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <random>
#include <vector>

// Scalar reference implementation — same math, one option at a time.
// Uses std::erfc for the normal CDF: N(x) = 0.5 * erfc(-x / sqrt(2)).
static void price_book_scalar(const OptionBook& book, double* out, int n) {
    for (int i = 0; i < n; ++i) {
        double S = book.S[i], K = book.K[i], r = book.r[i];
        double sig = book.sigma[i], T = book.T[i];

        double sqrtT = std::sqrt(T);
        double d1 = (std::log(S / K) + (r + 0.5 * sig * sig) * T) / (sig * sqrtT);
        double d2 = d1 - sig * sqrtT;

        double Nd1 = 0.5 * std::erfc(-d1 * M_SQRT1_2);
        double Nd2 = 0.5 * std::erfc(-d2 * M_SQRT1_2);

        out[i] = S * Nd1 - K * std::exp(-r * T) * Nd2;
    }
}

// random book. N must be a multiple of 4 for the AVX2 path.
struct Book {
    std::vector<double> S, K, r, sigma, T;
    std::vector<bool>   is_call;  // unused by pricer but matches struct
    int n;
};

static OptionBook make_random_book(int n, unsigned seed = 42) {
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> uS(50.0, 200.0);
    std::uniform_real_distribution<double> uK(50.0, 200.0);
    std::uniform_real_distribution<double> ur(0.0, 0.10);
    std::uniform_real_distribution<double> us(0.10, 0.60);
    std::uniform_real_distribution<double> uT(0.05, 2.00);
 
    OptionBook b;
    b.S.resize(n); b.K.resize(n); b.r.resize(n);
    b.sigma.resize(n); b.T.resize(n); b.is_call.assign(n, true);
    for (int i = 0; i < n; ++i) {
        b.S[i]     = uS(rng);
        b.K[i]     = uK(rng);
        b.r[i]     = ur(rng);
        b.sigma[i] = us(rng);
        b.T[i]     = uT(rng);
    }
    return b;
}

// Run the AVX2 pricer over a Book by feeding it 4-wide chunks.
static void run_avx2(const Book& b, double* out) {
    OptionBook chunk{};
    for (int i = 0; i < b.n; i += 4) {
        for (int j = 0; j < 4; ++j) {
            chunk.S[j]     = b.S[i + j];
            chunk.K[j]     = b.K[i + j];
            chunk.r[j]     = b.r[i + j];
            chunk.sigma[j] = b.sigma[i + j];
            chunk.T[j]     = b.T[i + j];
            chunk.is_call[j] = true;
        }
        price_book_avx2(chunk, out + i, 4);
    }
}

static void run_scalar(const Book& b, double* out) {
    OptionBook chunk{};
    for (int i = 0; i < b.n; i += 4) {
        for (int j = 0; j < 4; ++j) {
            chunk.S[j]     = b.S[i + j];
            chunk.K[j]     = b.K[i + j];
            chunk.r[j]     = b.r[i + j];
            chunk.sigma[j] = b.sigma[i + j];
            chunk.T[j]     = b.T[i + j];
        }
        price_book_scalar(chunk, out + i, 4);
    }
}


template <typename F>
static double time_best_ns(F&& fn, int warmup = 3, int reps = 20) {
    using clock = std::chrono::steady_clock;
    for (int i = 0; i < warmup; ++i) fn();

    double best = std::numeric_limits<double>::max();
    for (int i = 0; i < reps; ++i) {
        auto t0 = clock::now();
        fn();
        auto t1 = clock::now();
        double ns = std::chrono::duration<double, std::nano>(t1 - t0).count();
        best = std::min(best, ns);
    }
    return best;
}

int main(int argc, char** argv) {
    int N = (argc > 1) ? std::atoi(argv[1]) : 1'000'000;
    if (N % 4 != 0) N -= (N % 4);  // round down to multiple of 4
 
    std::printf("Workload: N = %d options\n\n", N);
 
    OptionBook book = make_random_book(N);
    std::vector<double> out_scalar(N), out_avx2(N);
 
    // Time both. price_book_avx2 already loops i += 4 internally, so we
    // hand it the full book in one call.
    double ns_scalar = time_best_ns([&]{ price_book_scalar(book, out_scalar.data(), N); });
    double ns_avx2   = time_best_ns([&]{ price_book_avx2  (book, out_avx2  .data(), N); });
 
    // Defeat dead-code elimination: print a checksum of each output.
    double sum_scalar = 0.0, sum_avx2 = 0.0;
    for (int i = 0; i < N; ++i) { sum_scalar += out_scalar[i]; sum_avx2 += out_avx2[i]; }
 
    // Accuracy: max abs and relative error vs scalar reference.
    double max_abs = 0.0, max_rel = 0.0;
    for (int i = 0; i < N; ++i) {
        double diff = std::fabs(out_avx2[i] - out_scalar[i]);
        max_abs = std::max(max_abs, diff);
        double denom = std::max(std::fabs(out_scalar[i]), 1e-12);
        max_rel = std::max(max_rel, diff / denom);
    }
 
    auto report = [&](const char* name, double ns) {
        double ns_per = ns / N;
        double mops   = 1e3 / ns_per;
        std::printf("  %-8s %12.0f ns total  %8.2f ns/option  %8.2f M options/sec\n",
                    name, ns, ns_per, mops);
    };
 
    std::printf("Timing (best of 20 runs after warmup):\n");
    report("Scalar:", ns_scalar);
    report("AVX2:",   ns_avx2);
    std::printf("\n  Speedup: %.2fx\n\n", ns_scalar / ns_avx2);
 
    std::printf("Accuracy (AVX2 vs scalar reference):\n");
    std::printf("  Max absolute error: %.3e\n", max_abs);
    std::printf("  Max relative error: %.3e\n", max_rel);
 
    std::printf("\nChecksums (sanity):\n");
    std::printf("  Scalar sum: %.6f\n", sum_scalar);
    std::printf("  AVX2   sum: %.6f\n", sum_avx2);

    return 0;
}