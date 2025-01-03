//
// Created by paolo on 03/01/2025.
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "App.hpp"
#include "Biliardo.hpp"
#include "doctest.h"

TEST_CASE("Testing App constructor") {
  const sf::ContextSettings settings{0, 0, 8, 4, 2, sf::ContextSettings::Default, false};

  SUBCASE("Invalid BiliardoType") {
    CHECK_THROWS_AS(bt::App(1, 1, 1, static_cast<bt::BiliardoType>(3), settings),
                    std::invalid_argument);
    CHECK_THROWS_AS(bt::App(1, 1, 1, static_cast<bt::BiliardoType>(-1), settings),
                    std::invalid_argument);
    CHECK_NOTHROW(bt::App(1, 1, 1, static_cast<bt::BiliardoType>(1), settings));
  }

  for (unsigned int i = 0; i < 3; i++) {
    SUBCASE("Testing that the constructed Biliardo instance is valid") {
      SUBCASE("Negative parameters") {
        CHECK_THROWS_WITH_AS(
            bt::App(-1, 1, 1, static_cast<bt::BiliardoType>(i), settings),
            "Il parametro \"l\" deve essere positivo; è stato fornito l = -1.000000",
            std::invalid_argument);
        CHECK_THROWS_WITH_AS(
            bt::App(1, -1, 1, static_cast<bt::BiliardoType>(i), settings),
            "Il parametro \"r1\" deve essere positivo; è stato fornito r1 = -1.000000",
            std::invalid_argument);
        CHECK_THROWS_WITH_AS(
            bt::App(1, 1, -1, static_cast<bt::BiliardoType>(i), settings),
            "Il parametro \"r2\" deve essere positivo; è stato fornito r2 = -1.000000",
            std::invalid_argument);

        CHECK_THROWS_WITH_AS(
            bt::App(-1, -1, 1, static_cast<bt::BiliardoType>(i), settings),
            "Il parametro \"l\" deve essere positivo; è stato fornito l = -1.000000",
            std::invalid_argument);
      }

      SUBCASE("Null parameters") {
        CHECK_THROWS_WITH_AS(
            bt::App(0, 1, 1, static_cast<bt::BiliardoType>(i), settings),
            "Il parametro \"l\" deve essere positivo; è stato fornito l = 0.000000",
            std::invalid_argument);
        CHECK_THROWS_WITH_AS(
            bt::App(1, 0, 1, static_cast<bt::BiliardoType>(i), settings),
            "Il parametro \"r1\" deve essere positivo; è stato fornito r1 = 0.000000",
            std::invalid_argument);
        CHECK_THROWS_WITH_AS(
            bt::App(1, 1, 0, static_cast<bt::BiliardoType>(i), settings),
            "Il parametro \"r2\" deve essere positivo; è stato fornito r2 = 0.000000",
            std::invalid_argument);

        CHECK_THROWS_WITH_AS(
            bt::App(0, 1, 0, static_cast<bt::BiliardoType>(i), settings),
            "Il parametro \"l\" deve essere positivo; è stato fornito l = 0.000000",
            std::invalid_argument);
      }

      SUBCASE("Valid instance") {
        const bt::App app(10.0, 2.0, 4.0, static_cast<bt::BiliardoType>(i), settings);
        CHECK(app.biliardo().l() == 10.0);
        CHECK(app.biliardo().r1() == 2.0);
        CHECK(app.biliardo().r2() == 4.0);
        CHECK(app.biliardo().type() == static_cast<bt::BiliardoType>(i));
        CHECK(doctest::Approx(app.biliardo().theta()) == std::atan((4.0 - 2.0) / 10.0));
      }
    }

    const bt::App app{50, 30, 10, static_cast<bt::BiliardoType>(i), settings};
    SUBCASE("Testing that all the launches are correctly generated") {
      SUBCASE("Single launches") {
        for (long unsigned int j = 0; j < 3; j++) {
          CHECK(app.getSingleLaunches()[j].size() == 1);
          CHECK(app.getSingleLaunchesIndexes()[j] == 0);
        }
      }

      SUBCASE("Multiple launches") {
        for (long unsigned int j = 0; j < 3; j++) {
          CHECK(app.getMultipleLaunches()[j].size() == 0);
          CHECK(app.getSingleLaunchesIndexes()[j] == 0);
        }
      }
    }

    SUBCASE("Testing that the designer is in the right state") {
      CHECK(app.designer().isDrawing() == true);
      CHECK(app.designer().isPaused() == false);
      CHECK(app.designer().hasCleared() == false);
      CHECK(app.designer().pointIndex() == 0);
      CHECK(app.designer().pathFraction() == 0);
      CHECK(app.designer().step() != 0);
      CHECK(app.designer().contrail().size() == 0);
    }
  }
}