//
// Created by paolo on 27/08/2024.
//

// TODO Testare l'algoritmo di rimbalzo

#include <TH1D.h>
#include <benchmark/benchmark.h>

#include <array>
#include <random>
#include <vector>

#include "Biliardo.hpp"
#include "toggleCout.hpp"

const double r1 = 30;
const double r2 = 10;
const double l = 100;

const double muY = 0;
const double sigmaY = r1 / 5;
const double muT = 0;
const double sigmaT = M_PI / 8;
const unsigned int N = 1e6;

static bt::Biliardo biliardo{l, r1, r2};

static void setupLeftBounded([[maybe_unused]] const benchmark::State &state) {
  biliardo.changeType(bt::leftBounded);
  disableCout(state);
}

static void setupRightBounded([[maybe_unused]] const benchmark::State &state) {
  biliardo.changeType(bt::rightBounded);
  disableCout(state);
}

static void setupOpen([[maybe_unused]] const benchmark::State &state) {
  biliardo.changeType(bt::open);
  disableCout(state);
}

static void launchParticlesAsyncLeftBounded(benchmark::State &state) {
  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    biliardo.multipleLaunch(muY, sigmaY, muT, sigmaT, N, histograms, true);
  }
}
BENCHMARK(launchParticlesAsyncLeftBounded)->Setup(setupLeftBounded)->Teardown(enableCout)->MinTime(5.0);

static void launchParticlesSyncLeftBounded(benchmark::State &state) {
  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    biliardo.multipleLaunch(muY, sigmaY, muT, sigmaT, N, histograms, false);
  }
}
BENCHMARK(launchParticlesSyncLeftBounded)->Setup(setupLeftBounded)->Teardown(enableCout)->MinTime(5.0);

static void launchParticlesAsyncRightBounded(benchmark::State &state) {
  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    biliardo.multipleLaunch(muY, sigmaY, muT, sigmaT, N, histograms, true);
  }
}
BENCHMARK(launchParticlesAsyncRightBounded)->Setup(setupRightBounded)->Teardown(enableCout)->MinTime(5.);

static void launchParticlesSyncRightBounded(benchmark::State &state) {
  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    biliardo.multipleLaunch(muY, sigmaY, muT, sigmaT, N, histograms, false);
  }
}
BENCHMARK(launchParticlesSyncRightBounded)->Setup(setupRightBounded)->Teardown(enableCout)->MinTime(5.);

static void launchParticlesAsyncOpen(benchmark::State &state) {
  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    biliardo.multipleLaunch(muY, sigmaY, muT, sigmaT, N, histograms, true);
  }
}
BENCHMARK(launchParticlesAsyncOpen)->Setup(setupOpen)->Teardown(enableCout)->MinTime(5.);

static void launchParticlesSyncOpen(benchmark::State &state) {
  for (auto _ : state) {
    std::array histograms = {TH1D("", "Istogramma delle y di uscita", 1000, -r1, r1),
                             TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};

    biliardo.multipleLaunch(muY, sigmaY, muT, sigmaT, N, histograms, false);
  }
}
BENCHMARK(launchParticlesSyncOpen)->Setup(setupOpen)->Teardown(enableCout)->MinTime(5.);

BENCHMARK_MAIN();
