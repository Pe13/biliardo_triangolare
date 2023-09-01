//
// Created by paolo on 29/08/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_GUI_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_GUI_HPP_

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Core.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/TextArea.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>

namespace bt {

class App;

struct Gui {
  tgui::Gui gui;

  tgui::VerticalLayout::Ptr wrapper{tgui::VerticalLayout::create()};

  // bottoni per cambiare tipo di biliardo
  tgui::HorizontalLayout::Ptr biliardoButtonsWrapper{tgui::HorizontalLayout::create()};
  tgui::Button::Ptr biliardoApertoBtn{tgui::Button::create("Biliardo aperto")};
  tgui::Button::Ptr biliardoChiusoDxBtn{tgui::Button::create("Biliardo chiuso Dx")};
  tgui::Button::Ptr biliardoChiusoSxBtn{tgui::Button::create("Biliardo chiuso Sx")};

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

  // bottoni per navigare tra i singoli lanci
  tgui::HorizontalLayout::Ptr navigateWrapper{tgui::HorizontalLayout::create()};
  tgui::Button::Ptr nextLaunchBtn{tgui::Button::create("→")};
  tgui::Button::Ptr previousLaunchBtn{tgui::Button::create("←")};
  tgui::Button::Ptr pauseBtn{tgui::Button::create("▶")};
  tgui::Button::Ptr reRunBtn{tgui::Button::create("⟳")};

  // area di testo per i dati statistici
  tgui::HorizontalLayout::Ptr textWrapper{tgui::HorizontalLayout::create()};
//  tgui::VerticalLayout::Ptr leftTextWrapper{tgui::VerticalLayout::create()};
  tgui::TextArea::Ptr leftText{tgui::TextArea::create()};
  tgui::TextArea::Ptr rightText{tgui::TextArea::create()};

  
  void create();
  void activate(App* app);
  void setSingleLaunchText(const std::vector<double>& launch);
  // TODO void setStatisticsText();

  Gui(sf::RenderWindow &window);
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_GUI_HPP_
