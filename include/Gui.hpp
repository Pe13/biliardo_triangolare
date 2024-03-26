//
// Created by paolo on 29/08/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_GUI_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_GUI_HPP_

#include <TH1D.h>

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Core.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/TextArea.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <algorithm>
#include <functional>
#include <memory>

namespace bt {
/**
 * @brief Formatta le stringe di input numerici in modo da invalidarle o renderle comprensibili al metodo
 * std::stod
 * @param str Stringa da formattare (non viene modificata)
 * @return La stringa formattata ma in formato std::string così da poter essere usata con i metodi tipo std::stod.
 * Oppure una stringa vuota nel caso non sia valida
 */
inline std::string format(const tgui::String& str) {
  if (str.empty()) {
    return "";
  }
  auto string = str;

  // rimuovo tutti gli apici e gli spazi (che possono essere usati per contare meglio le cifre inserite)
  string.remove('\'');
  string.remove(' ');

  string = string.toLower();  // così facendo mi basta controllare "e" e non "E"

  // trasformo le virgole in punti così da accettare entrambe le notazioni
  std::replace(string.begin(), string.end(), ',', '.');

  // salvo il segno del numero e dell'esponenziale per dopo, se non ci non ri aggiungerò nulla
  tgui::String sign{""};
  if (string.starts_with('+') || string.starts_with('-')) {
    sign = string[0];
    string.erase(string.begin());
  }
  tgui::String eSign{""};
  auto eSignPos = std::min(string.find("e+"), string.find("e-")) + 1;
  if (eSignPos != tgui::String::npos + 1) {
    eSign = string[eSignPos];
    string.erase(eSignPos, 1);
  }

  // invalido la stringa se l'ultimo carattere è una e
  if (string.back() == 'e') {
    return "";
  }

  // se contiene anche solo un carattere che non è tra quelli sottoelencati invalido la stringa
  if (string.find_first_not_of("0123456789e.") != tgui::String::npos) {
    return "";
  }
  // reinserisco l'eventuale segno del numero
  string.insert(0, sign);

  // se contiene più di un punto decimale o più di una "e" invalido la stringa
  if (std::count(string.begin(), string.end(), 'e') > 1 || std::count(string.begin(), string.end(), '.') > 1) {
    return "";
  }
  // reinserisco l'eventuale segno all'eventuale esponenziale
  eSignPos = string.find('e');
  if (eSignPos != tgui::String::npos) {
    string.insert(eSignPos + 1, eSign);
  }

  // se la "e" è prima del punto invalido la stringa
  if (string.find('e') < string.find('.') && string.find('.') != tgui::String::npos) {
    return "";
  }

  return string.toStdString();
}

/**
 * @brief Ritorna true se la stringa è un input valido, e in tal caso salva il risultato sotto forma di double nella
 * variabile fornita. In caso di fallimento la variabile non viene modificata
 * @param string La stringa da esaminare
 * @param val La variabile in cui salvare l'input
 * @return True se la conversione è andata a buon fine, false altrimenti
 */
inline bool isValidInput(const tgui::String& string, double& val) {
  try {
    val = std::stod(format(string));
    return true;
  } catch (std::invalid_argument&) {
    return false;
  } catch (std::out_of_range&) {
    return false;
  }
}

using iterator = std::vector<tgui::Widget::Ptr>::const_iterator;
/**
 * @brief Applica una funzione lambda a ciascun elemento di un determinato tipo nell'intervallo specificato, tenendo
 * traccia dell'indice.
 *
 * La funzione è ricorsiva e può gestire anche container nidificati all'interno dell'intervallo.
 *
 * @tparam T Il tipo di elemento a cui punta il puntatore al quale applicare la funzione lambda.
 * @tparam Function Il tipo della funzione lambda.
 * @param begin Un iteratore che punta all'inizio dell'intervallo.
 * @param end Un iteratore che punta alla fine dell'intervallo.
 * @param f La funzione lambda da applicare a ciascun elemento di tipo T.
 * @param index L'indice iniziale per tenere traccia della posizione.
 * @return L'indice dell'ultimo elemento processato nell'intervallo.
 *
 * Questa funzione template accetta un intervallo [begin, end) di tgui::Widget::Ptr e applica la funzione lambda fornita
 * a ciascun elemento all'interno dell'intervallo se questo può essere convertito in un puntatore del tipo specificato,
 * tenendo traccia dell'indice dell'elemento. La funzione lambda deve avere il seguente prototipo: `void lambda(const
 * std::shared_ptr<T>& element, unsigned int index)`.
 */
template <typename T, typename Function>
unsigned int forEachIndexed(iterator begin, iterator end, Function&& f, unsigned int index = 0) {
  std::for_each(begin, end, [&](tgui::Widget::Ptr ptr) {
    if (ptr->isContainer()) {
      tgui::Container::Ptr container = std::static_pointer_cast<tgui::Container>(
          ptr);  // posso usare uno static_cast dato che sono scuro sia un cast valido
      auto children = container->getWidgets();
      index = forEachIndexed<T>(children.begin(), children.end(), f, index);
    }
    auto target = std::dynamic_pointer_cast<T>(ptr);
    if (target) {
      f(target, index);
      index++;
    }
  });
  return index;
}

/**
 * @brief Applica una funzione lambda a ciascun elemento di un determinato tipo nell'intervallo specificato.
 *
 * La funzione è ricorsiva e può gestire anche container nidificati all'interno dell'intervallo.
 *
 * @tparam T Il tipo di elemento a cui applicare la funzione lambda.
 * @tparam Function Il tipo della funzione lambda.
 * @param begin Un iteratore che punta all'inizio dell'intervallo.
 * @param end Un iteratore che punta alla fine dell'intervallo.
 * @param f La funzione lambda da applicare a ciascun elemento.
 *
 * Questa funzione template accetta un intervallo [begin, end) e applica la funzione lambda fornita a ciascun elemento
 * all'interno dell'intervallo se questo può essere convertito in un puntatore del tipo specificato. La funzione lambda
 * deve avere il seguente prototipo: `void lambda(const std::shared_ptr<T>& element)`.
 */
template <typename T, typename Function>
void forEach(iterator begin, iterator end, Function&& f) {
  std::for_each(begin, end, [&](tgui::Widget::Ptr ptr) {
    if (ptr->isContainer()) {
      tgui::Container::Ptr container = std::static_pointer_cast<tgui::Container>(
          ptr);  // posso usare uno static_cast dato che sono scuro sia un cast valido
      auto children = container->getWidgets();
      forEach<T>(children.begin(), children.end(), f);
    }
    auto target = std::dynamic_pointer_cast<T>(ptr);
    if (target) {
      f(target);
    }
  });
}

class App;

class Gui {
  tgui::Gui gui_;

  tgui::VerticalLayout::Ptr wrapper_{tgui::VerticalLayout::create()};

  // bottoni per cambiare tipo di biliardo
  tgui::HorizontalLayout::Ptr biliardoButtonsWrapper_{tgui::HorizontalLayout::create()};
  tgui::Button::Ptr biliardoApertoBtn_{tgui::Button::create("Biliardo\naperto")};
  tgui::Button::Ptr biliardoChiusoDxBtn_{tgui::Button::create("Biliardo\nchiuso Dx")};
  tgui::Button::Ptr biliardoChiusoSxBtn_{tgui::Button::create("Biliardo\nchiuso Sx")};

  // bottone e campi per modificare il biliardo
  tgui::HorizontalLayout::Ptr newBiliardoWrapper_{tgui::HorizontalLayout::create()};
  tgui::VerticalLayout::Ptr leftNewBiliardoWrapper_{tgui::VerticalLayout::create()};
  tgui::Label::Ptr r1Label_{tgui::Label::create("r1:")};
  tgui::EditBox::Ptr r1Input_{tgui::EditBox::create()};
  tgui::Label::Ptr r2Label_{tgui::Label::create("r2:")};
  tgui::EditBox::Ptr r2Input_{tgui::EditBox::create()};
  tgui::VerticalLayout::Ptr rightNewBiliardoWrapper_{tgui::VerticalLayout::create()};
  tgui::Label::Ptr lLabel_{tgui::Label::create("l:")};
  tgui::EditBox::Ptr lInput_{tgui::EditBox::create()};
  tgui::Button::Ptr newBiliardoBtn_{tgui::Button::create("Conferma")};

  // bottone e campi per lanciare la singola particella
  tgui::HorizontalLayout::Ptr singleLaunchWrapper_{tgui::HorizontalLayout::create()};
  tgui::VerticalLayout::Ptr heightWrapper_{tgui::VerticalLayout::create()};
  tgui::Label::Ptr heightLabel_{tgui::Label::create("Altezza:")};
  tgui::EditBox::Ptr heightInput_{tgui::EditBox::create()};
  tgui::VerticalLayout::Ptr angleWrapper_{tgui::VerticalLayout::create()};
  tgui::Label::Ptr angleLabel_{tgui::Label::create("Angolo (rad):")};
  tgui::EditBox::Ptr angleInput_{tgui::EditBox::create()};
  tgui::VerticalLayout::Ptr singleLaunchBtnWrapper_{tgui::VerticalLayout::create()};
  tgui::Button::Ptr singleLaunchBtn_{tgui::Button::create("Lancio singolo")};

  // bottoni per navigare tra i singoli lanci
  tgui::HorizontalLayout::Ptr navigateLaunchesWrapper_{tgui::HorizontalLayout::create()};
  tgui::Button::Ptr nextLaunchBtn_{tgui::Button::create("→")};
  tgui::Button::Ptr previousLaunchBtn_{tgui::Button::create("←")};
  tgui::Button::Ptr pauseBtn_{tgui::Button::create("▶")};
  tgui::Button::Ptr reRunBtn_{tgui::Button::create("⟳")};

  // bottone e campi per lanciare N particelle
  tgui::HorizontalLayout::Ptr multipleLaunchWrapper_{tgui::HorizontalLayout::create()};
  tgui::VerticalLayout::Ptr leftMultipleLaunchWrapper_{tgui::VerticalLayout::create()};
  tgui::Label::Ptr muYLabel_{tgui::Label::create("µy:")};
  tgui::EditBox::Ptr muYInput_{tgui::EditBox::create()};
  tgui::Label::Ptr sigmaYLabel_{tgui::Label::create("σy:")};
  tgui::EditBox::Ptr sigmaYInput_{tgui::EditBox::create()};
  tgui::Label::Ptr muTLabel_{tgui::Label::create("µθ:")};
  tgui::EditBox::Ptr muTInput_{tgui::EditBox::create()};
  tgui::Label::Ptr sigmaTLabel_{tgui::Label::create("σθ:")};
  tgui::EditBox::Ptr sigmaTInput_{tgui::EditBox::create()};
  tgui::VerticalLayout::Ptr rightMultipleLaunchWrapper_{tgui::VerticalLayout::create()};
  tgui::Label::Ptr numberLabel_{tgui::Label::create("Numero:")};
  tgui::EditBox::Ptr numberInput_{tgui::EditBox::create()};
  tgui::Button::Ptr multipleLaunchBtn_{tgui::Button::create("Lancio multiplo")};

  // bottoni per navigare tra gli istogrammi e salvarli
  tgui::HorizontalLayout::Ptr navigateHistogramsWrapper_{tgui::HorizontalLayout::create()};
  tgui::Button::Ptr nextHistogramBtn_{tgui::Button::create("→")};
  tgui::Button::Ptr previousHistogramBtn_{tgui::Button::create("←")};
  tgui::Button::Ptr saveHistogramBtn_{tgui::Button::create("Salva\nGrafico")};

  // area di testo per i dati statistici
  tgui::HorizontalLayout::Ptr textWrapper_{tgui::HorizontalLayout::create()};
  tgui::TextArea::Ptr leftText_{tgui::TextArea::create()};
  tgui::TextArea::Ptr rightText_{tgui::TextArea::create()};

  void newBiliardoBtnPressed(App* app) const;
  void singleLaunchBtnPressed(App* app) const;
  void multipleLaunchBtnPressed(App* app) const;

  void create();
  void style() const;
  void activate(App* app) const;
  void setDefaultText() const;

 public:
  Gui(sf::RenderWindow& window, App* app);

  void handleEvent(sf::Event& event);
  void draw();
  void setSize(float width, float height) const;

  void setSingleLaunchText(const std::vector<double>& launch) const;
  void setStatisticsText(const std::array<TH1D, 2>& histograms) const;
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_GUI_HPP_
