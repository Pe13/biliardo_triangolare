//
// Created by paolo on 23/05/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_

#include <TH1D.h>

#include <array>
#include <memory>
#include <vector>

#include "Pool.hpp"
#include "Designer.hpp"
#include "Gui.hpp"

namespace bt {

class Gui;

class App {
  Pool pool_;

  sf::RenderWindow window_;
  sf::Event event_{};

  Designer designer_;

  Gui gui_;

  // TODO Documentare la scelta del vector of vector
  std::array<std::vector<std::shared_ptr<std::vector<double>>>, 3> singleLaunches_;
  std::array<size_t, 3> singleLaunchesIndexes_{0, 0, 0};
  std::array<std::vector<std::shared_ptr<std::array<TH1D, 2>>>, 3> multipleLaunches_;
  std::array<size_t, 3> multipleLaunchesIndexes_{0, 0, 0};
  std::array<std::vector<sf::Image>, 3> graphImages{{}};

  std::shared_ptr<std::vector<double>>& newSingleLaunch();
  std::shared_ptr<std::array<TH1D, 2>>& newHistograms();

  void handleEvents();

 public:
  App(double l, double r1, double r2, PoolType type, const sf::ContextSettings& settings);
  ~App() = default;
  void start();

  const Designer& designer() const { return designer_; }

  const Pool& pool() const { return pool_; }
  /**
   * @brief Deve essere chiamato quando le dimensioni del biliardo variano per aggiornare i lanci e
   * la parte grafica
   */
  [[nodiscard]] bool modifyPool(double l, double r1, double r2);
  [[nodiscard]] bool changePoolType(PoolType type);

  void pause();
  void reRun();

  // TODO Documentare la scelta del weak_ptr
  [[nodiscard]] std::weak_ptr<const std::vector<double>> singleLaunch(
      std::optional<double> initialY = std::nullopt,
      std::optional<double> initialDirection = std::nullopt);
  [[nodiscard]] std::weak_ptr<const std::array<TH1D, 2>> multipleLaunch(unsigned int N, double muY,
                                                                        double sigmaY, double muT,
                                                                        double sigmaT,
                                                                        bool async = true);

  const std::array<std::vector<std::shared_ptr<std::vector<double>>>, 3>& getSingleLaunches()
      const {
    return singleLaunches_;
  }
  const std::array<size_t, 3>& getSingleLaunchesIndexes() const { return singleLaunchesIndexes_; }
  const std::array<std::vector<std::shared_ptr<std::array<TH1D, 2>>>, 3>& getMultipleLaunches()
      const {
    return multipleLaunches_;
  }
  const std::array<size_t, 3>& getMultipleLaunchesIndexes() const {
    return multipleLaunchesIndexes_;
  }

  bool nextLaunch();
  bool previousLaunch();

  bool nextHistogram();
  bool previousHistogram();

  /**
   * @brief Salva l'istogramma selezionato in una macro root
   * @param filename Nome del file che si vuole generare (senza estensione). Se lasciato vuoto il
   * nome sarà dato in base alla data e ora attuale
   */
  void saveHistogram(const std::string& filename = "");
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
