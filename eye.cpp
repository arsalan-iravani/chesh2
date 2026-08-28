#include "eye.h"
#include "display.h"
#include "iris.h"
#include "animation.h"
#include <TFT_eSPI.h>
#include <math.h>

EyeClass eye;

void EyeClass::init() {
  irisGen.init(analogRead(A0));
  irisGen.setBaseColor(80,120,140);
  // initial draw
  drawSclera(display.tft());
  drawIrisAndPupil(display.tft());
  drawEyelids(display.tft());
}

void EyeClass::setExpression(Expression e) {
  // Reserved: animation handles expression parameters for now
}

void EyeClass::setGaze(float vx, float vy) {
  gazeX = constrain(vx, -1.0f, 1.0f);
  gazeY = constrain(vy, -1.0f, 1.0f);
}

void EyeClass::setPupil(float t) {
  pupilT = constrain(t, 0.0f, 1.0f);
}

void EyeClass::drawSclera(TFT_eSPI &tft) {
  // simple sclera gradient (optimized by drawing concentric circles)
  for (int r = eyeRadius; r > 0; --r) {
    float k = 1.0f - (float)r / (float)eyeRadius;
    uint8_t v = (uint8_t)constrain(230 - k*30, 180, 255);
    uint16_t c = tft.color565(v,v,v);
    tft.drawCircle(cx, cy, r, c);
  }
}

void EyeClass::drawIrisAndPupil(TFT_eSPI &tft) {
  int irisR = eyeRadius * 0.45; // iris size
  // compute gaze pixel offset
  int maxOffset = eyeRadius - irisR - 6;
  int ox = (int)(gazeX * maxOffset);
  int oy = (int)(gazeY * maxOffset * 0.7f);
  int icx = cx + ox;
  int icy = cy + oy;

  // draw iris by sampling polar coordinates inside radius
  tft.startWrite();
  for (int y = icy - irisR; y <= icy + irisR; ++y) {
    for (int x = icx - irisR; x <= icx + irisR; ++x) {
      int dx = x - icx;
      int dy = y - icy;
      if (dx*dx + dy*dy <= irisR*irisR) {
        uint16_t col = irisGen.sampleColorPolar(icx, icy, x, y, irisR);
        tft.drawPixel(x, y, col);
      }
    }
  }

  // pupil
  int pupilR = (int)(irisR * (0.12f + 0.45f * (1.0f - pupilT)));
  // dark pupil with subtle border
  tft.fillCircle(icx, icy, pupilR, 0x0000);
  // subtle pupil border
  tft.drawCircle(icx, icy, pupilR+1, tft.color565(30,30,30));
  tft.endWrite();
}

void EyeClass::drawEyelids(TFT_eSPI &tft) {
  // Use animation parameters to draw arcs representing eyelids
  float open = animation.getEyelidOpenness(); // 0 closed, 1 open
  // upper eyelid curve (simple mask for now)
  int lidOffset = (int)((1.0f - open) * 60.0f);
  int y = cy - 40 + lidOffset;
  tft.fillRect(0, 0, 240, y, tft.color565(30,20,20)); // simple block to simulate eyelid
  // lower eyelid
  int ly = cy + 40 - (int)((1.0f - open) * 20.0f);
  tft.fillRect(0, ly, 240, 240-ly, tft.color565(30,20,20));
}

void EyeClass::update(unsigned long dt) {
  TFT_eSPI &tft = display.tft();
  // redraw whole screen each frame for now
  tft.startWrite();
  drawSclera(tft);
  drawIrisAndPupil(tft);
  drawEyelids(tft);
  tft.endWrite();
}

void EyeClass::setColor(uint8_t r, uint8_t g, uint8_t b) {
  irisGen.setBaseColor(r,g,b);
}

uint16_t EyeClass::rgbTo565(uint8_t r, uint8_t g, uint8_t b) {
  return display.tft().color565(r,g,b);
}
