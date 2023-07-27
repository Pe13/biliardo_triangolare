//
// Created by paolo on 26/07/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_BILIARDOCHIUSO_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_BILIARDOCHIUSO_HPP_

#include "Biliardo.hpp"

namespace bt {

class BiliardoChiuso : public Biliardo {
  void collideLeft(double& angle) const { angle = -angle; }

 protected:
  void registerLeftCollision(double& x, double& y, const double& c, double& dir, LastHit& lastHit,
                             std::vector<double>& output) const override;
  void registerRightCollision(double const& x, double const& y, double const& a, double const& c, double const& dir,
                              std::vector<double>& output) const override;

 public:
  BiliardoChiuso(double l, double r1, double r2);

  BiliardoType type() const override;
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_BILIARDOCHIUSO_HPP_
