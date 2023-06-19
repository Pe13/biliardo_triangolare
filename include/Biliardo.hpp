//
// Created by paolo on 13/06/2023.
//

#ifndef BILIARDO_TRIANGOLARE_BILIARDO_HPP_
#define BILIARDO_TRIANGOLARE_BILIARDO_HPP_

#include <vector>
#include <random>

namespace bt {

class Biliardo {
  double l_;
  double r1_;
  double r2_;

  double theta_;

  std::default_random_engine rng_;
  std::uniform_real_distribution<double> dist_{0, 1};

  void collideTop(double& angle) const { angle += 2 * theta_; }
  void collideBottom(double& angle) const { angle = -(2 * theta_ + angle); }
  void collideLeft(double& angle) const { angle = -angle; }

 public:
  Biliardo(double l, double r1, double r2);

  double const& l() const { return l_; }
  double const& r1() const { return r1_; }
  double const& r2() const { return r2_; }


  // Questo metodo è in grado di lanciare una sola particella e restituisce un
  // vettore contenente le posizioni di tutti gli urti tra essa e i bordi del
  // biliardo
  void launchForDrawing(double const& initialY, double const& initialDirection,
                        std::vector<double>& output) const;
  void launchForDrawing(std::vector<double>& output); //non const perché l'rng ha bisogno di "cambiare" per funzionare
  // Questo metodo è in grado di lanciare multiple particelle con un unica
  // chiamata.
  // Restituisce un vettore contenente solo informazioni riguardo l'uscita delle
  // particelle dal biliardo
  std::vector<double> launch;
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_BILIARDO_HPP_
