//
// Created by paolo on 13/06/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_

#include <array>
#include <iostream>
#include <random>
#include <vector>

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

  bool isOut(const LastHit& lastHit) const;

 public:
  Biliardo(double l, double r1, double r2, BiliardoType type);
  ~Biliardo() = default;

  BiliardoType type() const;
  void changeType(BiliardoType type);

  const double& l() const { return l_; }
  const double& r1() const { return r1_; }
  const double& r2() const { return r2_; }

  /// no by reference se no ambigua
  // metodi per settare l, r1, e r2; ritornano delle const reference così si possono settare i parametri e al contempo
  // immagazzinare una loro reference
  // passare false come secondo argomento se si è gia controllato che il valore fornito sia positivo
  // in caso di errore il parametro non viene modificato
  template <class T>
  const double& l(T l, bool shouldCheck = true) {
    static_assert(std::is_arithmetic_v<T>);
    if (shouldCheck) {
      if (l <= 0) {
        std::cerr << "il parametro \"l\" deve essere positivo; è stato fornito" << l << '\n';
        return l_;
      }
    }
    l_ = double(l);
    theta_ = std::atan((r2_ - r1_) / l_);
    return l_;
  }
  template <class T>
  const double& r1(T r1, bool shouldCheck = true) {
    static_assert(std::is_arithmetic_v<T>);
    if (shouldCheck) {
      if (r1 <= 0) {
        std::cerr << "il parametro \"r1\" deve essere positivo; è stato fornito " << r1 << '\n';
        return r1_;
      }
    }
    r1_ = double(r1);
    theta_ = std::atan((r2_ - r1_) / l_);
    yNormalDist_ = std::normal_distribution<double>(0, r1_ / 5);
    return r1_;
  }
  template <class T>
  const double& r2(T r2, bool shouldCheck = true) {
    static_assert(std::is_arithmetic_v<T>);
    if (shouldCheck) {
      if (r2 <= 0) {
        std::cerr << "il parametro \"r2\" deve essere positivo; è stato fornito" << r2 << '\n';
        return r2_;
      }
    }
    r2_ = double(r2);
    theta_ = std::atan((r2_ - r1_) / l_);
    return r2_;
  }

  // Questo metodo è in grado di lanciare una sola particella e riempie un
  // vettore con le posizioni di tutti gli urti tra essa e i bordi del
  // biliardo, la direzione finale e quella iniziale
  void launchForDrawing(double const& initialY, double const& initialDirection, std::vector<double>& output);
  // questi prima generano le condizioni iniziali mancanti poi chiama il metodo sopra
  void launchForDrawing(std::vector<double>& output);
  void launchForDrawingNoY(double const& initialDirection, std::vector<double>& output);
  void launchForDrawingNoDir(double const& initialY, std::vector<double>& output);

  // Questo metodo è in grado di lanciare multiple particelle con un unica
  // chiamata.
  // Restituisce un vettore contenente solo informazioni riguardo l'uscita delle
  // particelle dal biliardo
  void launch(const unsigned int N, std::vector<double>& output);
  void multipleLaunch(const float muY, const float sigmaY, const float muT, const float sigmaT, const unsigned int N,
                      std::vector<double>& output);
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_
