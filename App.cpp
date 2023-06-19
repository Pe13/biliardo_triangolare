//
// Created by paolo on 23/05/2023.
//

#include "App.hpp"

#include <algorithm>
#include <array>
#include <iostream>

namespace bt {

// TODO creare un metodo static che chiama il costruttore così che nessun membro sia inizializzato di default
App::App(double l, double r1, double r2) : biliardo_(l, r1, r2) {
  // limitiamo gli fps per far avanzare più facilmente la pallina a velocità costante
  window_.setFramerateLimit(30);

  calcBordiBiliardo(l, r1, r2);

  biliardo_.launchForDrawing(singleLaunches_[0]);
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

  window_.draw(bordiBiliardo_.data(), 4, sf::LineStrip);

  window_.display();
}

void App::run() {
  while (window_.isOpen()) {
    handleEvent();
    draw();
  }
}

void App::calcBordiBiliardo(double l, double r1, double r2) {
  // calcolo il rapporto (pixel / unità di misura della simulazione) ottimale
  ratio_ = std::min(1280. * 0.8 / l, 720. * 0.4 / std::max(r1, r2));

  // calcolo l'offset verticale e orizzontale per centrare il biliardo nella finestra
  double width = l * ratio_;
  double horizontalFraction = width / 1280;
  xOffset_ = (1 - horizontalFraction) / 2 * 1280;

  double height = std::max(r1, r2) * ratio_ * 2;
  double verticalFraction = height / 720;
  yOffset_ = (1 - verticalFraction) / 2 * 720;

  // calcolo le effettive posizioni dei vertici
  bordiBiliardo_ = {
      sf::Vertex(sf::Vector2f(static_cast<float>(l * ratio_ + xOffset_),
                              static_cast<float>(height / 2 - (r2 * ratio_) + yOffset_)),
                 sf::Color::Black),
      sf::Vertex(sf::Vector2f(static_cast<float>(xOffset_), static_cast<float>(height / 2 - (r1 * ratio_) + yOffset_)),
                 sf::Color::Black),
      sf::Vertex(sf::Vector2f(static_cast<float>(xOffset_), static_cast<float>(height / 2 + r1 * ratio_ + yOffset_)),
                 sf::Color::Black),
      sf::Vertex(sf::Vector2f(static_cast<float>(l * ratio_ + xOffset_),
                              static_cast<float>(height / 2 + r2 * ratio_ + yOffset_)),
                 sf::Color::Black)};
}

}  // namespace bt