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

struct BiliardoGenerator {
  static std::default_random_engine rng;

  template <typename D>
  static bt::Biliardo generate(D& distribution, bt::BiliardoType type = bt::leftBounded) {
    return {distribution(rng), distribution(rng), distribution(rng), type};
  }

  template <typename D>
  static std::array<double, 3> generateParameters(D& distribution) {
    return {distribution(rng), distribution(rng), distribution(rng)};
  }
};

auto BiliardoGenerator::rng = std::default_random_engine(
    boost::implicit_cast<unsigned long long>(std::chrono::system_clock::now().time_since_epoch().count()));

TEST_CASE("Testing Biliardo constructor input checking") {
  SUBCASE("Negative parameters") {
    auto uniformDist = std::uniform_real_distribution(0.1, 1.1);
    auto biliardo = bt::Biliardo(1, 1, 1);

    for (int _ = 0; _ < 10; _++) {
      double l = uniformDist(BiliardoGenerator::rng) < 0.51 ? uniformDist(BiliardoGenerator::rng)
                                                            : -uniformDist(BiliardoGenerator::rng);
      double r1 = uniformDist(BiliardoGenerator::rng) < 0.51 ? uniformDist(BiliardoGenerator::rng)
                                                             : -uniformDist(BiliardoGenerator::rng);
      double r2 = uniformDist(BiliardoGenerator::rng) < 0.51 ? uniformDist(BiliardoGenerator::rng)
                                                             : -uniformDist(BiliardoGenerator::rng);

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

TEST_CASE("Testing Biliardo::changeType") {
  auto biliardo = bt::Biliardo(1, 1, 1);

  SUBCASE("Testing that it works") {
    CHECK(biliardo.changeType(bt::leftBounded) == true);
    CHECK(biliardo.type() == bt::leftBounded);

    CHECK(biliardo.changeType(bt::rightBounded) == true);
    CHECK(biliardo.type() == bt::rightBounded);

    CHECK(biliardo.changeType(bt::open) == true);
    CHECK(biliardo.type() == bt::open);
  }

  SUBCASE("Testing its inviolability") {
    auto type = biliardo.type();
    CHECK(biliardo.changeType(static_cast<bt::BiliardoType>(-3)) == false);
    CHECK(biliardo.type() == type);  // controllo che il tipo non sia cambiato

    CHECK(biliardo.changeType(static_cast<bt::BiliardoType>(3)) == false);
    CHECK(biliardo.type() == type);
  }
}

TEST_CASE("Testing parameters setters") {
  auto biliardo = bt::Biliardo{1, 1, 1};
  auto positiveDistribution = std::uniform_real_distribution<double>(0.1, 100);

  SUBCASE("l") {
    auto isValueSetTo = [&biliardo](const double value) {
      CHECK(biliardo.l() == value);
      CHECK(biliardo.theta() == std::atan((biliardo.r2() - biliardo.r1()) / biliardo.l()));
    };

    double positiveValue = positiveDistribution(BiliardoGenerator::rng);
    biliardo.l(positiveValue);
    isValueSetTo(positiveValue);

    double negativeValue = -positiveValue;
    biliardo.l(negativeValue);
    isValueSetTo(positiveValue);

    biliardo.l(0);
    isValueSetTo(positiveValue);

    biliardo.l(negativeValue, false);
    isValueSetTo(negativeValue);
  }

  SUBCASE("r1") {
    auto isValueSetTo = [&biliardo](const double value) {
      CHECK(biliardo.r1() == value);
      CHECK(biliardo.theta() == std::atan((biliardo.r2() - biliardo.r1()) / biliardo.l()));
      CHECK(biliardo.yNormalDist().mean() == 0);
      CHECK(biliardo.yNormalDist().stddev() == biliardo.r1() / 5);
    };

    double positiveValue = positiveDistribution(BiliardoGenerator::rng);
    biliardo.r1(positiveValue);
    isValueSetTo(positiveValue);

    double negativeValue = -positiveValue;
    biliardo.r1(negativeValue);
    isValueSetTo(positiveValue);

    biliardo.r1(0);
    isValueSetTo(positiveValue);

    biliardo.r1(negativeValue, false);
    isValueSetTo(negativeValue);
  }

  SUBCASE("r2") {
    auto isValueSetTo = [&biliardo](const double value) {
      CHECK(biliardo.r2() == value);
      CHECK(biliardo.theta() == std::atan((biliardo.r2() - biliardo.r1()) / biliardo.l()));
    };

    double positiveValue = positiveDistribution(BiliardoGenerator::rng);
    biliardo.r2(positiveValue);
    isValueSetTo(positiveValue);

    double negativeValue = -positiveValue;
    biliardo.r2(negativeValue);
    isValueSetTo(positiveValue);

    biliardo.r2(0);
    isValueSetTo(positiveValue);

    biliardo.r2(negativeValue, false);
    isValueSetTo(negativeValue);
  }

  SUBCASE("modify") {
    auto areParametersSetTo = [&biliardo](const std::array<double, 3>& parameters) {
      CHECK(biliardo.r1() == parameters[0]);
      CHECK(biliardo.r2() == parameters[1]);
      CHECK(biliardo.l() == parameters[2]);
      CHECK(biliardo.theta() == std::atan((parameters[1] - parameters[0]) / parameters[2]));
      CHECK(biliardo.yNormalDist().mean() == 0);
      CHECK(biliardo.yNormalDist().stddev() == parameters[0] / 5);
    };

    std::array<double, 3> rightParameters = BiliardoGenerator::generateParameters(positiveDistribution);

    CHECK(biliardo.modify(rightParameters[0], rightParameters[1], rightParameters[2]) == true);
    areParametersSetTo(rightParameters);

    auto wrongParameters = rightParameters;
    wrongParameters[1] *= -1;
    CHECK(biliardo.modify(wrongParameters[0], wrongParameters[1], wrongParameters[2]) == false);
    areParametersSetTo(rightParameters);

    wrongParameters[2] *= -1;
    CHECK(biliardo.modify(wrongParameters[0], wrongParameters[1], wrongParameters[2]) == false);
    areParametersSetTo(rightParameters);

    wrongParameters[0] *= -1;
    CHECK(biliardo.modify(wrongParameters[0], wrongParameters[1], wrongParameters[2]) == false);
    areParametersSetTo(rightParameters);

    CHECK(biliardo.modify(wrongParameters[0], wrongParameters[1], wrongParameters[2], false) == true);
    areParametersSetTo(wrongParameters);
  }
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

    static void fixAngle(double& angle) {
      // il mio algoritmo gestisce e crea anche angoli fuori dall'intervallo [-PI/2; PI/2] mentre quello che sto
      // usando per testarlo no, quindi se il risultato esce dal range ce lo reinserisco
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
    auto uniformDist = std::uniform_real_distribution<double>(0.1, 1.1);
    auto anglesDist = std::uniform_real_distribution(-M_PI / 2, M_PI / 2);
    for (int _ = 0; _ < 10; _++) {
      auto biliardo = BiliardoGenerator::generate(uniformDist);
      auto bouncer = Bouncer{biliardo};

      std::array<double, 10> angles{};
      std::generate(angles.begin(), angles.end(), [&]() { return anglesDist(BiliardoGenerator::rng); });

      for (const double angle : angles) {
        bouncer.checkAngle(angle);
      }
    }
  }
}
