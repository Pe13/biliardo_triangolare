//
// Created by paolo on 13/06/2023.
//

#include "Biliardo.hpp"

#include <TH1D.h>

#include <array>
#include <chrono>
#include <cmath>
#include <execution>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace bt {

// TODO testare che i vari metodi con controllo dell'input funzionino

bool Biliardo::isOut(const LastHit &lastHit) const {
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
  }
  return result;
}

void Biliardo::addParticleToMultipleLaunch(std::vector<Particle> &launch) {
  double y;
  double direction;
  do {
    y = yNormalDist_(rng_);
  } while (y <= -r1_ || y >= r1_);
  do {
    direction = thetaNormalDist_(rng_);
  } while (direction <= -M_PI / 2 || direction >= M_PI / 2);

  launch.emplace_back(y, direction);
}

bool Biliardo::findNextCollision(CollisionParameters &parameters) const {
  bool out = false;

  // retta direttrice passante per il punto: ax + c
  double a = std::tan(parameters.direction);
  double c = parameters.y - a * parameters.x;

  // retta alla quale appartiene la sponda superiore (per ottenere quella inferiore basta prenderla
  // tutta con il meno): bx + d
  const double &b = slope_;
  const double &d = r1_;

  parameters.x = (d - c) / (a - b);  // ascissa dell'intersezione con la sponda superiore
  if (parameters.x > 0 && parameters.x < l_ && parameters.lastHit != top) {
    parameters.y = a * parameters.x + c;
    collideTop(parameters.direction);
    parameters.lastHit = top;
  } else {
    parameters.x = -(d + c) / (a + b);  // ascissa dell'intersezione con la sponda inferiore
    if (parameters.x > 0 && parameters.x < l_ && parameters.lastHit != bottom) {
      parameters.y = a * parameters.x + c;
      collideBottom(parameters.direction);
      parameters.lastHit = bottom;
    } else if (std::abs(c) <= r1_ && parameters.lastHit != left) {
      parameters.x = 0;
      parameters.y = c;
      parameters.direction = -parameters.direction;
      parameters.lastHit = left;
      out = isOut(parameters.lastHit);
    } else {
      parameters.x = l_;
      parameters.y = a * parameters.x + c;
      parameters.direction = -parameters.direction;
      parameters.lastHit = right;
      out = isOut(parameters.lastHit);
    }
  }

  return out;
}

void Biliardo::launchForHistograms(Particle &particle) const {
  CollisionParameters parameters = {
      left,                // lastHit
      0,                   // x
      particle.y,          // y
      particle.direction,  // direction
  };

  while (!findNextCollision(parameters)) {
  }

  particle.y = parameters.y;
  particle.direction = parameters.direction;
}

void Biliardo::syncLaunch(const unsigned int N, std::array<TH1D, 2> &histograms) {
  std::vector<Particle> launch{};
  launch.reserve(N);
  std::cout << "generating syncLaunch...\n";
  // idealmente potrei usare un while sulla size del vector ma sarebbe meno leggibile
  for (unsigned int i = 0; i < N; i++) {
    addParticleToMultipleLaunch(launch);
  }

  std::cout << "launching...\n";
  std::for_each(launch.begin(), launch.end(),
                [this](auto &particle) { launchForHistograms(particle); });

  std::cout << "Filling histograms...\n";
  std::for_each(launch.begin(), launch.end(), [&](const auto &particle) {
    histograms[0].Fill(particle.y);
    histograms[1].Fill(particle.direction);
  });

  std::cout << "Done \n\n";
}

void Biliardo::asyncLaunch(const unsigned int N, std::array<TH1D, 2> &histograms) {
  std::vector<Particle> launch;
  launch.reserve(N);
  std::cout << "generating asyncLaunch...\n";
  for (unsigned int i = 0; i < N; i++) {
    addParticleToMultipleLaunch(launch);
  }

  std::cout << "launching...\n";
  std::for_each(std::execution::par_unseq, launch.begin(), launch.end(),
                [this](auto &particle) { launchForHistograms(particle); });

  std::cout << "Filling histograms...\n";
  std::for_each(launch.begin(), launch.end(), [&](const auto &particle) {
    histograms[0].Fill(particle.y);
    histograms[1].Fill(particle.direction);
  });

  std::cout << "Done \n\n";
}

void Biliardo::launchForDrawing_(const double initialY, const double initialDirection,
                                 std::vector<double> &output) const {
  CollisionParameters parameters = {
      left,              // lastHit
      0,                 // x
      initialY,          // y
      initialDirection,  // direction
  };

  do {
    output.push_back(parameters.x);
    output.push_back(parameters.y);
  } while (!findNextCollision(parameters));

  output.push_back(parameters.x);
  output.push_back(parameters.y);

  output.push_back(-parameters.direction);  // l'angolo è stato specchiato come se avesse rimbalzato
  output.push_back(initialDirection);
}

Biliardo::Biliardo(double l, double r1, double r2, BiliardoType type)
    : type_{type}, l_{l}, r1_{r1}, r2_{r2}, theta_{std::atan((r2_ - r1_) / l)},
      yNormalDist_(0, r1_ / 5) {
  // controllo che i parametri siano validi
  if (l <= 0 || r1_ <= 0 || r2_ <= 0) {
    std::array<std::string, 3> argName = {"l", "r1", "r2"};
    std::array<double *, 3> argList = {&l_, &r1_, &r2_};

    for (int i = 0; i < 3; i++) {
      if (*argList[i] <
          0) {  // se sono negativi uso il loro modulo e informo l'utente con un warning
        *argList[i] = -*argList[i];
        if (i == 1) {  // nel caso r1 sia negativo modifico anche la distribuzione gaussiana delle y
          yNormalDist_ = std::normal_distribution<double>(0, r1_ / 5);
        }
        std::cerr << "Warning: il parametro \"" << argName[i]
                  << "\" fornito è negativo, al suo posto verrà stato utilizzato il suo modulo\n";
      } else if (*argList[i] == 0) {  // se invece anche solo uno è nullo lancio un'eccezione
        throw std::invalid_argument("Il parametro " + argName[i] + " fornito è nullo");
      }
    }
  }

  if (type > 2) {
    throw std::invalid_argument("Il tipo fornito per la costruzione del Biliardo non è valido");
  }
}

BiliardoType Biliardo::type() const { return type_; }

bool Biliardo::changeType(const bt::BiliardoType type) {
  if (type <= 2) {
    type_ = type;
    return true;
  }
  std::cerr << "Warning: il tipo di biliardo richiesto non corrisponde a nessun tipo valido\n";
  return false;
}

const double &Biliardo::l(const double l, const bool shouldCheck) {
  if (shouldCheck && l <= 0) {
    std::cerr << "il parametro \"l\" deve essere positivo; è stato fornito" << l << '\n';
  } else {
    l_ = l;
    theta_ = std::atan((r2_ - r1_) / l_);
  }
  return l_;
}

const double &Biliardo::r1(const double r1, const bool shouldCheck) {
  if (shouldCheck && r1 <= 0) {
    std::cerr << "il parametro \"r1\" deve essere positivo; è stato fornito " << r1 << '\n';
  } else {
    r1_ = r1;
    theta_ = std::atan((r2_ - r1_) / l_);
    yNormalDist_ = std::normal_distribution<double>(0, r1_ / 5);
  }
  return r1_;
}

const double &Biliardo::r2(const double r2, const bool shouldCheck) {
  if (shouldCheck && r2 <= 0) {
    std::cerr << "il parametro \"r2\" deve essere positivo; è stato fornito" << r2 << '\n';
  } else {
    r2_ = r2;
    theta_ = std::atan((r2_ - r1_) / l_);
  }
  return r2_;
}

bool Biliardo::modify(const double r1, const double r2, const double l, const bool shouldCheck) {
  if (shouldCheck && (r1 <= 0 || r2 <= 0 || l <= 0)) {
    std::array<std::string, 3> argName = {"l", "r1", "r2"};
    std::array<double *, 3> argList = {&l_, &r1_, &r2_};

    for (int i = 0; i < 3; i++) {
      if (*argList[i] <= 0) {
        std::cerr << "Warning: il parametro \"" << argName[i] << "\" fornito non è positivo\n";
      }
    }
    std::cerr << "Il biliardo non è stato modificato.\n";
    return false;
  }

  r1_ = r1;
  r2_ = r2;
  l_ = l;

  theta_ = std::atan((r2_ - r1_) / l_);
  yNormalDist_ = std::normal_distribution<double>(0, r1_ / 5);
  return true;
}

bool Biliardo::launchForDrawing(const double initialY, const double initialDirection,
                                std::vector<double> &output, bool shouldCheck) const {
  if (shouldCheck) {
    if (std::abs(initialY) > r1_) {
      std::cerr << "Warning: il parametro initialY vale" << initialY
                << "ma il suo modulo deve essere minore di " << r1_ << '\n';
      return false;
    }
    if (std::abs(initialDirection) > M_PI / 2) {
      std::cerr << "Warning: il parametro initialDirection vale" << initialDirection
                << "ma il suo modulo deve essere minore di " << M_PI / 2 << '\n';
      return false;
    }
  }
  launchForDrawing_(initialY, initialDirection, output);
  return true;
}

void Biliardo::launchForDrawing(std::vector<double> &output) {
  double initialY = (2 * uniformDist_(rng_) - 1) * r1_;
  double initialDirection = (2 * uniformDist_(rng_) - 1) * M_PI / 2;
  launchForDrawing_(initialY, initialDirection, output);
}

bool Biliardo::launchForDrawingNoY(const double initialDirection, std::vector<double> &output,
                                   bool shouldCheck) {
  if (shouldCheck && std::abs(initialDirection) > M_PI / 2) {
    std::cerr << "Warning: il parametro initialDirection vale" << initialDirection
              << "ma il suo modulo deve essere minore di " << M_PI / 2 << '\n';
    return false;
  }
  double initialY = (2 * uniformDist_(rng_) - 1) * r1_;
  launchForDrawing_(initialY, initialDirection, output);
  return true;
}

bool Biliardo::launchForDrawingNoDir(const double initialY, std::vector<double> &output,
                                     bool shouldCheck) {
  if (shouldCheck && std::abs(initialY) > r1_) {
    std::cerr << "Warning: il parametro initialY vale" << initialY
              << "ma il suo modulo deve essere minore di " << r1_ << '\n';
    return false;
  }
  double initialDirection = (2 * uniformDist_(rng_) - 1) * M_PI / 2;
  launchForDrawing_(initialY, initialDirection, output);
  return true;
}

void Biliardo::multipleLaunch(double muY, double sigmaY, double muT, double sigmaT, unsigned int N,
                              std::array<TH1D, 2> &histograms, bool async) {
  yNormalDist_ = std::normal_distribution<double>(muY, sigmaY);
  thetaNormalDist_ = std::normal_distribution<double>(muT, sigmaT);

  // aggiorno il seed ad ogni chiamata così anche in caso di grandi generazioni di numeri la
  // sequenza non dovrebbe mai ripetersi
  rng_.seed(std::chrono::system_clock::now().time_since_epoch().count());

  if (async) {
    asyncLaunch(N, histograms);
  } else {
    syncLaunch(N, histograms);
  }
}

}  // namespace bt