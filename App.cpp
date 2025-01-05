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

#include "Pool.hpp"

namespace bt {

std::shared_ptr<std::vector<double>>& App::newSingleLaunch() {
  singleLaunches_[pool_.type()].emplace_back(std::make_shared<std::vector<double>>());
  singleLaunchesIndexes_[pool_.type()] = singleLaunches_[pool_.type()].size() - 1;
  return singleLaunches_[pool_.type()].back();
}

std::shared_ptr<std::array<TH1D, 2>>& App::newHistograms() {
  multipleLaunches_[pool_.type()].emplace_back(std::make_shared<std::array<TH1D, 2>>());
  multipleLaunchesIndexes_[pool_.type()] = multipleLaunches_[pool_.type()].size() - 1;
  auto& histograms = multipleLaunches_[pool_.type()].back();
  (*histograms)[0] = TH1D("", "Istogramma delle y di uscita", 1000, -pool_.r1(), pool_.r1());
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
        if (multipleLaunches_[pool_.type()].empty()) {
          designer_.changeSize(pool_, window_, gui_);
        } else {
          designer_.changeSize(
              pool_, *multipleLaunches_[pool_.type()][multipleLaunchesIndexes_[pool_.type()]],
              window_, gui_);
        }
        break;

      default:
        break;
    }
  }
}

App::App(const double l, const double r1, const double r2, const PoolType type,
         const sf::ContextSettings& settings)
    : pool_(l, r1, r2, type),
      window_{{1440, 810}, "Biliardo triangolare", sf::Style::Default, settings},
      designer_(window_), gui_(window_, this) {
  window_.setPosition(sf::Vector2i(100, 100));

  // limitiamo gli fps per far avanzare più facilmente la pallina a velocità costante
  window_.setFramerateLimit(60);
  window_.clear(sf::Color::White);

  // inizializzo tutti i vector di lanci singoli
  for (long unsigned int i = 0; i < 3; i++) {
    [[maybe_unused]] const bool typeChangeResult = pool_.changeType(static_cast<PoolType>(i));
    assert(typeChangeResult);
    const auto& newLaunch = newSingleLaunch();
    pool_.launchForDrawing(*newLaunch);
  }
  [[maybe_unused]] const bool typeChangeResult = pool_.changeType(type);
  assert(typeChangeResult);
  gui_.setSingleLaunchText(*singleLaunches_[pool_.type()][0]);
  designer_.calcPoolBorders(pool_);
  reRun();
}

void App::start() {
  while (window_.isOpen()) {
    handleEvents();
    gui_.draw();
    designer_(*singleLaunches_[pool_.type()][singleLaunchesIndexes_[pool_.type()]], window_);
    window_.display();
  }
}

// TODO documentare l'uso di [[maybe_unused]]
bool App::modifyPool(const double l, const double r1, const double r2) {
  if (!pool_.modify(l, r1, r2)) {
    return false;
  }

  const PoolType actualPoolType = pool_.type();

  // rigenero un lancio per ogni tipo di biliardo e rimuovo i vecchi istogrammi
  for (long unsigned int i = 0; i < 3; i++) {
    singleLaunches_[i].clear();
    multipleLaunches_[i].clear();
    [[maybe_unused]] const bool typeChangeResult = pool_.changeType(static_cast<PoolType>(i));
    assert(typeChangeResult);
    const auto& newLaunch = newSingleLaunch();
    pool_.launchForDrawing(*newLaunch);
  }

  [[maybe_unused]] const bool restoreTypeResult = pool_.changeType(actualPoolType);
  assert(restoreTypeResult);

  // aggiorno la parte grafica
  designer_.changePool(pool_, window_);
  gui_.restoreTextOnPoolChange(*singleLaunches_[pool_.type()][0]);

  // resetto gli indici a 0
  singleLaunchesIndexes_ = {0, 0, 0};
  multipleLaunchesIndexes_ = {0, 0, 0};

  reRun();  // faccio partire la nuova simulazione

  designer_.drawEmptyHistograms(pool_, window_);  // pulisco il grafico
  return true;
}

bool App::changePoolType(const PoolType type) {
  const auto actualPoolType = pool_.type();
  if (!pool_.changeType(type)) {
    return false;
  }
  if (actualPoolType == type) {
    return true;
  }
  designer_.calcPoolBorders(pool_);
  reRun();
  if (multipleLaunches_[pool_.type()].empty()) {
    designer_.drawEmptyHistograms(pool_, window_);
  } else {
    designer_.drawHistograms(*multipleLaunches_[pool_.type()][multipleLaunchesIndexes_[pool_.type()]],
                        window_);
  }
  return true;
}

void App::pause() { designer_.pause(); }

void App::reRun() {
  designer_.reRun(*singleLaunches_[pool_.type()][singleLaunchesIndexes_[pool_.type()]]);
}

std::weak_ptr<const std::vector<double>> App::singleLaunch(
    const std::optional<double> initialY, const std::optional<double> initialDirection) {
  const auto currentIndex = singleLaunchesIndexes_[pool_.type()];

  const auto& newLaunch = newSingleLaunch();

  if (!pool_.launchForDrawing(*newLaunch, initialY, initialDirection)) {
    std::cerr << "Warning: almeno uno dei parametri per il lancio singolo non è valido\n";
    singleLaunches_[pool_.type()].pop_back();
    singleLaunchesIndexes_[pool_.type()] = currentIndex;
    return std::shared_ptr<std::vector<double>>();  // shared pointer nullo
  }

  reRun();
  return newLaunch;
}

std::weak_ptr<const std::array<TH1D, 2>> App::multipleLaunch(const unsigned int N, const double muY,
                                                             const double sigmaY, const double muT,
                                                             const double sigmaT,
                                                             const bool async) {
  const auto currentIndex = multipleLaunchesIndexes_[pool_.type()];

  const auto& histograms = newHistograms();

  if (!pool_.multipleLaunch(N, muY, sigmaY, muT, sigmaT, *histograms, async)) {
    std::cerr << "Warning: almeno uno dei parametri per il lancio multiplo non è valido\n";
    multipleLaunches_[pool_.type()].pop_back();
    multipleLaunchesIndexes_[pool_.type()] = currentIndex;
    return std::shared_ptr<std::array<TH1D, 2>>();  // shared pointer nullo
  }

  designer_.drawHistograms(*histograms, window_);
  return histograms;
}

bool App::nextLaunch() {
  if (singleLaunchesIndexes_[pool_.type()] != singleLaunches_[pool_.type()].size() - 1) {
    singleLaunchesIndexes_[pool_.type()]++;
    reRun();
    return true;
  }
  return false;
}
bool App::previousLaunch() {
  if (singleLaunchesIndexes_[pool_.type()] != 0) {
    singleLaunchesIndexes_[pool_.type()]--;
    reRun();
    return true;
  }
  return false;
}

bool App::nextHistogram() {
  if (!multipleLaunches_[pool_.type()].empty() &&
      multipleLaunchesIndexes_[pool_.type()] != multipleLaunches_[pool_.type()].size() - 1) {
    multipleLaunchesIndexes_[pool_.type()]++;
    designer_.drawHistograms(*multipleLaunches_[pool_.type()][multipleLaunchesIndexes_[pool_.type()]],
                        window_);
    return true;
  }
  return false;
}

bool App::previousHistogram() {
  if (multipleLaunchesIndexes_[pool_.type()] != 0) {
    multipleLaunchesIndexes_[pool_.type()]--;
    designer_.drawHistograms(*multipleLaunches_[pool_.type()][multipleLaunchesIndexes_[pool_.type()]],
                        window_);
    return true;
  }
  return false;
}

void App::saveHistogram(const std::string& filename) const {
  if (multipleLaunches_[pool_.type()].empty()) {
    return;
  }

  auto& histograms = multipleLaunches_[pool_.type()][multipleLaunchesIndexes_[pool_.type()]];

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
    canvas.SaveSource((boost::format("grafico_%1%_%2%_%3%__%4%_%5%_%6%.cxx") % now.tm_mday %
                       (now.tm_mon + 1) % (now.tm_year + 1900) % now.tm_hour % now.tm_min %
                       now.tm_sec)
                          .str()
                          .c_str());
  } else {
    canvas.SaveSource((filename + ".cxx").c_str());
  }
}

}  // namespace bt