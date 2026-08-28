#pragma once
#include <Arduino.h>
#include "expression.h"

class EyeClass {
public:
  void init();
  void update(unsigned long dt);
  void setExpression(Expression e);
  void setGaze(float vx, float vy); // normalized -1..1
  void setPupil(float t); // 0..1
  void setColor(uint8_t r, uint8_t g, uint8_t b);
private:
  int cx = 120, cy = 120;
  int eyeRadius = 110;
  float gazeX = 0.0f, gazeY = 0.0f;
  float pupilT = 0.3f;
  void drawSclera(TFT_eSPI &tft);
  void drawIrisAndPupil(TFT_eSPI &tft);
  void drawEyelids(TFT_eSPI &tft);
  uint16_t rgbTo565(uint8_t r, uint8_t g, uint8_t b);
};

extern EyeClass eye;
