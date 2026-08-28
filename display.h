#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

class DisplayClass {
public:
  DisplayClass();
  void init();
  void loop();
  TFT_eSPI &tft();
  void setBrightness(uint8_t b);
private:
  TFT_eSPI tft_instance;
  uint8_t brightness;
};

extern DisplayClass display;
