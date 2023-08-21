//
// Created by paolo on 19/06/2023.
//

#include "Designer.hpp"

#include <SFML/Graphics.hpp>
#include <array>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace bt {

void Designer::calcFrame(const sf::Vector2u& size) {
  using namespace sf;
  frame_.update((std::array<Vertex, 4>{Vertex(Vector2f(rightOffset_, 0), Color::Black),
                                       Vertex(Vector2f(rightOffset_, static_cast<float>(size.y)), Color::Black),
                                       Vertex(Vector2f(rightOffset_, topOffset_), Color::Black),
                                       Vertex(Vector2f(static_cast<float>(size.x), topOffset_), Color::Black)})
                    .data());
}

void Designer::calcClearBiliardo(const bt::Biliardo& biliardo) {
  using namespace sf;
  auto max = static_cast<float>(std::max(biliardo.r1(), biliardo.r2()));
  clearBiliardo_.update(
      (std::array<Vertex, 4>{
           sf::Vertex(
               sf::Vector2f(static_cast<float>(biliardo.l() * ratio_ + simulationXOffset_ + particle_.getRadius()),
                            simulationHeight_ / 2.f - max * ratio_ - particle_.getRadius()),
               sf::Color::White),
           sf::Vertex(sf::Vector2f(simulationXOffset_ - particle_.getRadius(),
                                   simulationHeight_ / 2.f - max * ratio_ - particle_.getRadius()),
                      sf::Color::White),
           sf::Vertex(
               sf::Vector2f(static_cast<float>(biliardo.l() * ratio_ + simulationXOffset_ + particle_.getRadius()),
                            simulationHeight_ / 2.f + max * ratio_ + particle_.getRadius()),
               sf::Color::White),
           sf::Vertex(sf::Vector2f(simulationXOffset_ - particle_.getRadius(),
                                   simulationHeight_ / 2.f + max * ratio_ + particle_.getRadius()),
                      sf::Color::White)})
          .data());
}

void Designer::calcClearHisto(const sf::Vector2u& size) {
  using namespace sf;
  clearHisto_.update(
      (std::array<Vertex, 4>{Vertex(Vector2f(static_cast<float>(size.x), topOffset_), Color::White),
                             Vertex(Vector2f(rightOffset_, topOffset_), Color::White),
                             Vertex(Vector2f(static_cast<float>(size.x), static_cast<float>(size.y)), Color::White),
                             Vertex(Vector2f(rightOffset_, static_cast<float>(size.y)), Color::White)})
          .data());
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
  step_ = static_cast<float>(static_cast<double>(speed_) / (distance * ratio_));
}

Designer::Designer(Biliardo const& biliardo, const sf::Vector2u& size)
    : rightOffset_{widthLeftFraction_ * static_cast<float>(size.x)},
      topOffset_{heightTopFraction_ * static_cast<float>(size.y)}, simulationWidth_{(1.f - widthLeftFraction_) *
                                                                                    static_cast<float>(size.x)},
      simulationHeight_{heightTopFraction_ * static_cast<float>(size.y)} {
  frame_.create(4);
  frame_.setPrimitiveType(sf::Lines);
  calcFrame(size);

  bordiBiliardo_.create(4);
  calcBordiBiliardo(biliardo);

  clearBiliardo_.create(4);
  clearBiliardo_.setPrimitiveType(sf::TriangleStrip);
  calcClearBiliardo(biliardo);

  clearHisto_.create(4);
  clearHisto_.setPrimitiveType(sf::TriangleStrip);
  calcClearHisto(size);

  histoSprite_.setPosition(rightOffset_, topOffset_);
  histoSprite_.setColor(sf::Color::Black);

  particle_.setFillColor(sf::Color::Black);
  particle_.setPointCount(10);
}

void Designer::changeSize(const Biliardo& biliardo, const sf::Vector2u& size) {
  rightOffset_ = widthLeftFraction_ * static_cast<float>(size.x);
  topOffset_ = heightTopFraction_ * static_cast<float>(size.y);
  simulationWidth_ = (1.f - widthLeftFraction_) * static_cast<float>(size.x);
  simulationHeight_ = heightTopFraction_ * static_cast<float>(size.y);

  calcFrame(size);
  calcBordiBiliardo(biliardo);
}

void Designer::calcBordiBiliardo(Biliardo const& biliardo) {
  // calcolo il rapporto (pixel / unità di misura della simulazione) ottimale
  ratio_ = std::min(simulationWidth_ * 0.8f / static_cast<float>(biliardo.l()),
                    simulationHeight_ * 0.4f / static_cast<float>(std::max(biliardo.r1(), biliardo.r2())));

  // calcolo l'offset verticale e orizzontale per centrare il biliardo nella finestra
  float width = static_cast<float>(biliardo.l()) * ratio_;
  float horizontalFraction = width / simulationWidth_;
  simulationXOffset_ = (1.f - horizontalFraction) / 2.f * simulationWidth_ + rightOffset_;

  //  double height = std::max(biliardo.r1(), biliardo.r2()) * ratio_ * 2;
  //  double verticalFraction = height / simulationHeight_;
  //  yOffset_ = (1 - verticalFraction) / 2 * simulationHeight_;

  // ordino i Vertex nel buffer in base all'ordine in cui devono essere disegnati
  if (biliardo.type() == rightBounded) {
    bordiBiliardo_.setPrimitiveType(sf::LineStrip);
    bordiBiliardo_.update(
        (std::array<sf::Vertex, 4>{
             sf::Vertex(
                 sf::Vector2f(simulationXOffset_, static_cast<float>(simulationHeight_ / 2 - biliardo.r1() * ratio_)),
                 sf::Color::Black),
             sf::Vertex(sf::Vector2f(static_cast<float>(biliardo.l() * ratio_ + simulationXOffset_),
                                     static_cast<float>(simulationHeight_ / 2 - biliardo.r2() * ratio_)),
                        sf::Color::Black),
             sf::Vertex(sf::Vector2f(static_cast<float>(biliardo.l() * ratio_ + simulationXOffset_),
                                     static_cast<float>(simulationHeight_ / 2 + biliardo.r2() * ratio_)),
                        sf::Color::Black),
             sf::Vertex(
                 sf::Vector2f(simulationXOffset_, static_cast<float>(simulationHeight_ / 2 + biliardo.r1() * ratio_)),
                 sf::Color::Black)})
            .data());
  } else {
    if (biliardo.type() == open) {
      bordiBiliardo_.setPrimitiveType(sf::Lines);
    } else {
      bordiBiliardo_.setPrimitiveType(sf::LineStrip);
    }
    bordiBiliardo_.update(
        (std::array<sf::Vertex, 4>{
             sf::Vertex(sf::Vector2f(static_cast<float>(biliardo.l() * ratio_ + simulationXOffset_),
                                     static_cast<float>(simulationHeight_ / 2 - biliardo.r2() * ratio_)),
                        sf::Color::Black),
             sf::Vertex(
                 sf::Vector2f(simulationXOffset_, static_cast<float>(simulationHeight_ / 2 - biliardo.r1() * ratio_)),
                 sf::Color::Black),
             sf::Vertex(
                 sf::Vector2f(simulationXOffset_, static_cast<float>(simulationHeight_ / 2 + biliardo.r1() * ratio_)),
                 sf::Color::Black),
             sf::Vertex(sf::Vector2f(static_cast<float>(biliardo.l() * ratio_ + simulationXOffset_),
                                     static_cast<float>(simulationHeight_ / 2 + biliardo.r2() * ratio_)),
                        sf::Color::Black)})
            .data());
  }
}

void Designer::setPoints(std::vector<double>* points) {
  if (points != nullptr) {
    points_ = points;
    calcStep();
    reRun();
  } else {
    throw std::runtime_error("nullptr passed to setPoints");
  }
}

void Designer::previousLaunch(std::vector<double>* first) {
  if (points_ > first) {
    points_--;
    reRun();
  }
}

void Designer::nextLaunch(std::vector<double>* last) {
  if (points_ < last) {
    points_++;
    reRun();
  }
}

void Designer::reRun() {
  if (points_ == nullptr) {
    return;
  }
  isDrawing_ = true;
  hasCleared_ = false;
  pointIndex_ = 0;
  pathFraction_ = 0;
  calcStep();
  contrail_.clear();
  //  contrailIndex_ = 0;
}

void Designer::pause() {
  if (points_ == nullptr) {
    return;
  }
  isPaused_ = !isPaused_;
}

void Designer::initWindow(sf::RenderWindow& window) { window.draw(frame_); }

void Designer::updateHisto(sf::RenderWindow& window) {
  window.draw(clearHisto_);
  window.draw(frame_);
  window.draw(histoSprite_);
  window.display();
}

void Designer::operator()(sf::RenderWindow& window) {
  //  auto t1 = std::chrono::high_resolution_clock::now();
  //  auto t2 = t1;
  if (isPaused_) {
    return;
  }
  if (!hasCleared_) {
    window.draw(clearBiliardo_);
    window.draw(bordiBiliardo_);
    window.display();
    hasCleared_ = true;

  } else if (isDrawing_) {
    contrail_.front().color = sf::Color::White;
    window.draw(&contrail_.front(), 1, sf::Points);

    //    std::memmove(contrail_.data(), contrail_.data() + 1, (contrail_.size() - 1) * sizeof(sf::Vertex));

    //    contrail_.back() = sf::Vertex(particle_.getPosition() + sf::Vector2f(5, 5), sf::Color::Blue);

    contrail_.push_back(sf::Vertex(particle_.getPosition() + sf::Vector2f(5, 5), sf::Color::Blue));

    particle_.setFillColor(sf::Color::White);
    window.draw(particle_);

    particle_.setFillColor(sf::Color::Black);
    particle_.setPosition(
        toSfmlCord((*points_)[pointIndex_] * (1 - pathFraction_) + (*points_)[pointIndex_ + 2] * pathFraction_,
                   (*points_)[pointIndex_ + 1] * (1 - pathFraction_) + (*points_)[pointIndex_ + 3] * pathFraction_) -
        sf::Vector2f(5, 5));
    window.draw(particle_);
    window.draw(&contrail_.back(), 1, sf::Points);
    //    t2 = std::chrono::high_resolution_clock::now();

    //    contrailIndex_ =
    //        (contrailIndex_ == static_cast<int>(contrail_.size() - 1)) ? contrailIndex_ = 0 : contrailIndex_ += 1;
    window.draw(bordiBiliardo_);

    pathFraction_ += step_;
    if (pathFraction_ > 1) {
      pathFraction_ = 0;
      pointIndex_ += 2;
      if (pointIndex_ > points_->size() - 4) {
        pointIndex_ = 0;
        isDrawing_ = false;
        hasCleared_ = false;
      } else {
        calcStep();
      }
    }
    //    t1 = std::chrono::high_resolution_clock::now();

    window.display();
  }
  //  t2 = std::chrono::high_resolution_clock::now();
  //  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
  //  std::cout << duration.count() << "\n";
}

}  // namespace bt