//
// Created by paolo on 13/06/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_

#include <TH1D.h>

#include <array>
#include <iostream>
#include <random>
#include <vector>

#include "BiliardoFunctions.hpp"
#include "types.hpp"

namespace bt {

class Biliardo {
  BiliardoType type_;

  double l_;
  double r1_;
  double r2_;

  double theta_;

  BiliardoFunctions functions_;

  std::default_random_engine rng_;
  std::uniform_real_distribution<double> uniformDist_{0, 1};
  std::normal_distribution<double> yNormalDist_;
  std::normal_distribution<double> thetaNormalDist_{0, M_PI / 8};

  void collideTop(double& angle) const { angle = 2 * theta_ - angle; }
  void collideBottom(double& angle) const { angle = -2 * theta_ - angle; }

  void registerTopBottomCollision(double const& x, double& y, double const& a, double const& c,
                                  std::vector<double>& output) const;

  [[nodiscard]] bool isOut(const LastHit& lastHit) const;

 public:
  Biliardo(double l, double r1, double r2, BiliardoType type);
  ~Biliardo() = default;

  [[nodiscard]] BiliardoType type() const;

  /// \brief Consente di cambiare il tipo del biliardo
  /// \param type Il nuovo tipo che si vuole assegnare
  /// \return restituisce true se il cambio è avvenuto con successo, altrimenti ritorna false
  bool changeType(BiliardoType type);

  [[nodiscard]] const double& l() const { return l_; }
  [[nodiscard]] const double& r1() const { return r1_; }
  [[nodiscard]] const double& r2() const { return r2_; }

  // no by reference se no ambigua
  // metodi per settare l, r1, e r2; ritornano delle const reference così si possono settare i parametri e al contempo
  // immagazzinare una loro reference
  // passare false come secondo argomento se si è gia controllato che il valore fornito sia positivo
  // in caso di errore il parametro non viene modificato
  template <class T>
  const double& l(T l, bool shouldCheck = true) {
    static_assert(std::is_arithmetic_v<T>);
    if (shouldCheck) {
      if (l <= 0) {
        std::cerr << "il parametro \"l\" deve essere positivo; è stato fornito" << l << '\n';
        return l_;
      }
    }
    l_ = double(l);
    theta_ = std::atan((r2_ - r1_) / l_);
    return l_;
  }
  template <class T>
  const double& r1(T r1, bool shouldCheck = true) {
    static_assert(std::is_arithmetic_v<T>);
    if (shouldCheck) {
      if (r1 <= 0) {
        std::cerr << "il parametro \"r1\" deve essere positivo; è stato fornito " << r1 << '\n';
        return r1_;
      }
    }
    r1_ = double(r1);
    theta_ = std::atan((r2_ - r1_) / l_);
    yNormalDist_ = std::normal_distribution<double>(0, r1_ / 5);
    return r1_;
  }
  template <class T>
  const double& r2(T r2, bool shouldCheck = true) {
    static_assert(std::is_arithmetic_v<T>);
    if (shouldCheck) {
      if (r2 <= 0) {
        std::cerr << "il parametro \"r2\" deve essere positivo; è stato fornito" << r2 << '\n';
        return r2_;
      }
    }
    r2_ = double(r2);
    theta_ = std::atan((r2_ - r1_) / l_);
    return r2_;
  }

  /// \brief Metodo per lanciare una singola particella da disegnare fornendo tutti i parametri iniziali, NON effettua
  /// controlli sugli input.
  ///
  /// \param initialY Altezza iniziale della particella (compresa tra [-r1; r1])
  /// \param initialDirection Direzione iniziale della particella (compresa tra [-PI/2; PI/2])
  /// \param output il vettore che viene riempito con il risultato del lancio
  ///
  /// Questo metodo è in grado di lanciare una sola particella e riempie un
  /// vettore con le posizioni di tutti gli urti tra essa e i bordi del
  /// biliardo. Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
  void launchForDrawing(double const& initialY, double const& initialDirection, std::vector<double>& output);

  /// \brief Metodo per lanciare una singola particella da disegnare fornendo tutti i parametri iniziali.
  ///
  /// \param initialY Altezza iniziale della particella (compresa tra [-r1; r1])
  /// \param initialDirection Direzione iniziale della particella (compresa tra [-PI/2; PI/2])
  /// \param output Il vettore che viene riempito con il risultato del lancio
  /// \param shouldCheck Se viene passato true viene eseguito un check sulla validità dei parametri initialY
  /// e initialDirection. Se non è necessario che il metodo esegua questo controllo il parametro può essere omesso
  ///
  /// \return Ritorna true se gli input risultano nei range appropriati, altrimenti ritorna false
  ///
  /// Questo metodo è in grado di lanciare una sola particella e riempie un
  /// vettore con le posizioni di tutti gli urti tra essa e i bordi del
  /// biliardo. Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
  [[nodiscard]] bool launchForDrawing(double const& initialY, double const& initialDirection,
                                      std::vector<double>& output, bool shouldCheck);

  /// \brief Metodo per lanciare una singola particella generando i parametri secondo due distribuzioni uniformi
  /// tra tutti i valori accettati.
  /// \param output Il vettore che viene riempito con il risultato del lancio
  ///
  /// Questo metodo è in grado di lanciare una sola particella e riempie un
  /// vettore con le posizioni di tutti gli urti tra essa e i bordi del
  /// biliardo. Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
  void launchForDrawing(std::vector<double>& output);

  /// \brief Metodo per lanciare una singola particella fornendo solo la direzione iniziale e generando l'altezza
  /// iniziale secondo una distribuzione uniforme tra tutti i valori accettati. NON effettua il controllo sull'input.
  ///
  /// \param initialDirection Direzione iniziale della particella (compresa tra [-PI/2; PI/2])
  /// \param output Il vettore che viene riempito con il risultato del lancio
  ///
  /// Questo metodo è in grado di lanciare una sola particella e riempie un
  /// vettore con le posizioni di tutti gli urti tra essa e i bordi del
  /// biliardo. Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
  void launchForDrawingNoY(double const& initialDirection, std::vector<double>& output);

  /// \brief Metodo per lanciare una singola particella fornendo solo la direzione iniziale e generando l'altezza
  /// iniziale secondo una distribuzione uniforme tra tutti i valori accettati.
  ///
  /// \param initialDirection Direzione iniziale della particella (compresa tra [-PI/2; PI/2])
  /// \param output Il vettore che viene riempito con il risultato del lancio
  /// \param shouldCheck Se viene passato true viene eseguito un check sulla validità del parametro initialDirection.
  /// Se non è necessario che il metodo esegua questo controllo il parametro può essere omesso
  ///
  /// \return Ritorna true se l'input risulta nei range appropriato, altrimenti ritorna false
  ///
  /// Questo metodo è in grado di lanciare una sola particella e riempie un
  /// vettore con le posizioni di tutti gli urti tra essa e i bordi del
  /// biliardo. Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
  [[nodiscard]] bool launchForDrawingNoY(double const& initialDirection, std::vector<double>& output, bool shouldCheck);

  /// \brief Metodo per lanciare una singola particella fornendo solo l'altezza iniziale e generando la direzione
  /// iniziale secondo una distribuzione uniforme tra tutti i valori accettati. NON effettua il controllo
  /// sull'input.
  ///
  /// \param initialY Altezza iniziale della particella (compresa tra [-r1; r1])
  /// \param output Il vettore che viene riempito con il risultato del lancio
  ///
  /// Questo metodo è in grado di lanciare una sola particella e riempie un
  /// vettore con le posizioni di tutti gli urti tra essa e i bordi del
  /// biliardo. Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
  void launchForDrawingNoDir(const double& initialY, std::vector<double>& output);

  /// \brief Metodo per lanciare una singola particella fornendo solo l'altezza iniziale e generando la direzione
  /// iniziale secondo una distribuzione uniforme tra tutti i valori accettati.
  ///
  /// \param initialY Altezza iniziale della particella (compresa tra [-r1; r1])
  /// \param output Il vettore che viene riempito con il risultato del lancio
  /// \param shouldCheck Se viene passato true viene eseguito un check sulla validità del parametro initialY.
  ///  Se non è necessario che il metodo esegua questo controllo il parametro può essere omesso
  ///
  /// Questo metodo è in grado di lanciare una sola particella e riempie un
  /// vettore con le posizioni di tutti gli urti tra essa e i bordi del
  /// biliardo. Le ultime due posizioni sono occupate dalle direzioni finale e iniziale.
  [[nodiscard]] bool launchForDrawingNoDir(const double& initialY, std::vector<double>& output, bool shouldCheck);

  // Questo metodo è in grado di lanciare multiple particelle con un unica
  // chiamata.
  // Restituisce un vettore contenente solo informazioni riguardo l'uscita delle
  // particelle dal biliardo
  void launch(unsigned int N, std::array<TH1D, 2>& histograms);
  void multipleLaunch(float muY, float sigmaY, float muT, float sigmaT, unsigned int N,
                      std::array<TH1D, 2>& histograms);
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_
