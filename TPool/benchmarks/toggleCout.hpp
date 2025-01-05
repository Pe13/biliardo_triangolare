//
// Created by paolo on 27/08/2024.
//

#ifndef BILIARDO_TRIANGOLARE_TOGGLECOUT_HPP
#define BILIARDO_TRIANGOLARE_TOGGLECOUT_HPP

#include <iostream>

#include "benchmark/benchmark.h"

static void disableCout([[maybe_unused]] const benchmark::State &state) {
  std::cout.setstate(std::ios_base::failbit);
}

static void enableCout([[maybe_unused]] const benchmark::State &state) { std::cout.clear(); }

#endif  // BILIARDO_TRIANGOLARE_TOGGLECOUT_HPP
