//
// Created by paolo on 03/01/2025.
//

#include "App.hpp"
#include "Biliardo.hpp"
#include "doctest.h"

TEST_CASE("Testing App::modifyBiliardo") {
  const sf::ContextSettings settings{0, 0, 8, 4, 2, sf::ContextSettings::Default, false};
  for (unsigned int i = 0; i < 3; i++) {
    bt::App app(150, 2, 10, static_cast<bt::BiliardoType>(i), settings);

    SUBCASE("Valid modification") {
      SUBCASE("Testing that the Biliardo get modified correctly") {
        CHECK(app.modifyBiliardo(6, 14, 24) == true);
        CHECK(app.biliardo().l() == 6);
        CHECK(app.biliardo().r1() == 14);
        CHECK(app.biliardo().r2() == 24);
        CHECK(doctest::Approx(app.biliardo().slope()) ==
              (app.biliardo().r2() - app.biliardo().r1()) / app.biliardo().l());
        CHECK(doctest::Approx(app.biliardo().theta()) == std::atan(app.biliardo().slope()));
        CHECK(app.biliardo().type() == static_cast<bt::BiliardoType>(i));
      }

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

    SUBCASE("Invalid modifications") {
      const auto biliardoCopy = app.biliardo();
      const auto actualDesignerStep = app.designer().step();

      CHECK_FALSE(app.modifyBiliardo(-1, 1, 1));
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      CHECK_FALSE(app.modifyBiliardo(1, -1, 1));
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      CHECK_FALSE(app.modifyBiliardo(1, 1, -1));
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      CHECK_FALSE(app.modifyBiliardo(1, -1, -1));
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == actualDesignerStep);
    }
  }
}

TEST_CASE("Testing App::changeBiliardoType") {
  const sf::ContextSettings settings{0, 0, 8, 4, 2, sf::ContextSettings::Default, false};

  SUBCASE("Valid modification") {
    for (unsigned int i = 0; i < 3; i++) {
      const auto originalBiliardoType = static_cast<bt::BiliardoType>(i);

      for (unsigned int j = 0; j < 3; j++) {
        bt::App app(150, 2, 10, originalBiliardoType, settings);
        auto biliardoCopy = app.biliardo();
        const auto newBiliardoType = static_cast<bt::BiliardoType>(j);
        const auto originalStep = app.designer().step();

        [[maybe_unused]] const bool biliardoCopyChangeTypeResult =
            biliardoCopy.changeType(newBiliardoType);
        assert(biliardoCopyChangeTypeResult);

        CHECK(app.changeBiliardoType(newBiliardoType) == true);
        CHECK(app.biliardo() == biliardoCopy);

        if (i == j) {
          CHECK(app.designer().step() == originalStep);
        } else {
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
  }

  SUBCASE("Invalid BiliardoType") {
    constexpr auto invalidBiliardoType = static_cast<bt::BiliardoType>(3);
    for (unsigned int i = 0; i < 3; i++) {
      const auto originalBiliardoType = static_cast<bt::BiliardoType>(i);
      bt::App app(150, 2, 10, originalBiliardoType, settings);
      const auto biliardoCopy = app.biliardo();
      const auto originalStep = app.designer().step();

      CHECK(app.changeBiliardoType(invalidBiliardoType) == false);
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == originalStep);
    }
  }
}