//
// Created by paolo on 23/05/2023.
//

#include "App.hpp"

namespace bt {

App::App(double l, double r1, double r2) : biliardo_(l, r1, r2), designer_(biliardo_) {
  window_.setPosition(sf::Vector2i(100, 100));

  // limitiamo gli fps per far avanzare più facilmente la pallina a velocità costante
  window_.setFramerateLimit(120);
  window_.setVerticalSyncEnabled(false);
  window_.clear(sf::Color::White);

  biliardo_.launchForDrawing(singleLaunches_[0]);
  designer_.setPoints(&singleLaunches_[0]);
}

void App::handleEvent() {
  while (window_.pollEvent(event_)) {
    switch (event_.type) {
      case sf::Event::Closed:
        window_.close();
        break;  // non necessario ma carino

      case sf::Event::KeyPressed:
        switch (event_.key.code) {
          case sf::Keyboard::R:
            designer_.reRun();
            break;

          case sf::Keyboard::P:
            designer_.pause();
            break;

          case sf::Keyboard::N:
            singleLaunches_.emplace_back();
            biliardo_.launchForDrawing(singleLaunches_.back());
            designer_.setPoints(&singleLaunches_.back());
            break;

          case sf::Keyboard::Right:
            designer_.nextLaunch(&singleLaunches_.back());
            break ;

          case sf::Keyboard::Left:
            designer_.previousLaunch(&singleLaunches_.front());
            break ;

          default:
            break;
        }
        break;

      default:
        break;
    }
  }
}

void App::run() {
  while (window_.isOpen()) {
    handleEvent();
    designer_(window_);
  }
}

}  // namespace bt