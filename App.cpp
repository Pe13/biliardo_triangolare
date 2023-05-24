//
// Created by paolo on 23/05/2023.
//

#include "App.hpp"

namespace bt {

App::App(double l, double r1, double r2)
    : window_{{800, 600}, "Biliardo trinagolare"}, l_{l}, r1_{r1}, r2_{r2} {
  // limitiamo gli fps per far avanzare più facilmente la pallina a velocità
  // costante
  window_.setFramerateLimit(30);
}

void App::handleEvent() {
  while (window_.pollEvent(event_)) {
    if (event_.type == sf::Event::Closed) {
      window_.close();
    }
  }
}

void App::draw() {
  window_.clear(sf::Color::White);

  window_.display();
}

void App::run() {
  while (window_.isOpen()) {
    handleEvent();
    draw();
  }
}

}  // namespace bt