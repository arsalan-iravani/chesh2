#include "animation.h"
#include "eye.h"
#include <Arduino.h>

AnimationClass animation;

void AnimationClass::init() {
  // schedule first blink
  nextBlinkIn = 2000 + (random(0,4000));
  blinkTimer = 0;
}

void AnimationClass::update(unsigned long dt) {
  // gaze logic
  if (autoGaze) {
    static unsigned long gazeChange = 0;
    gazeChange += dt;
    if (gazeChange > 1200) {
      gazeChange = 0;
      targetGazeX = (random(-100,101))/100.0f;
      targetGazeY = (random(-60,61))/100.0f;
    }
  }
  // interpolate
  float speed = 0.01f * dt; // tuned
  gazeX += (targetGazeX - gazeX) * speed;
  gazeY += (targetGazeY - gazeY) * speed;
  eye.setGaze(gazeX, gazeY);

  // blinking
  blinkTimer += dt;
  if (!blinkingNow && blinkTimer > nextBlinkIn && blinking) {
    blinkingNow = true;
    blinkPhase = 0;
  }
  if (blinkingNow) {
    // blink phases: closing (0-150), closed (150-220), opening (220-370)
    blinkPhase += dt;
    if (blinkPhase < 150) {
      float t = blinkPhase / 150.0f;
      eyelidOpen = 1.0f - t;
    } else if (blinkPhase < 220) {
      eyelidOpen = 0.0f;
    } else if (blinkPhase < 370) {
      float t = (blinkPhase - 220) / 150.0f;
      eyelidOpen = t;
    } else {
      blinkingNow = false;
      blinkTimer = 0;
      nextBlinkIn = 2000 + random(0,5000);
      eyelidOpen = 1.0f;
    }
  }
}

void AnimationClass::setAutoGaze(bool v) { autoGaze = v; }
void AnimationClass::setBlinking(bool v) { blinking = v; }
void AnimationClass::setMicroSaccades(bool v) { micro = v; }
void AnimationClass::setTears(bool v) { tearsFlag = v; }
void AnimationClass::setPupilAnim(bool v) { pupilAnim = v; }

float AnimationClass::getEyelidOpenness() { return eyelidOpen; }
