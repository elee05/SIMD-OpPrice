#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include "Option.hpp"
#include "mc.hpp"

// Tolerances
//
// MC_CONVERGENCE_TOL: loose tolerance for "MC ≈ BS" tests. With ~200k paths
// the standard error on a vanilla price is roughly 0.02–0.05; we set tol
// to ~4 SEs so flakes are vanishingly rare but real bugs still trip it.
static constexpr double MC_CONVERGENCE_TOL = 0.10;
static constexpr int    N_PATHS            = 200'000;
static constexpr int    N_STEPS            = 50;
static constexpr uint64_t SEED             = 42;

// 1. Reproducibility: same seed + same inputs => identical output.
TEST(MonteCarlo, DeterministicForFixedSeed) {
    OptionParams p(100, 100, 0.05, 0.20, 1.0, true);
    double a = monte_carlo_price(p, 10'000, 50, SEED);
    double b = monte_carlo_price(p, 10'000, 50, SEED);
    EXPECT_DOUBLE_EQ(a, b);
}

// 2. Different seeds should generally produce different prices.
TEST(MonteCarlo, DifferentSeedsDiffer) {
    OptionParams p(100, 100, 0.05, 0.20, 1.0, true);
    double a = monte_carlo_price(p, 10'000, 50, 1);
    double b = monte_carlo_price(p, 10'000, 50, 2);
    EXPECT_NE(a, b);
}

// 3. Convergence to analytic Black-Scholes across the moneyness spectrum.
TEST(MonteCarlo, ConvergesToBlackScholes) {
    std::vector<OptionParams> cases = {
        {100, 100, 0.05, 0.20, 1.0, true},   // ATM call
        {100, 100, 0.05, 0.20, 1.0, false},  // ATM put
        {110,  90, 0.03, 0.30, 0.5, true},   // ITM call
        { 90, 110, 0.03, 0.30, 0.5, false},  // ITM put
        { 80, 100, 0.05, 0.25, 1.0, true},   // OTM call
        {120, 100, 0.05, 0.25, 1.0, false},  // OTM put
    };

    for (auto& p : cases) {
        double mc = monte_carlo_price(p, N_PATHS, N_STEPS, SEED);
        double bs = black_scholes(p);
        EXPECT_NEAR(mc, bs, MC_CONVERGENCE_TOL)
            << "MC=" << mc << " BS=" << bs
            << " S=" << p.S << " K=" << p.K
            << " is_call=" << p.is_call;
    }
}

// 4. Put-call parity on MC outputs.
//
// Parity C - P = S - K*exp(-rT) is a population-mean identity. With the same
// seed both legs use identical paths, but the sample mean of S_T still
// differs from its true expectation by MC noise, so parity only holds up to
// MC error unless we apply a martingale correction (rescaling S_T so the
// sample mean matches the forward). This impl doesn't, so tolerance is loose.
// The test still catches payoff sign bugs.
TEST(MonteCarlo, PutCallParity) {
    std::vector<OptionParams> cases = {
        {100, 100, 0.05, 0.20, 1.0, true},
        {110,  90, 0.03, 0.30, 0.5, true},
        { 50, 100, 0.05, 0.50, 2.0, true},
    };

    for (auto p : cases) {
        p.is_call = true;  double call = monte_carlo_price(p, N_PATHS, N_STEPS, SEED);
        p.is_call = false; double put  = monte_carlo_price(p, N_PATHS, N_STEPS, SEED);

        double lhs = call - put;
        double rhs = p.S - p.K * std::exp(-p.r * p.T);

        EXPECT_NEAR(lhs, rhs, MC_CONVERGENCE_TOL)
            << "Parity violated: C-P=" << lhs << " S-Ke^{-rT}=" << rhs;
    }
}

// 5. Zero-volatility edge case: payoff is deterministic. The simulated
// price should equal the discounted intrinsic of the forward.
TEST(MonteCarlo, ZeroVolatilityIsDeterministic) {
    OptionParams p(100, 90, 0.05, 0.0, 1.0, true);
    double mc = monte_carlo_price(p, 1'000, 10, SEED);

    double forward     = p.S * std::exp(p.r * p.T);
    double expected    = std::exp(-p.r * p.T) * std::max(forward - p.K, 0.0);

    EXPECT_NEAR(mc, expected, 1e-10);
}

// 6. For European vanillas under GBM, the SDE has an exact log-normal
// solution, so num_steps=1 should give the same distribution as num_steps=N.
// With a fixed seed the draws differ, but both should converge to BS.
TEST(MonteCarlo, SingleStepEqualsMultiStepInDistribution) {
    OptionParams p(100, 100, 0.05, 0.20, 1.0, true);
    double bs       = black_scholes(p);
    double mc_one   = monte_carlo_price(p, N_PATHS, 1,    SEED);
    double mc_many  = monte_carlo_price(p, N_PATHS, 100,  SEED);

    EXPECT_NEAR(mc_one,  bs, MC_CONVERGENCE_TOL);
    EXPECT_NEAR(mc_many, bs, MC_CONVERGENCE_TOL);
}