//
// Created by paolo on 23/05/2023.
//

//TODO Creare dei metodi pubblici per evitare di avere gui come friend

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_

#include <TH1D.h>

#include <array>
#include <vector>

#include "Biliardo.hpp"
#include "Designer.hpp"
#include "Gui.hpp"

namespace bt {

class Gui;

class App {
  friend class Gui;

  Biliardo biliardo_;

  Designer designer_{};

  sf::RenderWindow window_;
  sf::Event event_{};

  Gui gui_;

  std::array<std::vector<std::vector<double>>, 3> singleLaunches_;
  std::array<size_t, 3> singleLaunchesIndex_{0, 0, 0};
  std::array<std::vector<std::array<TH1D, 2>>, 3> multipleLaunches_;
  std::array<size_t, 3> multipleLaunchesIndex_{0, 0, 0};
  std::array<std::vector<sf::Image>, 3> graphImages{{}};

  std::vector<double>& newSingleLaunch();
  std::array<TH1D, 2>& newHistograms();

  void handleEvents();
  bool changeBiliardoType(BiliardoType type);

  /**
   * @brief Deve essere chiamato quando le dimensioni del biliardo variano per aggiornare i lanci e la parte grafica
   */
  void modifyBiliardo();

 public:
  App(double l, double r1, double r2, BiliardoType type, const sf::ContextSettings& settings);
  ~App() = default;
  void start();

  void pause();
  void reRun();

  bool nextLaunch();
  bool previousLaunch();

  bool nextHistogram();
  bool previousHistogram();

  /**
   * @brief Salva l'istogramma selezionato in una macro root
   * @param filename Nome del file che si vuole generare (senza estensione). Se lasciato vuoto il nome sarà dato in base
   * alla data e ora attuale
   */
  void saveHistogram(const std::string& filename = "");
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
