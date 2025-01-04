//
// Created by paolo on 23/05/2023.
//

#include "App.hpp"

#include <TCanvas.h>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <boost/format.hpp>
#include <cassert>
#include <ctime>
#include <memory>
#include <optional>

#include "Biliardo.hpp"

namespace bt {

std::shared_ptr<std::vector<double>>& App::newSingleLaunch() {
  singleLaunches_[biliardo_.type()].emplace_back(std::make_shared<std::vector<double>>());
  singleLaunchesIndexes_[biliardo_.type()] = singleLaunches_[biliardo_.type()].size() - 1;
  return singleLaunches_[biliardo_.type()].back();
}

std::shared_ptr<std::array<TH1D, 2>>& App::newHistograms() {
  multipleLaunches_[biliardo_.type()].emplace_back(std::make_shared<std::array<TH1D, 2>>());
  multipleLaunchesIndexes_[biliardo_.type()] = multipleLaunches_[biliardo_.type()].size() - 1;
  auto& histograms = multipleLaunches_[biliardo_.type()].back();
  (*histograms)[0] =
      TH1D("", "Istogramma delle y di uscita", 1000, -biliardo_.r1(), biliardo_.r1());
  (*histograms)[1] = TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2);
  return histograms;
}

void App::handleEvents() {
  while (window_.pollEvent(event_)) {
    gui_.handleEvent(event_);
    switch (event_.type) {
      case sf::Event::Closed:
        window_.close();
        break;  // non necessario ma carino

      case sf::Event::Resized:
        // evito che la finestra sia resa troppo piccola
        if (window_.getSize().x < 1280) {
          window_.setSize({1280, window_.getSize().y});
        }
        if (window_.getSize().y < 720) {
          window_.setSize({window_.getSize().x, 720});
        }

        // creo un'altra immagine dell'istogramma in accordo con la nuova dimensione della finestra
        if (multipleLaunches_[biliardo_.type()].empty()) {
          designer_.changeSize(biliardo_, window_, gui_);
        } else {
          designer_.changeSize(
              biliardo_,
              *multipleLaunches_[biliardo_.type()][multipleLaunchesIndexes_[biliardo_.type()]],
              window_, gui_);
        }
        break;

      default:
        break;
    }
  }
}

App::App(const double l, const double r1, const double r2, const BiliardoType type,
         const sf::ContextSettings& settings)
    : biliardo_(l, r1, r2, type),
      window_{{1280, 720}, "Biliardo triangolare", sf::Style::Default, settings},
      designer_(window_), gui_(window_, this) {
  window_.setPosition(sf::Vector2i(100, 100));

  // limitiamo gli fps per far avanzare più facilmente la pallina a velocità costante
  window_.setFramerateLimit(60);
  window_.clear(sf::Color::White);

  // inizializzo tutti i vector di lanci singoli
  for (long unsigned int i = 0; i < 3; i++) {
    [[maybe_unused]] const bool typeChangeResult =
        biliardo_.changeType(static_cast<BiliardoType>(i));
    assert(typeChangeResult);
    auto& newLaunch = newSingleLaunch();
    biliardo_.launchForDrawing(*newLaunch);
  }
  [[maybe_unused]] const bool typeChangeResult = biliardo_.changeType(type);
  assert(typeChangeResult);
  gui_.setSingleLaunchText(*singleLaunches_[biliardo_.type()][0]);
  designer_.calcBordiBiliardo(biliardo_);
  reRun();
}

void App::start() {
  while (window_.isOpen()) {
    handleEvents();
    gui_.draw();
    designer_(*singleLaunches_[biliardo_.type()][singleLaunchesIndexes_[biliardo_.type()]],
              window_);
    window_.display();
  }
}

// TODO documentare l'uso di [[maybe_unused]]
bool App::modifyBiliardo(const double l, const double r1, const double r2) {
  if (!biliardo_.modify(l, r1, r2)) {
    return false;
  }

  const BiliardoType actualBiliardoType = biliardo_.type();

  // rigenero un lancio per ogni tipo di biliardo e rimuovo i vecchi istogrammi
  for (long unsigned int i = 0; i < 3; i++) {
    singleLaunches_[i].clear();
    multipleLaunches_[i].clear();
    [[maybe_unused]] const bool typeChangeResult =
        biliardo_.changeType(static_cast<BiliardoType>(i));
    assert(typeChangeResult);
    auto& newLaunch = newSingleLaunch();
    biliardo_.launchForDrawing(*newLaunch);
  }

  [[maybe_unused]] const bool restoreTypeResult = biliardo_.changeType(actualBiliardoType);
  assert(restoreTypeResult);

  // aggiorno la parte grafica
  designer_.changeBiliardo(biliardo_, window_);
  gui_.restoreTextOnBiliardoChange(*singleLaunches_[biliardo_.type()][0]);

  // resetto gli indici a 0
  singleLaunchesIndexes_ = {0, 0, 0};
  multipleLaunchesIndexes_ = {0, 0, 0};

  reRun();  // faccio partire la nuova simulazione

  designer_.setCanvas(biliardo_, window_);  // pulisco il grafico
  return true;
}

bool App::changeBiliardoType(const BiliardoType type) {
  const auto actualBiliardoType = biliardo_.type();
  if (!biliardo_.changeType(type)) {
    return false;
  }
  if (actualBiliardoType == type) {
    return true;
  }
  designer_.calcBordiBiliardo(biliardo_);
  reRun();
  if (multipleLaunches_[biliardo_.type()].empty()) {
    designer_.setCanvas(biliardo_, window_);
  } else {
    designer_.setCanvas(
        *multipleLaunches_[biliardo_.type()][multipleLaunchesIndexes_[biliardo_.type()]], window_);
  }
  return true;
}

void App::pause() { designer_.pause(); }

void App::reRun() {
  designer_.reRun(*singleLaunches_[biliardo_.type()][singleLaunchesIndexes_[biliardo_.type()]]);
}

std::weak_ptr<const std::vector<double>> App::singleLaunch(
    const std::optional<double> initialY, const std::optional<double> initialDirection) {
  const auto currentIndex = singleLaunchesIndexes_[biliardo_.type()];

  auto& newLaunch = newSingleLaunch();

  if (!biliardo_.launchForDrawing(*newLaunch, initialY, initialDirection)) {
    std::cerr << "Warning: almeno uno dei parametri per il lancio singolo non è valido\n";
    singleLaunches_[biliardo_.type()].pop_back();
    singleLaunchesIndexes_[biliardo_.type()] = currentIndex;
    return std::shared_ptr<std::vector<double>>();  // shared pointer nullo
  }

  reRun();
  return newLaunch;
}

std::weak_ptr<const std::array<TH1D, 2>> App::multipleLaunch(const unsigned int N, const double muY,
                                                             const double sigmaY, const double muT,
                                                             const double sigmaT,
                                                             const bool async) {
  const auto currentIndex = multipleLaunchesIndexes_[biliardo_.type()];

  auto& histograms = newHistograms();

  if (!biliardo_.multipleLaunch(N, muY, sigmaY, muT, sigmaT, *histograms, async)) {
    std::cerr << "Warning: almeno uno dei parametri per il lancio multiplo non è valido\n";
    multipleLaunches_[biliardo_.type()].pop_back();
    multipleLaunchesIndexes_[biliardo_.type()] = currentIndex;
    return std::shared_ptr<std::array<TH1D, 2>>();  // shared pointer nullo
  }

  designer_.setCanvas(*histograms, window_);
  return histograms;
}

bool App::nextLaunch() {
  if (singleLaunchesIndexes_[biliardo_.type()] != singleLaunches_[biliardo_.type()].size() - 1) {
    singleLaunchesIndexes_[biliardo_.type()]++;
    reRun();
    return true;
  }
  return false;
}
bool App::previousLaunch() {
  if (singleLaunchesIndexes_[biliardo_.type()] != 0) {
    singleLaunchesIndexes_[biliardo_.type()]--;
    reRun();
    return true;
  }
  return false;
}

bool App::nextHistogram() {
  if (!multipleLaunches_[biliardo_.type()].empty() &&
      multipleLaunchesIndexes_[biliardo_.type()] !=
          multipleLaunches_[biliardo_.type()].size() - 1) {
    multipleLaunchesIndexes_[biliardo_.type()]++;
    designer_.setCanvas(
        *multipleLaunches_[biliardo_.type()][multipleLaunchesIndexes_[biliardo_.type()]], window_);
    return true;
  }
  return false;
}

bool App::previousHistogram() {
  if (multipleLaunchesIndexes_[biliardo_.type()] != 0) {
    multipleLaunchesIndexes_[biliardo_.type()]--;
    designer_.setCanvas(
        *multipleLaunches_[biliardo_.type()][multipleLaunchesIndexes_[biliardo_.type()]], window_);
    return true;
  }
  return false;
}

void App::saveHistogram(const std::string& filename) {
  if (multipleLaunches_[biliardo_.type()].empty()) {
    return;
  }

  auto& histograms =
      multipleLaunches_[biliardo_.type()][multipleLaunchesIndexes_[biliardo_.type()]];

  // for (const auto& h : histograms) {
  //   h.GetKurtosis();
  //   h.GetSkewness();
  // }

  auto canvas = TCanvas();
  canvas.SetCanvasSize(1920, 826);

  canvas.Divide(2);

  canvas.cd(1);
  (*histograms)[0].Draw();

  canvas.cd(2);
  (*histograms)[1].Draw();

  if (filename.empty()) {
    const std::time_t t = std::time(nullptr);  // get time now
    std::tm now{};
    localtime_r(&t, &now);
    canvas.SaveSource((boost::format("grafico_%1%_%2%_%3%__%4%_%5%_%6%.root") % now.tm_mday %
                       (now.tm_mon + 1) % (now.tm_year + 1900) % now.tm_hour % now.tm_min %
                       now.tm_sec)
                          .str()
                          .c_str());
  } else {
    canvas.SaveSource((filename + ".c++").c_str());
  }
}

}  // namespace bt