//
// Created by paolo on 23/05/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_


#include <array>
#include <vector>

#include "Biliardo.hpp"
#include "Designer.hpp"
#include "Gui.hpp"

namespace bt {

struct Gui;

class App {
  friend struct Gui;

  Biliardo biliardo_;

  Designer designer_{};

  sf::RenderWindow window_{{1280, 720}, "Biliardo trinagolare", sf::Style::Default};
  sf::Event event_{};

  Gui gui_;

  std::array<std::vector<std::vector<double>>, 3> singleLaunches_;
  std::array<std::vector<double>, 3> multipleLaunches_{{}};
  std::array<std::vector<sf::Image>, 3> graphImages{{}};

  void handleEvents();
  void handeKeyboardEvents(sf::Keyboard::Key key);
  void changeBiliardo(BiliardoType type);

 public:
  App(double l, double r1, double r2, BiliardoType type);
//  ~App();

  void run();
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
