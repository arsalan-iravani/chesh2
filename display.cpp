// display.cpp
#include "display.h"

DisplayClass::DisplayClass() : _tft(), _brightness(255) {}

void DisplayClass::init() {
  // TFT_eSPI must be configured in User_Setup.h for your pins and GC9A01
  _tft.init();
  _tft.setRotation(0); // chosen so center is (120,120)
  _tft.fillScreen(TFT_BLACK);
  Serial.println("Display initialized");
}

void DisplayClass::loop() {
  // Placeholder: backlight PWM or other housekeeping could go here
}

TFT_eSPI &DisplayClass::tft() { return _tft; }

void DisplayClass::setBrightness(uint8_t b) { _brightness = b; }

void DisplayClass::pushImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *data) {
  // TFT_eSPI's pushImage requires a non-const pointer; cast is safe here
  _tft.pushImage(x, y, w, h, (uint16_t*)data);
}

// define global instance
DisplayClass display;
