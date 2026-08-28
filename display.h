// display.h
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
  void pushImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *data);
  // hardware test sequence: fill colors and show text to verify wiring
  void testSequence();
private:
  TFT_eSPI _tft;
  uint8_t _brightness;
};

extern DisplayClass display;
