//
// Created by paolo on 23/05/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_

#include <iostream>

#include <SFML/Graphics.hpp>


namespace bt {

class App {

  sf::RenderWindow window_;
  sf::Event event_{};

  double l_;
  double r1_;
  double r2_;

 public:
  App(double l, double r1, double r2): window_{{800, 600}, "Biliardo trinagolare"}, l_{l}, r1_{r1}, r2_{r2} {
    std::cerr << "ciao\n";
    window_.setFramerateLimit(30);
  }
  
  void handleEvent() {
    
    while (window_.pollEvent(event_)) {
      if (event_.type == sf::Event::Closed) {
        window_.close();
      }
    }
  }

  void run() {
    while (window_.isOpen()) { handleEvent(); }
  }
};

}// namespace bt

#endif//BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
