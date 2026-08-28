#include "eye.h"
#include "display.h"
#include "iris.h"
#include "animation.h"
#include <TFT_eSPI.h>
#include <math.h>

EyeClass eye;

// easing helpers
static float clampf(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }
static float lerp(float a, float b, float t) { return a + (b - a) * t; }
static float easeInOutCubic(float t) { return t < 0.5f ? 4.0f*t*t*t : 1.0f - powf(-2.0f*t + 2.0f, 3.0f)/2.0f; }

void EyeClass::init() {
  irisGen.init(analogRead(A0));
  // generate a 128x128 iris cache
  if (!irisGen.generateCache(128)) {
    // fallback: small cache
    irisGen.generateCache(96);
  }
  irisGen.setBaseColor(80,120,140);
  // initial draw
  update(0);
}

void EyeClass::setExpression(Expression e) {
  currentExpression = e;
}

void EyeClass::setGaze(float vx, float vy) {
  gazeX = clampf(vx, -1.0f, 1.0f);
  gazeY = clampf(vy, -1.0f, 1.0f);
}

void EyeClass::setPupil(float t) {
  pupilT = clampf(t, 0.0f, 1.0f);
}

uint16_t EyeClass::blend(uint16_t bg, uint16_t fg, uint8_t alpha) {
  // simple alpha blend 0-255
  uint8_t r1 = (bg >> 11) & 0x1F; r1 = (r1 << 3) | (r1 >> 2);
  uint8_t g1 = (bg >> 5) & 0x3F; g1 = (g1 << 2) | (g1 >> 4);
  uint8_t b1 = bg & 0x1F; b1 = (b1 << 3) | (b1 >> 2);

  uint8_t r2 = (fg >> 11) & 0x1F; r2 = (r2 << 3) | (r2 >> 2);
  uint8_t g2 = (fg >> 5) & 0x3F; g2 = (g2 << 2) | (g2 >> 4);
  uint8_t b2 = fg & 0x1F; b2 = (b2 << 3) | (b2 >> 2);

  uint8_t ra = (uint8_t)((r2 * alpha + r1 * (255 - alpha)) / 255);
  uint8_t ga = (uint8_t)((g2 * alpha + g1 * (255 - alpha)) / 255);
  uint8_t ba = (uint8_t)((b2 * alpha + b1 * (255 - alpha)) / 255);

  return ((ra & 0xF8) << 8) | ((ga & 0xFC) << 3) | (ba >> 3);
}

void EyeClass::drawSclera(TFT_eSPI &tft) {
  // draw a soft gradient sclera with warm-white center and subtle shading near lids
  int rmax = eyeRadius;
  for (int r = rmax; r > 0; --r) {
    float k = 1.0f - (float)r / (float)rmax;
    // combine warm center and cooler edges
    uint8_t rc = (uint8_t)constrain(250 - k*18, 220, 255);
    uint8_t gc = (uint8_t)constrain(245 - k*22, 200, 255);
    uint8_t bc = (uint8_t)constrain(240 - k*28, 180, 255);
    uint16_t c = tft.color565(rc, gc, bc);
    tft.drawCircle(cx, cy, r, c);
  }
}

void EyeClass::drawHighlights(TFT_eSPI &tft, int icx, int icy, int irisR) {
  // two layered highlights: soft big one and small sharp one
  // main soft highlight (elliptical)
  int hx = icx - irisR/3;
  int hy = icy - irisR/3;
  int hw = irisR/3;
  int hh = irisR/5;
  // draw several translucent ellipses by blending over background
  for (int a = 0; a < 6; ++a) {
    int alpha = 40 - a*6; // decreasing alpha
    if (alpha <= 0) break;
    int w = hw - a*2;
    int h = hh - a*1;
    // draw filled ellipse by scanline approximation
    for (int dy = -h; dy <= h; ++dy) {
      int yy = hy + dy;
      float nx = (float)dy / (float)h;
      float span = w * sqrtf(1.0f - nx*nx);
      int x0 = hx - (int)span;
      int x1 = hx + (int)span;
      for (int x = x0; x <= x1; ++x) {
        uint16_t bg = tft.readPixel(x, yy);
        uint16_t col = tft.color565(255,255,255);
        uint16_t blended = blend(bg, col, alpha);
        tft.drawPixel(x, yy, blended);
      }
    }
  }
  // small specular
  int sx = icx + irisR/4;
  int sy = icy - irisR/4;
  tft.fillCircle(sx, sy, max(1, irisR/16), tft.color565(255,255,255));
}

void EyeClass::drawIrisAndPupil(TFT_eSPI &tft) {
  int irisR = (int)(eyeRadius * 0.48f);

  // compute gaze pixel offset
  int maxOffset = eyeRadius - irisR - 6;
  int ox = (int)(gazeX * maxOffset);
  int oy = (int)(gazeY * maxOffset * 0.7f);
  int icx = cx + ox;
  int icy = cy + oy;

  // draw iris from cache
  const uint16_t* cache = irisGen.cache();
  int csz = irisGen.cacheSize();
  int cr = irisGen.cacheRadius();
  if (cache && csz > 0) {
    // blit centered
    int x0 = icx - cr;
    int y0 = icy - cr;
    // use pushImage (assumes cache is csz x csz)
    display.pushImage(x0, y0, csz, csz, cache);
  } else {
    // fallback sample per pixel
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
  }

  // pupil
  int pupilR = (int)(irisR * (0.12f + 0.45f * (1.0f - pupilT)));
  // draw pupil with slight radial shadow to suggest depth
  for (int r = pupilR; r >= 0; --r) {
    uint8_t shade = (uint8_t)constrain(20 + (pupilR - r) * 2, 0, 255);
    uint16_t c = tft.color565(0,0,0);
    tft.fillCircle(icx, icy, r, c);
  }
  // pupil border
  tft.drawCircle(icx, icy, pupilR+1, tft.color565(20,20,20));

  // subtle pupil shadow on iris (dark crescent)
  int sx = icx - pupilR/2;
  int sy = icy - pupilR/2;
  for (int y = icy - irisR; y <= icy + irisR; ++y) {
    for (int x = icx - irisR; x <= icx + irisR; ++x) {
      int dx = x - sx;
      int dy = y - sy;
      int dist2 = dx*dx + dy*dy;
      int r2 = irisR*irisR;
      if (dist2 < r2) {
        // apply small darkening near shadow area
        if (dist2 < (pupilR+pupilR)*(pupilR+pupilR)) {
          // darker
          uint16_t bg = tft.readPixel(x,y);
          uint16_t dark = blend(bg, tft.color565(0,0,0), 20);
          tft.drawPixel(x,y,dark);
        }
      }
    }
  }

  // highlights
  drawHighlights(tft, icx, icy, irisR);
}

void EyeClass::drawEyelids(TFT_eSPI &tft) {
  // compute eyelid curve positions using eyelidOpen
  float open = animation.getEyelidOpenness(); // 0 closed, 1 open
  // base vertical extents
  int lidRise = (int)((1.0f - open) * 60.0f);
  // upper lid: parabola y = a*(x-cx)^2 + y0
  float a = 0.0008f * (20 + lidRise); // curvature
  int y0 = cy - 30 + lidRise; // apex y
  // fill upper eyelid region
  for (int x = 0; x < 240; ++x) {
    float dx = x - cx;
    int y = (int)(a * dx * dx + y0);
    if (y < 0) y = 0;
    if (y > 239) y = 239;
    // fill above this y
    tft.drawFastVLine(x, 0, y, tft.color565(40,30,30));
  }
  // lower lid: inverted parabola
  float b = -0.0006f * (20 + lidRise);
  int y1 = cy + 30 - (lidRise/3);
  for (int x = 0; x < 240; ++x) {
    float dx = x - cx;
    int y = (int)(b * dx * dx + y1);
    if (y < 0) y = 0;
    if (y > 239) y = 239;
    // fill below this y
    tft.drawFastVLine(x, y, 240-y, tft.color565(38,28,28));
  }
  // eyelid shadow bands near lid edges for depth
  // small band at upper edge
  for (int x = cx - 70; x <= cx + 70; ++x) {
    float dx = x - cx;
    int y = (int)(a * dx * dx + y0);
    int yy = y;
    for (int dy = 0; dy < 4; ++dy) {
      int py = yy + dy;
      if (py >= 0 && py < 240) {
        uint16_t bg = tft.readPixel(x, py);
        uint16_t col = blend(bg, tft.color565(0,0,0), 50);
        tft.drawPixel(x, py, col);
      }
    }
  }
}

void EyeClass::update(unsigned long dt) {
  TFT_eSPI &tft = display.tft();
  // choose to clear background black to increase contrast
  tft.fillScreen(TFT_BLACK);

  drawSclera(tft);
  drawIrisAndPupil(tft);
  drawEyelids(tft);
}

void EyeClass::setColor(uint8_t r, uint8_t g, uint8_t b) {
  irisGen.setBaseColor(r,g,b);
}
