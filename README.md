<h1 style="text-align: center">Simulazione della dinamica di un biliardo triangolare</h1>
<h3 style="text-align: center">Paolo Forni</h3>

## Indice

1. [Scelte progettuali e implementative](#scelte-progettuali-e-implementative)
2. [Compilare, testare ed eseguire](#compilare-testare-ed-eseguire)
3. [Come utilizzare il programma](#come-utilizzare-il-programma)
4. [Risultati ottenuti](#risultati-ottenuti)
5. [Strategie di test](#strategie-di-test)

## Scelte progettuali e implementative

### Librerie utilizzate

- [SFML](https://github.com/SFML/SFML): libreria grafica per raffigurare immagini e intercettare eventi da tastiera e
  mouse
- [tgui](https://github.com/texus/TGUI): libreria per la creazione dell'interfaccia grafica (*graphic user interface*)
- [ROOT](https://github.com/root-project/root): framework per l'analisi dati, in questo progetto è usato solo per la
  creazione di grafici e il calcolo dei parametri statistici sul loro contenuto
- [boost](https://github.com/boostorg/boost): raccolta di librerie applicabili a innumerevoli ambiti
- [doctest](https://github.com/doctest/doctest): libreria per la creazione di *unit test*
- [benchmark](https://github.com/google/benchmark): libreria per effettuare benchmark simili a *unit test*

### Struttura del progetto

Il progetto è diviso in due parti:

1. La libreria libTPool (*Triangular Pool*) che si occupa unicamente di eseguire la simulazione fisica.
2. Il programma biliardo_triangolare che fornisce un'interfaccia grafica per utilizzare le funzionalità di libTPool.

Al livello del codice il programma (libTPoll compresa) è composto da 4 componenti fondamentali, ognuno rappresentato da
una classe:

|                 |                                                                                                                                                                                                                                                                                                                                                                                        |
|:---------------:|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|       App       | Gestisce la struttura a eventi su cui si basa il programma, detiene il possesso di tutti i dati generati dalla simulazione e contiene al suo interno un'istanza di tutte le altre classi del progetto; tramite i suoi metodi pubblici permette agli altri componenti di interagire con essa, mentre gli input dell'utente sono catturati utilizzando le apposite funzionalità di SFML. |
| Pool (libTPool) | È responsabile della simulazione vera e propria, essa infatti si occupa di creare il contesto del biliardo, gestire la fisica dei rimbalzi e di generare delle condizioni di partenza valide quando queste non sono specificate dall'utente.                                                                                                                                           |
|    Designer     | Si occupa di tutto il comparto grafico della simulazione (non della *Gui*), è quindi responsabile della rappresentazione del biliardo, di quella dei grafici e dell'animazione dei lanci delle singole particelle.                                                                                                                                                                     |
|       Gui       | Gestisce l'interfaccia grafica dell'utente, quindi associare la chiamata di una funzione alla pressione di un bottone, leggere i dati inseriti nelle caselle di testo e aggiornare le scritte.                                                                                                                                                                                         |

### Funzionalità del programma

![bottoni_evidenziati](images/bottoni_evidenziati.jpg)
Nell'ordine con il quale compaiono nell'immagine:

- **Scelta del tipo di biliardo**: la libreria TPool permette di gestire biliardi sia aperti da entrambi i lati, sia
  chiusi da uno qualsiasi dei due; per fare ciò il programma presenta tre bottoni, uno per tipologia, nell'angolo in
  alto a sinistra (evidenziati in rosso nell'immagine).
  L'applicazione mantiene sempre in memoria i lanci associati a tutti i tipi di biliardo.
- **Scelta delle dimensioni del biliardo**: l'applicazione permette di modificare i parametri del biliardo a *run time*
  tramite gli appositi campi di testo e il bottone di conferma (evidenziati in giallo nell'immagine).
  I campi di testo riportano come *placeholder* i valori attuali dei vari parametri. **N.B. la modifica delle dimensioni
  del biliardo comporta il reset della memoria dei lanci!**
- **Lancio di una singola particella**: il programma è in grado di lanciare una singola particella il cui percorso verrà
  poi raffigurato nel riquadro in alto a destra della finestra; è possibile, ma non obbligatorio, indicare ordinata e
  direzione iniziali della particella tramite i campi appositi (evidenziati in verde scuro in figura) e poi premere il
  pulsante "lancio singolo" per dare il via alla simulazione.
- **Navigazione dei lanci singoli**: i bottoni evidenziati in blu permettono di navigare tra i lanci presenti in memoria
  e si dividono in due gruppi, il primo, costituito da quelli sulla sinistra, permette di scorre i lanci in memoria per
  il tipo di biliardo selezionato, il secondo, formato dai restanti, serve a mettere in pausa o far ripartire, anche da
  capo, la riproduzione del lancio attualmente selezionato.
- **Lancio di più particelle**

### Scelte implementative

#### La classe Pool

La classe Biliardo, al contrario di quanto ci si potrebbe aspettare (e di ciò che avveniva in un precedente momento
dello sviluppo) non utilizza il polimorfismo dinamico per gestire i tre tipi di configurazione simulati dal programma; i
motivi sono molteplici:

- I vantaggi legati alla possibilità di riferirsi ai puntatori delle sottoclassi con dei puntatori della classe madre
  non verrebbero sfruttati significativamente, viste le poche istanze della classe che si creano
- I metodi legati alla gestione degli urti delle particelle, che sarebbero chiaramente dichiarati come puramente
  virtuali nella classe madre, dovrebbero richiedere gli stessi argomenti per ogni tipo di biliardo. Ma i metodi
  specializzati delle sottoclassi hanno davvero bisogno di un sottoinsieme di quest'ultimi, ciò porterebbe alla
  creazione di un metodo che non utilizza tutti i parametri che accetta. (vedi appendice
  [Vecchio codice problematico](#vecchio-codice-problematico))

Per gestire i vari tipi di biliardo si utilizza invece il membro privato `type_` di tipo `BiliardoType`, un *enum* utile
anche per indicizzare gli array che contengono i dati riguardo i vari tipi di biliardo, e la *nested class*
`BiliardoFunctions` la quale espone i metodi che accettano tutti i parametri ed internamente chiama quelli
specializzati.

#### Lanci multipli

Per la simulazione di lanci multipli vengono utilizzati gli algoritmi della *standard library* per poter sfruttare le
*execution policy* migliorando le prestazioni

## Compilare, testare ed eseguire

Una volta clonato il repository ed aver installato tutte le dipendenze, digitare i seguenti comandi per configurare la
build con cmake:

```shell
cd build
cmake .. [-DCMAKE_BUILD_TYPE=<Build-type>] -DBUILD_BENCHMARK=TRUE /
  -DBUILD_TESTING=TRUE[-DROOT_DIR=<path/to/ROOTConfig.cmake>]
```

Compilando in `RELEASE` (default) mode si otterranno significativi miglioramenti nei lanci multipli soprattutto
all'aumentare
del numero di particelle generate.  
Per non compilare test e benchmark omettere i parametri o impostarli su `FALSE`.  
In base al tipo di installazione di ROOT, potrebbe non essere necessario specificare il parametro `ROOT_DIR`.

Una volta configurate le impostazioni di compilazione, avviare la build digitando:

```shell
cmake --build . [-j <N>]
```

Il parametro opzionale `-j N` imposta il numero di processori logici disponibili per il compilatore (in un sistema con 8
threads
un buon valore potrebbe essere 6).

Gli eseguibili generati si troveranno nella cartella build e nelle sue sottocartelle.

I test accettano degli argomenti da riga di comando per gestirne lo svolgimento e manipolare i risultati, per
visualizzarli lanciare l'eseguibile aggiungendo `--help`

## Come utilizzare il programma

## Risultati ottenuti

## Strategie di test

# Appendice

## Vecchio codice problematico

```c++
// include/Biliardo.hpp
class Biliardo {
 protected:
  virtual void registerLeftCollision(double& x, double& y, const double& c, double& dir, LastHit& lastHit,
                                         std::vector<double>& output) const = 0;
  ...
};

// include/BiliardoChiusoDx.hpp
class BiliardoChiusoDx : public Biliardo {
  ...
  void registerLeftCollision(double& x, double& y, const double& c, double& dir, LastHit& lastHit,
                                 std::vector<double>& output) const override;
  ...
};

// BiliardoChiusoDx.cpp
void BiliardoChiusoDx::registerLeftCollision(double& x, double& y, const double& c, double& dir, LastHit& lastHit,
                                             std::vector<double>& output) const {
  output.push_back(0);
  output.push_back(c);
  output.push_back(dir);
}

// include/BiliardoChiusoSx.hpp
class BiliardoChiusoSx : public Biliardo {
 protected:
  void registerLeftCollision(double& x, double& y, const double& c, double& dir, LastHit& lastHit,
                             std::vector<double>& output) const override;
  ...
};

// BiliardoChiusoSx.cpp
void BiliardoChiusoSx::registerLeftCollision(double& x, double& y, const double& c, double& dir, LastHit& lastHit,
                                             std::vector<double>& output) const {
  collideLeft(dir);
  x = 0;
  y = c;
  output.push_back(x);
  output.push_back(y);
  lastHit = left;
}
```