//
// Created by paolo on 23/05/2023.
//

#include "App.hpp"

#include "BiliardoAperto.hpp"
#include "BiliardoChiuso.hpp"

namespace bt {

App::App(double l, double r1, double r2) : biliardo_{new BiliardoAperto(l, r1, r2)}, designer_(*biliardo_) {
  window_.setPosition(sf::Vector2i(100, 100));

  // limitiamo gli fps per far avanzare più facilmente la pallina a velocità costante
  window_.setFramerateLimit(60);
  window_.setVerticalSyncEnabled(false);
  window_.clear(sf::Color::White);

  for (auto& v: singleLaunches_) {
    v.emplace_back();
    biliardo_->launchForDrawing(v[0]);
  }
  designer_.setPoints(&singleLaunches_[open][0]);
}

App::~App() { delete biliardo_; }

void App::handleEvent() {
  while (window_.pollEvent(event_)) {
    switch (event_.type) {
      case sf::Event::Closed:
        window_.close();
        break;  // non necessario ma carino

      case sf::Event::KeyPressed:
        switch (event_.key.code) {
          case sf::Keyboard::A:
            changeBiliardo(open);
            break;

          case sf::Keyboard::C:
            changeBiliardo(leftBounded);
            break;

          case sf::Keyboard::R:
            designer_.reRun();
            break;

          case sf::Keyboard::P:
            designer_.pause();
            break;

          case sf::Keyboard::N:
            singleLaunches_[biliardo_->type()].emplace_back();
            biliardo_->launchForDrawing(singleLaunches_[biliardo_->type()].back());
            designer_.setPoints(&singleLaunches_[biliardo_->type()].back());
            break;

          case sf::Keyboard::Right:
            designer_.nextLaunch(&singleLaunches_[biliardo_->type()].back());
            break;

          case sf::Keyboard::Left:
            designer_.previousLaunch(&singleLaunches_[biliardo_->type()].front());
            break;

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
void App::changeBiliardo(BiliardoType type) {
  Biliardo* biliardo;
  switch (type) {
    case open:
      biliardo = new BiliardoAperto(*biliardo_);
      break;
    case leftBounded:
      biliardo = new BiliardoChiuso(*biliardo_);
      break;
    case rightBounded:
      break;
  }
  delete biliardo_;
  biliardo_ = biliardo;

  designer_.calcBordiBiliardo(*biliardo_);
  designer_.setPoints(&singleLaunches_[type].back());
}

}  // namespace bt