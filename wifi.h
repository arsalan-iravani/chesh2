// wifi.h
#pragma once
#include <Arduino.h>

class WiFiAPClass {
public:
  void init();
  void handle();
};
extern WiFiAPClass wifiAP;
