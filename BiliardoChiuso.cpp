//
// Created by paolo on 26/07/2023.
//

#include "BiliardoChiuso.hpp"

namespace bt {

BiliardoChiuso::BiliardoChiuso(double l, double r1, double r2) : Biliardo(l, r1, r2, leftBounded){}

void BiliardoChiuso::registerLeftCollision(double& x, double& y, const double& c, double& dir, LastHit& lastHit,
                                           std::vector<double>& output) const {
  collideLeft(dir);
  x = 0;
  y = c;
  output.push_back(x);
  output.push_back(y);
  lastHit = left;
}
void BiliardoChiuso::registerRightCollision(const double& x, const double& y, const double& a, const double& c,
                                            const double& dir, std::vector<double>& output) const {
  output.push_back(l_);
  output.push_back(a * l_ + c);
  output.push_back(dir);
}
BiliardoType BiliardoChiuso::type() const { return leftBounded; }
}  // namespace bt