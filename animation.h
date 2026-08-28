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

  // gaze
  float gazeX = 0.0f, gazeY = 0.0f;
  float startGazeX = 0.0f, startGazeY = 0.0f;
  float targetGazeX = 0.0f, targetGazeY = 0.0f;
  unsigned long gazeStart = 0;
  unsigned long gazeDuration = 1000;

  // eyelids/blink
  float eyelidOpen = 1.0f;
  bool blinkingNow = false;
  unsigned long blinkStart = 0;
  unsigned long blinkDuration = 300; // ms total
  unsigned long nextBlinkAt = 0;

  // micro-saccades
  unsigned long lastMicro = 0;
  float microX = 0.0f, microY = 0.0f;
};

extern AnimationClass animation;
