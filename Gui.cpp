//
// Created by paolo on 29/08/2023.
//

#include "Gui.hpp"

#include <TH1D.h>

#include <algorithm>
#include <boost/numeric/conversion/converter.hpp>
#include <cmath>

#include "App.hpp"

namespace bt {

void Gui::newBiliardoBtnPressed(bt::App* app) const {
  std::array<double, 3> data{app->biliardo_.r1(), app->biliardo_.r2(), app->biliardo_.l()};
  bool hasChanged = false;
  bool error = false;

  forEachIndexed<tgui::EditBox>(
      newBiliardoWrapper_->getWidgets().begin(), newBiliardoWrapper_->getWidgets().end(),
      [&data, &hasChanged, &error](const tgui::EditBox::Ptr& box, unsigned int i) {
        if (isValidInput(box->getText(), data[i]) && data[i] > 0) {
          box->setDefaultText(tgui::String(data[i]));  // aggiorno il testo placeholder dell'EditBox
          box->setText("");                            // Pulisco l'EditBox
          hasChanged = true;
        } else if (!box->getText().empty()) {
          box->getRenderer()->setTextColor(tgui::Color::Red);
          error = true;
        }
      });

  if (hasChanged && !error) {
    app->biliardo_.modify(data[0], data[1], data[2], false);
    sigmaYInput_->setDefaultText(tgui::String(data[0] / 5));  // aggiorno il testo placeholder della sigmaY di default
    app->modifyBiliardo();
  }
}

void Gui::singleLaunchBtnPressed(bt::App* app) const {
  double y;
  double t;
  bool isY;
  bool isT;
  bool failed = false;

  // controllo quali input sono presenti

  if (heightInput_->getText().empty()) {
    isY = false;
  } else if (isValidInput(heightInput_->getText(), y) && std::abs(y) <= app->biliardo_.r1()) {
    isY = true;
  } else {
    heightInput_->getRenderer()->setTextColor(tgui::Color::Red);
    failed = true;
  }

  if (angleInput_->getText().empty()) {
    isT = false;
  } else if (isValidInput(angleInput_->getText(), t) && std::abs(t) <= M_PI / 2) {
    isT = true;
  } else {
    angleInput_->getRenderer()->setTextColor(tgui::Color::Red);
    failed = true;
  }

  if (failed) {
    return;
  }

  // gestisco i vari casi
  auto& newLaunch = app->newSingleLaunch();
  if (!isY && !isT) {  // se ne y ne teta sono indicate le genera entrambe
    app->biliardo_.launchForDrawing(newLaunch);
  } else if (isY && isT) {  // se invece sono entrambe indicate le usa semplicemente
    app->biliardo_.launchForDrawing(y, t, newLaunch, false);
  } else if (isY) {  // altrimenti genera solo quella mancante
    app->biliardo_.launchForDrawingNoDir(y, newLaunch, false);
  } else {
    app->biliardo_.launchForDrawingNoY(t, newLaunch, false);
  }

  app->reRun();
  setSingleLaunchText(newLaunch);
}

void Gui::multipleLaunchBtnPressed(App* app) const {
  // dichiaro e gestisco N come un float fino alla fine perché se no non funziona la sintassi con la "e" e posso fare
  // un controllo su un possibile overflow
  //    double muY = 0, sigmaY = static_cast<float>(app->biliardo_.r1() / 5), muT = 0, sigmaT = M_PI / 8, N = 1e6;
  std::array<double, 5> data{1e6, 0, app->biliardo_.r1() / 5, 0, M_PI / 8};
  const double& N = data[0];
  const double& muY = data[1];
  const double& sigmaY = data[2];
  const double& muT = data[3];
  const double& sigmaT = data[4];

  bool error = false;

  // mi limito a verificare che gli input siano validi perché se non presenti ci sono dei valori di default
  forEachIndexed<tgui::EditBox>(multipleLaunchWrapper_->getWidgets().begin(), multipleLaunchWrapper_->getWidgets().end(),
                                [&data, &error](const tgui::EditBox::Ptr& box, unsigned int i) {
                                  if (!isValidInput(box->getText(), data[i]) && !box->getText().empty()) {
                                    box->getRenderer()->setTextColor(tgui::Color::Red);
                                    error = true;
                                  }
                                });

  unsigned int N_;
  boost::numeric::converter<unsigned int, double> safeDoubleToUInt;
  try {
    N_ = safeDoubleToUInt(N);
  } catch (
      std::bad_cast&) {  // tutte le eccezioni sollevate dal converter dovrebbero essere sottoclassi di std::bad_cast
    numberInput_->getRenderer()->setTextColor(tgui::Color::Red);
    error = true;
  }

  // controllo che le deviazioni standard non siano negative o nulle
  if (sigmaY <= 0) {
    sigmaYInput_->getRenderer()->setTextColor(tgui::Color::Red);
    error = true;
  }
  if (sigmaT <= 0) {
    sigmaTInput_->getRenderer()->setTextColor(tgui::Color::Red);
    error = true;
  }

  if (error) {
    return;
  }

  auto& histograms = app->newHistograms();
  app->biliardo_.multipleLaunch(muY, sigmaY, muT, sigmaT, N_, histograms);
  app->designer_.setCanvas(histograms, app->window_);
  setStatisticsText(histograms);
}

void Gui::create() {
  gui_.add(wrapper_, "wrapper");

  wrapper_->getRenderer()->setPadding({0, 10});

  // bottoni per cambiare tipo di biliardo
  wrapper_->add(biliardoButtonsWrapper_, "buttonsWrapper");
  biliardoButtonsWrapper_->addSpace(.1f);
  biliardoButtonsWrapper_->add(biliardoApertoBtn_, "biliardoAperto");
  biliardoButtonsWrapper_->addSpace(.1f);
  biliardoButtonsWrapper_->add(biliardoChiusoSxBtn_, "biliardoChiusoSx");
  biliardoButtonsWrapper_->addSpace(.1f);
  biliardoButtonsWrapper_->add(biliardoChiusoDxBtn_, "biliardoChiusoDx");
  biliardoButtonsWrapper_->addSpace(.1f);

  // bottone e campi per modificare il biliardo
  wrapper_->add(newBiliardoWrapper_, 2.f, "newBiliardoWrapper");
  newBiliardoWrapper_->addSpace(.05f);
  newBiliardoWrapper_->add(leftNewBiliardoWrapper_, .425f, "leftNewBiliardoWrapper");
  leftNewBiliardoWrapper_->add(r1Label_, .7f, "r1Label");
  leftNewBiliardoWrapper_->add(r1Input_, "r1Input");
  leftNewBiliardoWrapper_->add(r2Label_, .7f, "r2Label");
  leftNewBiliardoWrapper_->add(r2Input_, "r2Input");
  newBiliardoWrapper_->addSpace(.05f);
  newBiliardoWrapper_->add(rightNewBiliardoWrapper_, .425f, "rightNewBiliardoWrapper");
  rightNewBiliardoWrapper_->add(lLabel_, .41f, "lLabel");
  rightNewBiliardoWrapper_->add(lInput_, .588f, "lInput");
  rightNewBiliardoWrapper_->addSpace(.2f);
  rightNewBiliardoWrapper_->add(newBiliardoBtn_, .8f, "newBiliardoBtn");
  newBiliardoWrapper_->addSpace(.05f);

  wrapper_->addSpace(.02f);

  // bottone e campi per lanciare la singola particella
  wrapper_->add(singleLaunchWrapper_, 1.f, "singleLaunchWrapper");
  singleLaunchWrapper_->addSpace(.05f);
  singleLaunchWrapper_->add(heightWrapper_, .28f, "heightWrapper");
  heightWrapper_->add(heightLabel_, .7f, "heightLabel");
  heightWrapper_->add(heightInput_, "heightInput");
  singleLaunchWrapper_->addSpace(.03f);
  singleLaunchWrapper_->add(angleWrapper_, .28f, "angleWrapper");
  angleWrapper_->add(angleLabel_, .7f, "angleLabel");
  angleWrapper_->add(angleInput_, "angleInput");
  singleLaunchWrapper_->addSpace(.03f);
  singleLaunchWrapper_->add(singleLaunchBtnWrapper_, .28f, "singleLaunchBtnWrapper");
  singleLaunchBtnWrapper_->addSpace(.2f);
  singleLaunchBtnWrapper_->add(singleLaunchBtn_, .8f, "singleLaunchBtn");
  singleLaunchWrapper_->addSpace(.05f);

  wrapper_->addSpace(.01f);

  // bottoni per navigare tra i singoli lanci
  wrapper_->add(navigateLaunchesWrapper_, .5f, "navigateLaunchesWrapper");
  navigateLaunchesWrapper_->addSpace(.1f);
  navigateLaunchesWrapper_->add(previousLaunchBtn_, "previousLaunchBtn");
  navigateLaunchesWrapper_->addSpace(.1f);
  navigateLaunchesWrapper_->add(nextLaunchBtn_, "nextLaunchBtn");
  navigateLaunchesWrapper_->addSpace(.1f);
  navigateLaunchesWrapper_->add(pauseBtn_, "pauseBtn");
  navigateLaunchesWrapper_->addSpace(.1f);
  navigateLaunchesWrapper_->add(reRunBtn_, "reRunBtn");
  navigateLaunchesWrapper_->addSpace(.1f);

  wrapper_->addSpace(.02f);

  // bottone e campi per lanciare N particelle
  wrapper_->add(multipleLaunchWrapper_, 2.5f, "multipleLaunchWrapper");
  multipleLaunchWrapper_->addSpace(.05f);
  multipleLaunchWrapper_->add(leftMultipleLaunchWrapper_, .4f, "leftMultipleLaunchWrapper");
  leftMultipleLaunchWrapper_->add(numberLabel_, .8f, "numberLabel");
  leftMultipleLaunchWrapper_->add(numberInput_, .7f, "numberInput");
  leftMultipleLaunchWrapper_->add(muYLabel_, .8f, "muYLabel");
  leftMultipleLaunchWrapper_->add(muYInput_, .7f, "muYInput");
  leftMultipleLaunchWrapper_->add(sigmaYLabel_, .8f, "sigmaYLabel");
  leftMultipleLaunchWrapper_->add(sigmaYInput_, .7f, "sigmaYInput");
  multipleLaunchWrapper_->addSpace(.1f);
  multipleLaunchWrapper_->add(rightMultipleLaunchWrapper_, .4f, "rightMultipleLaunchWrapper");
  rightMultipleLaunchWrapper_->add(muTLabel_, 1.1f, "mutLabel");
  rightMultipleLaunchWrapper_->add(muTInput_, "muTInput");
  rightMultipleLaunchWrapper_->add(sigmaTLabel_, 1.1f, "sigmaTLabel");
  rightMultipleLaunchWrapper_->add(sigmaTInput_, "sigmaTInput");
  rightMultipleLaunchWrapper_->addSpace(.8f);
  rightMultipleLaunchWrapper_->add(multipleLaunchBtn_, 1.7f, "multipleLaunchBtn");
  multipleLaunchWrapper_->addSpace(.05f);

  wrapper_->addSpace(.02f);

  // bottoni per navigare tra gli istogrammi
  wrapper_->add(navigateHistogramsWrapper_, .5f, "navigateHistogramsWrapper");
  navigateHistogramsWrapper_->addSpace(.1f);
  navigateHistogramsWrapper_->add(previousHistogramBtn_, "previousHistogramBtn");
  navigateHistogramsWrapper_->addSpace(.1f);
  navigateHistogramsWrapper_->add(nextHistogramBtn_, "nextHistogramBtn");
  navigateHistogramsWrapper_->addSpace(.1f);
  navigateHistogramsWrapper_->add(saveHistogramBtn_, "saveHistogramBtn");
  navigateHistogramsWrapper_->addSpace(.1f);

  wrapper_->addSpace(.03f);

  // area di testo per i dati statistici
  wrapper_->add(textWrapper_, 4.f, "textWrapper");
  textWrapper_->add(leftText_, .45f, "leftText");
  textWrapper_->addSpace(.08f);
  textWrapper_->add(rightText_, .45f, "rightText");
  textWrapper_->addSpace(.02f);
}

void Gui::style() const {
  leftText_->setEnabled(false);  // disattivo le TexArea per usarle come label ma con un font più sottile
  rightText_->setEnabled(false);
  leftText_->setTextSize(13);
  rightText_->setTextSize(13);
  // usando lo sharedRenderer modifico simultaneamente i due campi di testo
  // ho rimosso la texture di background dal tema se no il background color verrebbe ignorato
  leftText_->getSharedRenderer()->setBackgroundColor(tgui::Color::Black);
}

void Gui::activate(App* app) const {
  // ripristino il colore del testo di default all'ottenimento del focus nel caso questo sia diventato rosso a causa di
  // un dato non valido inserito
  forEach<tgui::EditBox>(gui_.getWidgets().begin(), gui_.getWidgets().end(), [](tgui::EditBox::Ptr const& box) {
    box->onFocus(
        [box] { box->getRenderer()->setTextColor(tgui::Color::White); });  // box catturato by value se no crash
  });

  // attivo le funzioni dei bottoni per cambiare tipo di biliardo
  biliardoApertoBtn_->onPress(&App::changeBiliardoType, app, open);
  biliardoChiusoDxBtn_->onPress(&App::changeBiliardoType, app, rightBounded);
  biliardoChiusoSxBtn_->onPress(&App::changeBiliardoType, app, leftBounded);

  // gestisco la modifica del biliardo
  r1Input_->setDefaultText(tgui::String(app->biliardo_.r1()));
  r2Input_->setDefaultText(tgui::String(app->biliardo_.r2()));
  lInput_->setDefaultText(tgui::String(app->biliardo_.l()));
  newBiliardoBtn_->onPress(&Gui::newBiliardoBtnPressed, this, app);

  // attivo i bottoni per navigare tra un lancio e l'altro
  previousLaunchBtn_->onPress(&App::previousLaunch, app);
  nextLaunchBtn_->onPress(&App::nextLaunch, app);
  pauseBtn_->onPress(&App::pause, app);
  reRunBtn_->onPress(&App::reRun, app);

  // attivo il bottone per i lanci singoli
  singleLaunchBtn_->onPress(&Gui::singleLaunchBtnPressed, this, app);

  // imposto i placeholder per gli editbox del lancio multiplo con i valori di default delle distribuzioni normali
  muYInput_->setDefaultText("0");
  sigmaYInput_->setDefaultText(tgui::String(app->biliardo_.r1() / 5));
  muTInput_->setDefaultText("0");
  sigmaTInput_->setDefaultText(tgui::String(M_PI / 8));
  numberInput_->setDefaultText("1'000'000");

  // attivo il bottone per i lanci multipli
  multipleLaunchBtn_->onPress(&Gui::multipleLaunchBtnPressed, this, app);

  // attivo i bottoni per navigare tra un istogramma e l'altro
  previousHistogramBtn_->onPress(&App::previousHistogram, app);
  nextHistogramBtn_->onPress(&App::nextHistogram, app);
  saveHistogramBtn_->onPress(&App::saveHistogram, app, "");
}

void Gui::setDefaultText() const {
  leftText_->setText(tgui::String::join(
      {
          "Lancio singolo:",
          " y iniziale:",
          "",
          " angolo iniziale:",
          "",
          "",
          "Lancio multiplo:",
          " media y:",
          "",
          " asimmetria y:",
          "",
          " media angoli:",
          "",
          " asimmetria angoli:",
          "",
      },
      '\n'));

  rightText_->setText(tgui::String::join(
      {
          "",
          " y finale:",
          "",
          " angolo finale:",
          "",
          "",
          "",
          " dev. y:",
          "",
          " curtosi y:",
          "",
          " dev. angoli:",
          "",
          " curtosi angoli:",
          "",
      },
      '\n'));
}

Gui::Gui(sf::RenderWindow& window, App* app) : gui_{window} {
  create();
  style();
  setDefaultText();
  activate(app);
}

void Gui::handleEvent(sf::Event& event) {
  gui_.handleEvent(event);
}

void Gui::draw() {
  gui_.draw();
}

void Gui::setSize(float width, float height) const {
  wrapper_->setSize(width, height);
}

void Gui::setSingleLaunchText(const std::vector<double>& launch) const {
  auto unchangedPartLeft = leftText_->getText().substr(leftText_->getText().find("\nL"));
  auto unchangedPartRight = rightText_->getText().substr(rightText_->getText().find("\n\n d"));

  leftText_->setText(tgui::String::join(
      {
          "Lancio singolo:",
          " y iniziale:",
          "  " + tgui::String(launch[1]),
          " angolo iniziale:",
          "  " + tgui::String(launch[launch.size() - 1]),
          unchangedPartLeft,
      },
      '\n'));
  rightText_->setText(tgui::String::join(
      {
          "",
          " y finale:",
          "  " + tgui::String(launch[launch.size() - 1 - 2]),
          " angolo finale:",
          "  " + tgui::String(launch[launch.size() - 1 - 1]),
          unchangedPartRight,
      },
      '\n'));
}

void Gui::setStatisticsText(const std::array<TH1D, 2>& histograms) const {
  auto unchangedPartLeft = leftText_->getText().substr(0, leftText_->getText().find("\n m"));
  auto unchangedPartRight = rightText_->getText().substr(0, rightText_->getText().find("\n\n d") + 1);

  leftText_->setText(tgui::String::join(
      {
          unchangedPartLeft,
          " media y:",
          " " + tgui::String(histograms[0].GetMean()),
          " asimmetria y:",
          " " + tgui::String(histograms[0].GetSkewness()),
          " media angoli:",
          " " + tgui::String(histograms[1].GetMean()),
          " asimmetria angoli:",
          " " + tgui::String(histograms[1].GetSkewness()),
      },
      '\n'));

  rightText_->setText(tgui::String::join(
      {
          unchangedPartRight,
          " dev. y:",
          " " + tgui::String(histograms[0].GetStdDev()),
          " curtosi y:",
          " " + tgui::String(histograms[0].GetKurtosis()),
          " dev. angoli:",
          " " + tgui::String(histograms[1].GetStdDev()),
          " curtosi angoli:",
          " " + tgui::String(histograms[1].GetKurtosis()),
      },
      '\n'));
}

}  // namespace bt