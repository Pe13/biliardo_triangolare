//
// Created by paolo on 29/08/2023.
//

#include "Gui.hpp"

#include <cmath>
#include <functional>

#include "App.hpp"

namespace bt {

template <typename Target>
void recursiveSearch(const tgui::Widget::Ptr &ptr, std::function<void(std::shared_ptr<Target> &)> &fun) {
  if (ptr->getWidgetType() == tgui::HorizontalLayout::StaticWidgetType) {
    auto layout = std::static_pointer_cast<tgui::HorizontalLayout>(ptr);
    for (auto &elt : layout->getWidgets()) {
      if (elt->getWidgetType() == Target::StaticWidgetType) {
        auto target = std::static_pointer_cast<Target>(elt);
        fun(target);
      }
      recursiveSearch(elt, fun);
    }
  } else if (ptr->getWidgetType() == tgui::VerticalLayout::StaticWidgetType) {
    auto layout = std::static_pointer_cast<tgui::VerticalLayout>(ptr);
    for (auto &elt : layout->getWidgets()) {
      if (elt->getWidgetType() == Target::StaticWidgetType) {
        auto target = std::static_pointer_cast<Target>(elt);
        fun(target);
      }
      recursiveSearch(elt, fun);
    }
  } else if (ptr->getWidgetType() == Target::StaticWidgetType) {
    auto target = std::static_pointer_cast<Target>(ptr);
    fun(target);
  }
}

void Gui::create() {
  gui.add(wrapper, "wrapper");

  wrapper->getRenderer()->setPadding({0, 10});

  // bottoni per cambiare tipo di biliardo
  wrapper->add(biliardoButtonsWrapper, "buttonsWrapper");
  biliardoButtonsWrapper->addSpace(.1);
  biliardoButtonsWrapper->add(biliardoApertoBtn, "biliardoAperto");
  biliardoButtonsWrapper->addSpace(.1);
  biliardoButtonsWrapper->add(biliardoChiusoDxBtn, "biliardoChiusoDx");
  biliardoButtonsWrapper->addSpace(.1);
  biliardoButtonsWrapper->add(biliardoChiusoSxBtn, "biliardoChiusoSx");

  wrapper->addSpace(.5);

  // bottone e campi per modificare il biliardo
  wrapper->add(newBiliardoWrapper, 2.5, "newBiliardoWrapper");
  newBiliardoWrapper->addSpace(.05);
  newBiliardoWrapper->add(newBiliardoInputWrapper, .3, "newBiliardoInputWrapper");
  newBiliardoInputWrapper->add(r1Label, .5, "r1Label");
  newBiliardoInputWrapper->add(r1Input, "r1Input");
  newBiliardoInputWrapper->add(r2Label, .5, "r2Label");
  newBiliardoInputWrapper->add(r2Input, "r2Input");
  newBiliardoInputWrapper->add(lLabel, .5, "lLabel");
  newBiliardoInputWrapper->add(lInput, "lInput");
  newBiliardoWrapper->addSpace(.2);
  newBiliardoWrapper->add(newBiliardoBtn, .4, "newBiliardoBtn");
  newBiliardoWrapper->addSpace(.05);

  wrapper->addSpace(.5);

  // bottone e campi per lanciare la singola particella
  wrapper->add(singleLaunchWrapper, 2, "singleLaunchWrapper");
  singleLaunchWrapper->addSpace(.05);
  singleLaunchWrapper->add(singleLaunchInputWrapper, .3, "singleLaunchInputWrapper");
  singleLaunchInputWrapper->add(heightLabel, .5, "heightLabel");
  singleLaunchInputWrapper->add(heightInput, "heightInput");
  singleLaunchInputWrapper->add(angleLabel, .5, "angleLabel");
  singleLaunchInputWrapper->add(angleInput, "angleInput");
  singleLaunchWrapper->addSpace(.2);
  singleLaunchWrapper->add(singleLaunchBtn, .4, "singleLaunchBtn");
  singleLaunchWrapper->addSpace(.05);

  wrapper->addSpace(.5);

  // bottone e campi per lanciare N particelle
  wrapper->add(multipleLaunchWrapper, 2.5, "multipleLaunchWrapper");
  multipleLaunchWrapper->addSpace(.05);
  multipleLaunchWrapper->add(multipleLaunchLeftWrapper, .3, "multipleLaunchLeftWrapper");
  multipleLaunchLeftWrapper->add(muYLabel, "muYLabel");
  multipleLaunchLeftWrapper->add(muYInput, "muYInput");
  multipleLaunchLeftWrapper->add(sigmaYLabel, "sigmaYLabel");
  multipleLaunchLeftWrapper->add(sigmaYInput, "sigmaYInput");
  multipleLaunchLeftWrapper->add(muTLabel, "mutLabel");
  multipleLaunchLeftWrapper->add(muTInput, "muTInput");
  multipleLaunchLeftWrapper->add(sigmaTLabel, "sigmaTLabel");
  multipleLaunchLeftWrapper->add(sigmaTInput, "sigmaTInput");
  multipleLaunchWrapper->addSpace(.2);
  multipleLaunchWrapper->add(multipleLaunchRightWrapper, .4, "multipleLaunchRightWrapper");
  multipleLaunchRightWrapper->add(numberLabel, "numberLabel");
  multipleLaunchRightWrapper->add(numberInput, "numberInput");
  multipleLaunchRightWrapper->addSpace(2);
  multipleLaunchRightWrapper->add(multipleLaunchBtn, 4, "multipleLaunchBtn");
  multipleLaunchWrapper->addSpace(.05);

  wrapper->addSpace(.5);

  // bottoni per navigare tra i singoli lanci
  wrapper->add(navigateWrapper, "navigateWrapper");
  navigateWrapper->addSpace(.1);
  navigateWrapper->add(previousLaunchBtn, "previousLaunchBtn");
  navigateWrapper->addSpace(.1);
  navigateWrapper->add(nextLaunchBtn, "nextLaunchBtn");
  navigateWrapper->addSpace(.1);
  navigateWrapper->add(pauseBtn, "pauseBtn");
  navigateWrapper->addSpace(.1);
  navigateWrapper->add(reRunBtn, "reRunBtn");
  navigateWrapper->addSpace(.1);
}

void Gui::activate(App *app) {
  biliardoApertoBtn->onPress([app] { app->changeBiliardo(open); });
  biliardoChiusoDxBtn->onPress([app] { app->changeBiliardo(rightBounded); });
  biliardoChiusoSxBtn->onPress([app] { app->changeBiliardo(leftBounded); });

  r1Input->setText(tgui::String(app->biliardo_.r1()));
  r2Input->setText(tgui::String(app->biliardo_.r2()));
  lInput->setText(tgui::String(app->biliardo_.l()));

  previousLaunchBtn->onPress(
      [app] { app->designer_.previousLaunch(&app->singleLaunches_[app->biliardo_.type()].front()); });
  nextLaunchBtn->onPress([app] { app->designer_.nextLaunch(&app->singleLaunches_[app->biliardo_.type()].back()); });
  pauseBtn->onPress([app] { app->designer_.pause(); });
  reRunBtn->onPress([app] { app->designer_.reRun(); });

  //  heightInput->onFocus([this] { heightInput->getRenderer()->setTextColor(tgui::Color::White); });
  //  angleInput->onFocus([this] { angleInput->getRenderer()->setTextColor(tgui::Color::White); });

  // ripristino il colore del testo di default nel caso questo sia diventato rosso a causa di un dato errato inserito
  std::function setWhiteColor{[](tgui::EditBox::Ptr &target) {
    target->onFocus([&target] { target->getRenderer()->setTextColor(tgui::Color::White); });
  }};
  recursiveSearch(std::static_pointer_cast<tgui::Widget>(wrapper), setWhiteColor);

  singleLaunchBtn->onPress([this, app] {
    float y, t;
    bool isY, isT;

    app->singleLaunches_[app->biliardo_.type()].emplace_back();

    if (heightInput->getText().attemptToFloat(y) && std::abs(y) <= app->biliardo_.r1()) {
      isY = true;
    } else if (heightInput->getText().empty()) {
      isY = false;
    } else {
      heightInput->getRenderer()->setTextColor(tgui::Color::Red);
      return;
    }

    if (angleInput->getText().attemptToFloat(t) && std::abs(t) <= M_PI / 2) {
      isT = true;
    } else if (angleInput->getText().empty()) {
      isT = false;
    } else {
      angleInput->getRenderer()->setTextColor(tgui::Color::Red);
      return;
    }

    if (!isY && !isT) {  // se ne y ne teta sono indicate le genera entrambe
      app->biliardo_.launchForDrawing(app->singleLaunches_[app->biliardo_.type()].back());
    } else if (isY && isT) {  // se invece sono entrambe indicate le usa semplicemente
      app->biliardo_.launchForDrawing(y, t, app->singleLaunches_[app->biliardo_.type()].back());
    } else if (isY) {  // altrimenti genera solo quella mancante
      app->biliardo_.launchForDrawingNoDir(y, app->singleLaunches_[app->biliardo_.type()].back());
    } else {
      app->biliardo_.launchForDrawingNoY(t, app->singleLaunches_[app->biliardo_.type()].back());
    }

    app->designer_.setPoints(&app->singleLaunches_[app->biliardo_.type()].back());
  });

  // imposto i placeholder per gli editbox del lancio multiplo
  muYInput->setDefaultText("0");
  sigmaYInput->setDefaultText(tgui::String(app->biliardo_.r1() / 5));
  muTInput->setDefaultText("0");
  sigmaTInput->setDefaultText(tgui::String(M_PI / 8));
  numberInput->setDefaultText("1'000'000");

  multipleLaunchBtn->onPress([this, app] {
    // creo un array con i parametri per usarlo nel for loop
    std::array<float, 5> parameters{0, static_cast<float>(app->biliardo_.r1() / 5), 0, M_PI / 8, 10e6};
    // non necessario ma mi permette di chiamare i singoli parametri per nome e non per numero
    enum { muY = 0, sigmaY = 1, muT = 2, sigmaT = 3, N = 4 };

    app->multipleLaunches_[app->biliardo_.type()].clear();

    // creata per passarla by reference a recursiveSearch
    std::function getMultipleLaunchInput{[&parameters, _i = 0](tgui::EditBox::Ptr &target) mutable {
      if (!target->getText().attemptToFloat(parameters[_i]) && !target->getText().empty()) {
        target->getRenderer()->setTextColor(tgui::Color::Red);
        return;
      }
      _i++;
    }};
    recursiveSearch(std::static_pointer_cast<tgui::Widget>(multipleLaunchWrapper), getMultipleLaunchInput);

    // controllo con un ciclo su tutti i widget dentro al multipleLaunchWrapper se gli EditBox contengono input validi
    int i = 0;
    for (const auto &list : multipleLaunchWrapper->getWidgets()) {
      for (const auto &widget : std::static_pointer_cast<tgui::VerticalLayout>(list)->getWidgets()) {
        if (widget->getWidgetType() == "EditBox") {
          auto editBox = std::static_pointer_cast<tgui::EditBox>(widget);
          if (!editBox->getText().attemptToFloat(parameters[i]) && !editBox->getText().empty()) {
            editBox->getRenderer()->setTextColor(tgui::Color::Red);
            return;
          }
          i++;
        }
      }
    }
    // TODO fare parsing della stringa prima dell'analisi (invalidare stringhe con lettere in mezzo e accettare apici
    // per separare le cifre)
    // controllo che le deviazioni standard non siano negative o nulle
    if (parameters[sigmaY] <= 0) {
      sigmaYInput->getRenderer()->setTextColor(tgui::Color::Red);
      return;
    }
    if (parameters[sigmaT] <= 0) {
      sigmaTInput->getRenderer()->setTextColor(tgui::Color::Red);
      return;
    }

    app->biliardo_.multipleLaunch(parameters[muY], parameters[sigmaY], parameters[muT], parameters[sigmaT],
                                  static_cast<int>(parameters[N]), app->multipleLaunches_[app->biliardo_.type()]);

    app->designer_.setCanvas(app->biliardo_.r1(), app->multipleLaunches_[app->biliardo_.type()], app->window_);
  });
}

Gui::Gui(sf::RenderWindow &window) : gui{window} { create(); }
}  // namespace bt