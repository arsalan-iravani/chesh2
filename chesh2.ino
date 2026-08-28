/*
  chesh2.ino
  Entry point for CHESH2 - Realistic Code-Generated Digital Eye
*/

#include <Arduino.h>
#include "display.h"
#include "eye.h"
#include "animation.h"
#include "wifi.h"
#include "web.h"

unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("CHESH2 starting...");

  // Initialize modules (global instances)
  display.init();
  eye.init();
  animation.init();
  wifiAP.init();
  webServer.init();

  lastMillis = millis();
}

void loop() {
  unsigned long now = millis();
  unsigned long dt = now - lastMillis;
  lastMillis = now;

  // Handle services
  wifiAP.handle();
  webServer.handle();

  // Update animations and rendering
  animation.update(dt);
  eye.update(dt);
  display.loop();
}
