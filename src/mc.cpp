#include "mc.hpp"

#include <random>
#include <cmath>
#include <algorithm>

double monte_carlo_price(
    OptionParams p,
    int num_paths,
    int num_steps,
    uint64_t seed
) {
    std::mt19937_64 rng(seed);
    std::normal_distribution<> normal(0.0, 1.0);

    double dt = p.T / num_steps;

    double drift =
        (p.r - 0.5 * p.sigma * p.sigma) * dt;

    double diffusion =
        p.sigma * std::sqrt(dt);

    double payoff_sum = 0.0;

    for (int i = 0; i < num_paths; ++i) {

        double S = p.S;

        for (int step = 0; step < num_steps; ++step) {

            double Z = normal(rng);

            S *= std::exp(
                drift + diffusion * Z
            );
        }

        double payoff;

        if (p.is_call)
            payoff = std::max(S - p.K, 0.0);
        else
            payoff = std::max(p.K - S, 0.0);

        payoff_sum += payoff;
    }

    return std::exp(-p.r * p.T)
        * payoff_sum / num_paths;
}