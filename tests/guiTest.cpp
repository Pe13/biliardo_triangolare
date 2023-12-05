//
// Created by paolo on 11/10/2023.
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "Gui.hpp"

#include <TGUI/String.hpp>

TEST_CASE("Testing the string formatter for the user input") {
  using String = tgui::String;
  using namespace bt;

  auto isCorrect = [](String& s) {
      CHECK(format(s) == s.toLower());
  };
  
  auto isWrong = [](String& s) {
    CHECK(format(s) == "");
  };
  
  String s;

  s = "156378";
  isCorrect(s);

  s = "-7382";
  isCorrect(s);

  s = "+43789";
  isCorrect(s);

  s = "3478-43";
  isWrong(s);

  s = "483974.74983";
  isCorrect(s);

  s = "4839.4389.29";
  isWrong(s);

  s = "4839..98320.8239";
  isWrong(s);

  s = "4839,,98320,8239";
  isWrong(s);

  s = "231e11";
  isCorrect(s);

  s = "3E4";
  isCorrect(s);

  s = "3e5e3";
  isWrong(s);

  s = "35e";
  isWrong(s);

  s = "5e-";
  isWrong(s);

  s = "5E,";
  isWrong(s);

  s = "4e+4";
  isCorrect(s);

  s = "-4e+4";
  isCorrect(s);

  s = "  62 374  ";
  CHECK(format(s) == "62374");

  s = "100'000'000";
  CHECK(format(s) == "100000000");

  s = "423,3";
  CHECK(format(s) == "423.3");

  s = "a3829";
  isWrong(s);

}