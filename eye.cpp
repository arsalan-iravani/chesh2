#include "eye.h"
#include "display.h"
#include "iris.h"
#include "animation.h"
#include <TFT_eSPI.h>

static int cx = 120, cy = 120; // center of round display
static int eyeRadius = 110;
static float gazeX = 0.0f, gazeY = 0.0f;
static float pupilT = 0.3f; // 0..1

namespace Eye {

void init() {
  IrisGen::init(analogRead(A0));
  IrisGen::setBaseColor(80,120,140);
  // initial draw
  displayInitial();
}

void setExpression(Expression e) {
  // For now animation module drives expression parameters; function reserved.
}

void setGaze(float vx, float vy) {
  gazeX = constrain(vx, -1.0f, 1.0f);
  gazeY = constrain(vy, -1.0f, 1.0f);
}

void setPupil(float t) {
  pupilT = constrain(t, 0.0f, 1.0f);
}

static void drawSclera(TFT_eSPI &tft) {
  // simple sclera gradient
  for (int r = eyeRadius; r > 0; --r) {
    float k = 1.0f - (float)r / (float)eyeRadius;
    uint8_t v = (uint8_t)constrain(230 - k*30, 180, 255);
    uint16_t c = ((v & 0xF8) << 8) | ((v & 0xFC) << 3) | (v >> 3);
    tft.drawCircle(cx, cy, r, c);
  }
}

static void drawIrisAndPupil(TFT_eSPI &tft) {
  int irisR = eyeRadius * 0.45; // iris size
  // compute gaze pixel offset
  int maxOffset = eyeRadius - irisR - 6;
  int ox = (int)(gazeX * maxOffset);
  int oy = (int)(gazeY * maxOffset * 0.7f);
  int icx = cx + ox;
  int icy = cy + oy;

  // draw iris by sampling polar coordinates inside radius
  for (int y = icy - irisR; y <= icy + irisR; ++y) {
    for (int x = icx - irisR; x <= icx + irisR; ++x) {
      int dx = x - icx;
      int dy = y - icy;
      if (dx*dx + dy*dy <= irisR*irisR) {
        uint16_t col = IrisGen::sampleColorPolar(icx, icy, x, y, irisR);
        tft.drawPixel(x, y, col);
      }
    }
  }

  // pupil
  int pupilR = (int)(irisR * (0.12f + 0.45f * (1.0f - pupilT)));
  // dark pupil with subtle border
  for (int r = pupilR; r >= 0; --r) {
    uint16_t c = 0x0000;
    tft.fillCircle(icx, icy, r, c);
  }
  // subtle pupil border
  tft.drawCircle(icx, icy, pupilR+1, rgbTo565(30,30,30));
}

static inline uint16_t rgbTo565(uint8_t r, uint8_t g, uint8_t b) {
  return (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

static void drawEyelids(TFT_eSPI &tft) {
  // Use animation parameters to draw arcs representing eyelids
  float open = Animation::getEyelidOpenness(); // 0 closed, 1 open
  // upper eyelid curve
  int lidOffset = (int)((1.0f - open) * 60.0f);
  int y = cy - 40 + lidOffset;
  tft.fillRect(0, 0, 240, y, rgbTo565(30,20,20)); // simple block to simulate eyelid
  // lower eyelid
  int ly = cy + 40 - (int)((1.0f - open) * 20.0f);
  tft.fillRect(0, ly, 240, 240-ly, rgbTo565(30,20,20));
}

void update(unsigned long dt) {
  TFT_eSPI &tft = Display::tft();
  // we will redraw whole screen each frame for simplicity and correctness
  tft.startWrite();
  drawSclera(tft);
  drawIrisAndPupil(tft);
  drawEyelids(tft);
  tft.endWrite();
}

static void displayInitial() {
  TFT_eSPI &tft = Display::tft();
  tft.fillScreen(TFT_BLACK);
  // initial full render
  update(0);
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  IrisGen::setBaseColor(r,g,b);
}

}
