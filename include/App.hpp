//
// Created by paolo on 23/05/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_

#include <SFML/Graphics.hpp>
#include <array>
#include <vector>

#include "BiliardoAperto.hpp"
#include "Designer.hpp"

namespace bt {

class App {
  sf::RenderWindow window_{{1280, 720}, "Biliardo trinagolare", sf::Style::Titlebar | sf::Style::Close};
  sf::Event event_{};

  Biliardo* biliardo_;

  std::array<std::vector<std::vector<double>>, 3> singleLaunches_;

  Designer designer_;

 public:
  App(double l, double r1, double r2);
  ~App();

  void handleEvent();
  void run();

  void changeBiliardo(BiliardoType type);
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
