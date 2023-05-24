//
// Created by paolo on 23/05/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_

#include <SFML/Graphics.hpp>
#include <iostream>

namespace bt {

class App {
  sf::RenderWindow window_;
  sf::Event event_{};

  double l_;
  double r1_;
  double r2_;

 public:
  App(double l, double r1, double r2);

  void handleEvent();

  void draw();

  void run();
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
