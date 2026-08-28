#pragma once
#include <Arduino.h>

namespace Animation {
  void init();
  void update(unsigned long dt);

  // controls
  void setAutoGaze(bool v);
  void setBlinking(bool v);
  void setMicroSaccades(bool v);
  void setTears(bool v);
  void setPupilAnim(bool v);

  // query
  float getEyelidOpenness();
}
