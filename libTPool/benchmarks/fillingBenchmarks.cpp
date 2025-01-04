//
// Created by paolo on 27/08/2024.
//

#include <TH1D.h>
#include <benchmark/benchmark.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <execution>
#include <random>
#include <vector>

#include "Pool.hpp"

static std::default_random_engine rng{
    static_cast<unsigned long long>(std::chrono::system_clock::now().time_since_epoch().count())};
static std::uniform_real_distribution<double> dist{-1, 1};

static void fillHistogramsAsync(benchmark::State &state) {
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
BENCHMARK(fillHistogramsAsync)->MinTime(5.);

static void fillHistogramsSync(benchmark::State &state) {
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
BENCHMARK(fillHistogramsSync)->MinTime(5.);