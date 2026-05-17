#ifndef OPTION_HPP
#define OPTION_HPP


struct OptionParams {
    double S;      // spot price
    double K;      // strike
    double r;      // risk-free rate
    double sigma;  // volatility
    double T;      // time to expiry
    bool   is_call;

    OptionParams(double S, double K, double r,
           double sigma, double T, bool is_call)
        : S(S), K(K), r(r),
          sigma(sigma), T(T), is_call(is_call) {}
};

#endif // OPTION_HPP