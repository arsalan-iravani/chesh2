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
  // fast pixel copy wrapper
  void pushImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *data);
private:
  TFT_eSPI tft_instance;
  uint8_t brightness;
};

extern DisplayClass display;
