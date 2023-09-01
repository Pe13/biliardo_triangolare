//
// Created by paolo on 29/08/2023.
//

#include "BiliardoFunctions.hpp"

#include <vector>

#include "types.hpp"

namespace bt {

void BiliardoFunctions::leftCollisionOut(const double& c, const double& direction, std::vector<double>& output) const {
  output.push_back(0);
  output.push_back(c);
  output.push_back(direction);
}

void BiliardoFunctions::leftCollision(double& x, double& y, const double& c, double& direction, LastHit& lastHit,
                                      std::vector<double>& output) const {
  collideLeft(direction);
  x = 0;
  y = c;
  output.push_back(x);
  output.push_back(y);
  lastHit = left;
}

void BiliardoFunctions::rightCollisionOut(const double& a, const double& c, const double& direction, const double& l,
                                          std::vector<double>& output) const {
  output.push_back(l);
  output.push_back(a * l + c);
  output.push_back(direction);
}

void BiliardoFunctions::rightCollision(double& x, double& y, const double& a, const double& c, LastHit& lastHit,
                                       double& direction, const double& l, std::vector<double>& output) const {
  collideRight(direction);
  x = l;
  y = c + a * l;
  output.push_back(x);
  output.push_back(y);
  lastHit = right;
}

void BiliardoFunctions::registerLeftCollision(const bt::BiliardoType type, double& x, double& y, const double& c,
                                              double& direction, LastHit& lastHit, std::vector<double>& output) const {
  if (type == leftBounded) {
    leftCollision(x, y, c, direction, lastHit, output);
  } else {
    leftCollisionOut(c, direction, output);
  }
}

void BiliardoFunctions::registerRightCollision(const bt::BiliardoType type, double& x, double& y, const double& a,
                                               const double& c, LastHit& lastHit, double& direction, const double& l,
                                               std::vector<double>& output) const {
  if (type == rightBounded) {
    rightCollision(x, y, a, c, lastHit, direction, l, output);
  } else {
    rightCollisionOut(a, c, direction, l, output);
  }
}

}  // namespace bt