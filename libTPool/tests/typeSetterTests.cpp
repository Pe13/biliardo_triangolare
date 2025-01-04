//
// Created by paolo on 02/01/2025.
//

#include <stdexcept>

#include "Biliardo.hpp"
#include "doctest.h"

TEST_SUITE("Biliardo::changeType") {
  TEST_CASE("Testing valid modifications") {
    auto biliardo = bt::Biliardo(1, 1, 1);
    CHECK(biliardo.changeType(bt::leftBounded) == true);
    CHECK(biliardo.type() == bt::leftBounded);

    CHECK(biliardo.changeType(bt::rightBounded) == true);
    CHECK(biliardo.type() == bt::rightBounded);

    CHECK(biliardo.changeType(bt::open) == true);
    CHECK(biliardo.type() == bt::open);
  }

  TEST_CASE("Testing invalid modifications") {
    auto biliardo = bt::Biliardo(1, 1, 1);
    const auto type = biliardo.type();
    CHECK(biliardo.changeType(static_cast<bt::BiliardoType>(3)) == false);
    CHECK(biliardo.type() == type);
  }
}