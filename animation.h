#pragma once
#include <Arduino.h>

class AnimationClass {
public:
  void init();
  void update(unsigned long dt);

  void setAutoGaze(bool v);
  void setBlinking(bool v);
  void setMicroSaccades(bool v);
  void setTears(bool v);
  void setPupilAnim(bool v);

  float getEyelidOpenness();
private:
  bool autoGaze = true;
  bool blinking = true;
  bool micro = true;
  bool tearsFlag = false;
  bool pupilAnim = true;

  float gazeX = 0.0f, gazeY = 0.0f;
  float targetGazeX = 0.0f, targetGazeY = 0.0f;
  float eyelidOpen = 1.0f;
  unsigned long blinkTimer = 0;
  unsigned long nextBlinkIn = 3000;
  bool blinkingNow = false;
  unsigned long blinkPhase = 0;
};

extern AnimationClass animation;
