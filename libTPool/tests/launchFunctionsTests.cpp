//
// Created by paolo on 02/01/2025.
//

#include <TH1D.h>

#include <optional>

#include "Biliardo.hpp"
#include "doctest.h"

TEST_CASE("Testing launch functions") {
  bt::Biliardo biliardo{1, 1, 1, bt::open};

  SUBCASE("Testing launchForDrawing parameters validation") {
    std::vector<double> output{};
    SUBCASE("Valid parameters") {
      CHECK(biliardo.launchForDrawing(output) == true);
      CHECK(biliardo.launchForDrawing(output, 0.5) == true);
      CHECK(biliardo.launchForDrawing(output, std::nullopt, 0.5) == true);
      CHECK(biliardo.launchForDrawing(output, 0.5, 0.5) == true);
    }

    SUBCASE("Invalid parameters") {
      CHECK(biliardo.launchForDrawing(output, 1) == false);
      CHECK(biliardo.launchForDrawing(output, -2) == false);

      CHECK(biliardo.launchForDrawing(output, std::nullopt, M_PI / 2) == false);
      CHECK(biliardo.launchForDrawing(output, std::nullopt, -M_PI) == false);
    }
  }

  SUBCASE("Testing that multipleLaunch produces histograms with the right number of entries") {
    SUBCASE("Async") {
      std::array<TH1D, 2> histograms{};
      histograms[0] = TH1D("", "Istogramma delle y di uscita", 1000, -biliardo.r1(), biliardo.r1());
      histograms[1] = TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2);
      biliardo.multipleLaunch(0, biliardo.r1() / 5, 0, M_PI / 8, 1e6, histograms);
      CHECK(histograms[0].GetEntries() == 1e6);
      CHECK(histograms[1].GetEntries() == 1e6);
    }

    SUBCASE("Sync") {
      std::array<TH1D, 2> histograms{};
      histograms[0] = TH1D("", "Istogramma delle y di uscita", 1000, -biliardo.r1(), biliardo.r1());
      histograms[1] = TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2);
      biliardo.multipleLaunch(0, biliardo.r1() / 5, 0, M_PI / 8, 1e6, histograms, false);
      CHECK(histograms[0].GetEntries() == 1e6);
      CHECK(histograms[1].GetEntries() == 1e6);
    }
  }
}