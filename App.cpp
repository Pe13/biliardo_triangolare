//
// Created by paolo on 23/05/2023.
//

#include "App.hpp"

#include <iostream>

namespace bt {

App::App(double l, double r1, double r2) : biliardo_(l, r1, r2), designer_(l, r1, r2) {
  window_.setPosition(sf::Vector2i(100, 100));

  // limitiamo gli fps per far avanzare più facilmente la pallina a velocità costante
  window_.setFramerateLimit(30);

  biliardo_.launchForDrawing(singleLaunches_[0]);

  for (auto const& e : singleLaunches_[0]) {
    std::cout << e << "\n";
  }

  std::cout << "\n\n\n";

  designer_.setPoints(&singleLaunches_[0]);
}

void App::handleEvent() {
  while (window_.pollEvent(event_)) {
    if (event_.type == sf::Event::Closed) {
      window_.close();
    }
  }
}

void App::draw() {}

void App::run() {
  while (window_.isOpen()) {
    handleEvent();
    designer_(window_);
  }
}

}  // namespace bt