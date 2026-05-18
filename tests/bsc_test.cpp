#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include "Option.hpp"

// tolerance constants
static constexpr double PRICE_TOL  = 1e-6;   // analytic vs reference
static constexpr double GREEK_TOL  = 1e-4;   // analytic vs FD
static constexpr double PARITY_TOL = 1e-10;  // put-call parity is exact

TEST(BlackScholes, PutCallParity) {
    std::vector<OptionParams> cases = {
        {100, 100, 0.05, 0.20, 1.0, true},  // standard case
        {110,  90, 0.03, 0.30, 0.5, true},  // ITM call
        { 50, 100, 0.05, 0.50, 2.0, false}, // OTM put
        {100, 100, 0.00, 0.20, 1.0, true},  // zero rate
        {100, 100, 0.05, 0.20, 0.01, true}, // near expiry
    };

    for (auto p : cases) {
        p.is_call = true;  double call = black_scholes(p);
        p.is_call = false; double put  = black_scholes(p);

        double lhs = call - put;
        double rhs = p.S - p.K * exp(-p.r * p.T);

        EXPECT_NEAR(lhs, rhs, PARITY_TOL)
            << "Parity violated: C-P=" << lhs << " S-Ke^{-rT}=" << rhs;
    }
}