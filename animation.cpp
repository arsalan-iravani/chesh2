#include "animation.h"
#include "eye.h"
#include <Arduino.h>

AnimationClass animation;

static float randf(float a, float b) { return a + (float)random(1000)/1000.0f * (b-a); }

void AnimationClass::init() {
  randomSeed(analogRead(A0) ^ micros());
  // schedule initial gaze
  targetGazeX = 0.0f; targetGazeY = 0.0f;
  startGazeX = 0.0f; startGazeY = 0.0f;
  gazeStart = millis();
  gazeDuration = 800;

  nextBlinkAt = millis() + 2000 + random(0,4000);
}

void AnimationClass::update(unsigned long dt) {
  unsigned long now = millis();
  // Gaze: handle transitions
  if (autoGaze) {
    if (now - gazeStart >= gazeDuration) {
      // pick a new target
      startGazeX = targetGazeX;
      startGazeY = targetGazeY;
      targetGazeX = randf(-0.9f, 0.9f);
      targetGazeY = randf(-0.6f, 0.6f);
      gazeStart = now;
      gazeDuration = 600 + random(0, 1600);
    }
    float t = (float)(now - gazeStart) / (float)gazeDuration;
    t = clampf(t, 0.0f, 1.0f);
    float et = easeInOutCubic(t);
    float gx = lerp(startGazeX, targetGazeX, et);
    float gy = lerp(startGazeY, targetGazeY, et);
    // micro-saccades: small quick impulses
    if (micro && now - lastMicro > 300 + random(0,800)) {
      lastMicro = now;
      microX = randf(-0.03f, 0.03f);
      microY = randf(-0.02f, 0.02f);
    }
    float microDecay = 0.9f;
    microX *= microDecay; microY *= microDecay;
    eye.setGaze(gx + microX, gy + microY);
  }

  // blinking state machine
  if (blinking) {
    if (!blinkingNow && now >= nextBlinkAt) {
      blinkingNow = true;
      blinkStart = now;
      blinkDuration = 180 + random(0,220); // total ms
    }
    if (blinkingNow) {
      unsigned long elapsed = now - blinkStart;
      float phase = (float)elapsed / (float)blinkDuration;
      if (phase < 0.5f) {
        eyelidOpen = 1.0f - easeInOutCubic(phase*2.0f);
      } else if (phase < 1.0f) {
        eyelidOpen = easeInOutCubic((phase-0.5f)*2.0f);
      } else {
        eyelidOpen = 1.0f;
        blinkingNow = false;
        nextBlinkAt = now + 2000 + random(0,6000);
      }
    }
  }
}

void AnimationClass::setAutoGaze(bool v) { autoGaze = v; }
void AnimationClass::setBlinking(bool v) { blinking = v; }
void AnimationClass::setMicroSaccades(bool v) { micro = v; }
void AnimationClass::setTears(bool v) { tearsFlag = v; }
void AnimationClass::setPupilAnim(bool v) { pupilAnim = v; }

float AnimationClass::getEyelidOpenness() { return eyelidOpen; }
