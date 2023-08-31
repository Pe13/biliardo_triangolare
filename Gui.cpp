//
// Created by paolo on 29/08/2023.
//

#include "Gui.hpp"

#include <algorithm>
#include <cmath>

#include "App.hpp"

namespace bt {
// TODO
//  1 - proteggersi dagli overflow negli input limitando il numero di caratteri e passando prima da un uint

// funzione per formattare le stringe di input numerici in modo da invalidarle o renderle comprensibili agli occhi del
// metodo tgui::String::attemptToFloat
tgui::String format(const tgui::String& str) {
  if (str.empty()) {
    return str;
  }
  auto string = str;

  // rimuovo tutti gli apici e gli spazi (che possono essere usati per contare meglio le cifre inserite
  string.remove('\'');
  string.remove(' ');

  string = string.toLower();  // così facendo mi basta controllare "e"

  // trasformo le virgole in punti così da accettare entrambe le notazioni
  std::replace(string.begin(), string.end(), ',', '.');

  // salvo il segno per dopo, se non c'è non ri aggiungerò nulla
  tgui::String sign{""};
  if (string.starts_with('+') || string.starts_with('-')) {
    sign = string[0];
    string.erase(string.begin());
  }
  // se contiene anche solo uno dei caratteri sottoelencati invalido la stringa
  if (string.find_first_not_of("0123456789e.") != tgui::String::npos) {
    return {""};
  }
  // reinserisco l'eventuale segno
  string.insert(0, sign);

  // se contiene più di un punto decimale o più di una "e" invalido la stringa
  if (std::count(string.begin(), string.end(), 'e') > 1 || std::count(string.begin(), string.end(), '.') > 1) {
    return {""};
  }
  // se la "e" è prima del punto invalido la stringa
  if (string.find('e') < string.find('.') && string.find('.') != tgui::String::npos) {
    return {""};
  }

  return string;
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
  biliardoButtonsWrapper->addSpace(.1);

  wrapper->addSpace(.5);

  // bottone e campi per modificare il biliardo
  wrapper->add(newBiliardoWrapper, 2.5, "newBiliardoWrapper");
  newBiliardoWrapper->addSpace(.05);
  newBiliardoWrapper->add(newBiliardoInputWrapper, .3, "newBiliardoInputWrapper");
  newBiliardoInputWrapper->add(r1Label, .7, "r1Label");
  newBiliardoInputWrapper->add(r1Input, "r1Input");
  newBiliardoInputWrapper->add(r2Label, .7, "r2Label");
  newBiliardoInputWrapper->add(r2Input, "r2Input");
  newBiliardoInputWrapper->add(lLabel, .7, "lLabel");
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

void Gui::activate(App* app) {
  // ripristino il colore del testo di default all'ottenimento del focus nel caso questo sia diventato rosso a causa di
  // un dato non valido inserito
  r1Input->onFocus([this] { r1Input->getRenderer()->setTextColor(tgui::Color::White); });
  r2Input->onFocus([this] { r2Input->getRenderer()->setTextColor(tgui::Color::White); });
  lInput->onFocus([this] { lInput->getRenderer()->setTextColor(tgui::Color::White); });
  heightInput->onFocus([this] { heightInput->getRenderer()->setTextColor(tgui::Color::White); });
  angleInput->onFocus([this] { angleInput->getRenderer()->setTextColor(tgui::Color::White); });
  muYInput->onFocus([this] { muYInput->getRenderer()->setTextColor(tgui::Color::White); });
  sigmaYInput->onFocus([this] { sigmaYInput->getRenderer()->setTextColor(tgui::Color::White); });
  muTInput->onFocus([this] { muTInput->getRenderer()->setTextColor(tgui::Color::White); });
  sigmaTInput->onFocus([this] { sigmaTInput->getRenderer()->setTextColor(tgui::Color::White); });
  numberInput->onFocus([this] { numberInput->getRenderer()->setTextColor(tgui::Color::White); });

  // attivo le funzioni dei bottoni per cambiare tipo di biliardo
  biliardoApertoBtn->onPress([app] { app->changeBiliardoType(open); });
  biliardoChiusoDxBtn->onPress([app] { app->changeBiliardoType(rightBounded); });
  biliardoChiusoSxBtn->onPress([app] { app->changeBiliardoType(leftBounded); });

  // gestisco la modifica del biliardo
  r1Input->setText(tgui::String(app->biliardo_.r1()));
  r2Input->setText(tgui::String(app->biliardo_.r2()));
  lInput->setText(tgui::String(app->biliardo_.l()));
  newBiliardoBtn->onPress([this, app] {
    float r1, r2, l;

    // cambio singolarmente i parametri se questi sono stati inseriti, controllando che siano validi
    if (format(r1Input->getText()).attemptToFloat(r1) && r1 > 0) {
      app->biliardo_.r1(r1, false);
      sigmaYInput->setDefaultText(tgui::String(r1 / 5));  // aggiorno il testo placeholder della sigmaY di default
    } else {
      r1Input->getRenderer()->setTextColor(tgui::Color::Red);
    }
    if (format(r2Input->getText()).attemptToFloat(r2) && r2 > 0) {
      app->biliardo_.r2(r2, false);
    } else {
      r2Input->getRenderer()->setTextColor(tgui::Color::Red);
    }
    if (format(lInput->getText()).attemptToFloat(l) && l > 0) {
      app->biliardo_.l(l, false);
    } else {
      lInput->getRenderer()->setTextColor(tgui::Color::Red);
    }

    app->modifyBiliardo();
  });

  // attivo i bottoni per navigare tra un lancio e l'altro
  previousLaunchBtn->onPress(
      [app] { app->designer_.previousLaunch(&app->singleLaunches_[app->biliardo_.type()].front()); });
  nextLaunchBtn->onPress([app] { app->designer_.nextLaunch(&app->singleLaunches_[app->biliardo_.type()].back()); });
  pauseBtn->onPress([app] { app->designer_.pause(); });
  reRunBtn->onPress([app] { app->designer_.reRun(); });

  // attivo il bottone per i lanci singoli
  singleLaunchBtn->onPress([this, app] {
    float y, t;
    bool isY, isT;

    app->singleLaunches_[app->biliardo_.type()].emplace_back();  // genero lo spazio per un nuovo lanci in memoria

    // controllo quali input sono presenti
    if (format(heightInput->getText()).attemptToFloat(y) && std::abs(y) <= app->biliardo_.r1()) {
      isY = true;
    } else if (heightInput->getText().empty()) {
      isY = false;
    } else {
      heightInput->getRenderer()->setTextColor(tgui::Color::Red);
      return;
    }

    if (format(angleInput->getText()).attemptToFloat(t) && std::abs(t) <= M_PI / 2) {
      isT = true;
    } else if (angleInput->getText().empty()) {
      isT = false;
    } else {
      angleInput->getRenderer()->setTextColor(tgui::Color::Red);
      return;
    }

    // gestisco i vari casi
    if (!isY && !isT) {  // se ne y ne teta sono indicate le genera entrambe
      app->biliardo_.launchForDrawing(app->singleLaunches_[app->biliardo_.type()].back());
    } else if (isY && isT) {  // se invece sono entrambe indicate le usa semplicemente
      app->biliardo_.launchForDrawing(y, t, app->singleLaunches_[app->biliardo_.type()].back());
    } else if (isY) {  // altrimenti genera solo quella mancante
      app->biliardo_.launchForDrawingNoDir(y, app->singleLaunches_[app->biliardo_.type()].back());
    } else {
      app->biliardo_.launchForDrawingNoY(t, app->singleLaunches_[app->biliardo_.type()].back());
    }

    app->designer_.setPoints(&app->singleLaunches_[app->biliardo_.type()].back());  // aggiorno il lancio da riprodurre
  });

  // imposto i placeholder per gli editbox del lancio multiplo con i valori di default delle distribuzioni normali
  muYInput->setDefaultText("0");
  sigmaYInput->setDefaultText(tgui::String(app->biliardo_.r1() / 5));
  muTInput->setDefaultText("0");
  sigmaTInput->setDefaultText(tgui::String(M_PI / 8));
  numberInput->setDefaultText("1'000'000");

  multipleLaunchBtn->onPress([this, app] {
    // dichiaro e gestisco N come un float fino alla fine perché se no non funziona la sintassi con la e
    float muY = 0, sigmaY = static_cast<float>(app->biliardo_.r1() / 5), muT = 0, sigmaT = M_PI / 8, N = 10e6;

    app->multipleLaunches_[app->biliardo_.type()].clear();  // pulisco la memoria per un nuovo lancio

    // catena di if poco elegante ma il for loop non funzionava
    // mi limito a verificare che gli input siano validi perché ho dei valori di default
    if (!format(muYInput->getText()).attemptToFloat(muY) && !muYInput->getText().empty()) {
      muYInput->getRenderer()->setTextColor((tgui::Color::Red));
      return;
    }
    if (!format(sigmaYInput->getText()).attemptToFloat(sigmaY) && !sigmaYInput->getText().empty()) {
      sigmaYInput->getRenderer()->setTextColor((tgui::Color::Red));
      return;
    }
    if (!format(muTInput->getText()).attemptToFloat(muT) && !muTInput->getText().empty()) {
      muTInput->getRenderer()->setTextColor((tgui::Color::Red));
      return;
    }
    if (!format(sigmaTInput->getText()).attemptToFloat(sigmaT) && !sigmaTInput->getText().empty()) {
      sigmaTInput->getRenderer()->setTextColor((tgui::Color::Red));
      return;
    }
    if (!format(numberInput->getText()).attemptToFloat(N) && !numberInput->getText().empty()) {
      numberInput->getRenderer()->setTextColor((tgui::Color::Red));
      return;
    }

    // controllo che le deviazioni standard non siano negative o nulle
    if (sigmaY <= 0) {
      sigmaYInput->getRenderer()->setTextColor(tgui::Color::Red);
      return;
    }
    if (sigmaT <= 0) {
      sigmaTInput->getRenderer()->setTextColor(tgui::Color::Red);
      return;
    }

    app->biliardo_.multipleLaunch(muY, sigmaY, muT, sigmaT, static_cast<int>(N),
                                  app->multipleLaunches_[app->biliardo_.type()]);

    app->designer_.setCanvas(app->biliardo_.r1(), app->multipleLaunches_[app->biliardo_.type()], app->window_);
  });
}

Gui::Gui(sf::RenderWindow& window) : gui{window} { create(); }
}  // namespace bt