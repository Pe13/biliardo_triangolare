//
// Created by paolo on 19/06/2023.
//

#ifndef BILIARDO_TRIANGOLARE__DRAWER_HPP_
#define BILIARDO_TRIANGOLARE__DRAWER_HPP_

#include <SFML/Graphics.hpp>
#include <array>
#include <vector>

namespace bt {

class Designer {
  std::vector<double>* points_{nullptr};

  double ratio_{};
  double xOffset_{};
  double yOffset_{};

  std::array<sf::Vertex, 4> bordiBiliardo_{};

  bool isDrawing_{false};
  bool hasCleared_{false};
  bool isPaused_{false};

  size_t pointIndex_{0};
  float pathFraction_{0};
  float step_{0};
  float speed_{100};  // pixel / s

  std::array<double, 100> contrail_{};

  sf::CircleShape particle_{5};

  void calcBordiBiliardo(double l, double r1, double r2);
  void calcStep();

 public:
  Designer(double l, double r1, double r2);

  void setPoints(std::vector<double>* points);
  void reRun();
  void pause();

  void operator()(sf::RenderWindow& window);

  template <typename num>
  sf::Vector2f toSfmlCord(num x, num y) const {
    static_assert(std::is_arithmetic<num>::value);
    return {static_cast<float>(xOffset_ + x * ratio_), static_cast<float>(360 - y * ratio_)};
  }
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE__DRAWER_HPP_
