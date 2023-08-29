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

bool isNumber(const std::string &s) { return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit); }

App::App(const double l, const double r1, const double r2, const BiliardoType type) : biliardo_(l, r1, r2, type) {
  window_.setPosition(sf::Vector2i(100, 100));

  // limitiamo gli fps per far avanzare più facilmente la pallina a velocità costante
  window_.setFramerateLimit(60);
  window_.setVerticalSyncEnabled(false);
  window_.clear(sf::Color::White);

  createGui();
  activateGui();

  // inizializzo tutti i vector di lanci singoli e setto il biliardo iniziale "aperto"
  for (int i = 2; i > -1; i--) {
    singleLaunches_[i].emplace_back();
    biliardo_.changeType(static_cast<BiliardoType>(i));
    biliardo_.launchForDrawing(singleLaunches_[i][0]);
  }
  designer_.calcBordiBiliardo(biliardo_);
  designer_.setPoints(&singleLaunches_[open][0]);
}

// App::~App() { delete biliardo_; }

void App::createGui() {
  gui_.add(wrapper_, "wrapper");

  wrapper_->getRenderer()->setPadding({0, 10});

  // bottoni per cambiare tipo di biliardo
  wrapper_->add(biliardoButtonsWrapper_, "buttonsWrapper");
  biliardoButtonsWrapper_->addSpace(.1);
  biliardoButtonsWrapper_->add(biliardoApertoBtn_, "biliardoAperto");
  biliardoButtonsWrapper_->addSpace(.1);
  biliardoButtonsWrapper_->add(biliardoChiusoDxBtn_, "biliardoChiusoDx");
  biliardoButtonsWrapper_->addSpace(.1);
  biliardoButtonsWrapper_->add(biliardoChiusoSxBtn_, "biliardoChiusoSx");

  wrapper_->addSpace(.5);

  // bottone e campi per modificare il biliardo
  wrapper_->add(newBiliardoWrapper_, 2.5, "newBiliardoWrapper");
  newBiliardoWrapper_->addSpace(.05);
  newBiliardoWrapper_->add(newBiliardoInputWrapper_, .3, "newBiliardoInputWrapper");
  newBiliardoInputWrapper_->add(r1Label_, .5, "r1Label");
  newBiliardoInputWrapper_->add(r1Input_, "r1Input");
  newBiliardoInputWrapper_->add(r2Label_, .5, "r2Label");
  newBiliardoInputWrapper_->add(r2Input_, "r2Input");
  newBiliardoInputWrapper_->add(lLabel_, .5, "lLabel");
  newBiliardoInputWrapper_->add(lInput_, "lInput");
  newBiliardoWrapper_->addSpace(.2);
  newBiliardoWrapper_->add(newBiliardoBtn_, .4, "newBiliardoBtn");
  newBiliardoWrapper_->addSpace(.05);

  wrapper_->addSpace(.5);

  // bottone e campi per lanciare la singola particella
  wrapper_->add(singleLaunchWrapper_, 2, "singleLaunchWrapper");
  singleLaunchWrapper_->addSpace(.05);
  singleLaunchWrapper_->add(singleLaunchInputWrapper_, .3, "singleLaunchInputWrapper");
  singleLaunchInputWrapper_->add(heightLabel_, .5, "heightLabel");
  singleLaunchInputWrapper_->add(heightInput_, "heightInput");
  singleLaunchInputWrapper_->add(angleLabel_, .5, "angleLabel");
  singleLaunchInputWrapper_->add(angleInput_, "angleInput");
  singleLaunchWrapper_->addSpace(.2);
  singleLaunchWrapper_->add(singleLaunchBtn_, .4, "singleLaunchBtn");
  singleLaunchWrapper_->addSpace(.05);

  wrapper_->addSpace(.5);

  // bottone e campi per lanciare N particelle
  wrapper_->add(multipleLaunchWrapper_, 2.5, "multipleLaunchWrapper");
  multipleLaunchWrapper_->addSpace(.05);
  multipleLaunchWrapper_->add(multipleLaunchLeftWrapper_, .3, "multipleLaunchLeftWrapper");
  multipleLaunchLeftWrapper_->add(muYLabel_, "muYLabel");
  multipleLaunchLeftWrapper_->add(muYInput_, "muYInput");
  multipleLaunchLeftWrapper_->add(sigmaYLabel_, "sigmaYLabel");
  multipleLaunchLeftWrapper_->add(sigmaYInput_, "sigmaYInput");
  multipleLaunchLeftWrapper_->add(muTLabel_, "mutLabel");
  multipleLaunchLeftWrapper_->add(muTInput_, "muTInput");
  multipleLaunchLeftWrapper_->add(sigmaTLabel_, "sigmaTLabel");
  multipleLaunchLeftWrapper_->add(sigmaTInput_, "sigmaTInput");
  multipleLaunchWrapper_->addSpace(.2);
  multipleLaunchWrapper_->add(multipleLaunchRightWrapper_, .4, "multipleLaunchRightWrapper");
  multipleLaunchRightWrapper_->add(numberLabel_, "numberLabel");
  multipleLaunchRightWrapper_->add(numberInput_, "numberInput");
  multipleLaunchRightWrapper_->addSpace(2);
  multipleLaunchRightWrapper_->add(multipleLaunchBtn_, 4, "multipleLaunchBtn");
  multipleLaunchWrapper_->addSpace(.05);

  wrapper_->addSpace(.5);

  // bottoni per navigare tra i singoli lanci
  wrapper_->add(navigateWrapper_, "navigateWrapper");
  navigateWrapper_->addSpace(.1);
  navigateWrapper_->add(previousLaunchBtn_, "previousLaunchBtn");
  navigateWrapper_->addSpace(.1);
  navigateWrapper_->add(nextLaunchBtn_, "nextLaunchBtn");
  navigateWrapper_->addSpace(.1);
  navigateWrapper_->add(pauseBtn_, "pauseBtn");
  navigateWrapper_->addSpace(.1);
  navigateWrapper_->add(reRunBtn_, "reRunBtn");
  navigateWrapper_->addSpace(.1);
}

void App::activateGui() {
  biliardoApertoBtn_->onPress([this] { changeBiliardo(open); });
  biliardoChiusoDxBtn_->onPress([this] { changeBiliardo(rightBounded); });
  biliardoChiusoSxBtn_->onPress([this] { changeBiliardo(leftBounded); });

  previousLaunchBtn_->onPress([this] { designer_.previousLaunch(&singleLaunches_[biliardo_.type()].front()); });
  nextLaunchBtn_->onPress([this] { designer_.nextLaunch(&singleLaunches_[biliardo_.type()].back()); });
  pauseBtn_->onPress([this] { designer_.pause(); });
  reRunBtn_->onPress([this] { designer_.reRun(); });

  singleLaunchBtn_->onPress([this] {
    float y, t;
    bool isY = false, isT = false;

    singleLaunches_[biliardo_.type()].emplace_back();

    if (heightInput_->getText().attemptToFloat(y)) {
      isY = true;
    }
    if (angleInput_->getText().attemptToFloat(t)) {
      isT = true;
    }

    if (!isY && !isT) {
      biliardo_.launchForDrawing(singleLaunches_[biliardo_.type()].back());
    } else if (isY && isT) {
      biliardo_.launchForDrawing(y, t, singleLaunches_[biliardo_.type()].back());
    } else if (isY) {
      biliardo_.launchForDrawingNoDir(y, singleLaunches_[biliardo_.type()].back());
    } else {
      biliardo_.launchForDrawingNoY(t, singleLaunches_[biliardo_.type()].back());
    }

    designer_.setPoints(&singleLaunches_[biliardo_.type()].back());
  });
}

void App::handleEvents() {
  while (window_.pollEvent(event_)) {
    gui_.handleEvent(event_);
    switch (event_.type) {
      case sf::Event::Closed:
        window_.close();
        break;  // non necessario ma carino

      case sf::Event::KeyPressed:
        handeKeyboardEvents(event_.key.code);
        break;

      case sf::Event::Resized:
        designer_.changeSize(biliardo_, multipleLaunches_[biliardo_.type()], window_, wrapper_);
        break;

      default:
        break;
    }
  }
}

void App::handeKeyboardEvents(const sf::Keyboard::Key key) {
  switch (key) {
      //    case sf::Keyboard::A:
      //      changeBiliardo(open);
      //      break;

      //    case sf::Keyboard::D:
      //      changeBiliardo(rightBounded);
      //      break;

    case sf::Keyboard::L: {  // graffe necessarie per dichiarare variabili in un case
      multipleLaunches_[biliardo_.type()].clear();
      biliardo_.launch(1e6, multipleLaunches_[biliardo_.type()]);
      designer_.setCanvas(biliardo_.r1(), multipleLaunches_[biliardo_.type()], window_);
      break;
    }
    case sf::Keyboard::N:
      singleLaunches_[biliardo_.type()].emplace_back();
      biliardo_.launchForDrawing(singleLaunches_[biliardo_.type()].back());
      designer_.setPoints(&singleLaunches_[biliardo_.type()].back());
      break;

      //    case sf::Keyboard::P:
      //      designer_.pause();
      //      break;

      //    case sf::Keyboard::R:
      //      designer_.reRun();
      //      break;

      //    case sf::Keyboard::S:
      //      changeBiliardo(leftBounded);
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
    gui_.draw();
    designer_(window_);
    window_.display();
  }
}
void App::changeBiliardo(BiliardoType type) {
  biliardo_.changeType(type);
  designer_.calcBordiBiliardo(biliardo_);
  designer_.setPoints(&singleLaunches_[type].back());
}

}  // namespace bt