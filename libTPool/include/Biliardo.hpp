//
// Created by paolo on 13/06/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_

#include <TH1D.h>

#include <array>
#include <chrono>
#include <iostream>
#include <optional>
#include <random>
#include <vector>

namespace bt {

enum BiliardoType: unsigned { open = 0, rightBounded = 1, leftBounded = 2 };
enum LastHit { left, right, top, bottom };

struct LaunchParameters {
  std::optional<double> initialY;
  std::optional<double> initialDirection;
};

class Biliardo {
  struct CollisionParameters {
    LastHit lastHit;
    double x;
    double y;
    double direction;
  };

  // Creo una struct apposita al posto di usare un semplice std::array<double, 2> perché
  // quest'ultimo è un aggregato e per questo non potrei costruirlo con emplace_back
  struct Particle {
    double y;
    double direction;
    Particle(const double y_, const double direction_) : y{y_}, direction{direction_} {}
  };

  BiliardoType type_;

  double l_;
  double r1_;
  double r2_;

  double theta_;
  double slope_{std::tan(theta_)};

  std::default_random_engine rng_{
      static_cast<unsigned long long>(std::chrono::system_clock::now().time_since_epoch().count())};
  std::uniform_real_distribution<double> uniformDist_{0, 1};

  [[nodiscard]] bool isOut_(const LastHit& lastHit) const;

  void addParticleToMultipleLaunch_(std::vector<Particle>& launch,
                                    std::normal_distribution<double>& yNormalDist,
                                    std::normal_distribution<double>& thetaNormalDist);
  [[nodiscard]] bool findNextCollision_(CollisionParameters& parameters) const;
  void launchForHistograms_(Particle& particle) const;
  void syncLaunch_(unsigned int N, std::array<TH1D, 2>& histograms,
                   std::normal_distribution<double>& yNormalDist,
                   std::normal_distribution<double>& thetaNormalDist);
  void asyncLaunch_(unsigned int N, std::array<TH1D, 2>& histograms,
                    std::normal_distribution<double>& yNormalDist,
                    std::normal_distribution<double>& thetaNormalDist);

  [[nodiscard]] bool validateLaunchForDrawingInput_(
      const std::optional<double>& initialY, const std::optional<double>& initialDirection) const;
  void initializeLaunchForDrawingInput_(std::optional<double>& initialY,
                                        std::optional<double>& initialDirection);

  /**
   * @brief Lancia una singola particella da disegnare fornendo tutti i parametri iniziali, NON
   * effettua controlli sugli input.
   *
   * @param output il vettore che viene riempito con il risultato del lancio
   * @param initialY Altezza iniziale della particella (compresa tra [-r1; r1])
   * @param initialDirection Direzione iniziale della particella (compresa tra [-PI/2; PI/2])
   *
   * Lancia una sola particella e riempie un vettore con le posizioni di tutti gli urti tra essa e i
   * bordi del biliardo. Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
   */
  void launchForDrawing_(std::vector<double>& output, double initialY,
                         double initialDirection) const;

 public:
  Biliardo(double l, double r1, double r2, BiliardoType type = leftBounded);

  [[nodiscard]] BiliardoType type() const;

  [[nodiscard]] const double& l() const { return l_; }
  [[nodiscard]] const double& r1() const { return r1_; }
  [[nodiscard]] const double& r2() const { return r2_; }
  [[nodiscard]] const double& theta() const { return theta_; }

  /**
   * @brief Consente di cambiare il tipo del biliardo
   * @param type Il nuovo tipo che si vuole assegnare
   * @return Restituisce true se il cambio è avvenuto con successo, altrimenti ritorna false
   */
  [[nodiscard]] bool changeType(BiliardoType type);

  Biliardo& l(double l);
  Biliardo& r1(double r1);
  Biliardo& r2(double r2);
  [[nodiscard]] bool modify(double l, double r1, double r2);

  void collideTop(double& angle) const { angle = 2 * theta_ - angle; }
  void collideBottom(double& angle) const { angle = -2 * theta_ - angle; }

  /**
   * @brief Lancia una singola particella da disegnare fornendo tutti i parametri iniziali.
   *
   * @param output Il vettore che viene riempito con il risultato del lancio
   * @param initialY Se viene passato true viene eseguito un check sulla validità dei parametri
   * initialY e initialDirection. Se non è necessario che il metodo esegua questo controllo il
   * parametro può essere omesso
   * @param initialDirection
   *
   * @return Ritorna true se gli input risultano nei range appropriati, altrimenti ritorna false
   *
   * Lancia una sola particella e riempie un vettore con le posizioni di tutti gli urti tra essa e i
   * bordi del biliardo. Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
   */
  bool launchForDrawing(std::vector<double>& output, std::optional<double> initialY = std::nullopt,
                        std::optional<double> initialDirection = {});

  // Questo metodo è in grado di lanciare multiple particelle con un unica
  // chiamata.
  // Restituisce un vettore contenente solo informazioni riguardo l'uscita delle
  // particelle dal biliardo
  void multipleLaunch(unsigned int N, double muY, double sigmaY, double muT, double sigmaT,
                      std::array<TH1D, 2>& histograms, bool async = true);
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_
