// animation.cpp
#include "animation.h"
#include "eye.h"
#include "util.h"
#include <Arduino.h>

AnimationClass animation;

void AnimationClass::init() {
  randomSeed(analogRead(A0) ^ micros());
  startGazeX = targetGazeX = 0.0f;
  startGazeY = targetGazeY = 0.0f;
  gazeStart = millis();
  gazeDuration = 800;
  nextBlinkAt = millis() + 1400 + random(0, 4800);
  eyelidOpen = 1.0f;
}

void AnimationClass::update(unsigned long dt) {
  unsigned long now = millis();

  // Auto gaze interpolation
  if (_autoGaze) {
    if ((now - gazeStart) >= gazeDuration) {
      startGazeX = targetGazeX; startGazeY = targetGazeY;
      targetGazeX = randf(-0.85f, 0.85f);
      targetGazeY = randf(-0.55f, 0.55f);
      gazeStart = now;
      gazeDuration = 600 + random(0, 1600);
    }
    float t = (float)(now - gazeStart) / (float)gazeDuration;
    t = clampf(t, 0.0f, 1.0f);
    float et = easeInOutCubic(t);
    float gx = lerp(startGazeX, targetGazeX, et);
    float gy = lerp(startGazeY, targetGazeY, et);

    // micro-saccades impulses occasionally
    if (_micro && (now - lastMicro) > (300 + random(0,800))) {
      lastMicro = now;
      microX = randf(-0.03f, 0.03f);
      microY = randf(-0.02f, 0.02f);
    }
    microX *= 0.88f; microY *= 0.88f;
    eye.setGaze(gx + microX, gy + microY);
  }

  // blinking state machine
  if (_blinking) {
    if (!blinkingNow && now >= nextBlinkAt) {
      blinkingNow = true;
      blinkStart = now;
      blinkDuration = 120 + random(0, 260);
    }
    if (blinkingNow) {
      unsigned long elapsed = now - blinkStart;
      float phase = (float)elapsed / (float)blinkDuration;
      if (phase < 0.5f) {
        eyelidOpen = 1.0f - easeInOutCubic(phase * 2.0f);
      } else if (phase < 1.0f) {
        eyelidOpen = easeInOutCubic((phase - 0.5f) * 2.0f);
      } else {
        eyelidOpen = 1.0f;
        blinkingNow = false;
        nextBlinkAt = now + 1800 + random(0, 5200);
      }
    }
  }
}

void AnimationClass::setAutoGaze(bool v) { _autoGaze = v; }
void AnimationClass::setBlinking(bool v) { _blinking = v; }
void AnimationClass::setMicroSaccades(bool v) { _micro = v; }
void AnimationClass::setTears(bool v) { _tears = v; }
void AnimationClass::setPupilAnim(bool v) { _pupilAnim = v; }

float AnimationClass::getEyelidOpenness() { return eyelidOpen; }
