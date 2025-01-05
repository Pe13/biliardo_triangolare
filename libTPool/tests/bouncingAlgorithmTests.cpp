//
// Created by paolo on 02/01/2025.
//

#include <cmath>

#include "Pool.hpp"
#include "doctest.h"

class Bouncer {
  const bt::Pool* pool_;
  double m_;

 public:
  explicit Bouncer(const bt::Pool& pool)
      : pool_{&pool}, m_{(pool_->r2() - pool_->r1()) / pool_->l()} {}

  [[nodiscard]] double topBounce(const double m) const {
    if (m_ == 0) {
      return -m;
    }
    const double _m_ = -1 / m_;  // coefficiente angolare della normale alla sponda superiore
    return (2 * _m_ + (_m_ * _m_ - 1) * m) / (1 - _m_ * _m_ + 2 * _m_ * m);
  }

  [[nodiscard]] double bottomBounce(const double m) const {
    if (m_ == 0) {
      return -m;
    }
    const double _m_ = 1 / m_;  // coefficiente angolare della normale alla sponda superiore
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

  void checkAngle(const double angle) const {
    double angleCopy = angle;
    double bouncedDir{};

    SUBCASE("collideTop") {
      bouncedDir = topBounce(std::tan(angle));
      pool_->collideTop(angleCopy);
    }

    SUBCASE("collideBottom") {
      bouncedDir = bottomBounce(std::tan(angle));
      pool_->collideBottom(angleCopy);
    }

    fixAngle(angleCopy);
    const double bouncedAngle = std::atan(bouncedDir);
    CHECK(doctest::Approx(bouncedAngle) == angleCopy);
  }
};

TEST_CASE("Testing bouncing algorithm consistency") {
  constexpr int poolCases = 21;
  static_assert(poolCases % 2 == 1);
  for (int i = -((poolCases - 1) / 2 - 1); i < (poolCases - 1) / 2 - 1; ++i) {
    bt::Pool pool{1, 50. + 50. * static_cast<double>(i) / ((poolCases - 1) / 2), 50};
    Bouncer bouncer{pool};
    constexpr int angleCases = 20;
    for (int j = 0; j < angleCases; j++) {
      double angle = -M_PI / 2 + M_PI * j / angleCases;
      SUBCASE("Checking that algorithm are involution functions") {
        SUBCASE("bt:Pool::collideTop") {
          double doubleBouncedAngle = angle;
          pool.collideTop(doubleBouncedAngle);
          pool.collideTop(doubleBouncedAngle);
          CHECK(doctest::Approx(angle) == doubleBouncedAngle);
        }

        SUBCASE("bt:Pool::collideBottom") {
          double doubleBouncedAngle = angle;
          pool.collideBottom(angle);
          pool.collideBottom(angle);
          CHECK(doctest::Approx(angle) == doubleBouncedAngle);
        }

        SUBCASE("Bouncer::topBounce") {
          const double doubleBouncedDir = bouncer.topBounce(bouncer.topBounce(std::tan(angle)));
          double doubleBouncedAngle = std::atan(doubleBouncedDir);
          CHECK(doctest::Approx(angle) == doubleBouncedAngle);
        }

        SUBCASE("Bouncer::bottomBounce") {
          const double doubleBouncedDir = bouncer.bottomBounce(bouncer.bottomBounce(std::tan(angle)));
          double doubleBouncedAngle = std::atan(doubleBouncedDir);
          CHECK(doctest::Approx(angle) == doubleBouncedAngle);
        }
      }
      SUBCASE("Checking algorithms compatibility") { bouncer.checkAngle(angle); }
    }
  }
}