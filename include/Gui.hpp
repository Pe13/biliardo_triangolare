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

namespace bt {

// funzione per formattare le stringe di input numerici in modo da invalidarle o renderle comprensibili al metodo
// tgui::String::attemptToFloat
inline tgui::String format(const tgui::String& str) {
  if (str.empty()) {
    return str;
  }
  auto string = str;

  // rimuovo tutti gli apici e gli spazi (che possono essere usati per contare meglio le cifre inserite)
  string.remove('\'');
  string.remove(' ');

  string = string.toLower();  // così facendo mi basta controllare "e"

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
  if (eSignPos != 0) {
    eSign = string[eSignPos];
    string.erase(eSignPos, 1);
  }

  // se contiene anche solo un carattere che non è tra quelli sottoelencati invalido la stringa
  if (string.find_first_not_of("0123456789e.") != tgui::String::npos) {
    return {""};
  }
  // reinserisco l'eventuale segno del numero
  string.insert(0, sign);

  // se contiene più di un punto decimale o più di una "e" invalido la stringa
  if (std::count(string.begin(), string.end(), 'e') > 1 || std::count(string.begin(), string.end(), '.') > 1) {
    return {""};
  }
  // reinserisco l'eventuale segno all'eventuale esponenziale
  eSignPos = string.find('e');
  if (eSignPos != tgui::String::npos) {
    string.insert(eSignPos + 1, eSign);
  }

  // se la "e" è prima del punto invalido la stringa
  if (string.find('e') < string.find('.') && string.find('.') != tgui::String::npos) {
    return {""};
  }

  return string;
}


class App;

struct Gui {
  tgui::Gui gui;

  tgui::VerticalLayout::Ptr wrapper{tgui::VerticalLayout::create()};

  // bottoni per cambiare tipo di biliardo
  tgui::HorizontalLayout::Ptr biliardoButtonsWrapper{tgui::HorizontalLayout::create()};
  tgui::Button::Ptr biliardoApertoBtn{tgui::Button::create("Biliardo\naperto")};
  tgui::Button::Ptr biliardoChiusoDxBtn{tgui::Button::create("Biliardo\nchiuso Dx")};
  tgui::Button::Ptr biliardoChiusoSxBtn{tgui::Button::create("Biliardo\nchiuso Sx")};

  // bottone e campi per modificare il biliardo
  tgui::HorizontalLayout::Ptr newBiliardoWrapper{tgui::HorizontalLayout::create()};
  tgui::VerticalLayout::Ptr leftNewBiliardoWrapper{tgui::VerticalLayout::create()};
  tgui::Label::Ptr r1Label{tgui::Label::create("r1:")};
  tgui::EditBox::Ptr r1Input{tgui::EditBox::create()};
  tgui::Label::Ptr r2Label{tgui::Label::create("r2:")};
  tgui::EditBox::Ptr r2Input{tgui::EditBox::create()};
  tgui::VerticalLayout::Ptr rightNewBiliardoWrapper{tgui::VerticalLayout::create()};
  tgui::Label::Ptr lLabel{tgui::Label::create("l:")};
  tgui::EditBox::Ptr lInput{tgui::EditBox::create()};
  tgui::Button::Ptr newBiliardoBtn{tgui::Button::create("Conferma")};

  // bottone e campi per lanciare la singola particella
  tgui::HorizontalLayout::Ptr singleLaunchWrapper{tgui::HorizontalLayout::create()};
  tgui::VerticalLayout::Ptr heightWrapper{tgui::VerticalLayout::create()};
  tgui::Label::Ptr heightLabel{tgui::Label::create("Altezza:")};
  tgui::EditBox::Ptr heightInput{tgui::EditBox::create()};
  tgui::VerticalLayout::Ptr angleWrapper{tgui::VerticalLayout::create()};
  tgui::Label::Ptr angleLabel{tgui::Label::create("Angolo (rad):")};
  tgui::EditBox::Ptr angleInput{tgui::EditBox::create()};
  tgui::VerticalLayout::Ptr singleLaunchBtnWrapper{tgui::VerticalLayout::create()};
  tgui::Button::Ptr singleLaunchBtn{tgui::Button::create("Lancio singolo")};

  // bottoni per navigare tra i singoli lanci
  tgui::HorizontalLayout::Ptr navigateLaunchesWrapper{tgui::HorizontalLayout::create()};
  tgui::Button::Ptr nextLaunchBtn{tgui::Button::create("→")};
  tgui::Button::Ptr previousLaunchBtn{tgui::Button::create("←")};
  tgui::Button::Ptr pauseBtn{tgui::Button::create("▶")};
  tgui::Button::Ptr reRunBtn{tgui::Button::create("⟳")};

  // bottone e campi per lanciare N particelle
  tgui::HorizontalLayout::Ptr multipleLaunchWrapper{tgui::HorizontalLayout::create()};
  tgui::VerticalLayout::Ptr leftMultipleLaunchWrapper{tgui::VerticalLayout::create()};
  tgui::Label::Ptr muYLabel{tgui::Label::create("µy:")};
  tgui::EditBox::Ptr muYInput{tgui::EditBox::create()};
  tgui::Label::Ptr sigmaYLabel{tgui::Label::create("σy:")};
  tgui::EditBox::Ptr sigmaYInput{tgui::EditBox::create()};
  tgui::Label::Ptr muTLabel{tgui::Label::create("µθ:")};
  tgui::EditBox::Ptr muTInput{tgui::EditBox::create()};
  tgui::Label::Ptr sigmaTLabel{tgui::Label::create("σθ:")};
  tgui::EditBox::Ptr sigmaTInput{tgui::EditBox::create()};
  tgui::VerticalLayout::Ptr rightMultipleLaunchWrapper{tgui::VerticalLayout::create()};
  tgui::Label::Ptr numberLabel{tgui::Label::create("Numero:")};
  tgui::EditBox::Ptr numberInput{tgui::EditBox::create()};
  tgui::Button::Ptr multipleLaunchBtn{tgui::Button::create("Lancio multiplo")};

  // bottoni per navigare tra gli istogrammi e salvarli
  tgui::HorizontalLayout::Ptr navigateHistogramsWrapper{tgui::HorizontalLayout::create()};
  tgui::Button::Ptr nextHistogramBtn{tgui::Button::create("→")};
  tgui::Button::Ptr previousHistogramBtn{tgui::Button::create("←")};
  tgui::Button::Ptr saveHistogramBtn{tgui::Button::create("Salva\nGrafico")};

  // area di testo per i dati statistici
  tgui::HorizontalLayout::Ptr textWrapper{tgui::HorizontalLayout::create()};
  //  tgui::VerticalLayout::Ptr leftTextWrapper{tgui::VerticalLayout::create()};
  tgui::TextArea::Ptr leftText{tgui::TextArea::create()};
  tgui::TextArea::Ptr rightText{tgui::TextArea::create()};

  Gui(sf::RenderWindow& window);

  void create();
  void style();
  void activate(App* app);
  void setDefaultText();
  void setSingleLaunchText(const std::vector<double>& launch);
  void setStatisticsText(const std::array<TH1D, 2>& histograms);
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_GUI_HPP_
