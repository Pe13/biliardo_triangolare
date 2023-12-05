//
// Created by paolo on 19/11/2023.
//

#include <TH1D.h>
#include <benchmark/benchmark.h>

#include <algorithm>
#include <array>
#include <boost/implicit_cast.hpp>
#include <chrono>
#include <execution>
#include <random>
#include <vector>

#include "Biliardo.hpp"

static void fillHistogramsAsync(benchmark::State &state) {
  std::default_random_engine rng{
      boost::implicit_cast<unsigned long long>(std::chrono::system_clock::now().time_since_epoch().count())};
  std::uniform_real_distribution<double> dist{-1, 1};

  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -1, 1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -1, 1)};

    std::vector<std::array<double, 2>> v(1e7);
    std::generate(v.begin(), v.end(), [&]() -> std::array<double, 2> { return {dist(rng), dist(rng)}; });

    std::for_each(v.begin(), v.end(), [&](const auto &arr) {
      std::for_each(std::execution::par_unseq, arr.begin(), arr.end(),
                    [&](const auto &item) { histograms[&item - arr.data()].Fill(item); });
    });
  }
}
BENCHMARK(fillHistogramsAsync);

static void fillHistogramsSync(benchmark::State &state) {
  std::default_random_engine rng{
      boost::implicit_cast<unsigned long long>(std::chrono::system_clock::now().time_since_epoch().count())};
  std::uniform_real_distribution<double> dist{-1, 1};

  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -1, 1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -1, 1)};

    std::vector<std::array<double, 2>> v(1e7);
    std::generate(v.begin(), v.end(), [&]() -> std::array<double, 2> { return {dist(rng), dist(rng)}; });

    std::for_each(v.begin(), v.end(), [&](const auto &arr) {
      histograms[0].Fill(arr[0]);
      histograms[1].Fill(arr[1]);
    });
  }
}
BENCHMARK(fillHistogramsSync);

static void launchParticlesAsync(benchmark::State &state) {
  double r1 = 30;
  double r2 = 10;
  double l = 100;

  double muY = 0;
  double sigmaY = r1 / 5;
  double muT = 0;
  double sigmaT = M_PI / 8;
  unsigned int N = 1e6;

  bt::Biliardo biliardo(l, r1, r2, bt::leftBounded);

  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    biliardo.multipleLaunch(muY, sigmaY, muT, sigmaT, N, histograms, true);
  }
}
BENCHMARK(launchParticlesAsync);

static void launchParticlesSync(benchmark::State &state) {
  double r1 = 30;
  double r2 = 10;
  double l = 100;

  double muY = 0;
  double sigmaY = r1 / 5;
  double muT = 0;
  double sigmaT = M_PI / 8;
  unsigned int N = 1e6;

  bt::Biliardo biliardo(l, r1, r2, bt::leftBounded);

  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    biliardo.multipleLaunch(muY, sigmaY, muT, sigmaT, N, histograms, false);
  }
}
BENCHMARK(launchParticlesSync);

BENCHMARK_MAIN();
