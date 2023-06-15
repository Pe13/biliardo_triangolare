//
// Created by paolo on 13/06/2023.
//

#ifndef BILIARDO_TRIANGOLARE_BILIARDO_HPP_
#define BILIARDO_TRIANGOLARE_BILIARDO_HPP_

#include <Eigen/Dense>
#include <vector>

namespace bt {

class Biliardo {
  double l_;
  double r1_;
  double r2_;

  double theta_{};

  void collideTop(double& angle) const { angle += 2 * theta_; }
  void collideBottom(double& angle) const { angle = -(2 * theta_ + angle); }
  void collideLeft(double& angle) const { angle = -angle; }

 public:
  Biliardo(double l, double r1, double r2);

  // Questo metodo è in grado di lanciare una sola particella e restituisce un
  // vettore contenente le posizioni di tutti gli urti tra essa e i bordi del
  // biliardo
  bool launchForDrawing(double const& initialX, double const& initialY, double const& initialDirection,
                        std::vector<double>& output) const;

  // Questo metodo è in grado di lanciare multiple particelle con un unica
  // chiamata.
  // Restituisce un vettore contenente solo informazioni riguardo l'uscita delle
  // particelle dal biliardo
  std::vector<double> launch;
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_BILIARDO_HPP_
