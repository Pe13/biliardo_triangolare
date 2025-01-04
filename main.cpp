#include <TGUI/Core.hpp>
#include <SFML/Window/ContextSettings.hpp>

#include "App.hpp"
#include "Pool.hpp"

int main() {
  tgui::Theme::setDefault("Black.txt");
  const sf::ContextSettings settings{0, 0, 8, 4, 2, sf::ContextSettings::Default, false};
  bt::App app{170, 2, 10, bt::open, settings};
  app.start();
  return 0;
}
