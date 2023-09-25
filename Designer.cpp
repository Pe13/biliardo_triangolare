//
// Created by paolo on 19/06/2023.
//

#include "Designer.hpp"

#include <TCanvas.h>
#include <TImage.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TVirtualX.h>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <array>
#include <cmath>
#include <stdexcept>

namespace bt {

void saveCanvasOnImage(sf::Image& histoImage, TCanvas& canvas) {
  if (gVirtualX->InheritsFrom("TGCocoa") && !gROOT->IsBatch() && canvas.GetCanvas() &&
      canvas.GetCanvas()->GetCanvasID() != -1) {
    canvas.Flush();

    const UInt_t w = canvas.GetWw();
    const UInt_t h = canvas.GetWh();

    const unsigned char* pixelData = gVirtualX->GetColorBits(canvas.GetCanvasID(), 0, 0, w, h);

    if (pixelData) {
      histoImage.create(w, h, pixelData);
    }
    delete pixelData;
  } else {
    TImage* img = TImage::Create();
    if (img) {
      img->FromPad(&canvas);
    }

    histoImage.create(img->GetWidth(), img->GetHeight(), reinterpret_cast<sf::Uint8*>(img->GetRgbaArray()));
    delete img;
  }
}

void Designer::calcFrame(const sf::Vector2u& size) {
  using namespace sf;
  frame_.update((std::array<Vertex, 4>{Vertex(Vector2f(rightOffset_, 0), Color::White),
                                       Vertex(Vector2f(rightOffset_, static_cast<float>(size.y)), Color::White),
                                       Vertex(Vector2f(rightOffset_, topOffset_), Color::White),
                                       Vertex(Vector2f(static_cast<float>(size.x), topOffset_), Color::White)})
                    .data());
}

void Designer::calcClearBiliardo(const bt::Biliardo& biliardo) {
  using namespace sf;
  auto max = static_cast<float>(std::max(biliardo.r1(), biliardo.r2()));
  clearBiliardo_.update(
      (std::array<Vertex, 4>{
           sf::Vertex(sf::Vector2f(
                          static_cast<float>(biliardo.l() * ratio_ + simulationXOffset_ + (particle_.getRadius() + 2)),
                          simulationHeight_ / 2.f - max * ratio_ - (particle_.getRadius() + 2)),
                      sf::Color::Black),
           sf::Vertex(sf::Vector2f(simulationXOffset_ - (particle_.getRadius() + 2),
                                   simulationHeight_ / 2.f - max * ratio_ - (particle_.getRadius() + 2)),
                      sf::Color::Black),
           sf::Vertex(sf::Vector2f(
                          static_cast<float>(biliardo.l() * ratio_ + simulationXOffset_ + (particle_.getRadius() + 2)),
                          simulationHeight_ / 2.f + max * ratio_ + (particle_.getRadius() + 2)),
                      sf::Color::Black),
           sf::Vertex(sf::Vector2f(simulationXOffset_ - (particle_.getRadius() + 2),
                                   simulationHeight_ / 2.f + max * ratio_ + (particle_.getRadius() + 2)),
                      sf::Color::Black)})
          .data());
}

void Designer::calcClearHisto(const sf::Vector2u& size) {
  using namespace sf;
  clearHisto_.update(
      (std::array<Vertex, 4>{Vertex(Vector2f(static_cast<float>(size.x), topOffset_), Color::White),
                             Vertex(Vector2f(rightOffset_, topOffset_), Color::White),
                             Vertex(Vector2f(static_cast<float>(size.x), static_cast<float>(size.y)), Color::White),
                             Vertex(Vector2f(rightOffset_, static_cast<float>(size.y)), Color::White)})
          .data());
}

void Designer::calcStep(const std::vector<double>& points) {
  double distance = std::sqrt(
      (points[pointIndex_] - points[pointIndex_ + 2]) * (points[pointIndex_] - points[pointIndex_ + 2]) +
      (points[pointIndex_ + 1] - points[pointIndex_ + 3]) * (points[pointIndex_ + 1] - points[pointIndex_ + 3]));
  // speedx = stepx * 30
  // speedx = step * ratio * 30
  // step = speedx / (ratio * 30)
  // frac = step / dist = speedx / (ratio * 30 * dist)
  step_ = static_cast<float>(static_cast<double>(speed_) / (distance * ratio_));
}

Designer::Designer() {
  // rimuovo le statistics box dai grafici dato che le indico tutte sulla sinistra
  gStyle->SetOptStat(0);

  frame_.create(4);
  frame_.setPrimitiveType(sf::Lines);

  bordiBiliardo_.create(4);

  clearBiliardo_.create(4);
  clearBiliardo_.setPrimitiveType(sf::TriangleStrip);

  clearHisto_.create(4);
  clearHisto_.setPrimitiveType(sf::TriangleStrip);

  histoSprite_.setColor(sf::Color::Black);

  particle_.setFillColor(sf::Color::White);
  particle_.setPointCount(10);
}

void Designer::initWindow(sf::RenderWindow& window) { window.draw(frame_); }

void Designer::changeSize(const Biliardo& biliardo, std::array<TH1D, 2>& histograms, sf::RenderWindow& window,
                          const tgui::VerticalLayout::Ptr& wrapper) {
  rightOffset_ = widthLeftFraction_ * static_cast<float>(window.getSize().x);
  topOffset_ = heightTopFraction_ * static_cast<float>(window.getSize().y);
  simulationWidth_ = (1.f - widthLeftFraction_) * static_cast<float>(window.getSize().x);
  simulationHeight_ = heightTopFraction_ * static_cast<float>(window.getSize().y);

  window.setView(sf::View(sf::FloatRect(sf::Vector2f(0, 0), static_cast<sf::Vector2f>(window.getSize()))));

  calcFrame(window.getSize());
  calcClearHisto(window.getSize());
  calcBordiBiliardo(biliardo);
  calcClearBiliardo(biliardo);

  wrapper->setSize(rightOffset_, window.getSize().y);

  window.clear(sf::Color::Black);
  window.draw(bordiBiliardo_);
  histoSprite_.setPosition(rightOffset_, topOffset_);
  setCanvas(histograms, window);
  window.draw(frame_);

  window.display();
}

void Designer::changeSize(const bt::Biliardo& biliardo, sf::RenderWindow& window,
                          const tgui::VerticalLayout::Ptr& wrapper) {
  auto histograms = std::array<TH1D, 2>{TH1D("", "Istogramma delle y di uscita", 1000, -biliardo.r1(), biliardo.r1()),
                               TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};
  changeSize(biliardo, histograms, window, wrapper);
}

void Designer::changeBiliardo(const bt::Biliardo& biliardo, sf::RenderWindow& window) {
  window.draw(clearBiliardo_);
  calcBordiBiliardo(biliardo);
  calcClearBiliardo(biliardo);
}

void Designer::calcBordiBiliardo(const Biliardo& biliardo) {
  // calcolo il rapporto (pixel / unità di misura della simulazione) ottimale
  ratio_ = std::min(simulationWidth_ * 0.8f / static_cast<float>(biliardo.l()),
                    simulationHeight_ * 0.4f / static_cast<float>(std::max(biliardo.r1(), biliardo.r2())));

  // calcolo l'offset orizzontale per centrare il biliardo nel riquadro
  float width = static_cast<float>(biliardo.l()) * ratio_;
  float horizontalFraction = width / simulationWidth_;
  simulationXOffset_ = (1.f - horizontalFraction) / 2.f * simulationWidth_ + rightOffset_;

  // ordino i Vertex nel buffer in base all'ordine in cui devono essere disegnati
  if (biliardo.type() == rightBounded) {
    bordiBiliardo_.setPrimitiveType(sf::LineStrip);
    bordiBiliardo_.update(
        (std::array<sf::Vertex, 4>{
             sf::Vertex(
                 sf::Vector2f(simulationXOffset_, static_cast<float>(simulationHeight_ / 2 - biliardo.r1() * ratio_)),
                 sf::Color::White),
             sf::Vertex(sf::Vector2f(static_cast<float>(biliardo.l() * ratio_ + simulationXOffset_),
                                     static_cast<float>(simulationHeight_ / 2 - biliardo.r2() * ratio_)),
                        sf::Color::White),
             sf::Vertex(sf::Vector2f(static_cast<float>(biliardo.l() * ratio_ + simulationXOffset_),
                                     static_cast<float>(simulationHeight_ / 2 + biliardo.r2() * ratio_)),
                        sf::Color::White),
             sf::Vertex(
                 sf::Vector2f(simulationXOffset_, static_cast<float>(simulationHeight_ / 2 + biliardo.r1() * ratio_)),
                 sf::Color::White)})
            .data());
  } else {
    if (biliardo.type() == open) {
      bordiBiliardo_.setPrimitiveType(sf::Lines);
    } else {
      bordiBiliardo_.setPrimitiveType(sf::LineStrip);
    }
    bordiBiliardo_.update(
        (std::array<sf::Vertex, 4>{
             sf::Vertex(sf::Vector2f(static_cast<float>(biliardo.l() * ratio_ + simulationXOffset_),
                                     static_cast<float>(simulationHeight_ / 2 - biliardo.r2() * ratio_)),
                        sf::Color::White),
             sf::Vertex(
                 sf::Vector2f(simulationXOffset_, static_cast<float>(simulationHeight_ / 2 - biliardo.r1() * ratio_)),
                 sf::Color::White),
             sf::Vertex(
                 sf::Vector2f(simulationXOffset_, static_cast<float>(simulationHeight_ / 2 + biliardo.r1() * ratio_)),
                 sf::Color::White),
             sf::Vertex(sf::Vector2f(static_cast<float>(biliardo.l() * ratio_ + simulationXOffset_),
                                     static_cast<float>(simulationHeight_ / 2 + biliardo.r2() * ratio_)),
                        sf::Color::White)})
            .data());
  }
}
// void Designer::setPoints(std::vector<double>* points) {
//   if (points != nullptr) {
//     points_ = points;
//     reRun();
//   } else {
//     throw std::runtime_error("nullptr passed to setPoints");
//   }
// }

// void Designer::previousLaunch(std::vector<double>* first) {
//   if (points_ > first) {
//     points_--;
//     reRun();
//   }
// }

// void Designer::nextLaunch(std::vector<double>* last) {
//   if (points_ < last) {
//     points_++;
//     reRun();
//   }
// }

void Designer::reRun(const std::vector<double>& points) {
  //  if (points_ == nullptr) {
  //    return;
  //  }
  isDrawing_ = true;
  isPaused_ = false;
  hasCleared_ = false;
  pointIndex_ = 0;
  pathFraction_ = 0;
  calcStep(points);
  contrail_.clear();
}

void Designer::pause() {
  //  if (points_ == nullptr) {
  //    return;
  //  }
  isPaused_ = !isPaused_;
}

void Designer::setCanvas(std::array<TH1D, 2>& histograms, sf::RenderWindow& window) {
  int width = static_cast<int>(static_cast<float>(window.getSize().x) - rightOffset_);
  int height = static_cast<int>(static_cast<float>(window.getSize().y) - topOffset_);
  TCanvas canvas = TCanvas("canvas", "canvas", width, height);
  canvas.SetCanvasSize(width, height);

  canvas.Divide(2);

  canvas.cd(1);
  histograms[0].Draw();  // non const
  canvas.cd(2);
  histograms[1].Draw();

  sf::Image histoImage;
  saveCanvasOnImage(histoImage, canvas);
  histoTexture_.loadFromImage(histoImage);
  histoSprite_.setTexture(histoTexture_, true);

  updateHisto(window);
}

void Designer::updateHisto(sf::RenderWindow& window) {
  window.draw(clearHisto_);
  window.draw(histoSprite_);
  window.draw(frame_);
  window.display();
}

void Designer::operator()(const std::vector<double>& points, sf::RenderWindow& window) {
  if (isPaused_) {
    return;
  }
  if (!hasCleared_) {
    window.draw(clearBiliardo_);
    window.draw(bordiBiliardo_);
    window.display();
    hasCleared_ = true;

  } else if (isDrawing_) {
    contrail_.front().color = sf::Color::Black;
    window.draw(&contrail_.front(), 1, sf::Points);
    contrail_.push_back(sf::Vertex(particle_.getPosition() + sf::Vector2f(5, 5), sf::Color(100, 100, 100)));

    particle_.setFillColor(sf::Color::Black);
    window.draw(particle_);

    particle_.setFillColor(sf::Color::White);
    particle_.setPosition(
        toSfmlCord(points[pointIndex_] * (1 - pathFraction_) + points[pointIndex_ + 2] * pathFraction_,
                   points[pointIndex_ + 1] * (1 - pathFraction_) + points[pointIndex_ + 3] * pathFraction_) -
        sf::Vector2f(5, 5));
    window.draw(particle_);
    window.draw(&contrail_.back(), 1, sf::Points);

    window.draw(bordiBiliardo_);

    pathFraction_ += step_;
    if (pathFraction_ > 1) {
      pathFraction_ = 0;
      pointIndex_ += 2;
      if (pointIndex_ > points.size() - 5) {
        pointIndex_ = 0;
        isDrawing_ = false;
        hasCleared_ = false;
      } else {
        calcStep(points);
      }
    }
  }
}

}  // namespace bt