#include "display.h"

DisplayClass::DisplayClass() : tft_instance(), brightness(255) {}

void DisplayClass::init() {
  // TFT_eSPI requires a proper User_Setup.h in the library configured for GC9A01 and the pins.
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
  // If backlight control pin exists, implement PWM here.
}

// Define the global instance
DisplayClass display;
