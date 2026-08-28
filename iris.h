#pragma once
#include <Arduino.h>
#include <stdint.h>

class IrisClass {
public:
  void init(uint32_t seed = 0);
  void setBaseColor(uint8_t r, uint8_t g, uint8_t b);
  uint16_t sampleColorPolar(int cx, int cy, int x, int y, int radius);
private:
  uint8_t baseR = 80, baseG = 120, baseB = 140;
  uint32_t irisSeed = 12345;
  float smoothstep(float a, float b, float x);
};

extern IrisClass irisGen;
