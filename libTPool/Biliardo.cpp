//
// Created by paolo on 13/06/2023.
//

#include "Biliardo.hpp"

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

bool operator==(const Biliardo &left, const Biliardo &right) {
  return left.l_ == right.l_ && left.r1_ == right.r1_ && left.r2_ == right.r2_ &&
         left.theta_ == right.theta_ && left.slope_ == right.slope_;
}

bool Biliardo::isOut_(const LastHit &lastHit) const {
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

void Biliardo::addParticleToMultipleLaunch_(std::vector<Particle> &launch,
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

bool Biliardo::findNextCollision_(CollisionParameters &parameters) const {
  bool out = false;

  // retta direttrice passante per il punto: ax + c
  const double a = std::tan(parameters.direction);
  const double c = parameters.y - a * parameters.x;

  // retta alla quale appartiene la sponda superiore (per ottenere quella inferiore basta prenderla
  // tutta con il meno): bx + d
  const double &b = slope_;
  const double &d = r1_;

  parameters.x = (d - c) / (a - b);  // ascissa dell'intersezione con la sponda superiore
  if (parameters.x > 0 && parameters.x < l_ && parameters.lastHit != top) {
    parameters.y = b * parameters.x + d;
    collideTop(parameters.direction);
    parameters.lastHit = top;
  } else {
    parameters.x = -(d + c) / (a + b);  // ascissa dell'intersezione con la sponda inferiore
    if (parameters.x > 0 && parameters.x < l_ && parameters.lastHit != bottom) {
      parameters.y = -b * parameters.x - d;
      collideBottom(parameters.direction);
      parameters.lastHit = bottom;
    } else if (std::abs(c) <= r1_ && parameters.lastHit != left) {
      parameters.x = 0;
      parameters.y = c;
      parameters.direction = -parameters.direction;
      parameters.lastHit = left;
      out = isOut_(parameters.lastHit);
    } else {
      parameters.x = l_;
      parameters.y = a * parameters.x + c;
      parameters.direction = -parameters.direction;
      parameters.lastHit = right;
      out = isOut_(parameters.lastHit);
    }
  }

  return out;
}

void Biliardo::launchForHistograms_(Particle &particle) const {
  CollisionParameters parameters = {
      left,                // lastHit
      0,                   // x
      particle.y,          // y
      particle.direction,  // direction
  };

  while (!findNextCollision_(parameters)) {
  }

  particle.y = parameters.y;
  particle.direction = parameters.direction;
}

void Biliardo::syncLaunch_(const unsigned int N, std::array<TH1D, 2> &histograms,
                           std::normal_distribution<double> &yNormalDist,
                           std::normal_distribution<double> &thetaNormalDist) {
  std::vector<Particle> launch{};
  launch.reserve(N);
  std::cout << "generating syncLaunch...\n";
  // idealmente potrei usare un while sulla size del vector ma sarebbe meno leggibile
  for (unsigned int i = 0; i < N; i++) {
    addParticleToMultipleLaunch_(launch, yNormalDist, thetaNormalDist);
  }

  std::cout << "launching...\n";
  std::for_each(launch.begin(), launch.end(),
                [this](auto &particle) { launchForHistograms_(particle); });

  std::cout << "Filling histograms...\n";
  std::for_each(launch.begin(), launch.end(), [&](const auto &particle) {
    histograms[0].Fill(particle.y);
    histograms[1].Fill(particle.direction);
  });

  std::cout << "Done \n\n";
}

void Biliardo::asyncLaunch_(const unsigned int N, std::array<TH1D, 2> &histograms,
                            std::normal_distribution<double> &yNormalDist,
                            std::normal_distribution<double> &thetaNormalDist) {
  std::vector<Particle> launch;
  launch.reserve(N);
  std::cout << "generating asyncLaunch...\n";
  for (unsigned int i = 0; i < N; i++) {
    addParticleToMultipleLaunch_(launch, yNormalDist, thetaNormalDist);
  }

  std::cout << "launching...\n";
  std::for_each(std::execution::par_unseq, launch.begin(), launch.end(),
                [this](auto &particle) { launchForHistograms_(particle); });

  std::cout << "Filling histograms...\n";
  std::for_each(launch.begin(), launch.end(), [&](const auto &particle) {
    histograms[0].Fill(particle.y);
    histograms[1].Fill(particle.direction);
  });

  std::cout << "Done \n\n";
}

bool Biliardo::validateLaunchForDrawingInput_(const std::optional<double> &initialY,
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

void Biliardo::initializeLaunchForDrawingInput_(std::optional<double> &initialY,
                                                std::optional<double> &initialDirection) {
  if (!initialY) {
    initialY = (2. * uniformDist_(rng_) - 1) * r1_;
  }
  if (!initialDirection) {
    initialDirection = (2. * uniformDist_(rng_) - 1) * M_PI / 2;
  }
}

void Biliardo::launchForDrawing_(std::vector<double> &output, const double initialY,
                                 const double initialDirection) const {
  CollisionParameters parameters = {
      left,              // lastHit
      0,                 // x
      initialY,          // y
      initialDirection,  // direction
  };

  do {
    output.push_back(parameters.x);
    output.push_back(parameters.y);
  } while (!findNextCollision_(parameters));

  output.push_back(parameters.x);
  output.push_back(parameters.y);

  output.push_back(-parameters.direction);  // l'angolo è stato specchiato come se avesse rimbalzato
  output.push_back(initialDirection);
}

Biliardo::Biliardo(const double l, const double r1, const double r2, const BiliardoType type)
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
    throw std::invalid_argument("Il tipo fornito per la costruzione del Biliardo non è valido");
  }
}

BiliardoType Biliardo::type() const { return type_; }

bool Biliardo::changeType(const BiliardoType type) {
  if (type <= 2) {
    type_ = type;
    return true;
  }
  std::cerr << "Warning: il tipo di biliardo richiesto non corrisponde a nessun tipo valido\n";
  return false;
}

Biliardo &Biliardo::l(const double l) {
  if (l <= 0) {
    throw std::invalid_argument(
        std::string{"Il parametro \"l\" deve essere positivo; è stato fornito l = "} +
        std::to_string(l));
  }
  l_ = l;
  slope_ = (r2_ - r1_) / l_;
  theta_ = std::atan(slope_);
  return *this;
}

Biliardo &Biliardo::r1(const double r1) {
  if (r1 <= 0) {
    throw std::invalid_argument("Il parametro \"r1\" deve essere positivo; è stato fornito r1 = " +
                                std::to_string(r1));
  }
  r1_ = r1;
  slope_ = (r2_ - r1_) / l_;
  theta_ = std::atan(slope_);
  return *this;
}

Biliardo &Biliardo::r2(const double r2) {
  if (r2 <= 0) {
    throw std::invalid_argument("Il parametro \"r2\" deve essere positivo; è stato fornito r2 = " +
                                std::to_string(r2));
  }
  r2_ = r2;
  slope_ = (r2_ - r1_) / l_;
  theta_ = std::atan(slope_);
  return *this;
}

bool Biliardo::modify(const double l, const double r1, const double r2) {
  if (r1 <= 0 || r2 <= 0 || l <= 0) {
    const std::array<std::string, 3> argNames = {"l", "r1", "r2"};
    const std::array<double *, 3> argList = {&l_, &r1_, &r2_};

    for (long unsigned i = 0; i < 3; i++) {
      if (*argList[i] <= 0) {
        std::cerr << "Warning: il parametro \"" << argNames[i] << "\": " << *argList[i]
                  << " fornito non è positivo\n";
      }
    }
    std::cerr << "Il biliardo non è stato modificato.\n";
    return false;
  }

  r1_ = r1;
  r2_ = r2;
  l_ = l;

  slope_ = (r2_ - r1_) / l_;
  theta_ = std::atan(slope_);
  return true;
}

bool Biliardo::launchForDrawing(std::vector<double> &output, std::optional<double> initialY,
                                std::optional<double> initialDirection) {
  if (!validateLaunchForDrawingInput_(initialY, initialDirection)) {
    return false;
  }

  initializeLaunchForDrawingInput_(initialY, initialDirection);
  launchForDrawing_(output, initialY.value(), initialDirection.value());
  return true;
}

void Biliardo::multipleLaunch(const unsigned int N, const double muY, const double sigmaY,
                              const double muT, const double sigmaT,
                              std::array<TH1D, 2> &histograms, const bool async) {
  auto yNormalDist = std::normal_distribution<double>(muY, sigmaY);
  auto thetaNormalDist = std::normal_distribution<double>(muT, sigmaT);

  // aggiorno il seed ad ogni chiamata così anche in caso di grandi generazioni di numeri la
  // sequenza non dovrebbe mai ripetersi
  rng_.seed(
      static_cast<long unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));

  if (async) {
    asyncLaunch_(N, histograms, yNormalDist, thetaNormalDist);
  } else {
    syncLaunch_(N, histograms, yNormalDist, thetaNormalDist);
  }
}

}  // namespace bt