#ifndef MC_HPP
#define MC_HPP

#pragma once
#include <cstdint>
#include "Option.hpp"

double monte_carlo_price(
    OptionParams params,
    int num_paths,
    int num_steps,
    uint64_t seed
);

#endif // MC_HPP