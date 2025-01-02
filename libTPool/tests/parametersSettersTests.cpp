//
// Created by paolo on 02/01/2025.
//

#include <cmath>
#include <stdexcept>

#include "Biliardo.hpp"
#include "doctest.h"

TEST_CASE("Testing parameters setters") {
  SUBCASE("Valid modifications") {
    constexpr double oldL = 1;
    constexpr double oldR1 = 2;
    constexpr double oldR2 = 3;

    bt::Biliardo biliardo(oldL, oldR1, oldR2);
    CHECK_NOTHROW(biliardo.l(4));
    CHECK(biliardo.l() == 4);
    CHECK(biliardo.r1() == oldR1);
    CHECK(biliardo.r2() == oldR2);
    CHECK(doctest::Approx(biliardo.slope()) == (biliardo.r2() - biliardo.r1()) / biliardo.l());
    CHECK(doctest::Approx(biliardo.theta()) == std::atan(biliardo.slope()));

    biliardo = bt::Biliardo(oldL, oldR1, oldR2);
    CHECK_NOTHROW(biliardo.r1(10));
    CHECK(biliardo.r1() == 10);
    CHECK(biliardo.l() == oldL);
    CHECK(biliardo.r2() == oldR2);
    CHECK(doctest::Approx(biliardo.slope()) == (biliardo.r2() - biliardo.r1()) / biliardo.l());
    CHECK(doctest::Approx(biliardo.theta()) == std::atan(biliardo.slope()));

    biliardo = bt::Biliardo(oldL, oldR1, oldR2);
    CHECK_NOTHROW(biliardo.r2(18));
    CHECK(biliardo.r2() == 18);
    CHECK(biliardo.l() == oldL);
    CHECK(biliardo.r1() == oldR1);
    CHECK(doctest::Approx(biliardo.slope()) == (biliardo.r2() - biliardo.r1()) / biliardo.l());
    CHECK(doctest::Approx(biliardo.theta()) == std::atan(biliardo.slope()));

    biliardo = bt::Biliardo(oldL, oldR1, oldR2);
    CHECK(biliardo.modify(6, 14, 24));
    CHECK(biliardo.l() == 6);
    CHECK(biliardo.r1() == 14);
    CHECK(biliardo.r2() == 24);
    CHECK(doctest::Approx(biliardo.slope()) == (biliardo.r2() - biliardo.r1()) / biliardo.l());
    CHECK(doctest::Approx(biliardo.theta()) == std::atan(biliardo.slope()));
  }

  SUBCASE("Invalid modifications") {
    bt::Biliardo biliardo{1, 1, 1};
    CHECK_THROWS_WITH_AS(biliardo.l(-1),
                         "Il parametro \"l\" deve essere positivo; è stato fornito l = -1.000000",
                         std::invalid_argument);
    CHECK_THROWS_WITH_AS(biliardo.r1(-1),
                         "Il parametro \"r1\" deve essere positivo; è stato fornito r1 = -1.000000",
                         std::invalid_argument);
    CHECK_THROWS_WITH_AS(biliardo.r2(-1),
                         "Il parametro \"r2\" deve essere positivo; è stato fornito r2 = -1.000000",
                         std::invalid_argument);
    CHECK_FALSE(biliardo.modify(-1, 1, 1));
    CHECK_FALSE(biliardo.modify(1, -1, 1));
    CHECK_FALSE(biliardo.modify(1, 1, -1));
    CHECK_FALSE(biliardo.modify(1, -1, -1));
  }
}