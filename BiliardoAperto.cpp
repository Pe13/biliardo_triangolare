//
// Created by paolo on 26/07/2023.
//

#include "BiliardoAperto.hpp"

#include <chrono>
#include <stdexcept>

namespace bt {

BiliardoAperto::BiliardoAperto(double l, double r1, double r2) : Biliardo(l, r1, r2, open) {}
BiliardoAperto::BiliardoAperto(const bt::Biliardo& biliardo) : Biliardo(biliardo, open) {}

void BiliardoAperto::registerLeftCollision(double& x, double& y, const double& c, double& dir, LastHit& lastHit,
                                           std::vector<double>& output) const {
  output.push_back(0);
  output.push_back(c);
  output.push_back(dir);
}

void BiliardoAperto::registerRightCollision(double& x, double& y, const double& a, const double& c, double& dir,
                                            LastHit& lastHit, std::vector<double>& output) const {
  output.push_back(l_);
  output.push_back(a * l_ + c);
  output.push_back(dir);
}

bool BiliardoAperto::isOut(const bt::LastHit lastHit) const {
  return true;
}

BiliardoType BiliardoAperto::type() const { return open; }
}  // namespace bt