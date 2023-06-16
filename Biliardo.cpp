//
// Created by paolo on 13/06/2023.
//

#include "Biliardo.hpp"

#include <cmath>
#include <vector>
#include <stdexcept>

namespace bt {

Biliardo::Biliardo(double l, double r1, double r2) : l_{l}, r1_{r1}, r2_{r2}, theta_{std::atan((r2_ - r1_) / l)} {
  if (r1 <= 0 || r2 <= 0) {
    throw std::runtime_error("r1 e r2 devono essere maggiori di 0");
  }
}

void Biliardo::launchForDrawing(const double& initialX, const double& initialY, const double& initialDirection,
                                std::vector<double>& output) const {
  enum LastHit { left, top, bottom };
  double x = initialX;
  double y = initialY;
  double dir = initialDirection;
  LastHit lastHit = left;

  output.push_back(x);
  output.push_back(y);

  // ad ogni iterazione vengono aggiunti due elementi al vettore ma nell'ultima viene aggiunto alla fine anche l'angolo
  // di uscita, quindi il vettore alla fine dell'ultima iterazione avrà un numero di elementi dispari
  while (output.size() % 2 == 0) {
    // retta direttrice passante per il punto: ax + c
    double a = std::tan(dir);
    double c = y - std::tan(dir) * x;

    // retta alla quale appartiene la sponda superiore (per ottenere quella inferiore basta prenderla tutta con il
    // meno): bx + d
    double b = std::tan(theta_);
    double d = -r1_;

    // queste lambda mi permettono di avere un codice più snello nello switch
    auto commitX = [&]() {
      y = a * x + c;
      output.push_back(x);
      output.push_back(y);
    };

    auto commitY = [&]() {
      collideLeft(dir);
      x = 0;
      y = c;
      output.push_back(x);
      output.push_back(y);
      lastHit = left;
    };

    auto particleOut = [&]() {
      output.push_back(l_);
      output.push_back(a * l_ + c);
      output.push_back(dir);
    };

    switch (lastHit) {
      case left:
        x = (d - c) / (a - b);  // ascissa dell'intersezione con la sponda superiore
        if (x > 0 && x < l_) {
          commitX();
          collideTop(dir);
          lastHit = top;
        } else {
          x = (-d - c) / (a + b);  // ascissa dell'intersezione con la sponda inferiore
          if (x > 0 && x < l_) {
            commitX();
            collideBottom(dir);
            lastHit = bottom;
          } else {
            particleOut();
          }
        }
        break;

      case top:
        if (std::abs(c) < r1_) {
          commitY();
          break;
        } else {
          x = (-d - c) / (a + b);
          if (x > 0 && x < l_) {
            commitX();
            collideBottom(dir);
            lastHit = bottom;
          } else {
            particleOut();
          }
        }
        break;
      case bottom:
        if (std::abs(c) < r1_) {
          commitY();
          break;
        } else {
          x = (d - c) / (a - b);
          if (x > 0 && x < l_) {
            commitX();
            collideTop(dir);
            lastHit = top;
          } else {
            particleOut();
          }
        }
        break;
    }
  }
}

}  // namespace bt