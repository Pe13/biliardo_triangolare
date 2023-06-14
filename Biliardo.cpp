//
// Created by paolo on 13/06/2023.
//

#include "Biliardo.hpp"

#include <Eigen/Dense>
#include <cmath>
#include <vector>

namespace bt {

Biliardo::Biliardo(double l, double r1, double r2) : l_{l}, r1_{r1}, r2_{r2} {
  double theta = std::atan(l / (r1_ - r2_));
  double beta = M_PI / 2 - theta;

  if (r1 > r2) {
    simmetriaSuperiore_ << std::cos(2 * beta), std::cos(2 * beta - M_PI / 2),
        std::sin(2 * beta), std::sin(2 * beta - M_PI / 2);
    simmetriaInferiore_ << std::cos(2 * beta), std::cos(2 * beta - M_PI / 2),
        - std::sin(2 * beta), -std::sin(2 * beta - M_PI / 2);
  }

  //  double theta = M_PI / 2 + 2 * std::atan(l / (r1_ - r2_));
}

std::vector<double> Biliardo::launchForDrawing(
    Eigen::Vector2d const& initialPosition,
    Eigen::Vector2d const& initialDirection,
    std::vector<double>& output) const {}

}  // namespace bt