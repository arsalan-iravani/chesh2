#include "animation.h"
#include "eye.h"
#include "expression.h"
#include <Arduino.h>

static bool autoGaze = true;
static bool blinking = true;
static bool micro = true;
static bool tears = false;
static bool pupilAnim = true;

// simple state
static float gazeX = 0.0f, gazeY = 0.0f;
static float targetGazeX = 0.0f, targetGazeY = 0.0f;
static float eyelidOpen = 1.0f;
static unsigned long blinkTimer = 0;
static unsigned long nextBlinkIn = 3000;
static bool blinkingNow = false;
static unsigned long blinkPhase = 0;

namespace Animation {

void init() {
  // schedule first blink
  nextBlinkIn = 2000 + (random(0,4000));
  blinkTimer = 0;
}

void update(unsigned long dt) {
  // gaze logic
  if (autoGaze) {
    // slowly move target occasionally
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
  Eye::setGaze(gazeX, gazeY);

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

  // set eyelid parameter somewhere accessible
}

void setAutoGaze(bool v) { autoGaze = v; }
void setBlinking(bool v) { blinking = v; }
void setMicroSaccades(bool v) { micro = v; }
void setTears(bool v) { tears = v; }
void setPupilAnim(bool v) { pupilAnim = v; }

float getEyelidOpenness() { return eyelidOpen; }

}
