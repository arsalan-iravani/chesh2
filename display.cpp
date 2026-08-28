#include "display.h"

static TFT_eSPI tft_instance = TFT_eSPI();
static uint8_t brightness = 255;

namespace Display {

void init() {
  // TFT_eSPI requires a proper User_Setup.h in the library configured for GC9A01 and the pins.
  tft_instance.init();
  tft_instance.setRotation(0);
  tft_instance.fillScreen(TFT_BLACK);
}

void loop() {
  // Placeholder for future tasks
}

TFT_eSPI &tft() { return tft_instance; }

void setBrightness(uint8_t b) {
  brightness = b;
  // If backlight control pin exists, implement PWM here. Not all modules expose backlight pin.
}

}
