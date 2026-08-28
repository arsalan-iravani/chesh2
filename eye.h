#pragma once
#include <Arduino.h>
#include "expression.h"
#include <TFT_eSPI.h>

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
  float pupilT = 0.35f;
  float eyelidOpen = 1.0f; // 0 closed, 1 open
  Expression currentExpression = EXP_NORMAL;

  void drawSclera(TFT_eSPI &tft);
  void drawIrisAndPupil(TFT_eSPI &tft);
  void drawEyelids(TFT_eSPI &tft);
  void drawHighlights(TFT_eSPI &tft, int icx, int icy, int irisR);
  uint16_t blend(uint16_t bg, uint16_t fg, uint8_t alpha); // alpha 0-255
};

extern EyeClass eye;
