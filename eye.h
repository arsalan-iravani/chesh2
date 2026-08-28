#pragma once
#include <Arduino.h>
#include "expression.h"

namespace Eye {
  void init();
  void update(unsigned long dt);
  void setExpression(Expression e);
  void setGaze(float vx, float vy); // normalized -1..1
  void setPupil(float t); // 0..1
  void setColor(uint8_t r, uint8_t g, uint8_t b);
}
