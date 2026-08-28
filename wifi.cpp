#include "wifi.h"

namespace WiFiAP {

void init() {
  const char *ssid = "ParaxEye";
  const char *password = "12345678";
  Serial.println("Starting WiFi AP...");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void handle() {
  // nothing for now
}

}
