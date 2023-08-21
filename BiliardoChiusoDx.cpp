//
// Created by paolo on 27/07/2023.
//

#include "BiliardoChiusoDx.hpp"

namespace bt {

void BiliardoChiusoDx::registerLeftCollision(double& x, double& y, const double& c, double& dir, LastHit& lastHit,
                                             std::vector<double>& output) const {
  output.push_back(0);
  output.push_back(c);
  output.push_back(dir);
}

void BiliardoChiusoDx::registerRightCollision(double& x, double& y, const double& a, const double& c, double& dir,
                                              LastHit& lastHit, std::vector<double>& output) const {
  collideRight(dir);
  x = l_;
  y = c + a * l_;
  output.push_back(x);
  output.push_back(y);
  lastHit = right;
}

bool BiliardoChiusoDx::isOut(const LastHit lastHit) const {
  if (lastHit == left) {return true;}
  return false;
}

BiliardoChiusoDx::BiliardoChiusoDx(double l, double r1, double r2) : Biliardo(l, r1, r2, rightBounded) {}

BiliardoChiusoDx::BiliardoChiusoDx(const Biliardo *biliardo) : Biliardo(biliardo, rightBounded) {}

BiliardoType BiliardoChiusoDx::type() const { return rightBounded; }

}  // namespace bt