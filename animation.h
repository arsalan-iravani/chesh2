// animation.h
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
  bool _autoGaze = true;
  bool _blinking = true;
  bool _micro = true;
  bool _tears = false;
  bool _pupilAnim = true;

  float startGazeX = 0.0f, startGazeY = 0.0f;
  float targetGazeX = 0.0f, targetGazeY = 0.0f;
  unsigned long gazeStart = 0;
  unsigned long gazeDuration = 900;

  unsigned long lastMicro = 0;
  float microX = 0.0f, microY = 0.0f;

  float eyelidOpen = 1.0f;
  bool blinkingNow = false;
  unsigned long blinkStart = 0;
  unsigned long blinkDuration = 200;
  unsigned long nextBlinkAt = 0;
};

extern AnimationClass animation;
