//
// Created by paolo on 29/08/2023.
//

#include "Gui.hpp"

#include <algorithm>
#include <boost/numeric/conversion/converter.hpp>
#include <cmath>

#include "App.hpp"

namespace bt {
// TODO
//  testare che i check siano ok

// funzione per formattare le stringe di input numerici in modo da invalidarle o renderle comprensibili al metodo
// tgui::String::attemptToFloat
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

  // salvo il segno del numero e dell'esponenziale per dopo, se non ci non ri aggiungerò nulla
  tgui::String sign{""};
  if (string.starts_with('+') || string.starts_with('-')) {
    sign = string[0];
    string.erase(string.begin());
  }
  tgui::String eSign{""};
  auto eSignPos = std::min(string.find("e+"), string.find("e-")) + 1;
  if (eSignPos != 0) {
    eSign = string[eSignPos];
    string.erase(eSignPos, 1);
  }

  // se contiene anche solo un carattere che non è tra quelli sottoelencati invalido la stringa
  if (string.find_first_not_of("0123456789e.") != tgui::String::npos) {
    return {""};
  }
  // reinserisco l'eventuale segno del numero
  string.insert(0, sign);

  // se contiene più di un punto decimale o più di una "e" invalido la stringa
  if (std::count(string.begin(), string.end(), 'e') > 1 || std::count(string.begin(), string.end(), '.') > 1) {
    return {""};
  }
  // reinserisco l'eventuale segno all'eventuale esponenziale
  eSignPos = string.find('e');
  if (eSignPos != tgui::String::npos) {
    string.insert(eSignPos, eSign);
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
  biliardoButtonsWrapper->add(biliardoChiusoSxBtn, "biliardoChiusoSx");
  biliardoButtonsWrapper->addSpace(.1);
  biliardoButtonsWrapper->add(biliardoChiusoDxBtn, "biliardoChiusoDx");
  biliardoButtonsWrapper->addSpace(.1);

  // bottone e campi per modificare il biliardo
  wrapper->add(newBiliardoWrapper, 2, "newBiliardoWrapper");
  newBiliardoWrapper->addSpace(.05);
  newBiliardoWrapper->add(leftNewBiliardoWrapper, .425, "leftNewBiliardoWrapper");
  leftNewBiliardoWrapper->add(r1Label, .7, "r1Label");
  leftNewBiliardoWrapper->add(r1Input, "r1Input");
  leftNewBiliardoWrapper->add(r2Label, .7, "r2Label");
  leftNewBiliardoWrapper->add(r2Input, "r2Input");
  newBiliardoWrapper->addSpace(.05);
  newBiliardoWrapper->add(rightNewBiliardoWrapper, .425, "rightNewBiliardoWrapper");
  rightNewBiliardoWrapper->add(lLabel, .41, "lLabel");
  rightNewBiliardoWrapper->add(lInput, .588, "lInput");
  rightNewBiliardoWrapper->addSpace(.2);
  rightNewBiliardoWrapper->add(newBiliardoBtn, .8, "newBiliardoBtn");
  newBiliardoWrapper->addSpace(.05);

  wrapper->addSpace(.1);

  // bottone e campi per lanciare la singola particella
  wrapper->add(singleLaunchWrapper, 1, "singleLaunchWrapper");
  singleLaunchWrapper->addSpace(.05);
  singleLaunchWrapper->add(heightWrapper, .28, "heightWrapper");
  heightWrapper->add(heightLabel, .7, "heightLabel");
  heightWrapper->add(heightInput, "heightInput");
  singleLaunchWrapper->addSpace(.03);
  singleLaunchWrapper->add(angleWrapper, .28, "angleWrapper");
  angleWrapper->add(angleLabel, .7, "angleLabel");
  angleWrapper->add(angleInput, "angleInput");
  singleLaunchWrapper->addSpace(.03);
  singleLaunchWrapper->add(singleLaunchBtnWrapper, .28, "singleLaunchBtnWrapper");
  singleLaunchBtnWrapper->addSpace(.2);
  singleLaunchBtnWrapper->add(singleLaunchBtn, .8, "singleLaunchBtn");
  singleLaunchWrapper->addSpace(.05);

  wrapper->addSpace(.05);

  // bottoni per navigare tra i singoli lanci
  wrapper->add(navigateLaunchesWrapper, .5, "navigateLaunchesWrapper");
  navigateLaunchesWrapper->addSpace(.1);
  navigateLaunchesWrapper->add(previousLaunchBtn, "previousLaunchBtn");
  navigateLaunchesWrapper->addSpace(.1);
  navigateLaunchesWrapper->add(nextLaunchBtn, "nextLaunchBtn");
  navigateLaunchesWrapper->addSpace(.1);
  navigateLaunchesWrapper->add(pauseBtn, "pauseBtn");
  navigateLaunchesWrapper->addSpace(.1);
  navigateLaunchesWrapper->add(reRunBtn, "reRunBtn");
  navigateLaunchesWrapper->addSpace(.1);

  wrapper->addSpace(.05);

  // bottone e campi per lanciare N particelle
  wrapper->add(multipleLaunchWrapper, 2.5, "multipleLaunchWrapper");
  multipleLaunchWrapper->addSpace(.05);
  multipleLaunchWrapper->add(leftMultipleLaunchWrapper, .4, "leftMultipleLaunchWrapper");
  leftMultipleLaunchWrapper->add(numberLabel, .8, "numberLabel");
  leftMultipleLaunchWrapper->add(numberInput, .7, "numberInput");
  leftMultipleLaunchWrapper->add(muYLabel, .8, "muYLabel");
  leftMultipleLaunchWrapper->add(muYInput, .7, "muYInput");
  leftMultipleLaunchWrapper->add(sigmaYLabel, .8, "sigmaYLabel");
  leftMultipleLaunchWrapper->add(sigmaYInput, .7, "sigmaYInput");
  multipleLaunchWrapper->addSpace(.1);
  multipleLaunchWrapper->add(rightMultipleLaunchWrapper, .4, "rightMultipleLaunchWrapper");
  rightMultipleLaunchWrapper->add(muTLabel, 1.1, "mutLabel");
  rightMultipleLaunchWrapper->add(muTInput, "muTInput");
  rightMultipleLaunchWrapper->add(sigmaTLabel, 1.1, "sigmaTLabel");
  rightMultipleLaunchWrapper->add(sigmaTInput, "sigmaTInput");
  rightMultipleLaunchWrapper->addSpace(.8);
  rightMultipleLaunchWrapper->add(multipleLaunchBtn, 1.7, "multipleLaunchBtn");
  multipleLaunchWrapper->addSpace(.05);

  wrapper->addSpace(.05);

  // bottoni per navigare tra gli istogrammi
  wrapper->add(navigateHistogramsWrapper, .5, "navigateHistogramsWrapper");
  navigateHistogramsWrapper->addSpace(.1);
  navigateHistogramsWrapper->add(previousHistogramBtn, "previousHistogramBtn");
  navigateHistogramsWrapper->addSpace(.1);
  navigateHistogramsWrapper->add(nextHistogramBtn, "nextHistogramBtn");
  navigateHistogramsWrapper->addSpace(.1);
  navigateHistogramsWrapper->add(saveHistogramBtn, "saveHistogramBtn");
  navigateHistogramsWrapper->addSpace(.1);

  wrapper->addSpace(.2);

  // area di testo per i dati statistici
  wrapper->add(textWrapper, 4, "textWrapper");
  textWrapper->add(leftText, .45, "leftText");
  textWrapper->addSpace(.08);
  textWrapper->add(rightText, .45, "rightText");
  textWrapper->addSpace(.02);
}

void Gui::style() {
  leftText->setEnabled(false);  // disattivo le TexArea per usarle come label ma con un font più sottile
  rightText->setEnabled(false);
  leftText->setTextSize(15);
  rightText->setTextSize(15);
  // usando lo sharedRenderer modifico simultaneamente i due campi di testo
  // ho rimosso la texture di background dal tema se no il background color verrebbe ignorato
  leftText->getSharedRenderer()->setBackgroundColor(tgui::Color::Black);
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
  r1Input->setDefaultText(tgui::String(app->biliardo_.r1()));
  r2Input->setDefaultText(tgui::String(app->biliardo_.r2()));
  lInput->setDefaultText(tgui::String(app->biliardo_.l()));
  newBiliardoBtn->onPress([this, app] {
    float r1, r2, l;

    // cambio singolarmente i parametri se questi sono stati inseriti, controllando che siano validi
    if (format(r1Input->getText()).attemptToFloat(r1) && r1 > 0) {
      app->biliardo_.r1(r1, false);
      r1Input->setDefaultText(tgui::String(r1));          // aggiorno il testo placeholder dell'EditBox
      sigmaYInput->setDefaultText(tgui::String(r1 / 5));  // aggiorno il testo placeholder della sigmaY di default
    } else {
      r1Input->getRenderer()->setTextColor(tgui::Color::Red);
    }
    if (format(r2Input->getText()).attemptToFloat(r2) && r2 > 0) {
      app->biliardo_.r2(r2, false);
      r2Input->setDefaultText(tgui::String(r2));  // aggiorno il testo placeholder dell'EditBox
    } else {
      r2Input->getRenderer()->setTextColor(tgui::Color::Red);
    }
    if (format(lInput->getText()).attemptToFloat(l) && l > 0) {
      app->biliardo_.l(l, false);
      lInput->setDefaultText(tgui::String(l));  // aggiorno il testo placeholder dell'EditBox
    } else {
      lInput->getRenderer()->setTextColor(tgui::Color::Red);
    }

    app->modifyBiliardo();
  });

  // attivo i bottoni per navigare tra un lancio e l'altro
  previousLaunchBtn->onPress([app] {
    if (app->singleLaunchesIndex_[app->biliardo_.type()] != 0) {
      app->singleLaunchesIndex_[app->biliardo_.type()]--;
      app->designer_.reRun(
          app->singleLaunches_[app->biliardo_.type()][app->singleLaunchesIndex_[app->biliardo_.type()]]);
    }
  });
  nextLaunchBtn->onPress([app] {
    if (app->singleLaunchesIndex_[app->biliardo_.type()] != app->singleLaunches_[app->biliardo_.type()].size() - 1) {
      app->singleLaunchesIndex_[app->biliardo_.type()]++;
      app->designer_.reRun(
          app->singleLaunches_[app->biliardo_.type()][app->singleLaunchesIndex_[app->biliardo_.type()]]);
    }
  });
  pauseBtn->onPress([app] { app->designer_.pause(); });
  reRunBtn->onPress([app] {
    app->designer_.reRun(app->singleLaunches_[app->biliardo_.type()][app->singleLaunchesIndex_[app->biliardo_.type()]]);
  });

  // attivo il bottone per i lanci singoli
  singleLaunchBtn->onPress([this, app] {
    float y, t;
    bool isY, isT;

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
    auto& newLaunch = app->newSingleLaunch();
    if (!isY && !isT) {  // se ne y ne teta sono indicate le genera entrambe
      app->biliardo_.launchForDrawing(newLaunch);
    } else if (isY && isT) {  // se invece sono entrambe indicate le usa semplicemente
      app->biliardo_.launchForDrawing(y, t, newLaunch);
    } else if (isY) {  // altrimenti genera solo quella mancante
      app->biliardo_.launchForDrawingNoDir(y, newLaunch);
    } else {
      app->biliardo_.launchForDrawingNoY(t, newLaunch);
    }

    app->designer_.reRun(app->singleLaunches_[app->biliardo_.type()][app->singleLaunchesIndex_[app->biliardo_.type()]]);
    setSingleLaunchText(newLaunch);
  });

  // imposto i placeholder per gli editbox del lancio multiplo con i valori di default delle distribuzioni normali
  muYInput->setDefaultText("0");
  sigmaYInput->setDefaultText(tgui::String(app->biliardo_.r1() / 5));
  muTInput->setDefaultText("0");
  sigmaTInput->setDefaultText(tgui::String(M_PI / 8));
  numberInput->setDefaultText("1'000'000");

  // attivo il bottone per i lanci multipli
  multipleLaunchBtn->onPress([this, app] {
    // dichiaro e gestisco N come un float fino alla fine perché se no non funziona la sintassi con la "e" e posso fare
    // un controllo su un possibile overflow
    float muY = 0, sigmaY = static_cast<float>(app->biliardo_.r1() / 5), muT = 0, sigmaT = M_PI / 8, N = 10e6;

    // catena di if poco elegante ma il for loop non funzionava
    // mi limito a verificare che gli input siano validi perché se non presenti ci sono dei valori di default
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
    // controllo che N non sia negativo o troppo grande per stare in un unsigned int
    unsigned int N_;
    boost::numeric::converter<unsigned int, float> safeFloatToUInt;
    try {
      N_ = safeFloatToUInt(N);
    } catch (
        std::bad_cast&) {  // tutte le eccezioni sollevate dal converter dovrebbero essere sottoclassi di std::bad_cast
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

    auto& histograms = app->newHistograms();
    app->biliardo_.multipleLaunch(muY, sigmaY, muT, sigmaT, N_, histograms);
    app->designer_.setCanvas(histograms, app->window_);
  });

  // attivo i bottoni per navigare tra un istogramma e l'altro
  previousHistogramBtn->onPress([app] {
    if (app->multipleLaunchesIndex_[app->biliardo_.type()] != 0) {
      app->multipleLaunchesIndex_[app->biliardo_.type()]--;
      app->designer_.setCanvas(
          app->multipleLaunches_[app->biliardo_.type()][app->multipleLaunchesIndex_[app->biliardo_.type()]],
          app->window_);
    }
  });
  nextHistogramBtn->onPress([app] {
    if (app->multipleLaunchesIndex_[app->biliardo_.type()] !=
        app->multipleLaunches_[app->biliardo_.type()].size() - 1) {
      app->multipleLaunchesIndex_[app->biliardo_.type()]++;
      app->designer_.setCanvas(
          app->multipleLaunches_[app->biliardo_.type()][app->multipleLaunchesIndex_[app->biliardo_.type()]],
          app->window_);
    }
  });
  saveHistogramBtn->onPress([app] { app->saveHistogram(); });
}

void Gui::setSingleLaunchText(const std::vector<double>& launch) {
  leftText->setText(tgui::String::join(
      {
          "Lancio singolo:",
          " y iniziale:",
          "  " + tgui::String(launch[1]),
          " angolo iniziale:",
          "  " + tgui::String(launch[launch.size() - 1]),
      },
      '\n'));
  rightText->setText(tgui::String::join(
      {
          "",
          " y finale:",
          "  " + tgui::String(launch[launch.size() - 1 - 2]),
          " angolo finale:",
          "  " + tgui::String(launch[launch.size() - 1 - 1]),
      },
      '\n'));
}

Gui::Gui(sf::RenderWindow& window) : gui{window} {
  create();
  style();
}
}  // namespace bt