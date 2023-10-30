#include <TGUI/Core.hpp>
#include <SFML/Window/ContextSettings.hpp>

#include "App.hpp"
#include "types.hpp"

int main() {
  tgui::Theme::setDefault("Black.txt");
  const sf::ContextSettings settings{0, 0, 8, 4, 2, sf::ContextSettings::Default, false};
  bt::App app{100, 30, 10, bt::open, settings};
  app.start();
  return 0;
}
