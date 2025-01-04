//
// Created by paolo on 04/01/2025.
//

#include <cmath>
#include <optional>

#include "App.hpp"
#include "Biliardo.hpp"
#include "doctest.h"
#include "testsUtilities.hpp"

TEST_SUITE("Testing launch methods") {
  TEST_CASE("Single launches") {
    SUBCASE("Valid parameters") {
      for (unsigned int i = 0; i < 3; i++) {
        bt::App app{150, 2, 10, static_cast<bt::BiliardoType>(i), settings};
        std::array<std::weak_ptr<const std::vector<double>>, 4> launches{};

        launches[0] = app.singleLaunch();
        CHECK_FALSE(launches[0].expired());
        CHECK(app.getSingleLaunches()[static_cast<bt::BiliardoType>(i)].size() == 2);
        CHECK(app.getSingleLaunchesIndexes()[static_cast<bt::BiliardoType>(i)] == 1);

        launches[1] = app.singleLaunch(1);
        for (unsigned int j = 0; j < 2; j++) {
          CHECK_FALSE(launches[j].expired());
        }
        CHECK(app.getSingleLaunches()[static_cast<bt::BiliardoType>(i)].size() == 3);
        CHECK(app.getSingleLaunchesIndexes()[static_cast<bt::BiliardoType>(i)] == 2);

        launches[2] = app.singleLaunch(std::nullopt, 0);
        for (unsigned int j = 0; j < 3; j++) {
          CHECK_FALSE(launches[j].expired());
        }
        CHECK(app.getSingleLaunches()[static_cast<bt::BiliardoType>(i)].size() == 4);
        CHECK(app.getSingleLaunchesIndexes()[static_cast<bt::BiliardoType>(i)] == 3);

        launches[3] = app.singleLaunch(-1.5, M_PI / 4);
        for (unsigned int j = 0; j < 4; j++) {
          CHECK_FALSE(launches[j].expired());
        }
        CHECK(app.getSingleLaunches()[static_cast<bt::BiliardoType>(i)].size() == 5);
        CHECK(app.getSingleLaunchesIndexes()[static_cast<bt::BiliardoType>(i)] == 4);

        // Controllo che i puntatori decadano quando i lanci vengono distrutti
        REQUIRE(app.modifyBiliardo(1, 1, 1) == true);
        for (const auto& launch : launches) {
          CHECK(launch.expired());
        }
      }
    }

    SUBCASE("Invalid parameters") {
      bt::App app{1, 1, 1, bt::open, settings};
      const auto singleLaunches = app.getSingleLaunches();
      const auto singleLaunchesIndexes = app.getSingleLaunchesIndexes();

      CHECK(app.singleLaunch(1).expired());
      CHECK(app.getSingleLaunches() == singleLaunches);
      CHECK(app.getSingleLaunchesIndexes() == singleLaunchesIndexes);

      CHECK(app.singleLaunch(-2).expired());
      CHECK(app.getSingleLaunches() == singleLaunches);
      CHECK(app.getSingleLaunchesIndexes() == singleLaunchesIndexes);

      CHECK(app.singleLaunch(std::nullopt, M_PI / 2).expired());
      CHECK(app.getSingleLaunches() == singleLaunches);
      CHECK(app.getSingleLaunchesIndexes() == singleLaunchesIndexes);

      CHECK(app.singleLaunch(std::nullopt, -M_PI).expired());
      CHECK(app.getSingleLaunches() == singleLaunches);
      CHECK(app.getSingleLaunchesIndexes() == singleLaunchesIndexes);
    }
  }

  TEST_CASE("Multiple launches") {
    SUBCASE("Valid parameters") {
      for (unsigned int i = 0; i < 3; i++) {
        bt::App app{150, 2, 10, static_cast<bt::BiliardoType>(i), settings};

        const auto validLaunch1 = app.multipleLaunch(1e6, 0., app.biliardo().r1() / 5, 0, M_PI / 8);
        CHECK_FALSE(validLaunch1.expired());
        CHECK(app.getMultipleLaunches()[static_cast<bt::BiliardoType>(i)].size() == 1);
        CHECK(app.getMultipleLaunchesIndexes()[static_cast<bt::BiliardoType>(i)] == 0);

        const auto validLaunch2 = app.multipleLaunch(1e6, 0., app.biliardo().r1() / 7, 0, M_PI / 9);
        CHECK_FALSE(validLaunch2.expired());
        CHECK_FALSE(validLaunch1.expired());
        CHECK(app.getMultipleLaunches()[static_cast<bt::BiliardoType>(i)].size() == 2);
        CHECK(app.getMultipleLaunchesIndexes()[static_cast<bt::BiliardoType>(i)] == 1);

        // Controllo che i puntatori decadano quando i lanci vengono distrutti
        REQUIRE(app.modifyBiliardo(1, 1, 1) == true);
        CHECK(validLaunch1.expired());
        CHECK(validLaunch2.expired());
      }
    }

    SUBCASE("Invalid parameters") {
      bt::App app{1, 1, 1, bt::open, settings};
      const auto multipleLaunches = app.getMultipleLaunches();
      const auto multipleLaunchesIndexes = app.getMultipleLaunchesIndexes();

      CHECK(app.multipleLaunch(1e6, 0., -app.biliardo().r1() / 5, 0, M_PI / 8).expired());
      CHECK(app.getMultipleLaunches() == multipleLaunches);
      CHECK(app.getMultipleLaunchesIndexes() == multipleLaunchesIndexes);

      CHECK(app.multipleLaunch(1e6, 0., app.biliardo().r1() / 5, 0, -M_PI / 8).expired());
      CHECK(app.getMultipleLaunches() == multipleLaunches);
      CHECK(app.getMultipleLaunchesIndexes() == multipleLaunchesIndexes);

      CHECK(app.multipleLaunch(1e6, 0., -app.biliardo().r1() / 5, 0, -M_PI / 8).expired());
      CHECK(app.getMultipleLaunches() == multipleLaunches);
      CHECK(app.getMultipleLaunchesIndexes() == multipleLaunchesIndexes);

      CHECK(app.multipleLaunch(1e6, 0., 0, 0, M_PI / 8).expired());
      CHECK(app.getMultipleLaunches() == multipleLaunches);
      CHECK(app.getMultipleLaunchesIndexes() == multipleLaunchesIndexes);

      CHECK(app.multipleLaunch(1e6, 0., app.biliardo().r1() / 5, 0, 0).expired());
      CHECK(app.getMultipleLaunches() == multipleLaunches);
      CHECK(app.getMultipleLaunchesIndexes() == multipleLaunchesIndexes);

      CHECK(app.multipleLaunch(1e6, 0., 0, 0, 0).expired());
      CHECK(app.getMultipleLaunches() == multipleLaunches);
      CHECK(app.getMultipleLaunchesIndexes() == multipleLaunchesIndexes);
    }
  }
}