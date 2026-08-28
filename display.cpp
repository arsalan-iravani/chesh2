#include "display.h"

DisplayClass::DisplayClass() : tft_instance(), brightness(255) {}

void DisplayClass::init() {
  tft_instance.init();
  tft_instance.setRotation(0);
  tft_instance.fillScreen(TFT_BLACK);
}

void DisplayClass::loop() {
  // Placeholder for future display tasks (backlight PWM, etc.)
}

TFT_eSPI &DisplayClass::tft() { return tft_instance; }

void DisplayClass::setBrightness(uint8_t b) {
  brightness = b;
}

void DisplayClass::pushImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *data) {
  // Use TFT_eSPI's pushImage if available
  tft_instance.pushImage(x, y, w, h, data);
}

// Define the global instance
DisplayClass display;
