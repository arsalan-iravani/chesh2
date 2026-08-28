#pragma once

#include <ESP8266WiFi.h>

class WiFiAPClass {
public:
  void init();
  void handle();
};

extern WiFiAPClass wifiAP;
