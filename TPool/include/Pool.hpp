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

enum PoolType : unsigned { open = 0, rightBounded = 1, leftBounded = 2 };
enum LastHit { left, right, top, bottom };

class Pool {
  // Creo una struct apposita al posto di usare un semplice std::array<double, 2> perché
  // quest'ultimo è un aggregato e per questo non potrei costruirlo con emplace_back
  struct Particle {
    double y;
    double direction;
    Particle(const double y_, const double direction_) : y{y_}, direction{direction_} {}
  };

  PoolType type_;

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
  /**
   * @brief Aggiorna i valori che gli vengono passati con quelli della collisione successiva,
   * ritornando true se questa corrisponde ad un'uscita dal biliardo.
   *
   * @param lastHit Ultimo bordo del biliardo contro il quale la particella ha rimbalzato.
   * @param x Ascissa dell'ultimo urto.
   * @param y Ordinata dell'ultimo urto.
   * @param direction Direzione uscente dall'ultimo urto.
   *
   * @return True se la particella è uscita dal biliardo, altrimenti false.
   */
  [[nodiscard]] bool findNextCollision_(LastHit& lastHit, double& x, double& y,
                                        double& direction) const;

  /**
   * @brief Si occupa del lancio di una singola particella senza tracciarne il percorso.
   * Dopo la sua chiamata la particella si trova nella condizione finale.
   *
   * @param particle Particella da lanciare
   */
  void launchForHistograms_(Particle& particle) const;

  /**
   * @brief Backend del metodo multipleLaunch; gestisce tutto il processo di generazione dei
   * parametri iniziali, di calcolo dei risultati e inserimento degli stessi negli istogrammi.
   * Il calcolo dei risultati può essere svolto in parallelo.
   *
   * @param N Numero di particelle da lanciare
   * @param histograms Array di istogrammi da riempire
   * @param yNormalDist Distribuzione gaussiana per generare le ordinate iniziali
   * @param thetaNormalDist Distribuzione gaussiana per generare le direzioni iniziali
   * @param parallel Booleano che definisce se il calcolo dei risultati deve essere svolto in
   * parallelo
   */
  void multipleLaunch_(unsigned int N, std::array<TH1D, 2>& histograms,
                       std::normal_distribution<double>& yNormalDist,
                       std::normal_distribution<double>& thetaNormalDist, bool parallel);

  [[nodiscard]] bool validateLaunchForDrawingInput_(
      const std::optional<double>& initialY, const std::optional<double>& initialDirection) const;
  void initializeLaunchForDrawingInput_(std::optional<double>& initialY,
                                        std::optional<double>& initialDirection);

  /**
   * @brief Backend del metodo pubblico launchForDrawing.
   * Lancia una singola particella da disegnare fornendo tutti i parametri iniziali, NON
   * effettua controlli sugli input.
   * Riempie un vettore con le posizioni di tutti gli urti tra la particella e i bordi del biliardo;
   * le direzioni, nell'ordine, finale e iniziale sono, gli ultimi due elementi dell'output.
   *
   * @param output Vettore che viene riempito con il risultato del lancio.
   * @param initialY Altezza iniziale della particella (compresa tra (-r1; r1)).
   * @param initialDirection Direzione iniziale della particella (compresa tra (-PI/2; PI/2)).
   *
   */
  void launchForDrawing_(std::vector<double>& output, double initialY,
                         double initialDirection) const;

 public:
  /**
   * @brief Costruttore della lasse Pool
   *
   * @param l Valore della lunghezza del biliardo.
   * @param r1 Valore della distanza delle due estremità sinistre dall'asse X (semi-apertura
   * sinistra).
   * @param r2 Valore della distanza delle due estremità destre dall'asse X (semi-apertura destra).
   * @param type Tipo del biliardo.
   *
   * @throw std::invalid_argument Segnala la presenza di uno o più argomenti non validi.
   */
  Pool(double l, double r1, double r2, PoolType type = leftBounded);

  [[nodiscard]] PoolType type() const;
  [[nodiscard]] const double& l() const { return l_; }
  [[nodiscard]] const double& r1() const { return r1_; }
  [[nodiscard]] const double& r2() const { return r2_; }
  [[nodiscard]] const double& theta() const { return theta_; }
  [[nodiscard]] const double& slope() const { return slope_; }

  [[nodiscard]] bool changeType(PoolType type);
  [[nodiscard]] bool l(double l);
  [[nodiscard]] bool r1(double r1);
  [[nodiscard]] bool r2(double r2);
  [[nodiscard]] bool modify(double l, double r1, double r2);

  friend bool operator==(const Pool& left, const Pool& right) {
    return left.l_ == right.l_ && left.r1_ == right.r1_ && left.r2_ == right.r2_ &&
           left.theta_ == right.theta_ && left.slope_ == right.slope_;
  }

  void collideTop(double& angle) const { angle = 2 * theta_ - angle; }
  void collideBottom(double& angle) const { angle = -2 * theta_ - angle; }

  /**
   * @name launchForDrawing
   * @brief Lancia una singola particella della quale si vuole ricostruire il percorso.
   * Se i parametri non contengono valore, questo viene generato secondo una distribuzione uniforme
   * nel range valido.
   * Riempie un vettore con le posizioni di tutti gli urti tra essa e i bordi del biliardo. Le
   * ultime due posizioni sono occupate dalle direzioni finale e iniziale.
   *
   * @param output Il vettore che viene riempito con il risultato del lancio.
   * @param initialY Il valore che indica l'ordinata iniziale della particella (compresa tra (-r1;
   * r1)).
   * @param initialDirection Il valore che indica la direzione iniziale della particella rispetto
   * all'asse X (in radianti, compresa tra (-PI/2; PI/2))
   *
   * @return True se i parametri forniti risultano nei range appropriati, altrimenti false
   */
  bool launchForDrawing(std::vector<double>& output, std::optional<double> initialY = std::nullopt,
                        std::optional<double> initialDirection = {});

  /**
   * @brief Genera secondo una distribuzione gaussiana definita dall'utente e lancia multiple
   * particelle.
   * Inserisce i dati ottenuti negli istogrammi dell'utente (non per forza vuoti).
   *
   * @param N Numero di particelle da lanciare
   * @param muY Media della distribuzione gaussiana per i valori di ordinata iniziali
   * @param sigmaY Deviazione standard della distribuzione gaussiana per i valori di ordinata
   * iniziali
   * @param muT Media della distribuzione gaussiana per le direzioni iniziali (in radianti)
   * @param sigmaT Deviazione standard della distribuzione gaussiana per le direzioni iniziali
   * @param histograms Coppia di istogrammi da riempire con i risultati dei lanci, prima quello
   * delle ordinate poi quello degli angoli
   * @param parallel Flag per usare l'algoritmo sequenziale o parallelo
   *
   * @return True se le deviazioni standard erano entrambe positive, altrimenti false
   */
  [[nodiscard]] bool multipleLaunch(unsigned int N, double muY, double sigmaY, double muT,
                                    double sigmaT, std::array<TH1D, 2>& histograms,
                                    bool parallel = true);
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_BILIARDO_HPP_
