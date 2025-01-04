//
// Created by paolo on 27/08/2024.
//

// TODO Testare l'algoritmo di rimbalzo

#include <TH1D.h>
#include <benchmark/benchmark.h>

#include <array>
#include <random>
#include <vector>

#include "Pool.hpp"
#include "toggleCout.hpp"

constexpr double r1 = 30;
constexpr double r2 = 10;
constexpr double l = 100;

constexpr double muY = 0;
constexpr double sigmaY = r1 / 5;
constexpr double muT = 0;
constexpr double sigmaT = M_PI / 8;
constexpr unsigned int N = 1e6;

static bt::Pool pool{l, r1, r2};

static void setupLeftBounded([[maybe_unused]] const benchmark::State &state) {
  [[maybe_unused]] const bool changeTypeResult = pool.changeType(bt::leftBounded);
  assert(changeTypeResult);
  disableCout(state);
}

static void setupRightBounded([[maybe_unused]] const benchmark::State &state) {
  [[maybe_unused]] const bool changeTypeResult = pool.changeType(bt::rightBounded);
  assert(changeTypeResult);
  disableCout(state);
}

static void setupOpen([[maybe_unused]] const benchmark::State &state) {
  [[maybe_unused]] const bool changeTypeResult = pool.changeType(bt::open);
  assert(changeTypeResult);
  disableCout(state);
}

static void launchParticlesAsyncLeftBounded(benchmark::State &state) {
  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    [[maybe_unused]] const bool multipleLaunchResult = pool.multipleLaunch(N, muY, sigmaY, muT, sigmaT, histograms, true);
    assert(multipleLaunchResult);
  }
}
BENCHMARK(launchParticlesAsyncLeftBounded)->Setup(setupLeftBounded)->Teardown(enableCout)->MinTime(5.0);

static void launchParticlesSyncLeftBounded(benchmark::State &state) {
  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    [[maybe_unused]] const bool multipleLaunchResult = pool.multipleLaunch(N, muY, sigmaY, muT, sigmaT, histograms, false);
    assert(multipleLaunchResult);
  }
}
BENCHMARK(launchParticlesSyncLeftBounded)->Setup(setupLeftBounded)->Teardown(enableCout)->MinTime(5.0);

static void launchParticlesAsyncRightBounded(benchmark::State &state) {
  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    [[maybe_unused]] const bool multipleLaunchResult = pool.multipleLaunch(N, muY, sigmaY, muT, sigmaT, histograms, true);
    assert(multipleLaunchResult);
  }
}
BENCHMARK(launchParticlesAsyncRightBounded)->Setup(setupRightBounded)->Teardown(enableCout)->MinTime(5.);

static void launchParticlesSyncRightBounded(benchmark::State &state) {
  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    [[maybe_unused]] const bool multipleLaunchResult = pool.multipleLaunch(N, muY, sigmaY, muT, sigmaT, histograms, false);
    assert(multipleLaunchResult);
  }
}
BENCHMARK(launchParticlesSyncRightBounded)->Setup(setupRightBounded)->Teardown(enableCout)->MinTime(5.);

static void launchParticlesAsyncOpen(benchmark::State &state) {
  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    [[maybe_unused]] const bool multipleLaunchResult = pool.multipleLaunch(N, muY, sigmaY, muT, sigmaT, histograms, true);
    assert(multipleLaunchResult);
  }
}
BENCHMARK(launchParticlesAsyncOpen)->Setup(setupOpen)->Teardown(enableCout)->MinTime(5.);

static void launchParticlesSyncOpen(benchmark::State &state) {
  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    [[maybe_unused]] const bool multipleLaunchResult = pool.multipleLaunch(N, muY, sigmaY, muT, sigmaT, histograms, false);
    assert(multipleLaunchResult);
  }
}
BENCHMARK(launchParticlesSyncOpen)->Setup(setupOpen)->Teardown(enableCout)->MinTime(5.);

BENCHMARK_MAIN();
