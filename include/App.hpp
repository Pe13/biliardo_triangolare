//
// Created by paolo on 23/05/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Core.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <array>
#include <vector>

#include "BiliardoAperto.hpp"
#include "Designer.hpp"

namespace bt {

class App {
  Biliardo* biliardo_;

  Designer designer_{};

  sf::RenderWindow window_{{1280, 720}, "Biliardo trinagolare", sf::Style::Default};
  sf::Event event_{};

  tgui::Gui gui_{window_};

  tgui::VerticalLayout::Ptr wrapper_{tgui::VerticalLayout::create()};

  // bottoni per cambiare tipo di biliardo
  tgui::HorizontalLayout::Ptr biliardoButtonsWrapper_{tgui::HorizontalLayout::create()};
  tgui::Button::Ptr biliardoApertoBtn_{tgui::Button::create("Biliardo aperto")};
  tgui::Button::Ptr biliardoChiusoDxBtn_{tgui::Button::create("Biliardo chiuso Dx")};
  tgui::Button::Ptr biliardoChiusoSxBtn_{tgui::Button::create("Biliardo chiuso Sx")};

  // bottone e campi per modificare il biliardo
  tgui::HorizontalLayout::Ptr newBiliardoWrapper_{tgui::HorizontalLayout::create()};
  tgui::VerticalLayout::Ptr newBiliardoInputWrapper_{tgui::VerticalLayout::create()};
  tgui::Label::Ptr r1Label_{tgui::Label::create("r1:")};
  tgui::EditBox::Ptr r1Input_{tgui::EditBox::create()};
  tgui::Label::Ptr r2Label_{tgui::Label::create("r2:")};
  tgui::EditBox::Ptr r2Input_{tgui::EditBox::create()};
  tgui::Label::Ptr lLabel_{tgui::Label::create("l:")};
  tgui::EditBox::Ptr lInput_{tgui::EditBox::create()};
  tgui::Button::Ptr newBiliardoBtn_{tgui::Button::create("Conferma")};

  // bottone e campi per lanciare la singola particella
  tgui::HorizontalLayout::Ptr singleLaunchWrapper_{tgui::HorizontalLayout::create()};
  tgui::VerticalLayout::Ptr singleLaunchInputWrapper_{tgui::VerticalLayout::create()};
  tgui::Label::Ptr heightLabel_{tgui::Label::create("Altezza:")};
  tgui::EditBox::Ptr heightInput_{tgui::EditBox::create()};
  tgui::Label::Ptr angleLabel_{tgui::Label::create("Angolo:")};
  tgui::EditBox::Ptr angleInput_{tgui::EditBox::create()};
  tgui::Button::Ptr singleLaunchBtn_{tgui::Button::create("Lancio singolo")};

  // bottone e campi per lanciare N particelle
  tgui::HorizontalLayout::Ptr multipleLaunchWrapper_{tgui::HorizontalLayout::create()};
  tgui::VerticalLayout::Ptr multipleLaunchLeftWrapper_{tgui::VerticalLayout::create()};
  tgui::Label::Ptr muYLabel_{tgui::Label::create("µy:")};
  tgui::EditBox::Ptr muYInput_{tgui::EditBox::create()};
  tgui::Label::Ptr sigmaYLabel_{tgui::Label::create("σy:")};
  tgui::EditBox::Ptr sigmaYInput_{tgui::EditBox::create()};
  tgui::Label::Ptr muTLabel_{tgui::Label::create("µθ:")};
  tgui::EditBox::Ptr muTInput_{tgui::EditBox::create()};
  tgui::Label::Ptr sigmaTLabel_{tgui::Label::create("σθ:")};
  tgui::EditBox::Ptr sigmaTInput_{tgui::EditBox::create()};
  tgui::VerticalLayout::Ptr multipleLaunchRightWrapper_{tgui::VerticalLayout::create()};
  tgui::Label::Ptr numberLabel_{tgui::Label::create("Numero:")};
  tgui::EditBox::Ptr numberInput_{tgui::EditBox::create()};
  tgui::Button::Ptr multipleLaunchBtn_{tgui::Button::create("Lancio multiplo")};

  // bottoni per navigare tra i singoli lanci
  tgui::HorizontalLayout::Ptr navigateWrapper_{tgui::HorizontalLayout::create()};
  tgui::Button::Ptr nextLaunchBtn_{tgui::Button::create(L"→")};
  tgui::Button::Ptr previousLaunchBtn_{tgui::Button::create(L"←")};
  tgui::Button::Ptr pauseBtn_{tgui::Button::create(L"▶")};
  tgui::Button::Ptr reRunBtn_{tgui::Button::create(L"⟳")};

  std::array<std::vector<std::vector<double>>, 3> singleLaunches_;
  std::array<std::vector<double>, 3> multipleLaunches_{{}};
  std::array<std::vector<sf::Image>, 3> graphImages{{}};

  void createGui();
  void activateGui();
  void handleEvents();
  void handeKeyboardEvents(sf::Keyboard::Key key);
  void changeBiliardo(BiliardoType type);

 public:
  App(double l, double r1, double r2);
  ~App();

  void run();
};

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_APP_HPP_
