<h1 style="text-align: center">Simulazione della dinamica di un biliardo triangolare</h1>
<h3 style="text-align: center">Paolo Forni</h3>

## Indice

1. [Scelte progettuali e implementative](#scelte-progettuali-e-implementative)
2. [Compilare, testare ed eseguire](#compilare-testare-ed-eseguire)
3. [Come utilizzare il programma](#come-utilizzare-il-programma)
4. [Risultati ottenuti](#risultati-ottenuti)
5. [Strategie di test](#strategie-di-test)

### Scelte progettuali e implementative

Librerie utilizzate:

- [SFML](https://github.com/SFML/SFML): libreria grafica per raffigurare immagini e intercettare evenit da tastirea e
  mouse
- [tgui](https://github.com/texus/TGUI): libreria per la creazione dell'interfaccia grafica (*graphic user interface*)
- [ROOT](https://github.com/root-project/root): framework per l'analisi dati, in questo progetto è usato solo per la
  creazione di grafici
- [boost](https://github.com/boostorg/boost): raccolta di librerie applicabili a innumerevoli ambiti
- [doctest](https://github.com/doctest/doctest): libreria per la creazione di *unit test*
- [benchmark](https://github.com/google/benchmark): libreria per effettuare benchmark simili a *unit test*

Il programma è composto da 4 componenti fondamentali, ognuno rappresentato da una classe o una *struct*:

1. App: la classe App gestisce la struttura a eventi su cui si basa il programma; tramite i suoi metodi pubblici
   permette agli altri componenti di interagire con essa, mentre gli input dell'utente sono catturati
   utilizzando le apposite funzionalità di SFML.  
   Inoltre è anche il possessore di tutti i dati generati dalla simulazione.
2. Biliardo: la classe Biliardo è responsabile della simulazione vera e propria, essa infatti si occupa di gestire la
   fisica dei rimbalzi, del calcolo statistico e della generazione delle condizioni di partenza quando queste non sono
   specifiacte dall'utente
3. Designer: la classe Designer si occupa di tutto il comparto grafico della simulazione (non della *gui_*), è quindi
   responsabile della rappresentazione del biliardo, di quella dei grafici e dell'animazione dei lanci delle
   singole particelle
4. Gui: la *struct* Gui

### Compilare, testare ed eseguire

### Come utilizzare il programma

### Risultati ottenuti

### Strategie di test