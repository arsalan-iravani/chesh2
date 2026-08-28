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

  // radial rings: combine sinusoidal rings with decaying amplitude
  float rings = 0.0f;
  rings += 0.5f * sinf(theta * 10.0f + irisSeed*1.0f);
  rings += 0.25f * sinf(theta * 30.0f - rn*12.0f + irisSeed*0.3f);
  // radial fiber variation
  float fibers = sinf((theta*40.0f + rn*120.0f) + cosf(rn*20.0f));

  // darken toward pupil and outer limbal ring
  float pupilFalloff = 1.0f - smoothstep(0.0f, 0.35f, rn);
  float limbFalloff = smoothstep(0.82f, 1.0f, rn);

  // combine
  float lum = 0.5f + 0.3f * rings + 0.15f * fibers - 0.4f*pupilFalloff - 0.6f*limbFalloff;
  // tweak by radial brightness variation
  lum *= (0.7f + 0.6f*(1.0f - rn));

  // random micro speckles using seeded simple hash
  uint32_t h = ((uint32_t)x*73856093u) ^ ((uint32_t)y*19349663u) ^ irisSeed;
  h = (h << 13) ^ h;
  float rnd = (1.0f - ((h * (h * h * 15731u + 789221u) + 1376312589u) & 0x7fffffff) / 1073741824.0f);
  lum += 0.05f * rnd;

  // base color mixing
  int rcol = constrain((int)(baseR * lum), 0, 255);
  int gcol = constrain((int)(baseG * lum), 0, 255);
  int bcol = constrain((int)(baseB * lum), 0, 255);

  // darken near pupil border
  if (rn < 0.18f) {
    float k = (0.18f - rn)/0.18f;
    rcol = rcol * (1.0f - 0.6f*k);
    gcol = gcol * (1.0f - 0.6f*k);
    bcol = bcol * (1.0f - 0.6f*k);
  }

  // add subtle highlight if near upper-left quadrant
  if (rn < 0.9f && theta > -0.7f && theta < 0.2f && rn < 0.6f) {
    // small bright region
    if (rnd > 0.6f && rn < 0.4f) {
      rcol = min(255, rcol + 10);
      gcol = min(255, gcol + 10);
      bcol = min(255, bcol + 10);
    }
  }

  return rgbTo565_local(rcol, gcol, bcol);
}

// Define the global instance
IrisClass irisGen;
