//
// Created by paolo on 27/07/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_BILIARDOCHIUSODX_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_BILIARDOCHIUSODX_HPP_

#include "Biliardo.hpp"

namespace bt {

class BiliardoChiusoDx : public Biliardo {
  void collideRight(double& angle) const { angle = -angle; }

 protected:
  void registerLeftCollision(double& x, double& y, const double& c, double& dir, LastHit& lastHit,
                             std::vector<double>& output) const override;
  void registerRightCollision(double& x, double& y, double const& a, double const& c, double& dir, LastHit& lastHit,
                              std::vector<double>& output) const override;

 public:
  BiliardoChiusoDx(double l, double r1, double r2);
  BiliardoChiusoDx(Biliardo const& biliardo);
  //  ~Biliardo

  BiliardoType type() const override;
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_BILIARDOCHIUSODX_HPP_
