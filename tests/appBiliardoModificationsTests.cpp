//
// Created by paolo on 03/01/2025.
//

#include "App.hpp"
#include "Pool.hpp"
#include "doctest.h"
#include "testsUtilities.hpp"

TEST_CASE("Testing App::modifyPool") {
  for (unsigned int i = 0; i < 3; i++) {
    bt::App app(150, 2, 10, static_cast<bt::PoolType>(i), settings);

    SUBCASE("Valid modification") {
      REQUIRE(app.modifyPool(6, 14, 24) == true);

      // Testo che il biliardo sia stato modificato correttamente
      // Non uso l'operatore == tra biliardi per avere un riscontro più dettagliato nel caso
      // qualcosa dovesse andare storto
      CHECK(app.pool().l() == 6);
      CHECK(app.pool().r1() == 14);
      CHECK(app.pool().r2() == 24);
      CHECK(doctest::Approx(app.pool().slope()) ==
            (app.pool().r2() - app.pool().r1()) / app.pool().l());
      CHECK(doctest::Approx(app.pool().theta()) == std::atan(app.pool().slope()));
      CHECK(app.pool().type() == static_cast<bt::PoolType>(i));

      // Controllo che tutti i lanci siano stati generati correttamente
      // Lanci singoli
      for (long unsigned int j = 0; j < 3; j++) {
        CHECK(app.getSingleLaunches()[j].size() == 1);
        CHECK(app.getSingleLaunchesIndexes()[j] == 0);
      }
      // Lanci multipli
      for (long unsigned int j = 0; j < 3; j++) {
        CHECK(app.getMultipleLaunches()[j].size() == 0);
        CHECK(app.getSingleLaunchesIndexes()[j] == 0);
      }

      // Controllo che sia stato chiamato App::reRun
      checkReRunWasCalled(app);
    }

    SUBCASE("Invalid modifications") {
      const auto poolCopy = app.pool();
      const auto actualDesignerStep = app.designer().step();

      // Controllo che un qualsiasi valore negativo faccia fallire la modifica
      CHECK_FALSE(app.modifyPool(-1, 1, 1));
      CHECK(app.pool() == poolCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      CHECK_FALSE(app.modifyPool(1, -1, 1));
      CHECK(app.pool() == poolCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      CHECK_FALSE(app.modifyPool(1, 1, -1));
      CHECK(app.pool() == poolCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      // Controllo che anche con più valori negativi la modifica fallisca
      CHECK_FALSE(app.modifyPool(1, -1, -1));
      CHECK(app.pool() == poolCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      // Controllo che un qualsiasi valore nullo faccia fallire la modifica
      CHECK_FALSE(app.modifyPool(0, 1, 1));
      CHECK(app.pool() == poolCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      CHECK_FALSE(app.modifyPool(1, 0, 1));
      CHECK(app.pool() == poolCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      CHECK_FALSE(app.modifyPool(1, 1, 0));
      CHECK(app.pool() == poolCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      // Controllo che anche con più valori nulli la modifica fallisca
      CHECK_FALSE(app.modifyPool(0, 1, 0));
      CHECK(app.pool() == poolCopy);
      CHECK(app.designer().step() == actualDesignerStep);
    }
  }
}

TEST_CASE("Testing App::changePoolType") {
  SUBCASE("Valid PoolType") {
    for (unsigned int i = 0; i < 3; i++) {
      const auto originalPoolType = static_cast<bt::PoolType>(i);

      for (unsigned int j = 0; j < 3; j++) {
        bt::App app(150, 2, 10, originalPoolType, settings);
        auto poolCopy = app.pool();
        const auto newPoolType = static_cast<bt::PoolType>(j);
        const auto originalStep = app.designer().step();

        [[maybe_unused]] const bool poolCopyChangeTypeResult = poolCopy.changeType(newPoolType);
        assert(poolCopyChangeTypeResult);

        CHECK(app.changePoolType(newPoolType) == true);
        CHECK(app.pool() == poolCopy);

        if (i == j) {
          CHECK(app.designer().step() == originalStep);
        } else {
          checkReRunWasCalled(app);
        }
      }
    }
  }

  SUBCASE("Invalid PoolType") {
    constexpr auto invalidPoolType = static_cast<bt::PoolType>(3);
    for (unsigned int i = 0; i < 3; i++) {
      const auto originalPoolType = static_cast<bt::PoolType>(i);
      bt::App app(150, 2, 10, originalPoolType, settings);
      const auto poolCopy = app.pool();
      const auto originalStep = app.designer().step();

      CHECK(app.changePoolType(invalidPoolType) == false);
      CHECK(app.pool() == poolCopy);
      CHECK(app.designer().step() == originalStep);
    }
  }
}