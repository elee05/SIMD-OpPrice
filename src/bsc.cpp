#include <iostream>
#include "Option.hpp"

#include <cmath>


double norm_cdf(double x) {
    return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}          
double black_scholes(const OptionParams& params) {
    // double d1 = (std::log(params.S / params.K) + (params.r + 0.5 * std::pow(params.sigma, 2)) * params.T) / (params.sigma * std::sqrt(params.T));
    // double d2 = d1 - params.sigma * std::sqrt(params.T);

    // if (params.is_call) {
    //     return params.S * norm_cdf(d1) - params.K * std::exp(-params.r * params.T) * norm_cdf(d2);
    // } else {
    //     return params.K * std::exp(-params.r * params.T) * norm_cdf(-d2) - params.S * norm_cdf(-d1);
    // }

    // Input validation / edge cases
    if (params.T <= 0.0) {
        // At expiry, return intrinsic value
        return params.is_call ? std::max(params.S - params.K, 0.0)
                              : std::max(params.K - params.S, 0.0);
    }

    if (params.sigma <= 0.0) {
        // Zero volatility → deterministic
        double forward = params.S * std::exp(params.r * params.T);
        return params.is_call ? std::max(forward - params.K, 0.0) * std::exp(-params.r * params.T)
                              : std::max(params.K - forward, 0.0) * std::exp(-params.r * params.T);
    }

    double sigma_sqrt_T = params.sigma * std::sqrt(params.T);
    double d1 = (std::log(params.S / params.K) + (params.r + 0.5 * params.sigma * params.sigma) * params.T) 
                / sigma_sqrt_T;
    double d2 = d1 - sigma_sqrt_T;

    if (params.is_call) {
        return params.S * norm_cdf(d1) 
             - params.K * std::exp(-params.r * params.T) * norm_cdf(d2);
    } else {
        return params.K * std::exp(-params.r * params.T) * norm_cdf(-d2) 
             - params.S * norm_cdf(-d1);
    }
}

int main() {
    OptionParams call{100.0, 100.0, 0.05, 0.2, 1.0, true};
    OptionParams put{100.0, 100.0, 0.05, 0.2, 1.0, false};

    std::cout << "Call price:  " << black_scholes(call) << '\n';
    std::cout << "Put price:   " << black_scholes(put)  << '\n';
    return 0;
}