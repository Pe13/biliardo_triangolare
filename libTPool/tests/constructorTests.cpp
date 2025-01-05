//
// Created by paolo on 14/06/2023.
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cmath>
#include <stdexcept>

#include "Pool.hpp"
#include "doctest.h"

TEST_SUITE("Pool constructor") {
  TEST_CASE("Testing default type") {
    CHECK_NOTHROW(bt::Pool(1, 1, 1));
    const auto pool = bt::Pool(1, 1, 1);
    CHECK(pool.type() == bt::leftBounded);
  }

  TEST_CASE("Testing valid instance construction") {
    for (unsigned int i = 0; i < 3; i++) {
      const bt::Pool pool(10.0, 2.0, 4.0, static_cast<bt::PoolType>(i));
      CHECK(pool.l() == 10.0);
      CHECK(pool.r1() == 2.0);
      CHECK(pool.r2() == 4.0);
      CHECK(pool.type() == static_cast<bt::PoolType>(i));
      CHECK(doctest::Approx(pool.theta()) == std::atan((4.0 - 2.0) / 10.0));
    }
  }

  TEST_CASE("Testing invalid parameters detection") {
    SUBCASE("Negative parameters") {
      CHECK_THROWS_WITH_AS(bt::Pool(-1, 1, 1),
                           "Il parametro \"l\" deve essere positivo; è stato fornito l = -1.000000",
                           std::invalid_argument);
      CHECK_THROWS_WITH_AS(
          bt::Pool(1, -1, 1),
          "Il parametro \"r1\" deve essere positivo; è stato fornito r1 = -1.000000",
          std::invalid_argument);
      CHECK_THROWS_WITH_AS(
          bt::Pool(1, 1, -1),
          "Il parametro \"r2\" deve essere positivo; è stato fornito r2 = -1.000000",
          std::invalid_argument);

      CHECK_THROWS_WITH_AS(bt::Pool(-1, -1, 1),
                           "Il parametro \"l\" deve essere positivo; è stato fornito l = -1.000000",
                           std::invalid_argument);
    }

    SUBCASE("Null parameters") {
      CHECK_THROWS_WITH_AS(bt::Pool(0, 1, 1),
                           "Il parametro \"l\" deve essere positivo; è stato fornito l = 0.000000",
                           std::invalid_argument);
      CHECK_THROWS_WITH_AS(
          bt::Pool(1, 0, 1),
          "Il parametro \"r1\" deve essere positivo; è stato fornito r1 = 0.000000",
          std::invalid_argument);
      CHECK_THROWS_WITH_AS(
          bt::Pool(1, 1, 0),
          "Il parametro \"r2\" deve essere positivo; è stato fornito r2 = 0.000000",
          std::invalid_argument);

      CHECK_THROWS_WITH_AS(bt::Pool(0, 1, 0),
                           "Il parametro \"l\" deve essere positivo; è stato fornito l = 0.000000",
                           std::invalid_argument);
    }

    SUBCASE("Invalid PoolType") {
      CHECK_THROWS_WITH_AS(bt::Pool(1, 1, 1, static_cast<bt::PoolType>(3)),
                           "Il tipo fornito per la costruzione del biliardo non è valido",
                           std::invalid_argument);
    }
  }
}
