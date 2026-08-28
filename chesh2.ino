/*
  chesh2.ino
  Entry point for CHESH2 - Realistic Code-Generated Digital Eye
  Minimal, modular structure. See README.md for wiring and library config.
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

  Display.init();
  Eye.init();
  Animation.init();
  WiFiAP.init();
  WebServerSetup.init();

  lastMillis = millis();
}

void loop() {
  unsigned long now = millis();
  unsigned long dt = now - lastMillis;
  lastMillis = now;

  WiFiAP.handle();
  WebServerSetup.handle();

  Animation.update(dt);
  Eye.update(dt);
  Display.loop();
}
