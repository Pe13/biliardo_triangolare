////
//// Created by paolo on 29/08/2023.
////
//
//#ifndef BILIARDO_TRIANGOLARE_BILIARDOFUNCTIONS_HPP_
//#define BILIARDO_TRIANGOLARE_BILIARDOFUNCTIONS_HPP_
//
//#include <functional>
//
//#include "Biliardo.hpp"
//
//namespace bt {
//
//class Biliardo::BiliardoFunctions {
//  void collideLeft(double& angle) const { angle = -angle; }
//  void collideRight(double& angle) const { angle = -angle; }
//
//  void leftCollisionOut(const double& c, const double& direction, std::vector<double>& output) const;
//
//  void leftCollision(double& x, double& y, const double& c, double& direction, LastHit& lastHit,
//                     std::vector<double>& output) const;
//
//  void rightCollisionOut(const double& a, const double& c, const double& direction, const double& l,
//                         std::vector<double>& output) const;
//  void rightCollision(double& x, double& y, const double& a, const double& c, LastHit& lastHit, double& direction,
//                      const double& l, std::vector<double>& output) const;
//
// public:
//  void registerLeftCollision(BiliardoType type, double& x, double& y, const double& c, double& direction,
//                             LastHit& lastHit, std::vector<double>& output) const;
//
//  void registerRightCollision(const BiliardoType type, double& x, double& y, const double& a, const double& c,
//                              LastHit& lastHit, double& direction, const double& l, std::vector<double>& output) const;
//};
//
//}  // namespace bt
//
//#endif  // BILIARDO_TRIANGOLARE_BILIARDOFUNCTIONS_HPP_
