//
// Created by paolo on 14/06/2023.
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cmath>
#include <stdexcept>

#include "Biliardo.hpp"
#include "doctest.h"

TEST_CASE("Testing Biliardo constructor") {
  SUBCASE("Default type") {
    CHECK_NOTHROW(bt::Biliardo(1, 1, 1));
    auto biliardo = bt::Biliardo(1, 1, 1);
    CHECK(biliardo.type() == bt::leftBounded);
  }

  SUBCASE("Valid instance") {
    bt::Biliardo billiard(10.0, 2.0, 4.0, bt::open);
    CHECK(billiard.l() == 10.0);
    CHECK(billiard.r1() == 2.0);
    CHECK(billiard.r2() == 4.0);
    CHECK(billiard.type() == bt::open);
    CHECK(doctest::Approx(billiard.theta()) == std::atan((4.0 - 2.0) / 10.0));
  }

  SUBCASE("Negative parameters") {
    CHECK_THROWS_WITH_AS(bt::Biliardo(-1, 1, 1),
                         "Il parametro \"l\" deve essere positivo; è stato fornito l = -1.000000",
                         std::invalid_argument);
    CHECK_THROWS_WITH_AS(bt::Biliardo(1, -1, 1),
                         "Il parametro \"r1\" deve essere positivo; è stato fornito r1 = -1.000000",
                         std::invalid_argument);
    CHECK_THROWS_WITH_AS(bt::Biliardo(1, 1, -1),
                         "Il parametro \"r2\" deve essere positivo; è stato fornito r2 = -1.000000",
                         std::invalid_argument);

    CHECK_THROWS_WITH_AS(bt::Biliardo(-1, -1, 1),
                         "Il parametro \"l\" deve essere positivo; è stato fornito l = -1.000000",
                         std::invalid_argument);
  }

  SUBCASE("Null parameters") {
    CHECK_THROWS_WITH_AS(bt::Biliardo(0, 1, 1),
                         "Il parametro \"l\" deve essere positivo; è stato fornito l = 0.000000",
                         std::invalid_argument);
    CHECK_THROWS_WITH_AS(bt::Biliardo(1, 0, 1),
                         "Il parametro \"r1\" deve essere positivo; è stato fornito r1 = 0.000000",
                         std::invalid_argument);
    CHECK_THROWS_WITH_AS(bt::Biliardo(1, 1, 0),
                         "Il parametro \"r2\" deve essere positivo; è stato fornito r2 = 0.000000",
                         std::invalid_argument);

    CHECK_THROWS_WITH_AS(bt::Biliardo(0, 1, 0),
                         "Il parametro \"l\" deve essere positivo; è stato fornito l = 0.000000",
                         std::invalid_argument);
  }

  SUBCASE("Invalid BiliardoType") {
    CHECK_THROWS_AS(bt::Biliardo(1, 1, 1, static_cast<bt::BiliardoType>(3)), std::invalid_argument);
    CHECK_THROWS_AS(bt::Biliardo(1, 1, 1, static_cast<bt::BiliardoType>(-1)),
                    std::invalid_argument);
    CHECK_NOTHROW(bt::Biliardo(1, 1, 1, static_cast<bt::BiliardoType>(1)));
  }
}
