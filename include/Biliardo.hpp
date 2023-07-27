//
// Created by paolo on 13/06/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_

#include <random>
#include <vector>

namespace bt {

enum LastHit { left, top, bottom };
enum BiliardoType { open = 0, rightBounded = 1, leftBounded = 2 };

class Biliardo {
 protected:

  BiliardoType type_;

  double l_;
  double r1_;
  double r2_;

  double theta_;

  std::default_random_engine rng_;
  std::uniform_real_distribution<double> dist_{0, 1};

  void collideTop(double& angle) const { angle = 2 * theta_ - angle; }
  void collideBottom(double& angle) const { angle = -(2 * theta_ + angle); }

  void registerTopBottomCollision(double const& x, double& y, double const& a, double const& c,
                                          std::vector<double>& output) const;
  virtual void registerLeftCollision(double& x, double& y, const double& c, double& dir, LastHit& lastHit,
                                     std::vector<double>& output) const = 0;
  virtual void registerRightCollision(double const& x, double const& y, double const& a, double const& c, double const& dir,
                                      std::vector<double>& output) const = 0;

 public:
  Biliardo(double l, double r1, double r2, BiliardoType type);
  virtual ~Biliardo() = default;

  virtual BiliardoType type() const = 0;

  double const& l() const { return l_; }
  double const& r1() const { return r1_; }
  double const& r2() const { return r2_; }

  // Questo metodo è in grado di lanciare una sola particella e restituisce un
  // vettore contenente le posizioni di tutti gli urti tra essa e i bordi del
  // biliardo
  void launchForDrawing(double const& initialY, double const& initialDirection,
                                std::vector<double>& output) const;
  void launchForDrawing(std::vector<double>& output);  // non const perché l'rng ha bisogno di "cambiare" per funzionare

  // Questo metodo è in grado di lanciare multiple particelle con un unica
  // chiamata.
  // Restituisce un vettore contenente solo informazioni riguardo l'uscita delle
  // particelle dal biliardo
  std::vector<double> launch;
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_
