//
// Created by paolo on 03/01/2025.
//

#include "App.hpp"
#include "Biliardo.hpp"
#include "doctest.h"
#include "testsUtilities.hpp"

TEST_CASE("Testing App::modifyBiliardo") {
  for (unsigned int i = 0; i < 3; i++) {
    bt::App app(150, 2, 10, static_cast<bt::BiliardoType>(i), settings);

    SUBCASE("Valid modification") {
      REQUIRE(app.modifyBiliardo(6, 14, 24) == true);

      // Testo che il biliardo sia stato modificato correttamente
      // Non uso l'operatore == tra biliardi per avere un riscontro più dettagliato nel caso
      // qualcosa dovesse andare storto
      CHECK(app.biliardo().l() == 6);
      CHECK(app.biliardo().r1() == 14);
      CHECK(app.biliardo().r2() == 24);
      CHECK(doctest::Approx(app.biliardo().slope()) ==
            (app.biliardo().r2() - app.biliardo().r1()) / app.biliardo().l());
      CHECK(doctest::Approx(app.biliardo().theta()) == std::atan(app.biliardo().slope()));
      CHECK(app.biliardo().type() == static_cast<bt::BiliardoType>(i));

      // Controllo che tutti i lanci siano stati generati correttamente
      // Lanci singoli
      for (long unsigned int j = 0; j < 3; j++) {
        CHECK(app.getSingleLaunches()[j].size() == 1);
        CHECK(app.getSingleLaunchesIndexes()[j] == 0);
      }
      // Lanci multipli
      for (long unsigned int j = 0; j < 3; j++) {
        CHECK(app.getMultipleLaunches()[j].size() == 0);
        CHECK(app.getSingleLaunchesIndexes()[j] == 0);
      }

      // Controllo che sia stato chiamato App::reRun
      checkReRunWasCalled(app);
    }

    SUBCASE("Invalid modifications") {
      const auto biliardoCopy = app.biliardo();
      const auto actualDesignerStep = app.designer().step();

      // Controllo che un qualsiasi valore negativo faccia fallire la modifica
      CHECK_FALSE(app.modifyBiliardo(-1, 1, 1));
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      CHECK_FALSE(app.modifyBiliardo(1, -1, 1));
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      CHECK_FALSE(app.modifyBiliardo(1, 1, -1));
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      // Controllo che anche con più valori negativi la modifica fallisca
      CHECK_FALSE(app.modifyBiliardo(1, -1, -1));
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      // Controllo che un qualsiasi valore nullo faccia fallire la modifica
      CHECK_FALSE(app.modifyBiliardo(0, 1, 1));
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      CHECK_FALSE(app.modifyBiliardo(1, 0, 1));
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      CHECK_FALSE(app.modifyBiliardo(1, 1, 0));
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == actualDesignerStep);

      // Controllo che anche con più valori nulli la modifica fallisca
      CHECK_FALSE(app.modifyBiliardo(0, 1, 0));
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == actualDesignerStep);
    }
  }
}

TEST_CASE("Testing App::changeBiliardoType") {
  SUBCASE("Valid BiliardoType") {
    for (unsigned int i = 0; i < 3; i++) {
      const auto originalBiliardoType = static_cast<bt::BiliardoType>(i);

      for (unsigned int j = 0; j < 3; j++) {
        bt::App app(150, 2, 10, originalBiliardoType, settings);
        auto biliardoCopy = app.biliardo();
        const auto newBiliardoType = static_cast<bt::BiliardoType>(j);
        const auto originalStep = app.designer().step();

        [[maybe_unused]] const bool biliardoCopyChangeTypeResult =
            biliardoCopy.changeType(newBiliardoType);
        assert(biliardoCopyChangeTypeResult);

        CHECK(app.changeBiliardoType(newBiliardoType) == true);
        CHECK(app.biliardo() == biliardoCopy);

        if (i == j) {
          CHECK(app.designer().step() == originalStep);
        } else {
          checkReRunWasCalled(app);
        }
      }
    }
  }

  SUBCASE("Invalid BiliardoType") {
    constexpr auto invalidBiliardoType = static_cast<bt::BiliardoType>(3);
    for (unsigned int i = 0; i < 3; i++) {
      const auto originalBiliardoType = static_cast<bt::BiliardoType>(i);
      bt::App app(150, 2, 10, originalBiliardoType, settings);
      const auto biliardoCopy = app.biliardo();
      const auto originalStep = app.designer().step();

      CHECK(app.changeBiliardoType(invalidBiliardoType) == false);
      CHECK(app.biliardo() == biliardoCopy);
      CHECK(app.designer().step() == originalStep);
    }
  }
}