#include <iostream>
#include "Option.hpp"

#include <cmath>


double norm_cdf(double x) {
    return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}  

double compute_d1(const OptionParams& params) {
    double sigma_sqrt_T = params.sigma * std::sqrt(params.T);
    return (std::log(params.S / params.K) + (params.r + 0.5 * params.sigma * params.sigma) * params.T) 
           / sigma_sqrt_T;
}
double black_scholes(const OptionParams& params) {
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
    double d1 = compute_d1(params);
    double d2 = d1 - sigma_sqrt_T;

    if (params.is_call) {
        return params.S * norm_cdf(d1) 
             - params.K * std::exp(-params.r * params.T) * norm_cdf(d2);
    } else {
        return params.K * std::exp(-params.r * params.T) * norm_cdf(-d2) 
             - params.S * norm_cdf(-d1);
    }
}

// GREEKS

// Delta — exact formula, not numerical
double delta_call(OptionParams p) {
    double d1 = compute_d1(p);
    return norm_cdf(d1);           // ∂C/∂S = N(d1)
}
double delta_put(OptionParams p) {
    double d1 = compute_d1(p);
    return norm_cdf(d1) - 1.0;    // ∂P/∂S = N(d1) - 1
}

double delta_fd(OptionParams p, double h = 0.01) {
    OptionParams up = p, down = p;
    up.S   += h;
    down.S -= h;
    return (black_scholes(up) - black_scholes(down)) / (2 * h);
    // central difference — more accurate than one-sided
}

double norm_pdf(double x) {
    return (1.0 / std::sqrt(2.0 * M_PI)) * std::exp(-0.5 * x * x);
}

double gamma(OptionParams p) {
    double d1 = compute_d1(p);
    return norm_pdf(d1) / (p.S * p.sigma * sqrt(p.T));
}
double gamma_fd(OptionParams p, double h = 0.01) {
    OptionParams up = p, down = p;
    up.S   += h;
    down.S -= h;
    delta_call(up);   // ∂C/∂S at S+h
    delta_call(down); // ∂C/∂S at S-h   
    return (delta_call(up) - delta_call(down)) / (2 * h);
    // central difference — more accurate than one-sided
}

double vega(OptionParams p) {
    double d1 = compute_d1(p);
    return p.S * norm_pdf(d1) * sqrt(p.T);
}
double vega_fd(OptionParams p, double h = 0.01) {
    OptionParams up = p, down = p;
    up.sigma   += h;
    down.sigma -= h;
    return (black_scholes(up) - black_scholes(down)) / (2 * h);
    // central difference — more accurate than one-sided
}



double theta_fd(OptionParams p, double h = 0.01) {
    OptionParams up = p, down = p;
    up.T   += h;
    down.T -= h;
    return (black_scholes(up) - black_scholes(down)) / (2 * h);
    // central difference — more accurate than one-sided
}

double rho_fd(OptionParams p, double h = 0.01) {
    OptionParams up = p, down = p;
    up.S   += h;
    down.S -= h;
    return (black_scholes(up) - black_scholes(down)) / (2 * h);
    // central difference — more accurate than one-sided
}



int main() {
    OptionParams call{100.0, 100.0, 0.05, 0.2, 1.0, true};
    OptionParams put{100.0, 100.0, 0.05, 0.2, 1.0, false};



    std::cout << "Call price:  " << black_scholes(call) << '\n';
    std::cout << "Put price:   " << black_scholes(put)  << '\n';

    std::cout << "Call delta (exact):  " << delta_call(call) << '\n';
    std::cout << "Call delta (FD):     " << delta_fd(call) << '\n';
    std::cout << "Gamma:              " << gamma(call) << '\n';
    std::cout << "Gamma (FD):         " << gamma_fd(call) << '\n';
    std::cout << "Vega:               " << vega(call)  << '\n';

    std::cout << "Vega (FD):          " << vega_fd(call) << '\n';
    std::cout << "Theta (FD):         " << theta_fd(call) << '\n';
    std::cout << "Rho (FD):           " << rho_fd(call) << '\n';
    return 0;
}