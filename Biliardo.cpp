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

Biliardo::Biliardo(double l, double r1, double r2, BiliardoType type)
    : type_{type}, l_{l}, r1_{r1}, r2_{r2}, theta_{std::atan((r2_ - r1_) / l)}, yNormalDist_(0, r1_ / 5) {

  // controllo che i parametri siano validi
  if (l <= 0 || r1_ <= 0 || r2_ <= 0) {
    std::array<std::string, 3> argName = {"l", "r1", "r2"};
    std::array<double *, 3> argList = {&l_, &r1_, &r2_};

    for (int i = 0; i < 3; i++) {
      if (*argList[i] < 0) { // se sono negativi uso il loro modulo e informo l'utente con un warning
        *argList[i] = -*argList[i];
        if (i == 1) { // nel caso r1 sia negativo modifico anche la distribuzione gaussiana delle y
          yNormalDist_ = std::normal_distribution<double>(0, r1_ / 5);
        }
        std::cerr << "Warning: il parametro \"" << argName[i]
                  << "\" fornito è negativo, al suo posto verrà stato utilizzato il suo modulo\n";
      } else if (*argList[i] == 0) { // se invece anche solo uno è nullo lancio un'eccezione
        throw std::invalid_argument("Il parametro " + argName[i] + " fornito è nullo");
      }
    }
  }

  if (type < 0 || type > 2) {
    throw std::invalid_argument("Il tipo fornito per la costruzione del Biliardo non è valido");
  }
}

BiliardoType Biliardo::type() const { return type_; }

bool Biliardo::changeType(const bt::BiliardoType type) {
  if (type >= 0 && type <= 2) {
    type_ = type;
    return true;
  }
  std::cerr << "Warning: il tipo di biliardo richiesto non corrisponde a nessun tipo valido\n";
  return false;
}

void Biliardo::registerTopBottomCollision(const double &x, double &y, const double &a, const double &c,
                                          std::vector<double> &output) const {
  y = a * x + c;
  output.push_back(x);
  output.push_back(y);
}

bool Biliardo::isOut(const LastHit &lastHit) const {
  switch (type_) {
    case open:
      return true;

    case rightBounded:
      if (lastHit == left) {
        return true;
      }
      return false;

    case leftBounded:
      if (lastHit == right) {
        return true;
      }
      return false;

    default:
      return false;
  }
}

void Biliardo::launchForDrawing(const double &initialY, const double &initialDirection,
                                std::vector<double> &output) const {
  double x = 0;
  double y = initialY;
  double direction = initialDirection;
  LastHit lastHit = left;

  output.push_back(x);
  output.push_back(y);

  // ad ogni iterazione vengono aggiunti due elementi al vettore ma nell'ultima viene aggiunto alla fine anche l'angolo
  // di uscita, quindi il vettore alla fine dell'ultima iterazione avrà un numero di elementi dispari
  while (output.size() % 2 == 0) {
    // retta direttrice passante per il punto: ax + c
    double a = std::tan(direction);
    double c = y - std::tan(direction) * x;

    // retta alla quale appartiene la sponda superiore (per ottenere quella inferiore basta prenderla tutta con il
    // meno): bx + d
    double b = std::tan(theta_);
    double d = r1_;

    switch (lastHit) {
      case left:
        x = (d - c) / (a - b);  // ascissa dell'intersezione con la sponda superiore
        if (x > 0 && x < l_) {
          registerTopBottomCollision(x, y, a, c, output);
          collideTop(direction);
          lastHit = top;
        } else {
          x = (-d - c) / (a + b);  // ascissa dell'intersezione con la sponda inferiore
          if (x > 0 && x < l_) {
            registerTopBottomCollision(x, y, a, c, output);
            collideBottom(direction);
            lastHit = bottom;
          } else {
            functions_.registerRightCollision(type_, x, y, a, c, lastHit, direction, l_, output);
            break;
          }
        }
        break;

      case right:
        x = (d - c) / (a - b);  // ascissa dell'intersezione con la sponda superiore
        if (x > 0 && x < l_) {
          registerTopBottomCollision(x, y, a, c, output);
          collideTop(direction);
          lastHit = top;
        } else {
          x = (-d - c) / (a + b);  // ascissa dell'intersezione con la sponda inferiore
          if (x > 0 && x < l_) {
            registerTopBottomCollision(x, y, a, c, output);
            collideBottom(direction);
            lastHit = bottom;
          } else {
            functions_.registerLeftCollision(type_, x, y, c, direction, lastHit, output);
            break;
          }
        }
        break;

      case top:
        if (std::abs(c) < r1_) {
          functions_.registerLeftCollision(type_, x, y, c, direction, lastHit, output);
          break;
        } else {
          x = (-d - c) / (a + b);
          if (x > 0 && x < l_) {
            registerTopBottomCollision(x, y, a, c, output);
            collideBottom(direction);
            lastHit = bottom;
          } else {
            functions_.registerRightCollision(type_, x, y, a, c, lastHit, direction, l_, output);
            break;
          }
        }
        break;
      case bottom:
        if (std::abs(c) < r1_) {
          functions_.registerLeftCollision(type_, x, y, c, direction, lastHit, output);
          break;
        } else {
          x = (d - c) / (a - b);
          if (x > 0 && x < l_) {
            registerTopBottomCollision(x, y, a, c, output);
            collideTop(direction);
            lastHit = top;
          } else {
            functions_.registerRightCollision(type_, x, y, a, c, lastHit, direction, l_, output);
            break;
          }
        }
        break;
    }
  }
  output.push_back(initialDirection);
}

Biliardo::Biliardo(double l, double r1, double r2, BiliardoType type)
    : type_{type}, l_{l}, r1_{r1}, r2_{r2}, theta_{std::atan((r2_ - r1_) / l)}, yNormalDist_(0, r1_ / 5) {
  // controllo che i parametri siano validi
  if (l <= 0 || r1_ <= 0 || r2_ <= 0) {
    std::array<std::string, 3> argName = {"l", "r1", "r2"};
    std::array<double *, 3> argList = {&l_, &r1_, &r2_};

    for (int i = 0; i < 3; i++) {
      if (*argList[i] < 0) {  // se sono negativi uso il loro modulo e informo l'utente con un warning
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

  if (type < 0 || type > 2) {
    throw std::invalid_argument("Il tipo fornito per la costruzione del Biliardo non è valido");
  }
}

BiliardoType Biliardo::type() const { return type_; }

bool Biliardo::changeType(const bt::BiliardoType type) {
  if (type >= 0 && type <= 2) {
    type_ = type;
    return true;
  }
  std::cerr << "Warning: il tipo di biliardo richiesto non corrisponde a nessun tipo valido\n";
  return false;
}

const double &Biliardo::l(double l, bool shouldCheck) {
  if (shouldCheck && l <= 0) {
    std::cerr << "il parametro \"l\" deve essere positivo; è stato fornito" << l << '\n';
    return l_;
  }
  l_ = l;
  theta_ = std::atan((r2_ - r1_) / l_);
  return l_;
}

const double &Biliardo::r1(double r1, bool shouldCheck) {
  if (shouldCheck && r1 <= 0) {
    std::cerr << "il parametro \"r1\" deve essere positivo; è stato fornito " << r1 << '\n';
    return r1_;
  }
  r1_ = r1;
  theta_ = std::atan((r2_ - r1_) / l_);
  yNormalDist_ = std::normal_distribution<double>(0, r1_ / 5);
  return r1_;
}

const double &Biliardo::r2(double r2, bool shouldCheck) {
  if (shouldCheck && r2 <= 0) {
    std::cerr << "il parametro \"r2\" deve essere positivo; è stato fornito" << r2 << '\n';
    return r2_;
  }
  r2_ = r2;
  theta_ = std::atan((r2_ - r1_) / l_);
  return r2_;
}
bool Biliardo::modify(double r1, double r2, double l, bool shouldCheck) {
  if (shouldCheck && (r1 <= 0 || r2 <= 0 || l <= 0)) {
    std::array<std::string, 3> argName = {"l", "r1", "r2"};
    std::array<double *, 3> argList = {&l_, &r1_, &r2_};

    for (int i = 0; i < 3; i++) {
      if (*argList[i] <= 0) {
        std::cerr << "Warning: il parametro \"" << argName[i]
                  << "\" fornito non è positivo\n";
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

bool Biliardo::launchForDrawing(const double &initialY, const double &initialDirection, std::vector<double> &output,
                                bool shouldCheck) {
  if (shouldCheck) {
    if (std::abs(initialY) > r1_) {
      std::cerr << "Warning: il parametro initialY vale" << initialY << "ma il suo modulo deve essere minore di " << r1_
                << '\n';
      return false;
    }
    if (std::abs(initialDirection) > M_PI / 2) {
      std::cerr << "Warning: il parametro initialDirection vale" << initialDirection
                << "ma il suo modulo deve essere minore di " << M_PI / 2 << '\n';
      return false;
    }
  }
  launchForDrawing(initialY, initialDirection, output);
  return true;
}

void Biliardo::launchForDrawing(std::vector<double> &output) {
  double initialY = (2 * uniformDist_(rng_) - 1) * r1_;
  double initialDirection = (2 * uniformDist_(rng_) - 1) * M_PI / 2;
  launchForDrawing(initialY, initialDirection, output);
}

void Biliardo::launchForDrawingNoY(const double &initialDirection, std::vector<double> &output) {
  double initialY = (2 * uniformDist_(rng_) - 1) * r1_;
  launchForDrawing(initialY, initialDirection, output);
}

bool Biliardo::launchForDrawingNoY(const double &initialDirection, std::vector<double> &output, bool shouldCheck) {
  if (shouldCheck && std::abs(initialDirection) > M_PI / 2) {
    std::cerr << "Warning: il parametro initialDirection vale" << initialDirection
              << "ma il suo modulo deve essere minore di " << M_PI / 2 << '\n';
    return false;
  }
  double initialY = (2 * uniformDist_(rng_) - 1) * r1_;
  launchForDrawing(initialY, initialDirection, output);
  return true;
}

void Biliardo::launchForDrawingNoDir(const double &initialY, std::vector<double> &output) {
  double initialDirection = (2 * uniformDist_(rng_) - 1) * M_PI / 2;
  launchForDrawing(initialY, initialDirection, output);
}

bool Biliardo::launchForDrawingNoDir(const double &initialY, std::vector<double> &output, bool shouldCheck) {
  if (shouldCheck && std::abs(initialY) > r1_) {
    std::cerr << "Warning: il parametro initialY vale" << initialY << "ma il suo modulo deve essere minore di " << r1_
              << '\n';
    return false;
  }
  double initialDirection = (2 * uniformDist_(rng_) - 1) * M_PI / 2;
  launchForDrawing(initialY, initialDirection, output);
  return true;
}

void Biliardo::syncLaunch(const unsigned int N, std::array<TH1D, 2> &histograms) {
  for (unsigned int _ = 0; _ < N; _++) {
    double x = 0;
    double y;
    double direction;
    do {
      y = yNormalDist_(rng_);
    } while (y <= -r1_ && y >= r1_);
    do {
      direction = thetaNormalDist_(rng_);
    } while (direction <= -M_PI / 2 && direction >= M_PI / 2);

    LastHit lastHit = left;

    bool out = false;
    while (!out) {
      // retta direttrice passante per il punto: ax + c
      double a = std::tan(direction);
      double c = y - std::tan(direction) * x;

      // retta alla quale appartiene la sponda superiore (per ottenere quella inferiore basta prenderla tutta con il
      // meno): bx + d
      double b = std::tan(theta_);
      double d = r1_;

      switch (lastHit) {
        case left:
          x = (d - c) / (a - b);  // ascissa dell'intersezione con la sponda superiore
          if (x > 0 && x < l_) {
            y = a * x + c;
            collideTop(direction);
            lastHit = top;
          } else {
            x = (-d - c) / (a + b);  // ascissa dell'intersezione con la sponda inferiore
            if (x > 0 && x < l_) {
              y = a * x + c;
              collideBottom(direction);
              lastHit = bottom;
            } else {
              x = l_;
              y = a * x + c;
              direction = -direction;
              lastHit = right;
              out = isOut(lastHit);

              break;
            }
          }
          break;

        case right:
          x = (d - c) / (a - b);  // ascissa dell'intersezione con la sponda superiore
          if (x > 0 && x < l_) {
            y = a * x + c;
            collideTop(direction);
            lastHit = top;
          } else {
            x = (-d - c) / (a + b);  // ascissa dell'intersezione con la sponda inferiore
            if (x > 0 && x < l_) {
              y = a * x + c;
              collideBottom(direction);
              lastHit = bottom;
            } else {
              x = 0;
              y = c;
              direction = -direction;
              lastHit = left;
              out = isOut(lastHit);
              break;
            }
          }
          break;

        case top:
          if (std::abs(c) < r1_) {
            x = 0;
            y = c;
            direction = -direction;
            lastHit = left;
            out = isOut(lastHit);
            break;
          } else {
            x = (-d - c) / (a + b);
            if (x > 0 && x < l_) {
              y = a * x + c;
              collideBottom(direction);
              lastHit = bottom;
            } else {
              x = l_;
              y = a * x + c;
              direction = -direction;
              lastHit = right;
              out = isOut(lastHit);
              break;
            }
          }
          break;
        case bottom:
          if (std::abs(c) < r1_) {
            x = 0;
            y = c;
            direction = -direction;
            lastHit = left;
            out = isOut(lastHit);
            break;
          } else {
            x = (d - c) / (a - b);
            if (x > 0 && x < l_) {
              y = a * x + c;
              collideTop(direction);
              lastHit = top;
            } else {
              x = l_;
              y = a * x + c;
              direction = -direction;
              lastHit = right;
              out = isOut(lastHit);
              break;
            }
          }
          break;
      }
    }
    histograms[0].Fill(y);
    histograms[1].Fill(direction);
  }
}

void Biliardo::launch(const unsigned int N, std::array<TH1D, 2> &histograms) {
  std::vector<std::array<double, 2>> v(N);

  std::cout << "generating launch...\n";
  std::generate(v.begin(), v.end(), [&]() -> std::array<double, 2> {
    double y;
    double direction;
    do {
      y = yNormalDist_(rng_);
    } while (y <= -r1_ && y >= r1_);
    do {
      direction = thetaNormalDist_(rng_);
    } while (direction <= -M_PI / 2 && direction >= M_PI / 2);

    return {y, direction};
  });

  std::cout << "launching...\n";
  std::for_each(std::execution::par_unseq, v.begin(), v.end(), [&](auto &launch) {
    LastHit lastHit = left;
    double x = 0;

    double &y = launch[0];
    double &direction = launch[1];

    bool out = false;
    while (!out) {
      // retta direttrice passante per il punto: ax + c
      double a = std::tan(direction);
      double c = y - std::tan(direction) * x;

      // retta alla quale appartiene la sponda superiore (per ottenere quella inferiore basta prenderla tutta con il
      // meno): bx + d
      double b = std::tan(theta_);
      double d = r1_;

      switch (lastHit) {
        case left:
          x = (d - c) / (a - b);  // ascissa dell'intersezione con la sponda superiore
          if (x > 0 && x < l_) {
            y = a * x + c;
            collideTop(direction);
            lastHit = top;
          } else {
            x = (-d - c) / (a + b);  // ascissa dell'intersezione con la sponda inferiore
            if (x > 0 && x < l_) {
              y = a * x + c;
              collideBottom(direction);
              lastHit = bottom;
            } else {
              x = l_;
              y = a * x + c;
              direction = -direction;
              lastHit = right;
              out = isOut(lastHit);

              break;
            }
          }
          break;

        case right:
          x = (d - c) / (a - b);  // ascissa dell'intersezione con la sponda superiore
          if (x > 0 && x < l_) {
            y = a * x + c;
            collideTop(direction);
            lastHit = top;
          } else {
            x = (-d - c) / (a + b);  // ascissa dell'intersezione con la sponda inferiore
            if (x > 0 && x < l_) {
              y = a * x + c;
              collideBottom(direction);
              lastHit = bottom;
            } else {
              x = 0;
              y = c;
              direction = -direction;
              lastHit = left;
              out = isOut(lastHit);
              break;
            }
          }
          break;

        case top:
          if (std::abs(c) < r1_) {
            x = 0;
            y = c;
            direction = -direction;
            lastHit = left;
            out = isOut(lastHit);
            break;
          } else {
            x = (-d - c) / (a + b);
            if (x > 0 && x < l_) {
              y = a * x + c;
              collideBottom(direction);
              lastHit = bottom;
            } else {
              x = l_;
              y = a * x + c;
              direction = -direction;
              lastHit = right;
              out = isOut(lastHit);
              break;
            }
          }
          break;
        case bottom:
          if (std::abs(c) < r1_) {
            x = 0;
            y = c;
            direction = -direction;
            lastHit = left;
            out = isOut(lastHit);
            break;
          } else {
            x = (d - c) / (a - b);
            if (x > 0 && x < l_) {
              y = a * x + c;
              collideTop(direction);
              lastHit = top;
            } else {
              x = l_;
              y = a * x + c;
              direction = -direction;
              lastHit = right;
              out = isOut(lastHit);
              break;
            }
          }
          break;
      }
    }
  });

  std::cout << "Filling histograms...\n";
  std::for_each(v.begin(), v.end(), [&](const auto &arr) {
    std::for_each(std::execution::par_unseq, arr.begin(), arr.end(),
                  [&](const auto &item) { histograms[&item - arr.data()].Fill(item); });
  });

  std::cout << "Done \n\n";
}

void Biliardo::syncMultipleLaunch(const double muY, const double sigmaY, const double muT, const double sigmaT,
                                  const unsigned int N, std::array<TH1D, 2> &histograms) {
  yNormalDist_ = std::normal_distribution<double>(muY, sigmaY);
  thetaNormalDist_ = std::normal_distribution<double>(muT, sigmaT);

  // aggiorno il seed ad ogni chiamata così anche in caso di grandi generazioni di numeri la sequenza non dovrebbe mai
  // ripetersi
  rng_.seed(std::chrono::system_clock::now().time_since_epoch().count());

  syncLaunch(N, histograms);
}

void Biliardo::multipleLaunch(const double muY, const double sigmaY, const double muT, const double sigmaT,
                              const unsigned int N, std::array<TH1D, 2> &histograms) {
  yNormalDist_ = std::normal_distribution<double>(muY, sigmaY);
  thetaNormalDist_ = std::normal_distribution<double>(muT, sigmaT);

  // aggiorno il seed ad ogni chiamata così anche in caso di grandi generazioni di numeri la sequenza non dovrebbe mai
  // ripetersi
  rng_.seed(std::chrono::system_clock::now().time_since_epoch().count());

  launch(N, histograms);
}

}  // namespace bt