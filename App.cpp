//
// Created by paolo on 23/05/2023.
//

#include "App.hpp"

#include "BiliardoAperto.hpp"
#include "BiliardoChiusoDx.hpp"
#include "BiliardoChiusoSx.hpp"

namespace bt {

App::App(double l, double r1, double r2)
    : biliardo_{new BiliardoChiusoSx(l, r1, r2)}, designer_(biliardo_, window_.getSize()) {
  window_.setPosition(sf::Vector2i(100, 100));

  // limitiamo gli fps per far avanzare più facilmente la pallina a velocità costante
  window_.setFramerateLimit(60);
  window_.setVerticalSyncEnabled(false);
  window_.clear(sf::Color::White);

  // inizializzo tutti i vector di lanci singoli e setto il biliardo iniziale "aperto"
  for (int i = 2; i > -1; i--) {
    singleLaunches_[i].emplace_back();
    biliardo_ = biliardo_->changeType(static_cast<BiliardoType>(i));
    biliardo_->launchForDrawing(singleLaunches_[i][0]);
  }
  designer_.calcBordiBiliardo(biliardo_);
  designer_.setPoints(&singleLaunches_[open][0]);
}

App::~App() { delete biliardo_; }

void App::handleEvents() {
  while (window_.pollEvent(event_)) {
    switch (event_.type) {
      case sf::Event::Closed:
        window_.close();
        break;  // non necessario ma carino

      case sf::Event::KeyPressed:
        handeKeyboardEvents(event_.key.code);
        break;

      default:
        break;
    }
  }
}

void App::handeKeyboardEvents(const sf::Keyboard::Key key) {
  switch (key) {
    case sf::Keyboard::A:
      changeBiliardo(open);
      break;

    case sf::Keyboard::D:
      changeBiliardo(rightBounded);
      break;

    case sf::Keyboard::L: {  // graffe necessarie per dichiarare variabili in un case
      multipleLaunches_[biliardo_->type()].clear();
      biliardo_->launch(1e6, multipleLaunches_[biliardo_->type()]);
      sf::Vector2f frac = designer_.screenFraction();
      auto size = static_cast<sf::Vector2f>(window_.getSize());
//      designer_.setCanvas(graph(static_cast<int>((1.f - frac.x) * size.x), static_cast<int>((1.f - frac.y) * size.y),
//                                biliardo_->r1(), multipleLaunches_[biliardo_->type()]),
//                          window_);
      designer_.setCanvas(biliardo_->r1(), multipleLaunches_[biliardo_->type()], window_);
      break;
    }
    case sf::Keyboard::N:
      singleLaunches_[biliardo_->type()].emplace_back();
      biliardo_->launchForDrawing(singleLaunches_[biliardo_->type()].back());
      designer_.setPoints(&singleLaunches_[biliardo_->type()].back());
      break;

    case sf::Keyboard::P:
      designer_.pause();
      break;

    case sf::Keyboard::R:
      designer_.reRun();
      break;

    case sf::Keyboard::S:
      changeBiliardo(leftBounded);
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
}

void App::run() {
  designer_.initWindow(window_);
  while (window_.isOpen()) {
    handleEvents();
    designer_(window_);
  }
}
void App::changeBiliardo(BiliardoType type) {
  if (type == biliardo_->type()) {
    return;
  }
  biliardo_ = biliardo_->changeType(type);
  designer_.calcBordiBiliardo(biliardo_);
  designer_.setPoints(&singleLaunches_[type].back());
}

}  // namespace bt