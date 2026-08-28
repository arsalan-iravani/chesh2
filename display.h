#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

namespace Display {
  void init();
  void loop();
  TFT_eSPI &tft();
  void setBrightness(uint8_t b);
}
