//
// Created by paolo on 04/01/2025.
//

#ifndef TESTSUTILITIES_HPP
#define TESTSUTILITIES_HPP

#include "App.hpp"
#include "doctest.h"

inline void checkReRunWasCalled(const bt::App& app) {
  CHECK(app.designer().isDrawing() == true);
  CHECK(app.designer().isPaused() == false);
  CHECK(app.designer().hasCleared() == false);
  CHECK(app.designer().pointIndex() == 0);
  CHECK(app.designer().pathFraction() == 0.f);
  CHECK(app.designer().step() != 0.f);
  CHECK(app.designer().contrail().size() == 0);
}

static const sf::ContextSettings settings{0, 0, 8, 4, 2, sf::ContextSettings::Default, false};

#endif  // TESTSUTILITIES_HPP
