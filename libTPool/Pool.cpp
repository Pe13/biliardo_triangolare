//
// Created by paolo on 13/06/2023.
//

#include "Pool.hpp"

#include <TH1D.h>

#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <execution>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace bt {

bool operator==(const Pool &left, const Pool &right) {
  return left.l_ == right.l_ && left.r1_ == right.r1_ && left.r2_ == right.r2_ &&
         left.theta_ == right.theta_ && left.slope_ == right.slope_;
}

bool Pool::isOut_(const LastHit &lastHit) const {
  bool result{};
  switch (type_) {
    case open:
      result = true;
      break;

    case rightBounded:
      result = lastHit == left;
      break;

    case leftBounded:
      result = lastHit == right;
      break;

    default:
      assert(false);
  }
  return result;
}

void Pool::addParticleToMultipleLaunch_(std::vector<Particle> &launch,
                                        std::normal_distribution<double> &yNormalDist,
                                        std::normal_distribution<double> &thetaNormalDist) {
  double y;
  double direction;
  do {
    y = yNormalDist(rng_);
  } while (y <= -r1_ || y >= r1_);
  do {
    direction = thetaNormalDist(rng_);
  } while (direction <= -M_PI / 2 || direction >= M_PI / 2);

  launch.emplace_back(y, direction);
}

bool Pool::findNextCollision_(LastHit &lastHit, double &x, double &y, double &direction) const {
  bool out = false;

  // retta direttrice passante per il punto: ax + c
  const double a = std::tan(direction);
  const double c = y - a * x;

  // retta alla quale appartiene la sponda superiore (per ottenere quella inferiore basta prenderla
  // tutta con il meno): bx + d
  const double &b = slope_;
  const double &d = r1_;

  x = (d - c) / (a - b);  // ascissa dell'intersezione con la sponda superiore
  if (x > 0 && x < l_ && lastHit != top) {
    y = b * x + d;
    collideTop(direction);
    lastHit = top;
  } else {
    x = -(d + c) / (a + b);  // ascissa dell'intersezione con la sponda inferiore
    if (x > 0 && x < l_ && lastHit != bottom) {
      y = -b * x - d;
      collideBottom(direction);
      lastHit = bottom;
    } else if (std::abs(c) <= r1_ && lastHit != left) {
      x = 0;
      y = c;
      direction = -direction;
      lastHit = left;
      out = isOut_(lastHit);
    } else {
      x = l_;
      y = a * x + c;
      direction = -direction;
      lastHit = right;
      out = isOut_(lastHit);
    }
  }

  return out;
}

void Pool::launchForHistograms_(Particle &particle) const {
  LastHit lastHit{left};
  double x{0};

  while (!findNextCollision_(lastHit, x, particle.y, particle.direction)) {
  }
}
void Pool::multipleLaunch_(const unsigned int N, std::array<TH1D, 2> &histograms,
                           std::normal_distribution<double> &yNormalDist,
                           std::normal_distribution<double> &thetaNormalDist, const bool parallel) {
  std::vector<Particle> launch{};
  launch.reserve(N);
  for (unsigned int i = 0; i < N; i++) {
    addParticleToMultipleLaunch_(launch, yNormalDist, thetaNormalDist);
  }

  std::cout << "launching...\n";
  if (parallel) {
    std::for_each(std::execution::par_unseq, launch.begin(), launch.end(),
                  [this](auto &particle) { launchForHistograms_(particle); });
  } else {
    std::for_each(std::execution::seq, launch.begin(), launch.end(),
                  [this](auto &particle) { launchForHistograms_(particle); });
  }

  std::for_each(launch.begin(), launch.end(), [&](const auto &particle) {
    histograms[0].Fill(particle.y);
    histograms[1].Fill(particle.direction);
  });

  std::cout << "Done \n\n";
}

bool Pool::validateLaunchForDrawingInput_(const std::optional<double> &initialY,
                                          const std::optional<double> &initialDirection) const {
  if (initialY && std::abs(initialY.value()) >= r1_) {
    std::cerr << "Warning: il parametro initialY vale" << initialY.value()
              << "ma il suo modulo deve essere minore di " << r1_ << '\n';
    return false;
  }
  if (initialDirection && std::abs(initialDirection.value()) >= M_PI / 2) {
    std::cerr << "Warning: il parametro initialDirection vale" << initialDirection.value()
              << "ma il suo modulo deve essere minore di " << M_PI / 2 << '\n';
    return false;
  }
  return true;
}

void Pool::initializeLaunchForDrawingInput_(std::optional<double> &initialY,
                                            std::optional<double> &initialDirection) {
  if (!initialY) {
    initialY = (2. * uniformDist_(rng_) - 1) * r1_;
  }
  if (!initialDirection) {
    initialDirection = (2. * uniformDist_(rng_) - 1) * M_PI / 2;
  }
}

void Pool::launchForDrawing_(std::vector<double> &output, const double initialY,
                             const double initialDirection) const {
  LastHit lastHit{left};
  double x{0};
  double y{initialY};
  double direction{initialDirection};

  do {
    output.push_back(x);
    output.push_back(y);
  } while (!findNextCollision_(lastHit, x, y, direction));

  output.push_back(x);
  output.push_back(y);

  output.push_back(-direction);  // l'angolo è stato specchiato come se avesse rimbalzato
  output.push_back(initialDirection);
}

Pool::Pool(const double l, const double r1, const double r2, const PoolType type)
    : type_{type}, l_{l}, r1_{r1}, r2_{r2}, theta_{std::atan((r2_ - r1_) / l)} {
  if (l <= 0 || r1_ <= 0 || r2_ <= 0) {
    const std::array<std::string, 3> argNames = {"l", "r1", "r2"};
    const std::array<double *, 3> argList = {&l_, &r1_, &r2_};

    for (long unsigned i = 0; i < 3; i++) {
      if (*argList[i] <= 0) {
        throw std::invalid_argument("Il parametro \"" + argNames[i] +
                                    "\" deve essere positivo; è stato fornito " + argNames[i] +
                                    " = " + std::to_string(*argList[i]));
      }
    }
  }

  if (type > 2) {
    throw std::invalid_argument("Il tipo fornito per la costruzione del biliardo non è valido");
  }
}

PoolType Pool::type() const { return type_; }

bool Pool::changeType(const PoolType type) {
  if (type > 2) {
    std::cerr << "Warning: il tipo di biliardo richiesto non corrisponde a nessun tipo valido\n";
    return false;
  }
  type_ = type;
  return true;
}

bool Pool::l(const double l) {
  if (l <= 0) {
    std::cerr << "Warning: il parametro \"l\" deve essere positivo; è stato fornito l = "
              << std::to_string(l) << '\n';
    return false;
  }
  l_ = l;
  slope_ = (r2_ - r1_) / l_;
  theta_ = std::atan(slope_);
  return true;
}

bool Pool::r1(const double r1) {
  if (r1 <= 0) {
    std::cerr << "Warning: il parametro \"r1\" deve essere positivo; è stato fornito r1 = "
              << std::to_string(r1) << '\n';
    return false;
  }
  r1_ = r1;
  slope_ = (r2_ - r1_) / l_;
  theta_ = std::atan(slope_);
  return true;
}

bool Pool::r2(const double r2) {
  if (r2 <= 0) {
    std::cerr << "Warning: il parametro \"r2\" deve essere positivo; è stato fornito r2 = "
              << std::to_string(r2) << '\n';
    return false;
  }
  r2_ = r2;
  slope_ = (r2_ - r1_) / l_;
  theta_ = std::atan(slope_);
  return true;
}

bool Pool::modify(const double l, const double r1, const double r2) {
  if (r1 <= 0 || r2 <= 0 || l <= 0) {
    const std::array<std::string, 3> argNames = {"l", "r1", "r2"};
    const std::array<double *, 3> argList = {&l_, &r1_, &r2_};

    for (long unsigned i = 0; i < 3; i++) {
      if (*argList[i] <= 0) {
        std::cerr << "Warning: il parametro \"" + argNames[i] +
                         "\" deve essere positivo; è stato fornito " + argNames[i] + " = " +
                         std::to_string(*argList[i]);
      }
    }
    std::cerr << "Warning: il biliardo non è stato modificato.\n";
    return false;
  }

  r1_ = r1;
  r2_ = r2;
  l_ = l;

  slope_ = (r2_ - r1_) / l_;
  theta_ = std::atan(slope_);
  return true;
}

bool Pool::launchForDrawing(std::vector<double> &output, std::optional<double> initialY,
                            std::optional<double> initialDirection) {
  if (!validateLaunchForDrawingInput_(initialY, initialDirection)) {
    return false;
  }

  initializeLaunchForDrawingInput_(initialY, initialDirection);
  launchForDrawing_(output, initialY.value(), initialDirection.value());
  return true;
}

bool Pool::multipleLaunch(const unsigned int N, const double muY, const double sigmaY,
                          const double muT, const double sigmaT, std::array<TH1D, 2> &histograms,
                          const bool parallel) {
  if (sigmaY <= 0) {
    std::cerr
        << "Warning: il parametro \"sigmaY\" deve essere positivo; è stato fornito sigmaY = " +
               std::to_string(sigmaY) + '\n';
    return false;
  }
  if (sigmaT <= 0) {
    std::cerr
        << "Warning: il parametro \"sigmaT\" deve essere positivo; è stato fornito sigmaY = " +
               std::to_string(sigmaT) + '\n';
    return false;
  }

  auto yNormalDist = std::normal_distribution<double>(muY, sigmaY);
  auto thetaNormalDist = std::normal_distribution<double>(muT, sigmaT);

  // aggiorno il seed ad ogni chiamata così anche in caso di grandi generazioni di numeri la
  // sequenza non dovrebbe mai ripetersi
  rng_.seed(
      static_cast<long unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));

  multipleLaunch_(N, histograms, yNormalDist, thetaNormalDist, parallel);
  return true;
}

}  // namespace bt