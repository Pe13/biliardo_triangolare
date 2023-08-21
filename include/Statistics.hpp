//
// Created by paolo on 27/07/2023.
//

#ifndef BILIARDO_TRIANGOLARE_INCLUDE_STATISTICS_HPP_
#define BILIARDO_TRIANGOLARE_INCLUDE_STATISTICS_HPP_

#include <TCanvas.h>
#include <TH1.h>
#include <TImage.h>
#include <TROOT.h>
#include <TVirtualX.h>

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "Designer.hpp"

namespace bt {

inline void saveCanvasOnImage(sf::Image &image, TCanvas *canvas) {
  if (gVirtualX->InheritsFrom("TGCocoa") && !gROOT->IsBatch() && canvas->GetCanvas() &&
      canvas->GetCanvas()->GetCanvasID() != -1) {
    canvas->Flush();

    const UInt_t w = canvas->GetWw();
    const UInt_t h = canvas->GetWh();

    const unsigned char *pixelData = gVirtualX->GetColorBits(canvas->GetCanvasID(), 0, 0, w, h);

    if (pixelData) {
      image.create(w, h, pixelData);
    }
    delete pixelData;
  } else {
    TImage *img = TImage::Create();
    if (img) {
      img->FromPad(canvas);
    }
    //    std::cout << img->Print("immagine.png") << '\n';
    image.create(img->GetWidth(), img->GetHeight(), reinterpret_cast<sf::Uint8 *>(img->GetRgbaArray()));
    delete img;
  }
}

inline void graph(int width, int height, double r1, const std::vector<double> &input, Designer &designer) {
  auto *canvas = new TCanvas("canvas", "canvas", width, height);
  canvas->Divide(2);

  TH1D yHisto("yhisto", "yHisto", 1000, -r1, r1);
  TH1D thetaHisto("thetaHisto", "thetaHisto", 1000, -M_PI / 2, M_PI / 2);

  for (size_t i = 0; i < input.size(); i += 2) {
    yHisto.Fill(input[i]);
    thetaHisto.Fill(input[i + 1]);
  }

  canvas->cd(1);
  yHisto.Draw();

  canvas->cd(2);
  thetaHisto.Draw();

  ////////////////////////////////////////////////////////////////////////
  /// part of Root library code assuming batch mode to create an image
  /// from a TPad (lines 1092-1117 TASImage.cxx Root 6.26)
  //  TVirtualPS *psave = gVirtualPS;
  //  gVirtualPS = (TVirtualPS *)gROOT->ProcessLineFast("new TImageDump()");
  //  gVirtualPS->Open(gPad->GetName(), 114);  // in memory
  //  gVirtualPS->SetBit(BIT(11));             // kPrintingPS
  //
  //  auto *itmp = (TASImage *)gVirtualPS->GetStream();
  //
  //  TVirtualPad *sav = gPad;
  //  gPad->Paint();
  //  gPad = sav;
  //
  //  auto yPixelArr = itmp->GetRgbaArray();  // questa è mia
  //
  //  delete gVirtualPS;
  //  gVirtualPS = psave;
  ////////////////////////////////////////////////////////////////////////

  sf::Image histoImage;
  saveCanvasOnImage(histoImage, canvas);
  designer.histoTexture_.loadFromImage(histoImage);
  designer.histoTexture_.setSmooth(true);
  designer.histoSprite_.setTexture(designer.histoTexture_);

  delete canvas;
}

}  // namespace bt

#endif  // BILIARDO_TRIANGOLARE_INCLUDE_STATISTICS_HPP_
