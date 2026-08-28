// iris.h
#pragma once
#include <Arduino.h>
#include <stdint.h>

class IrisClass {
public:
  void init(uint32_t seed = 0);
  void setBaseColor(uint8_t r, uint8_t g, uint8_t b);
  bool generateCache(int N); // create NxN RGB565 cache
  void freeCache();
  const uint16_t* cache() const { return _cache; }
  int cacheSize() const { return _cacheSize; }
  int cacheRadius() const { return _cacheRadius; }
  uint16_t sampleColor(int cx, int cy, int x, int y, int radius);
private:
  uint8_t _r=80,_g=120,_b=140;
  uint32_t _seed = 0xACE123;
  uint16_t *_cache = nullptr;
  int _cacheSize = 0;
  int _cacheRadius = 0;
  float smoothstep(float a, float b, float x);
  static inline uint16_t rgbTo565(uint8_t r, uint8_t g, uint8_t b) {
    return (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
  }
};

extern IrisClass irisGen;
