// util.h - small inline helpers
#pragma once
#include <Arduino.h>
#include <math.h>

inline float clampf(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }
inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
inline float easeInOutCubic(float t) { return t < 0.5f ? 4.0f*t*t*t : 1.0f - powf(-2.0f*t + 2.0f, 3.0f)/2.0f; }
inline float randf(float a, float b) { return a + ((float)random(1000)/1000.0f) * (b-a); }
