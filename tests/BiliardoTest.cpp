//
// Created by paolo on 14/06/2023.
//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cmath>
#include <iostream>

#include "Biliardo.hpp"
#include "doctest.h"
// #include <chrono>
// #include <random>

TEST_CASE("Checking that symmetry matrices work well") {
  bt::Biliardo biliardo(std::sqrt(3), 2, 1);

  Eigen::Vector2d v{std::cos(M_PI / 3), std::sin(M_PI / 3)};

  v = biliardo.getTopMatrix() * v;

  CHECK(v(0) == doctest::Approx(std::cos(M_PI/3)));
  CHECK(v(1) == doctest::Approx(std::sin(M_PI/3)));
}