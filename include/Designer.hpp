//
// Created by paolo on 19/06/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_DRAWER_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_DRAWER_HPP_

#include <TH1D.h>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <array>
#include <boost/circular_buffer.hpp>
#include <vector>

#include "Biliardo.hpp"

namespace bt {

class Designer {

  const float widthLeftFraction_{.3f};
  const float heightTopFraction_{.5f};
  float rightOffset_;
  float topOffset_;

  float simulationWidth_;
  float simulationHeight_;

  float ratio_;
  float simulationXOffset_;

  sf::VertexBuffer frame_{sf::VertexBuffer::Usage::Static};
  sf::VertexBuffer bordiBiliardo_{sf::VertexBuffer::Usage::Static};
  sf::VertexBuffer clearBiliardo_{sf::VertexBuffer::Usage::Static};
  sf::VertexBuffer clearHisto_{sf::VertexBuffer::Usage::Static};

  sf::Texture histoTexture_;
  sf::Sprite histoSprite_;

  bool isDrawing_{false};
  bool hasCleared_{true};
  bool isPaused_{false};

  std::size_t pointIndex_{0};
  float pathFraction_{0};
  float step_{0};
  const float speed_{10};  // pixel / s

  boost::circular_buffer<sf::Vertex> contrail_{100};

  sf::CircleShape particle_{4};

  void calcFrame(const sf::Vector2u& size);
  void calcClearBiliardo(const Biliardo& biliardo);
  void calcClearHisto(const sf::Vector2u& size);
  void calcStep(const std::vector<double>& points);

 public:
  Designer();
  ~Designer() = default;

  void initWindow(sf::RenderWindow& window);

  void changeSize(const Biliardo& biliardo, std::array<TH1D, 2>& histograms, sf::RenderWindow& window,
                  const tgui::VerticalLayout::Ptr& wrapper);
  void changeSize(const Biliardo& biliardo, sf::RenderWindow& window, const tgui::VerticalLayout::Ptr& wrapper);
  void changeBiliardo(const Biliardo& biliardo, sf::RenderWindow& window);
  void calcBordiBiliardo(const Biliardo& biliardo);
  void reRun(const std::vector<double>& points);
  void pause();

  void setCanvas(std::array<TH1D, 2>& histograms, sf::RenderWindow& window);
  void setCanvas(const Biliardo& biliardo, sf::RenderWindow& window);
  void updateHisto(sf::RenderWindow& window) const;

  void operator()(const std::vector<double>& points, sf::RenderWindow& window);

  template <typename num>
  sf::Vector2f toSfmlCord(num x, num y) const {
    static_assert(std::is_arithmetic_v<num>);
    return {static_cast<float>(simulationXOffset_ + x * ratio_),
            static_cast<float>(simulationHeight_ / 2 - y * ratio_)};
  }
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_DRAWER_HPP_
