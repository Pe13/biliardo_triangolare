//
// Created by paolo on 23/05/2023.
//

#include "App.hpp"

#include <TCanvas.h>
#include <TStyle.h>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <boost/format.hpp>
#include <cassert>
#include <ctime>
#include <optional>

#include "Biliardo.hpp"

namespace bt {

std::vector<double>& App::newSingleLaunch() {
  singleLaunches_[biliardo_.type()].emplace_back();
  singleLaunchesIndex_[biliardo_.type()] = singleLaunches_[biliardo_.type()].size() - 1;
  return singleLaunches_[biliardo_.type()].back();
}

std::array<TH1D, 2>& App::newHistograms() {
  multipleLaunches_[biliardo_.type()].emplace_back();
  multipleLaunchesIndex_[biliardo_.type()] = multipleLaunches_[biliardo_.type()].size() - 1;
  auto& histograms = multipleLaunches_[biliardo_.type()].back();
  histograms[0] = TH1D("", "Istogramma delle y di uscita", 1000, -biliardo_.r1(), biliardo_.r1());
  histograms[1] = TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2);
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
            multipleLaunches_[biliardo_.type()][multipleLaunchesIndex_[biliardo_.type()]],
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

  // inizializzo tutti i vector di lanci singoli e setto il biliardo iniziale "aperto"
  for (int i = 0; i < 3; i++) {
    [[maybe_unused]] const bool typeChangeResult =
        biliardo_.changeType(static_cast<BiliardoType>(i));
    assert(typeChangeResult);
    auto& newLaunch = newSingleLaunch();
    biliardo_.launchForDrawing(newLaunch, std::nullopt, std::nullopt);
    gui_.setSingleLaunchText(newLaunch);  // aggiorno il testo dei dati riguardo il singolo lancio
  }
  designer_.calcBordiBiliardo(biliardo_);
  reRun();
}

void App::start() {
  while (window_.isOpen()) {
    handleEvents();
    gui_.draw();
    designer_(singleLaunches_[biliardo_.type()][singleLaunchesIndex_[biliardo_.type()]], window_);
    window_.display();
  }
}

// TODO documentare l'uso di [[maybe_unused]]
void App::modifyBiliardo(const double l, const double r1, const double r2) {
  [[maybe_unused]] bool modificationResult = biliardo_.modify(l, r1, r2);
  assert(modificationResult);
  const BiliardoType biliardoType = biliardo_.type();  // salvo il tipo attuale
  // rigenero un lancio per ogni tipo di biliardo e rimuovo i vecchi istogrammi
  for (long unsigned int i = 0; i < 3; i++) {
    singleLaunches_[i].clear();
    multipleLaunches_[i].clear();
    assert(biliardo_.changeType(static_cast<BiliardoType>(i)) == true);
    auto& newLaunch = newSingleLaunch();
    biliardo_.launchForDrawing(newLaunch, std::nullopt, std::nullopt);
  }
  [[maybe_unused]] bool restoreTypeResult = biliardo_.changeType(biliardoType);
  assert(restoreTypeResult);  // ripristino il tipo attuale
  // aggiorno la parte grafica
  designer_.changeBiliardo(biliardo_, window_);
  // resetto gli indici a 0
  singleLaunchesIndex_ = {0, 0, 0};
  multipleLaunchesIndex_ = {0, 0, 0};
  reRun();                                  // faccio partire la nuova simulazione
  designer_.setCanvas(biliardo_, window_);  // pulisco il grafico
}

bool App::changeBiliardoType(const BiliardoType type) {
  if (biliardo_.changeType(type)) {
    designer_.calcBordiBiliardo(biliardo_);
    reRun();
    if (multipleLaunches_[biliardo_.type()].empty()) {
      designer_.setCanvas(biliardo_, window_);
    } else {
      designer_.setCanvas(
          multipleLaunches_[biliardo_.type()][multipleLaunchesIndex_[biliardo_.type()]], window_);
    }
    return true;
  }
  return false;
}

void App::pause() { designer_.pause(); }

void App::reRun() {
  designer_.reRun(singleLaunches_[biliardo_.type()][singleLaunchesIndex_[biliardo_.type()]]);
}

const std::vector<double>& App::singleLaunch(const std::optional<double> initialY,
                                             const std::optional<double> initialDirection) {
  auto& newLaunch = newSingleLaunch();
  biliardo_.launchForDrawing(newLaunch, initialY, initialDirection);

  reRun();
  return newLaunch;
}

const std::array<TH1D, 2>& App::multipleLaunch(const unsigned int N, const double muY, const double sigmaY,
                                               const double muT, const double sigmaT, const bool async) {
  auto& histograms = newHistograms();
  biliardo_.multipleLaunch(N, muY, sigmaY, muT, sigmaT, histograms, async);
  designer_.setCanvas(histograms, window_);
  return histograms;
}


bool App::nextLaunch() {
  if (singleLaunchesIndex_[biliardo_.type()] != singleLaunches_[biliardo_.type()].size() - 1) {
    singleLaunchesIndex_[biliardo_.type()]++;
    reRun();
    return true;
  }
  return false;
}
bool App::previousLaunch() {
  if (singleLaunchesIndex_[biliardo_.type()] != 0) {
    singleLaunchesIndex_[biliardo_.type()]--;
    reRun();
    return true;
  }
  return false;
}

bool App::nextHistogram() {
  if (!multipleLaunches_[biliardo_.type()].empty() &&
      multipleLaunchesIndex_[biliardo_.type()] != multipleLaunches_[biliardo_.type()].size() - 1) {
    multipleLaunchesIndex_[biliardo_.type()]++;
    designer_.setCanvas(
        multipleLaunches_[biliardo_.type()][multipleLaunchesIndex_[biliardo_.type()]], window_);
    return true;
  }
  return false;
}

bool App::previousHistogram() {
  if (multipleLaunchesIndex_[biliardo_.type()] != 0) {
    multipleLaunchesIndex_[biliardo_.type()]--;
    designer_.setCanvas(
        multipleLaunches_[biliardo_.type()][multipleLaunchesIndex_[biliardo_.type()]], window_);
    return true;
  }
  return false;
}

void App::saveHistogram(const std::string& filename) {
  if (multipleLaunches_[biliardo_.type()].empty()) {
    return;
  }

  auto& histograms = multipleLaunches_[biliardo_.type()][multipleLaunchesIndex_[biliardo_.type()]];

  // for (const auto& h : histograms) {
  //   h.GetKurtosis();
  //   h.GetSkewness();
  // }

  auto canvas = TCanvas();
  canvas.SetCanvasSize(1920, 826);

  canvas.Divide(2);

  canvas.cd(1);
  histograms[0].Draw();

  canvas.cd(2);
  histograms[1].Draw();

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