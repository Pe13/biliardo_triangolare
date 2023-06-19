//
// Created by paolo on 23/05/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_

#include "Biliardo.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>
#include <vector>

namespace bt {

class App {
  sf::RenderWindow window_{{1280, 720}, "Biliardo trinagolare", sf::Style::Titlebar | sf::Style::Close};
  sf::Event event_{};

  Biliardo biliardo_;

  double ratio_{};
  double xOffset_{};
  double yOffset_{};

  std::array<sf::Vertex, 4> bordiBiliardo_{};

  std::vector<std::vector<double>> singleLaunches_{1};

  bool isDrawing = true;

 public:
  App(double l, double r1, double r2);
  void handleEvent();
  void draw();
  void run();

  void calcBordiBiliardo(double l, double r1, double r2);
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
