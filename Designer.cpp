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
#include <array>
#include <cmath>
#include <stdexcept>

#include "Gui.hpp"

namespace bt {

// dal source code di root
// https://root.cern.ch/doc/v626/TPadPainter_8cxx_source.html#l00560
void saveCanvasOnImage(sf::Image& histoImage, TCanvas& canvas) {
  if (gVirtualX->InheritsFrom("TGCocoa") && !gROOT->IsBatch() && canvas.GetCanvas() &&
      canvas.GetCanvas()->GetCanvasID() != -1) {
    canvas.Flush();

    const UInt_t w = canvas.GetWw();
    const UInt_t h = canvas.GetWh();

    const std::unique_ptr<unsigned char[]> pixelData(
        gVirtualX->GetColorBits(static_cast<long unsigned>(canvas.GetCanvasID()), 0, 0, w, h));

    if (pixelData) {
      histoImage.create(w, h, pixelData.get());
    }
  } else {
    const std::unique_ptr<TImage> img(TImage::Create());
    if (!img) {
      throw std::runtime_error("Root non è riuscito a creare un'immagine");
    }
    img->FromPad(&canvas);
    const std::unique_ptr<unsigned int[]> rgbaArray(img->GetRgbaArray());
    histoImage.create(img->GetWidth(), img->GetHeight(),
                      reinterpret_cast<sf::Uint8*>(rgbaArray.get()));
  }
}

void Designer::calcFrame(const sf::Vector2u& size) {
  using namespace sf;
  frame_.update(std::array<Vertex, 4>{
      Vertex(Vector2f(rightOffset_, 0), Color::White),
      Vertex(Vector2f(rightOffset_, static_cast<float>(size.y)), Color::White),
      Vertex(Vector2f(rightOffset_, topOffset_), Color::White),
      Vertex(Vector2f(static_cast<float>(size.x), topOffset_), Color::White)}
                    .data());
}

void Designer::calcClearPool(const Pool& pool) {
  const auto max = static_cast<float>(std::max(pool.r1(), pool.r2()));
  clearPool_.update(std::array<sf::Vertex, 4>{
      sf::Vertex(sf::Vector2f(static_cast<float>(pool.l() * ratio_ + simulationXOffset_ +
                                                 (particle_.getRadius() + 2)),
                              simulationHeight_ / 2.f - max * ratio_ - (particle_.getRadius() + 2)),
                 sf::Color::Black),
      sf::Vertex(sf::Vector2f(simulationXOffset_ - (particle_.getRadius() + 2),
                              simulationHeight_ / 2.f - max * ratio_ - (particle_.getRadius() + 2)),
                 sf::Color::Black),
      sf::Vertex(sf::Vector2f(static_cast<float>(pool.l() * ratio_ + simulationXOffset_ +
                                                 (particle_.getRadius() + 2)),
                              simulationHeight_ / 2.f + max * ratio_ + (particle_.getRadius() + 2)),
                 sf::Color::Black),
      sf::Vertex(sf::Vector2f(simulationXOffset_ - (particle_.getRadius() + 2),
                              simulationHeight_ / 2.f + max * ratio_ + (particle_.getRadius() + 2)),
                 sf::Color::Black)}
                        .data());
}

void Designer::calcClearHisto(const sf::Vector2u& size) {
  using namespace sf;
  clearHisto_.update(std::array<Vertex, 4>{
      Vertex(Vector2f(static_cast<float>(size.x), topOffset_), Color::White),
      Vertex(Vector2f(rightOffset_, topOffset_), Color::White),
      Vertex(Vector2f(static_cast<float>(size.x), static_cast<float>(size.y)), Color::White),
      Vertex(Vector2f(rightOffset_, static_cast<float>(size.y)), Color::White)}
                         .data());
}

void Designer::calcStep(const std::vector<double>& points) {
  const double& x1 = points[pointIndex_];
  const double& y1 = points[pointIndex_ + 1];
  const double& x2 = points[pointIndex_ + 2];
  const double& y2 = points[pointIndex_ + 3];
  const double distance = std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

  // frazione della distanza tra 2 urti da percorrere in un frame
  step_ = static_cast<float>(static_cast<double>(speed_) / (distance * ratio_));
}

Designer::Designer(sf::RenderWindow& window)
    : rightOffset_{widthLeftFraction_ * static_cast<float>(window.getSize().x)},
      topOffset_{heightTopFraction_ * static_cast<float>(window.getSize().y)},
      simulationWidth_{(1.f - widthLeftFraction_) * static_cast<float>(window.getSize().x)},
      simulationHeight_{heightTopFraction_ * static_cast<float>(window.getSize().y)} {
  // rimuovo le statistics box dai grafici dato che le indico tutte sulla sinistra
  gStyle->SetOptStat(0);

  frame_.create(4);
  frame_.setPrimitiveType(sf::Lines);

  poolBorders_.create(4);

  clearPool_.create(4);
  clearPool_.setPrimitiveType(sf::TriangleStrip);

  clearHisto_.create(4);
  clearHisto_.setPrimitiveType(sf::TriangleStrip);

  histoSprite_.setColor(sf::Color::Black);

  particle_.setFillColor(sf::Color::White);
  particle_.setPointCount(10);

  calcFrame(window.getSize());
  window.draw(frame_);
}

void Designer::changeSize(const Pool& pool, std::array<TH1D, 2>& histograms,
                          sf::RenderWindow& window, const Gui& gui) {
  rightOffset_ = widthLeftFraction_ * static_cast<float>(window.getSize().x);
  topOffset_ = heightTopFraction_ * static_cast<float>(window.getSize().y);
  simulationWidth_ = (1.f - widthLeftFraction_) * static_cast<float>(window.getSize().x);
  simulationHeight_ = heightTopFraction_ * static_cast<float>(window.getSize().y);

  window.setView(
      sf::View(sf::FloatRect(sf::Vector2f(0, 0), static_cast<sf::Vector2f>(window.getSize()))));

  calcFrame(window.getSize());
  calcClearHisto(window.getSize());
  calcPoolBorders(pool);
  calcClearPool(pool);

  gui.setSize(rightOffset_, static_cast<float>(window.getSize().y));

  window.clear(sf::Color::Black);
  window.draw(poolBorders_);
  histoSprite_.setPosition(rightOffset_, topOffset_);
  drawHistograms(histograms, window);
  window.draw(frame_);

  window.display();
}

void Designer::changeSize(const Pool& pool, sf::RenderWindow& window, const Gui& gui) {
  auto histograms =
      std::array<TH1D, 2>{TH1D("", "Istogramma delle y di uscita", 1000, -pool.r1(), pool.r1()),
                          TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};
  changeSize(pool, histograms, window, gui);
}

void Designer::changePool(const Pool& pool, sf::RenderWindow& window) {
  window.draw(clearPool_);
  calcPoolBorders(pool);
  calcClearPool(pool);
}

void Designer::calcPoolBorders(const Pool& pool) {
  // calcolo il rapporto (pixel / unità di misura della simulazione) ottimale
  ratio_ = std::min(simulationWidth_ * 0.8f / static_cast<float>(pool.l()),
                    simulationHeight_ * 0.4f / static_cast<float>(std::max(pool.r1(), pool.r2())));

  // calcolo l'offset orizzontale per centrare il biliardo nel riquadro
  const float width = static_cast<float>(pool.l()) * ratio_;
  const float horizontalFraction = width / simulationWidth_;
  simulationXOffset_ = (1.f - horizontalFraction) / 2.f * simulationWidth_ + rightOffset_;

  // ordino i Vertex nel buffer in base all'ordine in cui devono essere disegnati
  if (pool.type() == rightBounded) {
    poolBorders_.setPrimitiveType(sf::LineStrip);
    poolBorders_.update(std::array<sf::Vertex, 4>{
        sf::Vertex(sf::Vector2f(simulationXOffset_,
                                static_cast<float>(simulationHeight_ / 2 - pool.r1() * ratio_)),
                   sf::Color::White),
        sf::Vertex(sf::Vector2f(static_cast<float>(pool.l() * ratio_ + simulationXOffset_),
                                static_cast<float>(simulationHeight_ / 2 - pool.r2() * ratio_)),
                   sf::Color::White),
        sf::Vertex(sf::Vector2f(static_cast<float>(pool.l() * ratio_ + simulationXOffset_),
                                static_cast<float>(simulationHeight_ / 2 + pool.r2() * ratio_)),
                   sf::Color::White),
        sf::Vertex(sf::Vector2f(simulationXOffset_,
                                static_cast<float>(simulationHeight_ / 2 + pool.r1() * ratio_)),
                   sf::Color::White)}
                            .data());
  } else {
    if (pool.type() == open) {
      poolBorders_.setPrimitiveType(sf::Lines);
    } else {
      poolBorders_.setPrimitiveType(sf::LineStrip);
    }
    poolBorders_.update(std::array<sf::Vertex, 4>{
        sf::Vertex(sf::Vector2f(static_cast<float>(pool.l() * ratio_ + simulationXOffset_),
                                static_cast<float>(simulationHeight_ / 2 - pool.r2() * ratio_)),
                   sf::Color::White),
        sf::Vertex(sf::Vector2f(simulationXOffset_,
                                static_cast<float>(simulationHeight_ / 2 - pool.r1() * ratio_)),
                   sf::Color::White),
        sf::Vertex(sf::Vector2f(simulationXOffset_,
                                static_cast<float>(simulationHeight_ / 2 + pool.r1() * ratio_)),
                   sf::Color::White),
        sf::Vertex(sf::Vector2f(static_cast<float>(pool.l() * ratio_ + simulationXOffset_),
                                static_cast<float>(simulationHeight_ / 2 + pool.r2() * ratio_)),
                   sf::Color::White)}
                            .data());
  }
}

void Designer::reRun(const std::vector<double>& points) {
  isDrawing_ = true;
  isPaused_ = false;
  hasCleared_ = false;
  pointIndex_ = 0;
  pathFraction_ = 0;
  calcStep(points);
  contrail_.clear();
}

void Designer::pause() { isPaused_ = !isPaused_; }

void Designer::drawHistograms(std::array<TH1D, 2>& histograms, sf::RenderWindow& window) {
  // creo la canvas
  const auto width =
      static_cast<unsigned int>(static_cast<float>(window.getSize().x) - rightOffset_);
  const auto height =
      static_cast<unsigned int>(static_cast<float>(window.getSize().y) - topOffset_);
  TCanvas canvas("canvas", "canvas", static_cast<int>(width), static_cast<int>(height));
  canvas.SetCanvasSize(width, height);

  canvas.Divide(2);

  // disegno l'istogramma sulla canvas
  canvas.cd(1);
  histograms[0].Draw();  // non const
  canvas.cd(2);
  histograms[1].Draw();

  // creo la texture a partire dalla canvas e la assegno allo sprite
  sf::Image histoImage;
  saveCanvasOnImage(histoImage, canvas);
  histoTexture_.loadFromImage(histoImage);
  histoSprite_.setTexture(histoTexture_, true);

  // disegno lo sprite e aggiorno il tutto
  window.draw(clearHisto_);
  window.draw(histoSprite_);
  window.draw(frame_);
  window.display();
}

void Designer::drawEmptyHistograms(const Pool& pool, sf::RenderWindow& window) {
  auto histograms =
      std::array<TH1D, 2>{TH1D("", "Istogramma delle y di uscita", 1000, -pool.r1(), pool.r1()),
                          TH1D("", "Istogramma degli angoli di uscita", 1000, -M_PI / 2, M_PI / 2)};
  drawHistograms(histograms, window);
}

void Designer::operator()(const std::vector<double>& points, sf::RenderWindow& window) {
  if (isPaused_) {
    return;
  }
  if (!hasCleared_) {
    window.draw(clearPool_);
    window.draw(poolBorders_);
    window.display();
    hasCleared_ = true;

  } else if (isDrawing_) {
    contrail_.front().color = sf::Color::Black;
    window.draw(&contrail_.front(), 1, sf::Points);
    contrail_.push_back(
        sf::Vertex(particle_.getPosition() + sf::Vector2f(5, 5), sf::Color(100, 100, 100)));

    particle_.setFillColor(sf::Color::Black);
    window.draw(particle_);

    particle_.setFillColor(sf::Color::White);
    particle_.setPosition(toSfmlCord(points[pointIndex_] * (1 - pathFraction_) +
                                         points[pointIndex_ + 2] * pathFraction_,
                                     points[pointIndex_ + 1] * (1 - pathFraction_) +
                                         points[pointIndex_ + 3] * pathFraction_) -
                          sf::Vector2f(5, 5));
    window.draw(particle_);
    window.draw(&contrail_.back(), 1, sf::Points);

    window.draw(poolBorders_);

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