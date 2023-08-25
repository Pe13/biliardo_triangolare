//
// Created by paolo on 19/06/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_DRAWER_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_DRAWER_HPP_

#include <TH1D.h>

#include <SFML/Graphics.hpp>
#include <array>
#include <boost/circular_buffer.hpp>
#include <vector>

#include "Biliardo.hpp"

namespace bt {

class Designer {

  std::vector<double>* points_{nullptr};

  const float widthLeftFraction_{.3};
  const float heightTopFraction_{.5};
  float rightOffset_;
  float topOffset_;

  float simulationWidth_;
  float simulationHeight_;

  float ratio_{};
  float simulationXOffset_{};
  float simulationYOffset_{};

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
  void calcClearBiliardo(const Biliardo* biliardo);
  void calcClearHisto(const sf::Vector2u& size);
  void calcStep();

 public:
  Designer(const Biliardo* biliardo, const sf::Vector2u& size);
  ~Designer() = default;

  void initWindow(sf::RenderWindow& window);

  sf::Vector2f screenFraction() const { return {widthLeftFraction_, heightTopFraction_}; }

  void changeSize(const Biliardo* biliardo, const sf::Vector2u& size);
  void calcBordiBiliardo(Biliardo const* biliardo);
  void setPoints(std::vector<double>* points);
  void previousLaunch(std::vector<double>* first);
  void nextLaunch(std::vector<double>* last);
  void reRun();
  void pause();

  void setCanvas(const double& r1, const std::vector<double>& input, sf::RenderWindow& window);
  void updateHisto(sf::RenderWindow& window);

  void operator()(sf::RenderWindow& window);

  template <typename num>
  sf::Vector2f toSfmlCord(num x, num y) const {
    static_assert(std::is_arithmetic<num>::value);
    return {static_cast<float>(simulationXOffset_ + x * ratio_),
            static_cast<float>(simulationHeight_ / 2 - y * ratio_)};
  }
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_DRAWER_HPP_
