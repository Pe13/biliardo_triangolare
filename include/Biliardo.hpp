//
// Created by paolo on 13/06/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_

#include <TH1D.h>

#include <array>
#include <boost/implicit_cast.hpp>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include "BiliardoFunctions.hpp"
#include "types.hpp"

namespace bt {

class Biliardo {
  class BiliardoFunctions {  // la definisco qui perché ne devo creare un istanza all'interno del Biliardo
    void collideSide(double& angle) const { angle = -angle; }

    void leftCollisionOut(const double& c, const double& direction, std::vector<double>& output) const;

    void leftCollision(double& x, double& y, const double& c, double& direction, LastHit& lastHit,
                       std::vector<double>& output) const;

    void rightCollisionOut(const double& a, const double& c, const double& direction, const double& l,
                           std::vector<double>& output) const;
    void rightCollision(double& x, double& y, const double& a, const double& c, LastHit& lastHit, double& direction,
                        const double& l, std::vector<double>& output) const;

   public:
    void registerLeftCollision(BiliardoType type, double& x, double& y, const double& c, double& direction,
                               LastHit& lastHit, std::vector<double>& output) const;

    void registerRightCollision(BiliardoType type, double& x, double& y, const double& a, const double& c,
                                LastHit& lastHit, double& direction, const double& l,
                                std::vector<double>& output) const;
  };

  BiliardoType type_;

  double l_;
  double r1_;
  double r2_;

  double theta_;

  BiliardoFunctions functions_;

  std::default_random_engine rng_{
      boost::implicit_cast<unsigned long long>(std::chrono::system_clock::now().time_since_epoch().count())};
  std::uniform_real_distribution<double> uniformDist_{0, 1};
  std::normal_distribution<double> yNormalDist_;
  std::normal_distribution<double> thetaNormalDist_{0, M_PI / 8};

  void registerTopBottomCollision(double const& x, double& y, double const& a, double const& c,
                                  std::vector<double>& output) const;

  [[nodiscard]] bool isOut(const LastHit& lastHit) const;

  [[nodiscard]] std::array<double, 2> generateParticle();
  void launchForHistograms(std::array<double, 2>& launch) const;
  void syncLaunch(unsigned int N, std::array<TH1D, 2>& histograms);
  void asyncLaunch(unsigned int N, std::array<TH1D, 2>& histograms);

  /**
   * @brief Lancia una singola particella da disegnare fornendo tutti i parametri iniziali, NON effettua controlli sugli
   * input.
   *
   * @param initialY Altezza iniziale della particella (compresa tra [-r1; r1])
   * @param initialDirection Direzione iniziale della particella (compresa tra [-PI/2; PI/2])
   * @param output il vettore che viene riempito con il risultato del lancio
   *
   * Lancia una sola particella e riempie un vettore con le posizioni di tutti gli urti tra essa e i bordi del biliardo.
   * Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
   */
  void launchForDrawing_(const double& initialY, const double& initialDirection, std::vector<double>& output) const;

 public:
  Biliardo(double l, double r1, double r2, BiliardoType type = leftBounded);
  ~Biliardo() = default;

  [[nodiscard]] BiliardoType type() const;

  /**
   * @brief Consente di cambiare il tipo del biliardo
   * @param type Il nuovo tipo che si vuole assegnare
   * @return restituisce true se il cambio è avvenuto con successo, altrimenti ritorna false
   */
  bool changeType(BiliardoType type);

  [[nodiscard]] const double& l() const { return l_; }
  [[nodiscard]] const double& r1() const { return r1_; }
  [[nodiscard]] const double& r2() const { return r2_; }
  [[nodiscard]] const double& theta() const { return theta_; }

  // no by reference se no ambigua
  // metodi per settare l, r1, e r2; ritornano delle const reference così si possono settare i parametri e al contempo
  // immagazzinare una loro reference
  // passare false come secondo argomento se si è gia controllato che il valore fornito sia positivo
  // in caso di errore il parametro non viene modificato
  const double& l(double l, bool shouldCheck = true);
  const double& r1(double r1, bool shouldCheck = true);
  const double& r2(double r2, bool shouldCheck = true);
  bool modify(double r1, double r2, double l, bool shouldCheck = true);

  void collideTop(double& angle) const { angle = 2 * theta_ - angle; }
  void collideBottom(double& angle) const { angle = -2 * theta_ - angle; }

  /**
   * @brief Lancia una singola particella da disegnare fornendo tutti i parametri iniziali.
   *
   * @param initialY Altezza iniziale della particella (compresa tra [-r1; r1])
   * @param initialDirection Direzione iniziale della particella (compresa tra [-PI/2; PI/2])
   * @param output Il vettore che viene riempito con il risultato del lancio
   * @param shouldCheck Se viene passato true viene eseguito un check sulla validità dei parametri initialY e
   * initialDirection. Se non è necessario che il metodo esegua questo controllo il parametro può essere omesso
   *
   * @return Ritorna true se gli input risultano nei range appropriati, altrimenti ritorna false
   *
   * Lancia una sola particella e riempie un vettore con le posizioni di tutti gli urti tra essa e i bordi del biliardo.
   * Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
   */
  bool launchForDrawing(const double& initialY, const double& initialDirection, std::vector<double>& output,
                        bool shouldCheck = true) const;

  /**
   * @brief Lancia una singola particella generando i parametri secondo due distribuzioni uniformi tra tutti i valori
   * accettati.
   *
   * @param output Il vettore che viene riempito con il risultato del lancio
   *
   * Lancia una sola particella e riempie un vettore con le posizioni di tutti gli urti tra essa e i bordi del biliardo.
   * Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
   */
  void launchForDrawing(std::vector<double>& output);

  /**
   * @brief Lancia una singola particella fornendo solo la direzione iniziale e generando l'altezza iniziale secondo una
   * distribuzione uniforme tra tutti i valori accettati.
   *
   * @param initialDirection Direzione iniziale della particella (compresa tra [-PI/2; PI/2])
   * @param output Il vettore che viene riempito con il risultato del lancio
   * @param shouldCheck Se viene passato true viene eseguito un check sulla validità del parametro initialDirection. Se
   * non è necessario che il metodo esegua questo controllo il parametro può essere omesso
   *
   * @return Ritorna true se l'input risulta nei range appropriati, altrimenti ritorna false
   *
   * Lancia una sola particella e riempie un vettore con le posizioni di tutti gli urti tra essa e i bordi del biliardo.
   * Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
   */

  bool launchForDrawingNoY(double const& initialDirection, std::vector<double>& output, bool shouldCheck = true);

  /**
   * @brief Lancia una singola particella fornendo solo l'altezza iniziale e generando la direzione iniziale secondo una
   * distribuzione uniforme tra tutti i valori accettati.
   *
   * @param initialY Altezza iniziale della particella (compresa tra [-r1; r1])
   * @param output Il vettore che viene riempito con il risultato del lancio
   * @param shouldCheck Se viene passato true viene eseguito un check sulla validità del parametro initialY. Se non è
   * necessario che il metodo esegua questo controllo il parametro può essere omesso
   *
   * Lancia una sola particella e riempie un vettore con le posizioni di tutti gli urti tra essa e i bordi del biliardo.
   * Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
   */
  bool launchForDrawingNoDir(const double& initialY, std::vector<double>& output, bool shouldCheck = true);

  // Questo metodo è in grado di lanciare multiple particelle con un unica
  // chiamata.
  // Restituisce un vettore contenente solo informazioni riguardo l'uscita delle
  // particelle dal biliardo
  void multipleLaunch(double muY, double sigmaY, double muT, double sigmaT, unsigned int N,
                      std::array<TH1D, 2>& histograms, bool async = true);
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_
