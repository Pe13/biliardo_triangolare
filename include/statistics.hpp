//
// Created by paolo on 27/07/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_STATISTICS_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_STATISTICS_HPP_

#include <algorithm>
#include <array>
#include <cmath>
#include <execution>
#include <numeric>
#include <vector>

namespace bt {

struct Statistics {
  const double yAverage;
  const double yStdDev;
  const double ySymmetryCoefficient;
  const double yFlatteningCoefficient;

  const double thetaAverage;
  const double thetaStdDev;
  const double thetaSymmetryCoefficient;
  const double thetaFlatteningCoefficient;
};

inline Statistics statistics(std::vector<double>& input) {
  // separo in due parti il vector, nella prima ci sono le y di arrivo, nella seconda gli angoli
  auto thetaBegin = std::partition(input.begin(), input.end(),
                                   [&](const auto& item) -> bool { return !((&item - input.data()) % 2); });

  double N = input.size() / 2;
  double ySum = std::reduce(std::execution::par, input.begin(), thetaBegin);
  double thetaSum = std::reduce(std::execution::par, thetaBegin, input.end());

  double yAverage = ySum / N;
  double thetaAverage = thetaSum / N;

  // per parallelizzare le operazioni creo una copia di input, per ogni elemento ricavo lo scarto al quadrato e poi li
  // sommo tutti utilizzando std::reduce nel calcolo per la deviazione standard
  std::vector<double> squaredDeviations{input};
  auto thetaSDBegin = std::next(squaredDeviations.begin(), N);  // creo un iteratore equivalente a thetaBegin
  std::for_each(std::execution::par, squaredDeviations.begin(), thetaSDBegin,
                [&](auto& item) { item = (item - yAverage) * (item - yAverage); });
  std::for_each(std::execution::par, thetaSDBegin, squaredDeviations.end(),
                [&](auto& item) { item = (item - yAverage) * (item - yAverage); });

  double yStdDev = std::sqrt(1. / (N - 1) * std::reduce(std::execution::par, squaredDeviations.begin(), thetaSDBegin));
  double thetaStdDev = std::sqrt(1. / (N - 1) * std::reduce(std::execution::par, thetaSDBegin, squaredDeviations.end()));

  std::vector<double> normalizedInput{input};
  auto thetaNormBegin = std::next(normalizedInput.begin(), N);
  // https://www.corsi.univr.it/documenti/OccorrenzaIns/matdid/matdid592250.pdf

  return {yAverage, yStdDev, , , thetaAverage, thetaStdDev, , };
}

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_STATISTICS_HPP_
