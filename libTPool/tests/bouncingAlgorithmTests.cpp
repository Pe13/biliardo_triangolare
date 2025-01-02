//
// Created by paolo on 02/01/2025.
//

#include <cmath>

#include "Biliardo.hpp"
#include "doctest.h"

class Bouncer {
  const bt::Biliardo* biliardo_;
  double m_;

 public:
  explicit Bouncer(const bt::Biliardo& biliardo)
      : biliardo_{&biliardo}, m_{(biliardo_->r2() - biliardo_->r1()) / biliardo_->l()} {}

  [[nodiscard]] double topBounce(const double m) const {
    if (m_ == 0) {
      return -m;
    }
    double _m_ = -1 / m_;  // coefficiente angolare della normale alla sponda superiore
    return (2 * _m_ + (_m_ * _m_ - 1) * m) / (1 - _m_ * _m_ + 2 * _m_ * m);
  }

  [[nodiscard]] double bottomBounce(const double m) const {
    if (m_ == 0) {
      return -m;
    }
    double _m_ = 1 / m_;  // coefficiente angolare della normale alla sponda superiore
    return (2 * _m_ + (_m_ * _m_ - 1) * m) / (1 - _m_ * _m_ + 2 * _m_ * m);
  }

  static void fixAngle(double& angle) {
    // il mio algoritmo gestisce e crea anche angoli fuori dall'intervallo [-PI/2; PI/2] mentre
    // quello che sto usando per testarlo no, quindi se il risultato esce dal range ce lo
    // reinserisco
    if (angle > M_PI / 2) {
      angle -= M_PI;
    } else if (angle < -M_PI / 2) {
      angle += M_PI;
    }
  }

  void checkAngle(double angle) const {
    SUBCASE("collideTop") {
      double bouncedDir = topBounce(std::tan(angle));
      double bouncedAngle = std::atan(bouncedDir);
      double angleCopy = angle;
      biliardo_->collideTop(angleCopy);
      fixAngle(angleCopy);

      CHECK(doctest::Approx(bouncedAngle) == angleCopy);
    }

    SUBCASE("collideBottom") {
      double bouncedDir = bottomBounce(std::tan(angle));
      double bouncedAngle = std::atan(bouncedDir);
      double angleCopy = angle;
      biliardo_->collideBottom(angleCopy);
      fixAngle(angleCopy);

      CHECK(doctest::Approx(bouncedAngle) == angleCopy);
    }
  }
};

TEST_CASE("Testing bouncing algorithm consistency") {
  constexpr int biliardoCases = 21;
  static_assert(biliardoCases % 2 == 1);
  for (int i = -((biliardoCases - 1) / 2 - 1); i < (biliardoCases - 1) / 2 - 1; ++i) {
    bt::Biliardo biliardo{1, 50. + 50. * static_cast<double>(i) / ((biliardoCases - 1) / 2), 50};
    Bouncer bouncer{biliardo};
    constexpr int angleCases = 20;
    for (int j = 0; j < angleCases; j++) {
      double angle = -M_PI / 2 + M_PI * j / angleCases;
      SUBCASE("Checking that algorithm are involution functions") {
        SUBCASE("bt:Biliardo::collideTop") {
          double doubleBouncedAngle = angle;
          biliardo.collideTop(doubleBouncedAngle);
          biliardo.collideTop(doubleBouncedAngle);
          CHECK(doctest::Approx(angle) == doubleBouncedAngle);
        }

        SUBCASE("bt:Biliardo::collideBottom") {
          double doubleBouncedAngle = angle;
          biliardo.collideBottom(angle);
          biliardo.collideBottom(angle);
          CHECK(doctest::Approx(angle) == doubleBouncedAngle);
        }

        SUBCASE("Bouncer::topBounce") {
          double doubleBouncedDir = bouncer.topBounce(bouncer.topBounce(std::tan(angle)));
          double doubleBouncedAngle = std::atan(doubleBouncedDir);
          CHECK(doctest::Approx(angle) == doubleBouncedAngle);
        }

        SUBCASE("Bouncer::bottomBounce") {
          double doubleBouncedDir = bouncer.bottomBounce(bouncer.bottomBounce(std::tan(angle)));
          double doubleBouncedAngle = std::atan(doubleBouncedDir);
          CHECK(doctest::Approx(angle) == doubleBouncedAngle);
        }
      }
      SUBCASE("Checking algorithms compatibility") { bouncer.checkAngle(angle); }
    }
  }
}