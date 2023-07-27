//
// Created by paolo on 26/07/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_BILIARDOCHIUSOSX_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_BILIARDOCHIUSOSX_HPP_

#include "Biliardo.hpp"

namespace bt {

class BiliardoChiusoSx : public Biliardo {
  void collideLeft(double& angle) const { angle = -angle; }

 protected:
  void registerLeftCollision(double& x, double& y, const double& c, double& dir, LastHit& lastHit,
                             std::vector<double>& output) const override;
  void registerRightCollision(double& x, double& y, const double& a, const double& c,
                              double& dir, LastHit& lastHit, std::vector<double>& output) const override;


 public:
  BiliardoChiusoSx(double l, double r1, double r2);
  BiliardoChiusoSx(Biliardo const& biliardo);
  //  ~BiliardoChiusoSx() = default;

  BiliardoType type() const override;
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_BILIARDOCHIUSOSX_HPP_
