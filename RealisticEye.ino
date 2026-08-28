// RealisticEye.ino
// Entrypoint for the RealisticEye project

#include <Arduino.h>
#include "display.h"
#include "iris.h"
#include "eye.h"
#include "animation.h"
#include "wifi.h"
#include "web.h"

unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println();
  Serial.println("RealisticEye starting...");

  // Initialize hardware and services
  display.init();   // initializes TFT
  eye.init();       // generates iris cache, draws initial frame
  animation.init(); // initialize animation state
  wifiAP.init();    // start soft AP
  webServer.init(); // start web server

  lastMillis = millis();
}

void loop() {
  unsigned long now = millis();
  unsigned long dt = now - lastMillis;
  lastMillis = now;

  // Keep network responsive
  wifiAP.handle();
  webServer.handle();

  // Update animations and render changes (non-blocking)
  animation.update(dt);
  eye.update(dt);

  // Any display housekeeping (currently none)
  display.loop();
}
