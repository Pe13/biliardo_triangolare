//
// Created by paolo on 23/05/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_

#include "Biliardo.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>

namespace bt {

class App {
  sf::RenderWindow window_;
  sf::Event event_{};

  Biliardo biliardo_;

  double ratio_{};
  double xOffset_{};
  double yOffset_{};

  std::array<sf::Vertex, 4> bordiBiliardo_{};


 public:
  App(double l, double r1, double r2);

  void handleEvent();

  void draw();

  void run();

  void drawBiliardo();
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
