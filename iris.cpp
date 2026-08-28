// iris.cpp
#include "iris.h"
#include "util.h"
#include <math.h>
#include <stdlib.h>

void IrisClass::init(uint32_t seed) {
  // seed from provided value or analog noise
  _seed = seed ? seed : (uint32_t)(micros() ^ analogRead(A0));
}

void IrisClass::setBaseColor(uint8_t r, uint8_t g, uint8_t b) {
  _r = r; _g = g; _b = b;
  if (_cache) {
    int s = _cacheSize;
    freeCache();
    // best-effort regenerate
    generateCache(s);
  }
}

float IrisClass::smoothstep(float a, float b, float x) {
  if (x <= a) return 0.0f;
  if (x >= b) return 1.0f;
  x = (x - a) / (b - a);
  return x * x * (3.0f - 2.0f * x);
}

uint16_t IrisClass::sampleColor(int cx, int cy, int x, int y, int radius) {
  int dx = x - cx;
  int dy = y - cy;
  float r = sqrtf((float)dx * dx + (float)dy * dy);
  if (r > radius) return rgbTo565(200,200,200);

  float rn = r / (float)radius;
  float theta = atan2f((float)dy, (float)dx);

  // multi-scale fiber field
  float fibers = 0.0f;
  fibers += 0.6f * sinf(theta * 18.0f + (float)(_seed & 0xFF) * 0.001f);
  fibers += 0.3f * sinf(theta * 44.0f - rn * 8.0f);
  fibers += 0.12f * sinf(theta * 88.0f + rn * 20.0f);

  // crypts/dark pockets: deterministic hash-derived pattern
  uint32_t h = ((uint32_t)((int)(theta*100.0f)) * 374761393u) ^ ((uint32_t)((int)(rn*100.0f)) * 668265263u) ^ _seed;
  h = (h ^ (h >> 13)) * 1274126177u;
  float spot = (float)(h & 0xFFFF) / 65535.0f;
  float crypt = (spot - 0.5f) * 0.36f * smoothstep(0.05f, 0.9f, 1.0f - fabsf(rn - 0.45f));

  // rings
  float rings = 0.16f * sinf(rn * 30.0f + (_seed & 0xFF));

  float tex = fibers * (1.0f - rn*0.7f) + rings + crypt;

  // base luminance
  float lum = 0.65f + 0.5f * (1.0f - rn);
  lum += 0.16f * tex;

  float outerDark = smoothstep(0.78f, 1.0f, rn);
  float innerDark = 1.0f - smoothstep(0.0f, 0.16f, rn);

  int rr = constrain((int)(_r * lum * (1.0f - 0.32f * outerDark) * (0.78f + 0.22f*innerDark)), 0, 255);
  int gg = constrain((int)(_g * lum * (1.0f - 0.32f * outerDark) * (0.78f + 0.22f*innerDark)), 0, 255);
  int bb = constrain((int)(_b * lum * (1.0f - 0.32f * outerDark) * (0.78f + 0.22f*innerDark)), 0, 255);

  if (spot > 0.988f && rn < 0.7f) {
    rr = min(255, rr + 20); gg = min(255, gg + 20); bb = min(255, bb + 20);
  }

  return rgbTo565((uint8_t)rr, (uint8_t)gg, (uint8_t)bb);
}

bool IrisClass::generateCache(int N) {
  freeCache();
  if (N < 32) N = 32;
  if (N > 220) N = 220;

  uint16_t *buf = (uint16_t*)malloc(sizeof(uint16_t) * N * N);
  if (!buf) {
    // allocation failed
    _cache = nullptr;
    _cacheSize = 0;
    _cacheRadius = 0;
    return false;
  }

  _cache = buf;
  _cacheSize = N;
  _cacheRadius = N/2;
  int cx = _cacheRadius;
  int cy = _cacheRadius;
  int radius = _cacheRadius - 1;

  for (int y = 0; y < N; ++y) {
    for (int x = 0; x < N; ++x) {
      int dx = x - cx;
      int dy = y - cy;
      if (dx*dx + dy*dy <= radius*radius) {
        _cache[y*N + x] = sampleColor(cx, cy, x, y, radius);
      } else {
        // neutral background color to avoid transparent artifacts on blit
        _cache[y*N + x] = rgbTo565(200,200,200);
      }
    }
  }
  return true;
}

void IrisClass::freeCache() {
  if (_cache) {
    free(_cache);
    _cache = nullptr;
  }
  _cacheSize = 0;
  _cacheRadius = 0;
}

IrisClass irisGen;
