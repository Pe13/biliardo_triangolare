//
// Created by paolo on 19/06/2023.
//

#include "Designer.hpp"

#include <TCanvas.h>
#include <TImage.h>
#include <TROOT.h>
#include <TVirtualX.h>

#include <SFML/Graphics.hpp>
#include <array>
#include <cmath>
#include <cstring>
#include <iostream>
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

void Designer::calcClearBiliardo(const bt::Biliardo* biliardo) {
  using namespace sf;
  auto max = static_cast<float>(std::max(biliardo->r1(), biliardo->r2()));
  clearBiliardo_.update(
      (std::array<Vertex, 4>{
           sf::Vertex(
               sf::Vector2f(static_cast<float>(biliardo->l() * ratio_ + simulationXOffset_ + particle_.getRadius()),
                            simulationHeight_ / 2.f - max * ratio_ - particle_.getRadius()),
               sf::Color::Black),
           sf::Vertex(sf::Vector2f(simulationXOffset_ - particle_.getRadius(),
                                   simulationHeight_ / 2.f - max * ratio_ - particle_.getRadius()),
                      sf::Color::Black),
           sf::Vertex(
               sf::Vector2f(static_cast<float>(biliardo->l() * ratio_ + simulationXOffset_ + particle_.getRadius()),
                            simulationHeight_ / 2.f + max * ratio_ + particle_.getRadius()),
               sf::Color::Black),
           sf::Vertex(sf::Vector2f(simulationXOffset_ - particle_.getRadius(),
                                   simulationHeight_ / 2.f + max * ratio_ + particle_.getRadius()),
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

void Designer::calcStep() {
  double distance = std::sqrt(((*points_)[pointIndex_] - (*points_)[pointIndex_ + 2]) *
                                  ((*points_)[pointIndex_] - (*points_)[pointIndex_ + 2]) +
                              ((*points_)[pointIndex_ + 1] - (*points_)[pointIndex_ + 3]) *
                                  ((*points_)[pointIndex_ + 1] - (*points_)[pointIndex_ + 3]));
  // speedx = stepx * 30
  // speedx = step * ratio * 30
  // step = speedx / (ratio * 30)
  // frac = step / dist = speedx / (ratio * 30 * dist)
  step_ = static_cast<float>(static_cast<double>(speed_) / (distance * ratio_));
}

Designer::Designer() {
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

void Designer::changeSize(const Biliardo* biliardo, const sf::Event::SizeEvent& size, const std::vector<double>& input,
                          sf::RenderWindow& window) {
  rightOffset_ = widthLeftFraction_ * static_cast<float>(size.width);
  topOffset_ = heightTopFraction_ * static_cast<float>(size.height);
  simulationWidth_ = (1.f - widthLeftFraction_) * static_cast<float>(size.width);
  simulationHeight_ = heightTopFraction_ * static_cast<float>(size.height);

  window.setView(sf::View(sf::FloatRect(0, 0, size.width, size.height)));

  calcFrame(window.getSize());
  calcBordiBiliardo(biliardo);
  calcClearHisto(window.getSize());
  calcClearBiliardo(biliardo);

  window.clear(sf::Color::Black);
  window.draw(bordiBiliardo_);
  histoSprite_.setPosition(rightOffset_, topOffset_);
  setCanvas(biliardo->r1(), input, window);
  window.draw(frame_);

  window.display();
}

void Designer::calcBordiBiliardo(Biliardo const* biliardo) {
  // calcolo il rapporto (pixel / unità di misura della simulazione) ottimale
  ratio_ = std::min(simulationWidth_ * 0.8f / static_cast<float>(biliardo->l()),
                    simulationHeight_ * 0.4f / static_cast<float>(std::max(biliardo->r1(), biliardo->r2())));

  // calcolo l'offset verticale e orizzontale per centrare il biliardo nella finestra
  float width = static_cast<float>(biliardo->l()) * ratio_;
  float horizontalFraction = width / simulationWidth_;
  simulationXOffset_ = (1.f - horizontalFraction) / 2.f * simulationWidth_ + rightOffset_;

  //  double height = std::max(biliardo.r1(), biliardo.r2()) * ratio_ * 2;
  //  double verticalFraction = height / simulationHeight_;
  //  yOffset_ = (1 - verticalFraction) / 2 * simulationHeight_;

  // ordino i Vertex nel buffer in base all'ordine in cui devono essere disegnati
  if (biliardo->type() == rightBounded) {
    bordiBiliardo_.setPrimitiveType(sf::LineStrip);
    bordiBiliardo_.update(
        (std::array<sf::Vertex, 4>{
             sf::Vertex(
                 sf::Vector2f(simulationXOffset_, static_cast<float>(simulationHeight_ / 2 - biliardo->r1() * ratio_)),
                 sf::Color::White),
             sf::Vertex(sf::Vector2f(static_cast<float>(biliardo->l() * ratio_ + simulationXOffset_),
                                     static_cast<float>(simulationHeight_ / 2 - biliardo->r2() * ratio_)),
                        sf::Color::White),
             sf::Vertex(sf::Vector2f(static_cast<float>(biliardo->l() * ratio_ + simulationXOffset_),
                                     static_cast<float>(simulationHeight_ / 2 + biliardo->r2() * ratio_)),
                        sf::Color::White),
             sf::Vertex(
                 sf::Vector2f(simulationXOffset_, static_cast<float>(simulationHeight_ / 2 + biliardo->r1() * ratio_)),
                 sf::Color::White)})
            .data());
  } else {
    if (biliardo->type() == open) {
      bordiBiliardo_.setPrimitiveType(sf::Lines);
    } else {
      bordiBiliardo_.setPrimitiveType(sf::LineStrip);
    }
    bordiBiliardo_.update(
        (std::array<sf::Vertex, 4>{
             sf::Vertex(sf::Vector2f(static_cast<float>(biliardo->l() * ratio_ + simulationXOffset_),
                                     static_cast<float>(simulationHeight_ / 2 - biliardo->r2() * ratio_)),
                        sf::Color::White),
             sf::Vertex(
                 sf::Vector2f(simulationXOffset_, static_cast<float>(simulationHeight_ / 2 - biliardo->r1() * ratio_)),
                 sf::Color::White),
             sf::Vertex(
                 sf::Vector2f(simulationXOffset_, static_cast<float>(simulationHeight_ / 2 + biliardo->r1() * ratio_)),
                 sf::Color::White),
             sf::Vertex(sf::Vector2f(static_cast<float>(biliardo->l() * ratio_ + simulationXOffset_),
                                     static_cast<float>(simulationHeight_ / 2 + biliardo->r2() * ratio_)),
                        sf::Color::White)})
            .data());
  }
}
void Designer::setPoints(std::vector<double>* points) {
  if (points != nullptr) {
    points_ = points;
    reRun();
  } else {
    throw std::runtime_error("nullptr passed to setPoints");
  }
}

void Designer::previousLaunch(std::vector<double>* first) {
  if (points_ > first) {
    points_--;
    reRun();
  }
}

void Designer::nextLaunch(std::vector<double>* last) {
  if (points_ < last) {
    points_++;
    reRun();
  }
}

void Designer::reRun() {
  if (points_ == nullptr) {
    return;
  }
  isDrawing_ = true;
  hasCleared_ = false;
  pointIndex_ = 0;
  pathFraction_ = 0;
  calcStep();
  contrail_.clear();
}

void Designer::pause() {
  if (points_ == nullptr) {
    return;
  }
  isPaused_ = !isPaused_;
}

void Designer::setCanvas(const double& r1, const std::vector<double>& input, sf::RenderWindow& window) {
  int width = static_cast<int>(static_cast<float>(window.getSize().x) - rightOffset_);
  int height = static_cast<int>(static_cast<float>(window.getSize().y) - topOffset_);
  TCanvas canvas = TCanvas("canvas", "canvas", width, height);
  canvas.SetCanvasSize(width, height);

  TH1D yHisto("yhisto", "Istogramma delle y di uscita", 1000, -r1, r1);
  TH1D thetaHisto("thetaHisto", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2);

  for (size_t i = 0; i < input.size(); i += 2) {
    yHisto.Fill(input[i]);
    thetaHisto.Fill(input[i + 1]);
  }

  canvas.Divide(2);

  canvas.cd(1);
  yHisto.Draw();
  canvas.cd(2);
  thetaHisto.Draw();

  sf::Image histoImage;
  saveCanvasOnImage(histoImage, canvas);
  histoTexture_.loadFromImage(histoImage);
  histoSprite_.setTexture(histoTexture_, true);

  updateHisto(window);
  reRun();
}

void Designer::updateHisto(sf::RenderWindow& window) {
  window.draw(clearHisto_);
  window.draw(histoSprite_);
  window.draw(frame_);
  window.display();
}

void Designer::operator()(sf::RenderWindow& window) {
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
        toSfmlCord((*points_)[pointIndex_] * (1 - pathFraction_) + (*points_)[pointIndex_ + 2] * pathFraction_,
                   (*points_)[pointIndex_ + 1] * (1 - pathFraction_) + (*points_)[pointIndex_ + 3] * pathFraction_) -
        sf::Vector2f(5, 5));
    window.draw(particle_);
    window.draw(&contrail_.back(), 1, sf::Points);

    window.draw(bordiBiliardo_);

    pathFraction_ += step_;
    if (pathFraction_ > 1) {
      pathFraction_ = 0;
      pointIndex_ += 2;
      if (pointIndex_ > points_->size() - 4) {
        pointIndex_ = 0;
        isDrawing_ = false;
        hasCleared_ = false;
      } else {
        calcStep();
      }
    }

    window.display();
  }
}

}  // namespace bt