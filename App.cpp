//
// Created by paolo on 23/05/2023.
//

#include "App.hpp"

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TextStyle.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <algorithm>

#include "Biliardo.hpp"

namespace bt {

App::App(const double l, const double r1, const double r2, const BiliardoType type)
    : biliardo_(l, r1, r2, type), gui_(window_) {
  gui_.activate(this);

  window_.setPosition(sf::Vector2i(100, 100));

  // limitiamo gli fps per far avanzare più facilmente la pallina a velocità costante
  window_.setFramerateLimit(60);
  window_.setVerticalSyncEnabled(false);
  window_.clear(sf::Color::White);

  // inizializzo tutti i vector di lanci singoli e setto il biliardo iniziale "aperto"
  for (int i = 2; i > -1; i--) {
    singleLaunches_[i].emplace_back();
    biliardo_.changeType(static_cast<BiliardoType>(i));
    biliardo_.launchForDrawing(singleLaunches_[i][0]);
  }
  designer_.calcBordiBiliardo(biliardo_);
  designer_.setPoints(&singleLaunches_[open][0]);
}

void App::handleEvents() {
  while (window_.pollEvent(event_)) {
    gui_.gui.handleEvent(event_);
    switch (event_.type) {
      case sf::Event::Closed:
        window_.close();
        break;  // non necessario ma carino

      case sf::Event::KeyPressed:
        handeKeyboardEvents(event_.key.code);
        break;

      case sf::Event::Resized:
        designer_.changeSize(biliardo_, multipleLaunches_[biliardo_.type()], window_, gui_.wrapper);
        break;

      default:
        break;
    }
  }
}

void App::handeKeyboardEvents(const sf::Keyboard::Key key) {
  switch (key) {
      //    case sf::Keyboard::A:
      //      changeBiliardoType(open);
      //      break;

      //    case sf::Keyboard::D:
      //      changeBiliardoType(rightBounded);
      //      break;

    case sf::Keyboard::L: {  // graffe necessarie per dichiarare variabili in un case
      multipleLaunches_[biliardo_.type()].clear();
      biliardo_.launch(1e6, multipleLaunches_[biliardo_.type()]);
      designer_.setCanvas(biliardo_.r1(), multipleLaunches_[biliardo_.type()], window_);
      break;
    }
      //    case sf::Keyboard::N:
      //      singleLaunches_[biliardo_.type()].emplace_back();
      //      biliardo_.launchForDrawing(singleLaunches_[biliardo_.type()].back());
      //      designer_.setPoints(&singleLaunches_[biliardo_.type()].back());
      //      break;

      //    case sf::Keyboard::P:
      //      designer_.pause();
      //      break;

      //    case sf::Keyboard::R:
      //      designer_.reRun();
      //      break;

      //    case sf::Keyboard::S:
      //      changeBiliardoType(leftBounded);
      //      break;

      //    case sf::Keyboard::Right:
      //      designer_.nextLaunch(&singleLaunches_[biliardo_->type()].back());
      //      break;

      //    case sf::Keyboard::Left:
      //      designer_.previousLaunch(&singleLaunches_[biliardo_->type()].front());
      //      break;

    default:
      break;
  }
}

void App::run() {
  designer_.initWindow(window_);
  while (window_.isOpen()) {
    handleEvents();
    gui_.gui.draw();
    designer_(window_);
    window_.display();
  }
}
void App::changeBiliardoType(BiliardoType type) {
  biliardo_.changeType(type);
  designer_.calcBordiBiliardo(biliardo_);
  designer_.setPoints(&singleLaunches_[type].back());
}

void App::modifyBiliardo() {
  for (int i = 2; i > -1; i--) {
    singleLaunches_[i].clear();
    singleLaunches_[i].emplace_back();
    biliardo_.changeType(static_cast<BiliardoType>(i));
    biliardo_.launchForDrawing(singleLaunches_[i][0]);
  }
  designer_.changeBiliardo(biliardo_, window_);
  designer_.setPoints(&singleLaunches_[biliardo_.type()][0]);
}

}  // namespace bt