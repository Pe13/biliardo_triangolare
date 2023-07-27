//
// Created by paolo on 13/06/2023.
//

#include "Biliardo.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace bt {

Biliardo::Biliardo(double l, double r1, double r2, BiliardoType type)
    : type_{type}, l_{l}, r1_{r1}, r2_{r2}, theta_{std::atan((r2_ - r1_) / l)},
      rng_(std::chrono::system_clock::now().time_since_epoch().count()), yNormalDist_(0, r1_ / 5) {
  if (r1 <= 0 || r2 <= 0) {
    throw std::runtime_error("r1 e r2 devono essere maggiori di 0");
  }
}

Biliardo::Biliardo(const bt::Biliardo &biliardo, bt::BiliardoType type)
    : type_{type}, l_{biliardo.l()}, r1_{biliardo.r1()}, r2_{biliardo.r2()}, theta_{std::atan((r2_ - r1_) / l_)},
      rng_(std::chrono::system_clock::now().time_since_epoch().count()), yNormalDist_(0, r1_ / 5) {}

void Biliardo::registerTopBottomCollision(const double &x, double &y, const double &a, const double &c,
                                          std::vector<double> &output) const {
  y = a * x + c;
  output.push_back(x);
  output.push_back(y);
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
            registerRightCollision(x, y, a, c, direction, lastHit, output);
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
            registerLeftCollision(x, y, c, direction, lastHit, output);
            break;
          }
        }
        break;

      case top:
        if (std::abs(c) < r1_) {
          registerLeftCollision(x, y, c, direction, lastHit, output);
          break;
        } else {
          x = (-d - c) / (a + b);
          if (x > 0 && x < l_) {
            registerTopBottomCollision(x, y, a, c, output);
            collideBottom(direction);
            lastHit = bottom;
          } else {
            registerRightCollision(x, y, a, c, direction, lastHit, output);
            break;
          }
        }
        break;
      case bottom:
        if (std::abs(c) < r1_) {
          registerLeftCollision(x, y, c, direction, lastHit, output);
          break;
        } else {
          x = (d - c) / (a - b);
          if (x > 0 && x < l_) {
            registerTopBottomCollision(x, y, a, c, output);
            collideTop(direction);
            lastHit = top;
          } else {
            registerRightCollision(x, y, a, c, direction, lastHit, output);
            break;
          }
        }
        break;
    }
  }
}

void Biliardo::launchForDrawing(std::vector<double> &output) {
  double initialY = (2 * uniformDist_(rng_) - 1) * r1_;
  double initialDirection = (2 * uniformDist_(rng_) - 1) * M_PI / 2;
  std::cout << "y: " << initialY << "\n";
  std::cout << "theta: " << initialDirection << "\n";
  launchForDrawing(initialY, initialDirection, output);
}

void Biliardo::launch(long long N, std::vector<double>& output) {
  output.reserve(2 * N);
  for (long long _ = 0; _ < N; _++) {
    double x = 0;
    double y, direction;
    do {
      y = yNormalDist_(rng_);
    } while (y >= -r1_ && y <= r1_);
    do {
      direction = thetaNormalDist_(rng_);
    } while (direction >= -M_PI / 2 && direction <= M_PI / 2);
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
    output.push_back(y);
    output.push_back(direction);
  }
}

}  // namespace bt