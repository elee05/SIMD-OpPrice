#pragma once

#include "option.hpp"

double monte_carlo_price(
    OptionParams params,
    int num_paths,
    int num_steps,
    uint64_t seed
);