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

- [SFML](https://github.com/SFML/SFML): libreria grafica per raffigurare immagini e intercettare evenit da tastirea e
  mouse
- [tgui](https://github.com/texus/TGUI): libreria per la creazione dell'interfaccia grafica (*graphic user interface*)
- [ROOT](https://github.com/root-project/root): framework per l'analisi dati, in questo progetto è usato solo per la
  creazione di grafici
- [boost](https://github.com/boostorg/boost): raccolta di librerie applicabili a innumerevoli ambiti
- [doctest](https://github.com/doctest/doctest): libreria per la creazione di *unit test*
- [benchmark](https://github.com/google/benchmark): libreria per effettuare benchmark simili a *unit test*

### Struttura del programma

Il programma è composto da 4 componenti fondamentali, ognuno rappresentato da una classe:

1. App: la classe App gestisce la struttura a eventi su cui si basa il programma; tramite i suoi metodi pubblici
   permette agli altri componenti di interagire con essa, mentre gli input dell'utente sono catturati
   utilizzando le apposite funzionalità di SFML.  
   Inoltre è anche il possessore di tutti i dati generati dalla simulazione.
2. Biliardo: la classe Biliardo è responsabile della simulazione vera e propria, essa infatti si occupa di gestire la
   fisica dei rimbalzi, del calcolo statistico e della generazione delle condizioni di partenza quando queste non sono
   specificate dall'utente
3. Designer: la classe Designer si occupa di tutto il comparto grafico della simulazione (non della *Gui*), è quindi
   responsabile della rappresentazione del biliardo, di quella dei grafici e dell'animazione dei lanci delle
   singole particelle
4. Gui: la classe Gui gestisce appunto l'interfaccia grafica dell'utente, quindi bottoni, campi di testo e scritte
   (tutte nella colonna di destra della finestra)

### Scelte implementative

#### La classe Biliardo

La classe Biliardo, al contrario di quanto ci si potrebbe aspettare (e di ciò che avveniva in un precedente momento
dello sviluppo) non utilizza il polimorfismo dinamico per gestire i tre tipi di configurazione simulati dal programma; i
motivi sono molteplici:

- I vantaggi legati alla possibilità di riferirsi ai puntatori delle sottoclassi con dei puntatori della classe madre
  non verrebbero sfruttati significativamente, viste le poche istanze della classe che si creano
- I metodi legati alla gestione degli urti delle particelle, che sarebbero chiaramente dichiarati come puramente
  virtuali nella classe madre,

## Compilare, testare ed eseguire

Dopo aver clonato il repository ed aver installato tutte le dipendenze, digitare i seguenti comandi per configurare la
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