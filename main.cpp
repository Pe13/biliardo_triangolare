#include <TGUI/Core.hpp>

#include "App.hpp"
#include "types.hpp"

int main() {
  tgui::Theme::setDefault("Black.txt");
  bt::App app{100, 30, 10, bt::open};
  app.start();
  return 0;
}
