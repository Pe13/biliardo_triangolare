//
// Created by paolo on 23/05/2023.
//

#include "App.hpp"

#include <TCanvas.h>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <boost/format.hpp>
#include <ctime>

#include "Biliardo.hpp"

namespace bt {

App::App(const double l, const double r1, const double r2, const BiliardoType type)
    : biliardo_(l, r1, r2, type), gui_(window_) {
  gui_.activate(this);

  window_.setPosition(sf::Vector2i(100, 100));

  // limitiamo gli fps per far avanzare più facilmente la pallina a velocità costante
  window_.setFramerateLimit(60);
  window_.clear(sf::Color::White);

  // inizializzo tutti i vector di lanci singoli e setto il biliardo iniziale "aperto"
  for (int i = 2; i > -1; i--) {
    biliardo_.changeType(static_cast<BiliardoType>(i));
    auto& newLaunch = newSingleLaunch();
    biliardo_.launchForDrawing(newLaunch);
    gui_.setSingleLaunchText(newLaunch);  // aggiorno il testo dei dati riguardo il singolo lancio
  }
  designer_.calcBordiBiliardo(biliardo_);
  reRun();
}

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
    gui_.gui.handleEvent(event_);
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
          designer_.changeSize(biliardo_, window_, gui_.wrapper);
        } else {
          designer_.changeSize(biliardo_, multipleLaunches_[biliardo_.type()][multipleLaunchesIndex_[biliardo_.type()]],
                               window_, gui_.wrapper);
        }
        break;

      default:
        break;
    }
  }
}

void App::start() {
  designer_.initWindow(window_);
  while (window_.isOpen()) {
    handleEvents();
    gui_.gui.draw();
    designer_(singleLaunches_[biliardo_.type()][singleLaunchesIndex_[biliardo_.type()]], window_);
    window_.display();
  }
}

bool App::changeBiliardoType(BiliardoType type) {
  if (biliardo_.changeType(type)) {
    designer_.calcBordiBiliardo(biliardo_);
    reRun();
    if (multipleLaunches_[biliardo_.type()].empty()) {
      designer_.setCanvas(biliardo_, window_);
    } else {
      designer_.setCanvas(multipleLaunches_[biliardo_.type()][multipleLaunchesIndex_[biliardo_.type()]], window_);
    }
    return true;
  }
  return false;
}

void App::modifyBiliardo() {
  BiliardoType oldType = biliardo_.type();  // salvo il tipo attuale
  // rigenero un lancio per ogni tipo di biliardo e rimuovo i vecchi istogrammi
  for (int i = 2; i > -1; i--) {
    singleLaunches_[i].clear();
    multipleLaunches_[i].clear();
    biliardo_.changeType(static_cast<BiliardoType>(i));
    auto& newLaunch = newSingleLaunch();
    biliardo_.launchForDrawing(newLaunch);
  }
  biliardo_.changeType(oldType);  // ripristino il tipo attuale
  // aggiorno la parte grafica
  designer_.changeBiliardo(biliardo_, window_);
  // resetto gli indici a 0
  singleLaunchesIndex_ = {0, 0, 0};
  multipleLaunchesIndex_ = {0, 0, 0};
  reRun();                                  // faccio partire la nuova simulazione
  designer_.setCanvas(biliardo_, window_);  // pulisco il grafico
}

void App::pause() { designer_.pause(); }

void App::reRun() { designer_.reRun(singleLaunches_[biliardo_.type()][singleLaunchesIndex_[biliardo_.type()]]); }

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
    designer_.setCanvas(multipleLaunches_[biliardo_.type()][multipleLaunchesIndex_[biliardo_.type()]], window_);
    return true;
  }
  return false;
}

bool App::previousHistogram() {
  if (multipleLaunchesIndex_[biliardo_.type()] != 0) {
    multipleLaunchesIndex_[biliardo_.type()]--;
    designer_.setCanvas(multipleLaunches_[biliardo_.type()][multipleLaunchesIndex_[biliardo_.type()]], window_);
    return true;
  }
  return false;
}

void App::saveHistogram(const std::string& filename) {
  if (multipleLaunches_[biliardo_.type()].empty()) {
    return;
  }
  TCanvas canvas = TCanvas();
  canvas.SetCanvasSize(2560, 1440);

  canvas.Divide(2);

  auto& histograms = multipleLaunches_[biliardo_.type()][multipleLaunchesIndex_[biliardo_.type()]];

  canvas.cd(1);
  histograms[0].Draw();

  canvas.cd(2);
  histograms[1].Draw();

  if (filename.empty()) {
    std::time_t t = std::time(nullptr);  // get time now
    std::tm* now = std::localtime(&t);
    canvas.SaveAs((boost::format("grafico_%1%-%2%-%3%_%4%:%5%:%6%.png") % now->tm_mday % (now->tm_mon + 1) %
                   (now->tm_year + 1900) % now->tm_hour % now->tm_min % now->tm_sec)
                      .str()
                      .c_str());
  } else {
    canvas.SaveAs(filename.c_str());
  }
}

}  // namespace bt