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
  std::array<std::vector<double>, 3> multipleLaunches_{{}};
  std::array<std::vector<sf::Image>, 3> graphImages{{}};

//  const float widthLeftFraction_{.3};
//  const float heightTopFraction_{.5};
  Designer designer_;

  void handleEvents();
  void handeKeyboardEvents(const sf::Keyboard::Key key);
  void changeBiliardo(BiliardoType type);

 public:
  App(double l, double r1, double r2);
  ~App();

  void run();
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
