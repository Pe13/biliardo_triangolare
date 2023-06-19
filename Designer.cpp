//
// Created by paolo on 19/06/2023.
//

#include "Designer.hpp"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <stdexcept>
#include <iostream>

namespace bt {

void Designer::calcBordiBiliardo(double l, double r1, double r2) {
  // calcolo il rapporto (pixel / unità di misura della simulazione) ottimale
  ratio_ = std::min(1280. * 0.8 / l, 720. * 0.4 / std::max(r1, r2));

  // calcolo l'offset verticale e orizzontale per centrare il biliardo nella finestra
  double width = l * ratio_;
  double horizontalFraction = width / 1280;
  xOffset_ = (1 - horizontalFraction) / 2 * 1280;

  //  double height = std::max(r1, r2) * ratio_ * 2;
  //  double verticalFraction = height / 720;
  //  yOffset_ = (1 - verticalFraction) / 2 * 720;

  // calcolo le effettive posizioni dei vertici
  bordiBiliardo_ = {
      sf::Vertex(sf::Vector2f(static_cast<float>(l * ratio_ + xOffset_), static_cast<float>(360 - r2 * ratio_)),
                 sf::Color::Black),
      sf::Vertex(sf::Vector2f(static_cast<float>(xOffset_), static_cast<float>(360 - r1 * ratio_)), sf::Color::Black),
      sf::Vertex(sf::Vector2f(static_cast<float>(xOffset_), static_cast<float>(360 + r1 * ratio_)), sf::Color::Black),
      sf::Vertex(sf::Vector2f(static_cast<float>(l * ratio_ + xOffset_), static_cast<float>(360 + r2 * ratio_)),
                 sf::Color::Black)};
}

void Designer::calcStep() {
  double distance = std::sqrt(((*points_)[pointIndex_] - (*points_)[pointIndex_ + 2]) *
                                  ((*points_)[pointIndex_] - (*points_)[pointIndex_ + 2]) +
                              ((*points_)[pointIndex_ + 1] - (*points_)[pointIndex_ + 3]) *
                                  ((*points_)[pointIndex_ + 1] - (*points_)[pointIndex_ + 3]));
  // speedx = stepx * 30
  // speedx = step * ratio * 30
  // step = speedx / (ratio * 30)
  // frac = step / dist = speedx / (ratio * 30 * dist)
  step_ = static_cast<float>(static_cast<double>(speed_) / (distance * ratio_ * 30));
  std::cout << "step: " << step_ << "\n";
}

Designer::Designer(double l, double r1, double r2) {
  calcBordiBiliardo(l, r1, r2);
  particle_.setFillColor(sf::Color::Black);
}

void Designer::setPoints(std::vector<double>* points) {
  if (points != nullptr) {
    points_ = points;
    calcStep();
    isDrawing_ = true;
  } else {
    throw std::runtime_error("nullptr passed to setPoints");
  }
}

void Designer::reRun() {
  if (points_ != nullptr) {
    isDrawing_ = true;
  }
}

void Designer::operator()(sf::RenderWindow& window) {
  if (isDrawing_) {
    window.clear(sf::Color::White);
    window.draw(bordiBiliardo_.data(), 4, sf::LineStrip);

    particle_.setPosition(
        toSfmlCord((*points_)[pointIndex_] * (1 - pathFraction_) + (*points_)[pointIndex_ + 2] * pathFraction_,
                   (*points_)[pointIndex_ + 1] * (1 - pathFraction_) + (*points_)[pointIndex_ + 3] * pathFraction_));
    window.draw(particle_);

    pathFraction_ += step_;
    if (pathFraction_ > 1) {
      pathFraction_ = 0;
      pointIndex_ += 4;
      if (pointIndex_ > points_->size() - 2) {
        pointIndex_ = 0;
        isDrawing_ = false;
        hasCleared_ = false;
      }
    }

    window.display();
  } else if (!hasCleared_) {
    window.clear(sf::Color::White);
    window.draw(bordiBiliardo_.data(), 4, sf::LineStrip);
    window.display();
    hasCleared_ = true;
  }
}

}  // namespace bt