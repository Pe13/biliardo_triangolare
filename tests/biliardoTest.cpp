//
// Created by paolo on 14/06/2023.
//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <TBenchmark.h>
#include <TH1D.h>

#include <boost/implicit_cast.hpp>
#include <chrono>
#include <cmath>
#include <execution>
#include <iostream>
#include <random>

#include "Biliardo.hpp"
#include "doctest.h"

TEST_CASE("Choosing the best method to launch multiple particles") {
  TBenchmark benchmark;

  double r1 = 30;
  double r2 = 10;
  double l = 100;

  double muY = 0;
  double sigmaY = r1 / 5;
  double muT = 0;
  double sigmaT = M_PI / 8;
  unsigned int N = 1e6;

  bt::Biliardo biliardo(l, r1, r2, bt::leftBounded);

  std::cout << "launching async...\n";

  for (int _ = 0; _ < 10; _++) {
    benchmark.Start("single async");
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    biliardo.multipleLaunch(muY, sigmaY, muT, sigmaT, N, histograms);
    benchmark.Show("single async");
  }

  std::cout << "launching sync...\n";

  for (int _ = 0; _ < 10; _++) {
    benchmark.Start("single sync");
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    biliardo.syncMultipleLaunch(muY, sigmaY, muT, sigmaT, N, histograms);
    benchmark.Show("single sync");
  }
}

TEST_CASE("Choosing the best method to fill histograms") {
  std::default_random_engine rng{
      boost::implicit_cast<unsigned long long>(std::chrono::system_clock::now().time_since_epoch().count())};
  std::uniform_real_distribution<double> dist{-1, 1};

  TBenchmark benchmark;

  benchmark.Start("async");

  for (int _ = 0; _ < 100; _++) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -1, 1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -1, 1)};

    std::vector<std::array<double, 2>> v(1e7);
    std::generate(v.begin(), v.end(), [&]() -> std::array<double, 2> { return {dist(rng), dist(rng)}; });

    std::for_each(v.begin(), v.end(), [&](const auto &arr) {
      std::for_each(std::execution::par_unseq, arr.begin(), arr.end(),
                    [&](const auto &item) { histograms[&item - arr.data()].Fill(item); });
    });
  }

  benchmark.Show("async");

  benchmark.Start("sync");

  for (int _ = 0; _ < 100; _++) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -1, 1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -1, 1)};

    std::vector<std::array<double, 2>> v(1e7);
    std::generate(v.begin(), v.end(), [&]() -> std::array<double, 2> { return {dist(rng), dist(rng)}; });

    std::for_each(v.begin(), v.end(), [&](const auto &arr) {
      histograms[0].Fill(arr[0]);
      histograms[1].Fill(arr[1]);
    });
  }

  benchmark.Show("sync");
}