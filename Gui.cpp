//
// Created by paolo on 29/08/2023.
//

#include "Gui.hpp"

#include <TH1D.h>

#include <boost/numeric/conversion/converter.hpp>
#include <cmath>

#include "App.hpp"

namespace bt {

void Gui::newPoolBtnPressed(App* app) const {
  std::array<std::optional<double>, 3> newParameters{app->pool().r1(), app->pool().r2(),
                                                     app->pool().l()};
  bool hasChanged = false;
  bool error = false;

  forEachIndexed<tgui::EditBox>(
      newPoolWrapper_->getWidgets().begin(), newPoolWrapper_->getWidgets().end(),
      [&newParameters, &hasChanged, &error](const tgui::EditBox::Ptr& inputBox,
                                            const unsigned int i) {
        if (const auto readValue = inputStringToDouble(inputBox->getText())) {
          newParameters[i] = readValue;
        }

        if (newParameters[i] && newParameters[i].value() > 0) {
          inputBox->setDefaultText(tgui::String(newParameters[i].value()));
          inputBox->setText("");
          hasChanged = true;
        } else if (!inputBox->getText().empty()) {
          inputBox->getRenderer()->setTextColor(tgui::Color::Red);
          error = true;
        }
      });

  if (hasChanged && !error) {
    [[maybe_unused]] const bool modificationResult = app->modifyPool(
        newParameters[2].value(), newParameters[0].value(), newParameters[1].value());
    assert(modificationResult);
    sigmaYInput_->setDefaultText(tgui::String(
        app->pool().r1() / 5.));  // aggiorno il testo placeholder della sigmaY di default
  }
}

void Gui::singleLaunchBtnPressed(App* app) const {
  const std::optional<double> y = inputStringToDouble(heightInput_->getText());
  const std::optional<double> t = inputStringToDouble(angleInput_->getText());
  bool failed = false;

  if (!heightInput_->getText().empty() &&
      (!y.has_value() || std::abs(y.value()) >= app->pool().r1())) {
    heightInput_->getRenderer()->setTextColor(tgui::Color::Red);
    failed = true;
  }

  if (!angleInput_->getText().empty() && (!t.has_value() || std::abs(t.value()) >= M_PI / 2)) {
    angleInput_->getRenderer()->setTextColor(tgui::Color::Red);
    failed = true;
  }

  if (failed) {
    return;
  }

  const auto launch = app->singleLaunch(y, t);
  assert(!launch.expired());
  setSingleLaunchText(*launch.lock());
}

void Gui::multipleLaunchBtnPressed(App* app) const {
  // dichiaro e gestisco N come un float fino alla fine perché se no non funziona la sintassi con la
  // "e" e posso fare un controllo su un possibile overflow
  std::array<std::optional<double>, 5> launchParameters{1e6, 0, app->pool().r1() / 5, 0, M_PI / 8};
  const double& N = launchParameters[0].value();
  const double& muY = launchParameters[1].value();
  const double& sigmaY = launchParameters[2].value();
  const double& muT = launchParameters[3].value();
  const double& sigmaT = launchParameters[4].value();

  bool error = false;

  // Controllo che tutti i campi, se compilati, siano numeri validi
  forEachIndexed<tgui::EditBox>(
      multipleLaunchWrapper_->getWidgets().begin(), multipleLaunchWrapper_->getWidgets().end(),
      [&launchParameters, &error](const tgui::EditBox::Ptr& inputBox, const unsigned int i) {
        if (!inputBox->getText().empty()) {
          launchParameters[i] = inputStringToDouble(inputBox->getText());
          if (!launchParameters[i]) {
            inputBox->getRenderer()->setTextColor(tgui::Color::Red);
            error = true;
          }
        }
      });

  // controllo che le deviazioni standard non siano negative o nulle
  if (sigmaY <= 0) {
    sigmaYInput_->getRenderer()->setTextColor(tgui::Color::Red);
    error = true;
  }
  if (sigmaT <= 0) {
    sigmaTInput_->getRenderer()->setTextColor(tgui::Color::Red);
    error = true;
  }

  // Converto N in unsigned int stando attento che non ci sia overflow
  unsigned int N_{};
  try {
    constexpr boost::numeric::converter<unsigned int, double> safeDoubleToUInt;
    N_ = safeDoubleToUInt(N);
  } catch (std::bad_cast&) {
    numberInput_->getRenderer()->setTextColor(tgui::Color::Red);
    error = true;
  }

  if (error) {
    return;
  }

  const auto launch = app->multipleLaunch(N_, muY, sigmaY, muT, sigmaT);
  assert(!launch.expired());
  setStatisticsText(*launch.lock());
}

void Gui::create() {
  gui_.add(wrapper_, "wrapper");

  wrapper_->getRenderer()->setPadding({0, 10});

  // bottoni per cambiare tipo di biliardo
  wrapper_->add(poolButtonsWrapper_, "buttonsWrapper");
  poolButtonsWrapper_->addSpace(.1f);
  poolButtonsWrapper_->add(openPoolBtn_, "biliardoAperto");
  poolButtonsWrapper_->addSpace(.1f);
  poolButtonsWrapper_->add(leftClosedPoolBtn_, "biliardoChiusoSx");
  poolButtonsWrapper_->addSpace(.1f);
  poolButtonsWrapper_->add(rightClosedPoolBtn_, "biliardoChiusoDx");
  poolButtonsWrapper_->addSpace(.1f);

  // bottone e campi per modificare il biliardo
  wrapper_->add(newPoolWrapper_, 2.f, "newBiliardoWrapper");
  newPoolWrapper_->addSpace(.05f);
  newPoolWrapper_->add(leftNewPoolWrapper_, .425f, "leftNewBiliardoWrapper");
  leftNewPoolWrapper_->add(r1Label_, .7f, "r1Label");
  leftNewPoolWrapper_->add(r1Input_, "r1Input");
  leftNewPoolWrapper_->add(r2Label_, .7f, "r2Label");
  leftNewPoolWrapper_->add(r2Input_, "r2Input");
  newPoolWrapper_->addSpace(.05f);
  newPoolWrapper_->add(rightNewPoolWrapper_, .425f, "rightNewBiliardoWrapper");
  rightNewPoolWrapper_->add(lLabel_, .41f, "lLabel");
  rightNewPoolWrapper_->add(lInput_, .588f, "lInput");
  rightNewPoolWrapper_->addSpace(.2f);
  rightNewPoolWrapper_->add(newPoolBtn_, .8f, "newBiliardoBtn");
  newPoolWrapper_->addSpace(.05f);

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
  // disattivo le TexArea per usarle come label ma con un font più sottile
  leftText_->setEnabled(false);
  rightText_->setEnabled(false);
  leftText_->setTextSize(13);
  rightText_->setTextSize(13);

  // usando lo sharedRenderer modifico simultaneamente i due campi di testo
  // ho rimosso la texture di background dal tema se no il background color verrebbe ignorato
  leftText_->getSharedRenderer()->setBackgroundColor(tgui::Color::Black);
}

void Gui::activate(App* app) const {
  // ripristino il colore del testo di default all'ottenimento del focus nel caso questo sia
  // diventato rosso a causa di un dato non valido inserito
  forEach<tgui::EditBox>(gui_.getWidgets().begin(), gui_.getWidgets().end(),
                         [](tgui::EditBox::Ptr const& box) {
                           box->onFocus([box] {
                             box->getRenderer()->setTextColor(tgui::Color::White);
                           });  // box catturato by value se no crash
                         });

  // attivo le funzioni dei bottoni per cambiare tipo di biliardo
  openPoolBtn_->onPress(&App::changePoolType, app, open);
  rightClosedPoolBtn_->onPress(&App::changePoolType, app, rightBounded);
  leftClosedPoolBtn_->onPress(&App::changePoolType, app, leftBounded);

  // gestisco la modifica del biliardo
  r1Input_->setDefaultText(tgui::String(app->pool().r1()));
  r2Input_->setDefaultText(tgui::String(app->pool().r2()));
  lInput_->setDefaultText(tgui::String(app->pool().l()));
  newPoolBtn_->onPress(&Gui::newPoolBtnPressed, this, app);

  // attivo i bottoni per navigare tra un lancio e l'altro
  previousLaunchBtn_->onPress(&App::previousLaunch, app);
  nextLaunchBtn_->onPress(&App::nextLaunch, app);
  pauseBtn_->onPress(&App::pause, app);
  reRunBtn_->onPress(&App::reRun, app);

  // attivo il bottone per i lanci singoli
  singleLaunchBtn_->onPress(&Gui::singleLaunchBtnPressed, this, app);

  // imposto i placeholder per gli editbox del lancio multiplo con i valori di default delle
  // distribuzioni normali
  muYInput_->setDefaultText("0");
  sigmaYInput_->setDefaultText(tgui::String(app->pool().r1() / 5));
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

void Gui::handleEvent(const sf::Event& event) { gui_.handleEvent(event); }

void Gui::draw() { gui_.draw(); }

void Gui::setSize(const float width, const float height) const { wrapper_->setSize(width, height); }

void Gui::setSingleLaunchText(const std::vector<double>& launch) const {
  const auto unchangedPartLeft = leftText_->getText().substr(leftText_->getText().find("\nL"));
  const auto unchangedPartRight =
      rightText_->getText().substr(rightText_->getText().find("\n\n d"));

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
  const auto unchangedPartLeft = leftText_->getText().substr(0, leftText_->getText().find("\n m"));
  const auto unchangedPartRight =
      rightText_->getText().substr(0, rightText_->getText().find("\n\n d") + 1);

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

void Gui::restoreTextOnPoolChange(const std::vector<double>& singleLaunch) const {
  setDefaultText();
  setSingleLaunchText(singleLaunch);
}

}  // namespace bt