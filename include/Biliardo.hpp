//
// Created by paolo on 13/06/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_

#include <functional>
#include <random>
#include <vector>
#include <array>

#include "BiliardoFunctions.hpp"
#include "types.hpp"

namespace bt {

class Biliardo {

  BiliardoType type_;

  double l_;
  double r1_;
  double r2_;

  double theta_;

  BiliardoFunctions functions_;

  std::default_random_engine rng_;
  std::uniform_real_distribution<double> uniformDist_{0, 1};
  std::normal_distribution<double> yNormalDist_;
  std::normal_distribution<double> thetaNormalDist_{0, M_PI / 8};

  void collideTop(double& angle) const { angle = 2 * theta_ - angle; }
  void collideBottom(double& angle) const { angle = -(2 * theta_ + angle); }

  void registerTopBottomCollision(double const& x, double& y, double const& a, double const& c,
                                  std::vector<double>& output) const;

  bool isOut(const LastHit &lastHit) const;

 public:
  Biliardo(double l, double r1, double r2, BiliardoType type);
//  Biliardo(const Biliardo* biliardo, BiliardoType type);
  ~Biliardo() = default;

  BiliardoType type() const;
  void changeType(BiliardoType type);

  const double& l() const { return l_; }
  const double& r1() const { return r1_; }
  const double& r2() const { return r2_; }

  // Questo metodo è in grado di lanciare una sola particella e restituisce un
  // vettore contenente le posizioni di tutti gli urti tra essa e i bordi del
  // biliardo
  void launchForDrawing(double const& initialY, double const& initialDirection, std::vector<double>& output);
  void launchForDrawing(std::vector<double>& output);  // non const perché l'rng ha bisogno di "cambiare" per funzionare
  void launchForDrawingNoY(double const& initialDirection, std::vector<double>& output);
  void launchForDrawingNoDir(double const& initialY, std::vector<double>& output);

  // Questo metodo è in grado di lanciare multiple particelle con un unica
  // chiamata.
  // Restituisce un vettore contenente solo informazioni riguardo l'uscita delle
  // particelle dal biliardo
  void launch(long long N, std::vector<double>& output);
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_
