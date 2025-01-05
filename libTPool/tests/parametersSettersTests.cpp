//
// Created by paolo on 02/01/2025.
//

#include <cmath>

#include "Pool.hpp"
#include "doctest.h"

TEST_SUITE("Pool parameters setters") {
  TEST_CASE("Testing valid modifications") {
    constexpr double oldL = 1;
    constexpr double oldR1 = 2;
    constexpr double oldR2 = 3;

    bt::Pool pool(oldL, oldR1, oldR2);

    SUBCASE("Test l setter") {
      REQUIRE(pool.l(4));
      CHECK(pool.l() == 4);
      CHECK(pool.r1() == oldR1);
      CHECK(pool.r2() == oldR2);
      CHECK(doctest::Approx(pool.slope()) == (pool.r2() - pool.r1()) / pool.l());
      CHECK(doctest::Approx(pool.theta()) == std::atan(pool.slope()));
    }

    SUBCASE("Test r1 setter") {
      REQUIRE(pool.r1(10));
      CHECK(pool.r1() == 10);
      CHECK(pool.l() == oldL);
      CHECK(pool.r2() == oldR2);
      CHECK(doctest::Approx(pool.slope()) == (pool.r2() - pool.r1()) / pool.l());
      CHECK(doctest::Approx(pool.theta()) == std::atan(pool.slope()));
    }

    SUBCASE("Test r2 setter") {
      REQUIRE(pool.r2(18));
      CHECK(pool.r2() == 18);
      CHECK(pool.l() == oldL);
      CHECK(pool.r1() == oldR1);
      CHECK(doctest::Approx(pool.slope()) == (pool.r2() - pool.r1()) / pool.l());
      CHECK(doctest::Approx(pool.theta()) == std::atan(pool.slope()));
    }

    SUBCASE("modify") {
      REQUIRE(pool.modify(6, 14, 24));
      CHECK(pool.l() == 6);
      CHECK(pool.r1() == 14);
      CHECK(pool.r2() == 24);
      CHECK(doctest::Approx(pool.slope()) == (pool.r2() - pool.r1()) / pool.l());
      CHECK(doctest::Approx(pool.theta()) == std::atan(pool.slope()));
    }
  }

  TEST_CASE("Testing invalid modifications") {
    bt::Pool pool{1, 1, 1};
    const auto poolCopy = pool;

    SUBCASE("Test individual setters with a negative value") {
      CHECK_FALSE(pool.l(-1));
      CHECK(pool == poolCopy);

      CHECK_FALSE(pool.r1(-1));
      CHECK(pool == poolCopy);

      CHECK_FALSE(pool.r2(-1));
      CHECK(pool == poolCopy);
    }

    SUBCASE("Test individual setters with a null value") {
      CHECK_FALSE(pool.l(0));
      CHECK(pool == poolCopy);

      CHECK_FALSE(pool.r1(0));
      CHECK(pool == poolCopy);

      CHECK_FALSE(pool.r2(0));
      CHECK(pool == poolCopy);
    }

    SUBCASE("Test modify with negative values") {
      // Controllo che un qualsiasi valore negativo faccia fallire la modifica
      CHECK_FALSE(pool.modify(-1, 1, 1));
      CHECK(pool == poolCopy);

      CHECK_FALSE(pool.modify(1, -1, 1));
      CHECK(pool == poolCopy);

      CHECK_FALSE(pool.modify(1, 1, -1));
      CHECK(pool == poolCopy);

      // Controllo che anche con più valori negativi la modifica fallisca
      CHECK_FALSE(pool.modify(1, -1, -1));
      CHECK(pool == poolCopy);
    }

    SUBCASE("Test modify with null values") {
      // Controllo che un qualsiasi valore nullo faccia fallire la modifica
      CHECK_FALSE(pool.modify(0, 1, 1));
      CHECK(pool == poolCopy);

      CHECK_FALSE(pool.modify(1, 0, 1));
      CHECK(pool == poolCopy);

      CHECK_FALSE(pool.modify(1, 1, 0));
      CHECK(pool == poolCopy);

      // Controllo che anche con più valori nulli la modifica fallisca
      CHECK_FALSE(pool.modify(0, 1, 0));
      CHECK(pool == poolCopy);
    }
  }
}