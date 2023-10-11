//
// Created by paolo on 11/10/2023.
//

#define CORRECT CHECK(format(s) == s.toLower())
#define WRONG CHECK(format(s) == "")

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "Gui.hpp"

#include <TGUI/String.hpp>
//#include <TGUI/Widgets/EditBox.hpp>

TEST_CASE("Testing the string parser for the user input") {
  using String = tgui::String;
  using namespace bt;

  String s;

  s = "156378";
  CORRECT;

  s = "-7382";
  CORRECT;

  s = "+43789";
  CORRECT;

  s = "3478-43";
  WRONG;

  s = "483974.74983";
  CORRECT;

  s = "4839.4389.29";
  WRONG;

  s = "4839..98320.8239";
  WRONG;

  s = "4839,,98320,8239";
  WRONG;

  s = "231e11";
  CORRECT;

  s = "3E4";
  CORRECT;

  s = "3e5e3";
  WRONG;

  s = "4e+4";
  CORRECT;

  s = "-4e+4";
  CORRECT;

  s = "  62 374  ";
  CHECK(format(s) == "62374");

  s = "100'000'000";
  CHECK(format(s) == "100000000");

  s = "423,3";
  CHECK(format(s) == "423.3");

  s = "a3829";
  WRONG;

}