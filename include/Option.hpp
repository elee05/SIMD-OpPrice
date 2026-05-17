#ifndef OPTION_HPP
#define OPTION_HPP





struct OptionParams {
    double S;      // spot price
    double K;      // strike
    double r;      // risk-free rate
    double sigma;  // volatility
    double T;      // time to expiry
    bool   is_call;
};

#endif // OPTION_HPP