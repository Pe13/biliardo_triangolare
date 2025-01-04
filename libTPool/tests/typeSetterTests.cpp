//
// Created by paolo on 02/01/2025.
//

#include <stdexcept>

#include "Pool.hpp"
#include "doctest.h"

TEST_SUITE("Pool::changeType") {
  TEST_CASE("Testing valid modifications") {
    bt::Pool pool{1, 1, 1};
    CHECK(pool.changeType(bt::leftBounded) == true);
    CHECK(pool.type() == bt::leftBounded);

    CHECK(pool.changeType(bt::rightBounded) == true);
    CHECK(pool.type() == bt::rightBounded);

    CHECK(pool.changeType(bt::open) == true);
    CHECK(pool.type() == bt::open);
  }

  TEST_CASE("Testing invalid modifications") {
    bt::Pool pool{1, 1, 1};
    const auto type = pool.type();
    CHECK(pool.changeType(static_cast<bt::PoolType>(3)) == false);
    CHECK(pool.type() == type);
  }
}