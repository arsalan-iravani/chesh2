#include "iris.h"
#include <math.h>
#include <Arduino.h>

static inline uint16_t rgbTo565_local(uint8_t r, uint8_t g, uint8_t b) {
  return (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

void IrisClass::init(uint32_t seed) {
  irisSeed = seed ? seed : 987654321;
}

void IrisClass::setBaseColor(uint8_t r, uint8_t g, uint8_t b) {
  baseR = r; baseG = g; baseB = b;
  // regenerate cache if present
  if (cacheBuf) {
    generateCache(cacheSz);
  }
}

float IrisClass::smoothstep(float a, float b, float x) {
  if (x <= a) return 0.0f;
  if (x >= b) return 1.0f;
  x = (x - a) / (b - a);
  return x * x * (3.0f - 2.0f * x);
}

uint16_t IrisClass::sampleColorPolar(int cx, int cy, int x, int y, int radius) {
  int dx = x - cx;
  int dy = y - cy;
  float r = sqrtf((float)dx*dx + (float)dy*dy);
  if (r > radius) return rgbTo565_local(200,200,200); // outside iris; light gray placeholder

  float rn = r / (float)radius; // 0..1
  float theta = atan2f((float)dy, (float)dx);

  // multi-scale radial features
  float fibers = 0.0f;
  fibers += 0.8f * sinf(theta * 18.0f + irisSeed * 0.0001f);
  fibers += 0.35f * sinf(theta * 46.0f - rn * 8.0f + irisSeed * 0.00007f);
  fibers += 0.12f * sinf(theta * 96.0f + rn * 30.0f);

  // random localized crypto/crypts: use hash on angle sectors
  uint32_t h = ((uint32_t)((int)(theta*1000.0f)) * 374761393u) ^ ((uint32_t)((int)(rn*1000.0f)) * 668265263u) ^ irisSeed;
  h = (h ^ (h >> 13)) * 1274126177u;
  float spot = (float)(h & 0xFFFF) / 65535.0f;
  float crypt = (spot - 0.5f) * 0.4f * smoothstep(0.2f, 0.9f, 1.0f - fabsf(rn - 0.5f));

  // radial rings with decay
  float rings = 0.2f * sinf(rn * 30.0f + irisSeed * 0.0003f) + 0.08f * sinf(rn * 60.0f);

  // combine texture
  float tex = fibers * (1.0f - rn*0.6f) + rings + crypt;

  // base luminance by radius: slightly darker near pupil & outer
  float lum = 0.6f + 0.4f * (1.0f - rn);
  lum += 0.18f * tex;

  // color mixing across tones
  // define darker outer ring and inner ring tint
  float outerDark = smoothstep(0.75f, 1.0f, rn);
  float innerDark = 1.0f - smoothstep(0.0f, 0.2f, rn);

  int rcol = (int)(baseR * lum * (1.0f - 0.25f * outerDark) * (0.8f + 0.2f*innerDark));
  int gcol = (int)(baseG * lum * (1.0f - 0.25f * outerDark) * (0.8f + 0.2f*innerDark));
  int bcol = (int)(baseB * lum * (1.0f - 0.25f * outerDark) * (0.8f + 0.2f*innerDark));

  // small bright highlight streaks
  if (spot > 0.98f && rn < 0.6f) {
    rcol = min(255, rcol + 20);
    gcol = min(255, gcol + 20);
    bcol = min(255, bcol + 20);
  }

  // clamp
  rcol = constrain(rcol, 0, 255);
  gcol = constrain(gcol, 0, 255);
  bcol = constrain(bcol, 0, 255);

  return rgbTo565_local((uint8_t)rcol, (uint8_t)gcol, (uint8_t)bcol);
}

bool IrisClass::generateCache(int cacheSize) {
  // free existing
  freeCache();
  if (cacheSize <= 0) return false;
  // limit cacheSize to reasonable value
  if (cacheSize > 200) cacheSize = 200;
  cacheBuf = (uint16_t*)malloc(sizeof(uint16_t) * cacheSize * cacheSize);
  if (!cacheBuf) {
    cacheSz = 0;
    cacheRadiusVal = 0;
    return false;
  }
  cacheSz = cacheSize;
  cacheRadiusVal = cacheSize / 2;

  int cx = cacheRadiusVal;
  int cy = cacheRadiusVal;
  int radius = cacheRadiusVal - 1;

  for (int y = 0; y < cacheSize; ++y) {
    for (int x = 0; x < cacheSize; ++x) {
      int dx = x - cx;
      int dy = y - cy;
      if (dx*dx + dy*dy <= radius*radius) {
        cacheBuf[y*cacheSize + x] = sampleColorPolar(cx, cy, x, y, radius);
      } else {
        cacheBuf[y*cacheSize + x] = rgbTo565_local(200,200,200); // placeholder transparent-like
      }
    }
  }
  return true;
}

void IrisClass::freeCache() {
  if (cacheBuf) {
    free(cacheBuf);
    cacheBuf = nullptr;
  }
  cacheSz = 0;
  cacheRadiusVal = 0;
}

// define global instance
IrisClass irisGen;
