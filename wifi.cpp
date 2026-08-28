// wifi.cpp
#include "wifi.h"
#include <ESP8266WiFi.h>
#include <Arduino.h>

WiFiAPClass wifiAP;

void WiFiAPClass::init() {
  const char* ssid = "RealisticEye";
  const char* pass = "12345678";
  Serial.println("Starting WiFi AP...");
  WiFi.mode(WIFI_AP);
  IPAddress local(192,168,4,1);
  IPAddress gw(192,168,4,1);
  IPAddress sn(255,255,255,0);
  WiFi.softAPConfig(local, gw, sn);
  bool ok = WiFi.softAP(ssid, pass);
  if (!ok) {
    Serial.println("softAP failed");
  } else {
    Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());
  }
}

void WiFiAPClass::handle() {
  // reserved for future (reconnect checks, DNS, captive portal)
}
