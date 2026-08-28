// eye.h
#pragma once
#include <Arduino.h>
#include "expression.h"

class EyeClass {
public:
  void init();
  void update(unsigned long dt);
  void setExpression(Expression e);
  void setGaze(float x, float y); // -1..1
  void setPupil(float t); // 0..1
  void setColor(uint8_t r, uint8_t g, uint8_t b);
private:
  int _cx = 120, _cy = 120;
  int _eyeRadius = 110;
  float _gazeX = 0.0f, _gazeY = 0.0f;
  float _pupilT = 0.35f;
  Expression _expression = EXP_NORMAL;

  void drawSclera();
  void drawIrisAndPupil();
  void drawHighlights(int icx, int icy, int irisR);
  void drawEyelids();
  void drawTears();
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
};

extern EyeClass eye;
