//
// Created by paolo on 02/01/2025.
//

#include <TH1D.h>

#include <optional>

#include "Pool.hpp"
#include "doctest.h"

TEST_SUITE("Pool launch functions") {
  TEST_CASE("Testing launchForDrawing parameters validation") {
    bt::Pool pool{1, 1, 1, bt::open};
    std::vector<double> output{};
    SUBCASE("Valid parameters") {
      CHECK(pool.launchForDrawing(output) == true);
      CHECK(pool.launchForDrawing(output, 0.5) == true);
      CHECK(pool.launchForDrawing(output, std::nullopt, 0.5) == true);
      CHECK(pool.launchForDrawing(output, 0.5, 0.5) == true);
    }

    SUBCASE("Invalid parameters") {
      CHECK(pool.launchForDrawing(output, 1) == false);
      CHECK(pool.launchForDrawing(output, -2) == false);

      CHECK(pool.launchForDrawing(output, std::nullopt, M_PI / 2) == false);
      CHECK(pool.launchForDrawing(output, std::nullopt, -M_PI) == false);
    }
  }

  TEST_CASE("Testing that multipleLaunch produces histograms with the right number of entries") {
    bt::Pool pool{1, 1, 1, bt::open};

    std::array<TH1D, 2> histograms{};
    histograms[0] = TH1D("", "Istogramma delle y di uscita", 1000, -pool.r1(), pool.r1());
    histograms[1] = TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2);

    SUBCASE("Async") {
      REQUIRE(pool.multipleLaunch(1e6, 0., pool.r1() / 5, 0, M_PI / 8, histograms));
    }

    SUBCASE("Sync") {
      REQUIRE(pool.multipleLaunch(1e6, 0., pool.r1() / 5, 0, M_PI / 8, histograms, false));
    }

    CHECK(histograms[0].GetEntries() == 1e6);
    CHECK(histograms[1].GetEntries() == 1e6);
  }
}