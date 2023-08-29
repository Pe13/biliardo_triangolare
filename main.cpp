#include <TGUI/Core.hpp>

#include "App.hpp"

int main() {
  tgui::Theme::setDefault("Black.txt");
  bt::App app{100, 30, 10};
  app.run();
  return 0;
}
