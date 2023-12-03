//
// Created by paolo on 14/06/2023.
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <algorithm>
#include <array>
#include <boost/implicit_cast.hpp>
#include <chrono>
#include <cmath>
#include <random>
#include <stdexcept>

#include "Biliardo.hpp"
#include "doctest.h"

TEST_CASE("Testing Biliardo constructor") {
  SUBCASE("Negative parameters") {
    auto uniformDist = std::uniform_real_distribution(0.1, 1.1);
    auto rng = std::default_random_engine(
        boost::implicit_cast<unsigned long long>(std::chrono::system_clock::now().time_since_epoch().count()));
    auto biliardo = bt::Biliardo(1, 1, 1);

    for (int _ = 0; _ < 10; _++) {
      double l = uniformDist(rng) < 0.51 ? uniformDist(rng) : -uniformDist(rng);
      double r1 = uniformDist(rng) < 0.51 ? uniformDist(rng) : -uniformDist(rng);
      double r2 = uniformDist(rng) < 0.51 ? uniformDist(rng) : -uniformDist(rng);

      biliardo = bt::Biliardo(l, r1, r2);
      CHECK(doctest::Approx(biliardo.l()) == std::abs(l));
      CHECK(doctest::Approx(biliardo.r1()) == std::abs(r1));
      CHECK(doctest::Approx(biliardo.r2()) == std::abs(r2));
    }
  }

  SUBCASE("Null parameters") {
    CHECK_THROWS_WITH_AS(bt::Biliardo(0, 1, -1), "Il parametro l fornito è nullo", std::invalid_argument);
    CHECK_THROWS_WITH_AS(bt::Biliardo(2, 0, -1), "Il parametro r1 fornito è nullo", std::invalid_argument);
    CHECK_THROWS_WITH_AS(bt::Biliardo(2, 1, 0), "Il parametro r2 fornito è nullo", std::invalid_argument);
  }

  SUBCASE("Invalid BiliardoType") {
    CHECK_THROWS_AS(bt::Biliardo(1, 1, 1, static_cast<bt::BiliardoType>(3)), std::invalid_argument);
    CHECK_THROWS_AS(bt::Biliardo(1, 1, 1, static_cast<bt::BiliardoType>(-1)), std::invalid_argument);
    CHECK_NOTHROW(bt::Biliardo(1, 1, 1, static_cast<bt::BiliardoType>(1)));
  }
}

TEST_CASE("Testing Biliardo::changeType inviolability") {
  auto biliardo = bt::Biliardo(1, 1, 1);

  CHECK(biliardo.changeType(bt::leftBounded) == true);
  CHECK(biliardo.type() == bt::leftBounded);

  CHECK(biliardo.changeType(bt::open) == true);
  CHECK(biliardo.type() == bt::open);

  CHECK(biliardo.changeType(static_cast<bt::BiliardoType>(-3)) == false);
  CHECK(biliardo.type() == bt::open);  // controllo che il tipo non sia cambiato

  CHECK(biliardo.changeType(static_cast<bt::BiliardoType>(3)) == false);
  CHECK(biliardo.type() == bt::open);
}

TEST_CASE("Testing bouncing algorithm consistency") {
  class Bouncer {
    const bt::Biliardo* biliardo_;

    double m_;

   public:
    explicit Bouncer(const bt::Biliardo& biliardo)
        : biliardo_{&biliardo}, m_{(biliardo_->r2() - biliardo_->r1()) / biliardo_->l()} {}

    [[nodiscard]] double topBounce(const double m) const {
      if (m_ == 0) {
        return -m;
      } else {
        double _m_ = -1 / m_;  // coefficiente angolare della normale alla sponda superiore
        return (2 * _m_ + (_m_ * _m_ - 1) * m) / (1 - _m_ * _m_ + 2 * _m_ * m);
      }
    }

    [[nodiscard]] double bottomBounce(const double m) const {
      if (m_ == 0) {
        return -m;
      } else {
        double _m_ = 1 / m_;  // coefficiente angolare della normale alla sponda superiore
        return (2 * _m_ + (_m_ * _m_ - 1) * m) / (1 - _m_ * _m_ + 2 * _m_ * m);
      }
    }

    void checkAngle(double angle) const {
      // controllo che entrambi gli algoritmi applicati due volte ritornino l'angolo di partenza
      double doubleBouncedAngle = angle;
      biliardo_->collideTop(doubleBouncedAngle);
      biliardo_->collideTop(doubleBouncedAngle);
      CHECK(doctest::Approx(angle) == doubleBouncedAngle);
    }
  };

  SUBCASE("Checking that algorithm are involution functions") {
    constexpr int biliardoCases = 21;
    static_assert(biliardoCases % 2 == 1);
    for (int i = -((biliardoCases - 1) / 2 - 1); i < (biliardoCases - 1) / 2 - 1; i++) {
      auto biliardo = bt::Biliardo{1, 50. + 50. * static_cast<double>(i) / ((biliardoCases - 1) / 2), 50};
      auto bouncer = Bouncer{biliardo};
      const int angleCases = 20;
      for (int j = 0; j < angleCases; j++) {
        double angle = -M_PI / 2 + M_PI * j / angleCases;
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
    }
  }

  SUBCASE("Randomized tests") {
    auto rng = std::default_random_engine(
        boost::implicit_cast<unsigned long long>(std::chrono::system_clock::now().time_since_epoch().count()));
    auto uniformDist = std::uniform_real_distribution(0.1, 1.1);
    auto anglesDist = std::uniform_real_distribution(-M_PI / 2, M_PI / 2);
    for (int _ = 0; _ < 10; _++) {
      double l = uniformDist(rng);
      double r1 = uniformDist(rng);
      double r2 = uniformDist(rng);

      auto biliardo = bt::Biliardo(l, r1, r2);
      auto bouncer = Bouncer{biliardo};

      std::array<double, 10> angles{};
      std::generate(angles.begin(), angles.end(), [&]() { return anglesDist(rng); });

      for (const double angle : angles) {
        SUBCASE("collideTop") {
          double bouncedDir = bouncer.topBounce(std::tan(angle));
          double bouncedAngle = std::atan(bouncedDir);
          double angleCopy = angle;
          biliardo.collideTop(angleCopy);
          // il mio algoritmo gestisce e crea anche angoli fuori dall'intervallo [-PI/2; PI/2] mentre quello che sto
          // usando per testarlo no, quindi se il risultato esce dal range ce lo reinserisco
          if (angleCopy > M_PI / 2) {
            angleCopy -= M_PI;
          } else if (angleCopy < -M_PI / 2) {
            angleCopy += M_PI;
          }
          CHECK(doctest::Approx(bouncedAngle) == angleCopy);
        }

        SUBCASE("collideBottom") {
          double bouncedDir = bouncer.bottomBounce(std::tan(angle));
          double bouncedAngle = std::atan(bouncedDir);
          double angleCopy = angle;
          biliardo.collideBottom(angleCopy);
          // il mio algoritmo gestisce e crea anche angoli fuori dall'intervallo [-PI/2; PI/2] mentre quello che sto
          // usando per testarlo no, quindi se il risultato esce dal range ce lo reinserisco
          if (angleCopy > M_PI / 2) {
            angleCopy -= M_PI;
          } else if (angleCopy < -M_PI / 2) {
            angleCopy += M_PI;
          }
          CHECK(doctest::Approx(bouncedAngle) == angleCopy);
        }
      }
    }
  }
}