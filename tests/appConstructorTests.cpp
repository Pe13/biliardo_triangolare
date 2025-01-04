//
// Created by paolo on 03/01/2025.
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "App.hpp"
#include "Pool.hpp"
#include "doctest.h"
#include "testsUtilities.hpp"

TEST_SUITE("App constructor") {
  TEST_CASE("Invalid PoolType") {
    CHECK_THROWS_WITH_AS(bt::App(1, 1, 1, static_cast<bt::PoolType>(3), settings),
                         "Il tipo fornito per la costruzione del Biliardo non è valido",
                         std::invalid_argument);
  }

  TEST_CASE("Invalid Pool parameters") {
    for (unsigned int i = 0; i < 3; i++) {
      const auto type = static_cast<bt::PoolType>(i);

      SUBCASE("Negative parameters") {
        CHECK_THROWS_WITH_AS(
            bt::App(-1, 1, 1, type, settings),
            "Il parametro \"l\" deve essere positivo; è stato fornito l = -1.000000",
            std::invalid_argument);
        CHECK_THROWS_WITH_AS(
            bt::App(1, -1, 1, type, settings),
            "Il parametro \"r1\" deve essere positivo; è stato fornito r1 = -1.000000",
            std::invalid_argument);
        CHECK_THROWS_WITH_AS(
            bt::App(1, 1, -1, type, settings),
            "Il parametro \"r2\" deve essere positivo; è stato fornito r2 = -1.000000",
            std::invalid_argument);

        CHECK_THROWS_WITH_AS(
            bt::App(-1, -1, 1, type, settings),
            "Il parametro \"l\" deve essere positivo; è stato fornito l = -1.000000",
            std::invalid_argument);
      }

      SUBCASE("Null parameters") {
        CHECK_THROWS_WITH_AS(
            bt::App(0, 1, 1, type, settings),
            "Il parametro \"l\" deve essere positivo; è stato fornito l = 0.000000",
            std::invalid_argument);
        CHECK_THROWS_WITH_AS(
            bt::App(1, 0, 1, type, settings),
            "Il parametro \"r1\" deve essere positivo; è stato fornito r1 = 0.000000",
            std::invalid_argument);
        CHECK_THROWS_WITH_AS(
            bt::App(1, 1, 0, type, settings),
            "Il parametro \"r2\" deve essere positivo; è stato fornito r2 = 0.000000",
            std::invalid_argument);

        CHECK_THROWS_WITH_AS(
            bt::App(0, 1, 0, type, settings),
            "Il parametro \"l\" deve essere positivo; è stato fornito l = 0.000000",
            std::invalid_argument);
      }
    }
  }

  TEST_CASE("Valid instance") {
    for (unsigned int i = 0; i < 3; i++) {
      const auto type = static_cast<bt::PoolType>(i);
      const bt::App app(10.0, 2.0, 4.0, type, settings);

      // Testo che il biliardo sia stato modificato correttamente
      // Non uso l'operatore == tra biliardi per avere un riscontro più dettagliato nel caso
      // qualcosa dovesse andare storto
      CHECK(app.pool().l() == 10.0);
      CHECK(app.pool().r1() == 2.0);
      CHECK(app.pool().r2() == 4.0);
      CHECK(app.pool().type() == type);
      CHECK(doctest::Approx(app.pool().theta()) == std::atan((4.0 - 2.0) / 10.0));

      // Controllo che tutti i lanci siano generati correttamente
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
  }
}
