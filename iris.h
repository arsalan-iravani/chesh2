#pragma once
#include <Arduino.h>
#include <stdint.h>
#include "expression.h"

namespace IrisGen {
  void init(uint32_t seed = 0);
  // baseColor: RGB 0-255
  void setBaseColor(uint8_t r, uint8_t g, uint8_t b);
  uint16_t sampleColorPolar(int cx, int cy, int x, int y, int radius);
}
