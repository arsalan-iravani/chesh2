// eye.cpp
#include "eye.h"
#include "display.h"
#include "iris.h"
#include "animation.h"
#include "util.h"
#include <TFT_eSPI.h>
#include <math.h>

EyeClass eye;

// Default iris cache size (tune for memory). We'll try larger, fallback if malloc fails.
#ifndef IRIS_CACHE_TRY
#define IRIS_CACHE_TRY 112
#endif

static inline uint16_t make565(uint8_t r, uint8_t g, uint8_t b) {
  return (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

void EyeClass::init() {
  irisGen.init(analogRead(A0));
  // attempt to generate the cache at the desired size with graceful fallback
  if (!irisGen.generateCache(IRIS_CACHE_TRY)) {
    Serial.println("Iris cache allocation failed at primary size; trying 96");
    if (!irisGen.generateCache(96)) {
      Serial.println("Iris cache fallback to 96 failed; trying 80");
      irisGen.generateCache(80);
    }
  }
  // default eye color: bluish
  irisGen.setBaseColor(80,120,180);
  // initial draw cycle
  update(0);
}

void EyeClass::setExpression(Expression e) {
  _expression = e;
}

void EyeClass::setGaze(float x, float y) {
  _gazeX = clampf(x, -1.0f, 1.0f);
  _gazeY = clampf(y, -1.0f, 1.0f);
}

void EyeClass::setPupil(float t) {
  _pupilT = clampf(t, 0.0f, 1.0f);
}

void EyeClass::setColor(uint8_t r, uint8_t g, uint8_t b) {
  irisGen.setBaseColor(r,g,b);
  // regenerate cache handled by irisGen
}

uint16_t EyeClass::color565(uint8_t r, uint8_t g, uint8_t b) {
  return make565(r,g,b);
}

void EyeClass::drawSclera() {
  TFT_eSPI &tft = display.tft();

  int rmax = _eyeRadius;

  // radial gradient by concentric circles (cheap)
  for (int r = rmax; r > 0; --r) {
    float k = 1.0f - (float)r / (float)rmax;
    uint8_t rc = (uint8_t)constrain(244 - k*20, 220, 255);
    uint8_t gc = (uint8_t)constrain(240 - k*24, 200, 255);
    uint8_t bc = (uint8_t)constrain(238 - k*28, 190, 255);
    uint16_t col = color565(rc,gc,bc);
    tft.drawCircle(_cx, _cy, r, col);
  }

  // lightweight veins: 3 small faint strokes/dots
  tft.drawLine(_cx - 28, _cy + 18, _cx - 20, _cy + 12, color565(220,60,60));
  tft.drawLine(_cx + 36, _cy - 16, _cx + 24, _cy - 8, color565(230,90,90));
  tft.drawPixel(_cx - 8, _cy + 32, color565(200,70,70));
}

void EyeClass::drawHighlights(int icx, int icy, int irisR) {
  TFT_eSPI &tft = display.tft();

  // Soft highlight: draw multiple concentric light ellipses approximated with circles
  int hx = icx - irisR/3;
  int hy = icy - irisR/3;
  for (int i = 0; i < 5; ++i) {
    int r = (irisR/6) - i;
    if (r <= 0) break;
    // produce a lighter color but not pure white, to simulate soft reflection
    uint8_t v = 255 - i * 28;
    uint8_t rr = (uint8_t)min(255, (int)v);
    uint16_t col = color565(rr, rr, (uint8_t)min(255, rr + 8));
    tft.fillCircle(hx, hy, r, col);
  }

  // sharp specular
  tft.fillCircle(icx + irisR/4, icy - irisR/4, max(1, irisR/16), color565(255,255,255));
}

void EyeClass::drawIrisAndPupil() {
  TFT_eSPI &tft = display.tft();
  int irisR = (int)(_eyeRadius * 0.48f);

  // gaze offset inside allowable range
  int maxOffset = _eyeRadius - irisR - 6;
  int ox = (int)(_gazeX * maxOffset);
  int oy = (int)(_gazeY * maxOffset * 0.7f);
  int icx = _cx + ox;
  int icy = _cy + oy;

  // draw cached iris if available
  const uint16_t *cache = irisGen.cache();
  int csz = irisGen.cacheSize();
  int cr = irisGen.cacheRadius();
  if (cache && csz > 0) {
    int x0 = icx - cr;
    int y0 = icy - cr;
    display.pushImage(x0, y0, csz, csz, cache);
  } else {
    // fallback: sample per pixel (slow)
    for (int y = icy - irisR; y <= icy + irisR; ++y) {
      for (int x = icx - irisR; x <= icx + irisR; ++x) {
        int dx = x - icx; int dy = y - icy;
        if (dx*dx + dy*dy <= irisR*irisR) {
          uint16_t col = irisGen.sampleColor(icx, icy, x, y, irisR);
          tft.drawPixel(x,y,col);
        }
      }
    }
  }

  // pupil
  int pupilR = (int)(irisR * (0.12f + 0.6f * (1.0f - _pupilT)));
  // layered pupil fill to suggest depth
  tft.fillCircle(icx, icy, pupilR+2, color565(6,6,6));
  tft.fillCircle(icx, icy, pupilR, color565(0,0,0));
  tft.drawCircle(icx, icy, pupilR+1, color565(12,12,12));

  int sx = icx - pupilR/2;
  int sy = icy - pupilR/2;
  tft.fillCircle(sx, sy, pupilR + 6, color565(6,6,6));
  tft.fillCircle(icx, icy, pupilR, color565(0,0,0));

  // highlights
  drawHighlights(icx, icy, irisR);
}

void EyeClass::drawEyelids() {
  TFT_eSPI &tft = display.tft();
  float open = animation.getEyelidOpenness();

  // expression-based bias to openness
  float bias = 0.0f;
  switch (_expression) {
    case EXP_HAPPY: bias = 0.08f; break;
    case EXP_SAD: bias = -0.18f; break;
    case EXP_ANGRY: bias = -0.22f; break;
    case EXP_SURPRISED: bias = 0.28f; break;
    case EXP_SLEEPY: bias = -0.36f; break;
    case EXP_CONFUSED: bias = 0.0f; break;
    case EXP_CRYING: bias = -0.22f; break;
    default: bias = 0.0f;
  }
  open = clampf(open + bias, 0.0f, 1.0f);

  int lidRange = 60;
  int lidOffset = (int)((1.0f - open) * lidRange);

  // upper parabola
  float a = 0.00085f * (20 + lidOffset);
  int apex = _cy - 30 + lidOffset;
  for (int x = 0; x < 240; ++x) {
    float dx = x - _cx;
    int y = (int)(a * dx * dx + apex);
    y = constrain(y, 0, 239);
    tft.drawFastVLine(x, 0, y, color565(34,26,26));
  }

  // lower parabola
  float b = -0.0006f * (20 + lidOffset);
  int apex2 = _cy + 30 - (lidOffset / 3);
  for (int x = 0; x < 240; ++x) {
    float dx = x - _cx;
    int y = (int)(b * dx * dx + apex2);
    y = constrain(y, 0, 239);
    tft.drawFastVLine(x, y, 240 - y, color565(34,26,26));
  }

  // thin eyelid shadow band
  for (int x = _cx - 70; x <= _cx + 70; ++x) {
    float dx = x - _cx;
    int y = (int)(a * dx * dx + apex);
    if (y >= 0 && y < 238) {
      tft.fillRect(x, y, 1, 2, color565(10,10,10));
    }
  }
}

void EyeClass::drawTears() {
  if (_expression != EXP_CRYING) return;
  TFT_eSPI &tft = display.tft();
  int irisR = (int)(_eyeRadius * 0.48f);
  int ox = (int)(_gazeX * (_eyeRadius - irisR - 4));
  int oy = (int)(_gazeY * (_eyeRadius - irisR - 4) * 0.7f);
  int icx = _cx + ox;
  int icy = _cy + oy;

  tft.fillEllipse(icx + 10, icy + irisR/2 + 6, 4, 6, color565(200,220,255));
  tft.fillEllipse(icx - 8, icy + irisR/2 + 10, 3, 5, color565(220,230,255));
}

void EyeClass::update(unsigned long dt) {
  TFT_eSPI &tft = display.tft();
  tft.fillScreen(TFT_BLACK);

  drawSclera();
  drawIrisAndPupil();
  drawEyelids();
  drawTears();
}
