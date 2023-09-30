//
// Created by paolo on 13/06/2023.
//

#include "Biliardo.hpp"

#include <TH1D.h>

#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <execution>

namespace bt {

// TODO testare il costruttore
// TODO testare l'algoritmo che calcola i rimbalzi
// TODO testare che changeType non permetta di assegnare un tipo invalido
// TODO testare che i vari metodi con controllo dell'input funzionino

Biliardo::Biliardo(double l, double r1, double r2, BiliardoType type)
    : type_{type}, l_{l}, r1_{r1}, r2_{r2}, theta_{std::atan((r2_ - r1_) / l)},
      rng_(std::chrono::system_clock::now().time_since_epoch().count()), yNormalDist_(0, r1_ / 5) {
  if (r1 <= 0 || r2 <= 0) {
    throw std::runtime_error("r1 e r2 devono essere maggiori di 0");
  }
  if (type < 0 || type > 2) {
    throw std::runtime_error("Il tipo fornito per la costruzione del Biliardo non è valido");
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

void Biliardo::launchForDrawing(const double &initialY, const double &initialDirection, std::vector<double> &output) {
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
  if (shouldCheck) {
    if (std::abs(initialDirection) > M_PI / 2) {
      std::cerr << "Warning: il parametro initialDirection vale" << initialDirection
                << "ma il suo modulo deve essere minore di " << M_PI / 2 << '\n';
      return false;
    }
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
  if (shouldCheck) {
    if (std::abs(initialY) > r1_) {
      std::cerr << "Warning: il parametro initialY vale" << initialY << "ma il suo modulo deve essere minore di " << r1_
                << '\n';
      return false;
    }
  }
  double initialDirection = (2 * uniformDist_(rng_) - 1) * M_PI / 2;
  launchForDrawing(initialY, initialDirection, output);
  return true;
}

 void Biliardo::syncLaunch(const unsigned int N, std::array<TH1D, 2> &histograms) {
   //  output.reserve(2 * N);
   for (unsigned int _ = 0; _ < N; _++) {
     double x = 0;
     double y, direction;
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
    double y, direction;
    do {
      y = yNormalDist_(rng_);
    } while (y <= -r1_ && y >= r1_);
    do {
      direction = thetaNormalDist_(rng_);
    } while (direction <= -M_PI / 2 && direction >= M_PI / 2);

    return {y, direction};
  });

  std::cout << "launching...\n";
  std::for_each(std::execution::par, v.begin(), v.end(), [&](auto &launch) {
    LastHit lastHit = left;
    double x = 0;

    double& y = launch[0];
    double& direction = launch[1];

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
    std::for_each(std::execution::par_unseq, arr.begin(), arr.end(), [&] (const auto &item) {
      histograms[&item - arr.data()].Fill(item);
    });
  });

  std::cout << "Done \n\n";
}

void Biliardo::syncMultipleLaunch(const float muY, const float sigmaY, const float muT, const float sigmaT,
                              const unsigned int N, std::array<TH1D, 2> &histograms) {
  yNormalDist_ = std::normal_distribution<double>(muY, sigmaY);
  thetaNormalDist_ = std::normal_distribution<double>(muT, sigmaT);

  // aggiorno il seed ad ogni chiamata così anche in caso di grandi generazioni di numeri la sequenza non dovrebbe mai
  // ripetersi
  rng_.seed(std::chrono::system_clock::now().time_since_epoch().count());

  syncLaunch(N, histograms);
}

void Biliardo::multipleLaunch(const float muY, const float sigmaY, const float muT, const float sigmaT,
                              const unsigned int N, std::array<TH1D, 2> &histograms) {
  yNormalDist_ = std::normal_distribution<double>(muY, sigmaY);
  thetaNormalDist_ = std::normal_distribution<double>(muT, sigmaT);

  // aggiorno il seed ad ogni chiamata così anche in caso di grandi generazioni di numeri la sequenza non dovrebbe mai
  // ripetersi
  rng_.seed(std::chrono::system_clock::now().time_since_epoch().count());

  launch(N, histograms);
}

}  // namespace bt