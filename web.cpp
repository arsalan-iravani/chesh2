// web.cpp
#include "web.h"
#include "eye.h"
#include "animation.h"
#include "iris.h"
#include "display.h"
#include "webpage.h"
#include <ESP8266WebServer.h>
#include <Arduino.h>
#include "util.h"

WebServerClass webServer;

class WebImpl {
public:
  ESP8266WebServer server{80};

  void begin() {
    server.on("/", std::bind(&WebImpl::handleRoot, this));
    server.on("/setColor", std::bind(&WebImpl::handleSetColor, this));
    server.on("/setGaze", std::bind(&WebImpl::handleSetGaze, this));
    server.on("/setExpression", std::bind(&WebImpl::handleSetExpression, this));
    server.on("/setPupil", std::bind(&WebImpl::handleSetPupil, this));
    server.on("/toggle", std::bind(&WebImpl::handleToggle, this));
    server.onNotFound(std::bind(&WebImpl::handleNotFound, this));
    server.begin();
    Serial.println("Web server started on port 80");
  }

  void handleClient() { server.handleClient(); }

private:
  void handleRoot() { server.send_P(200, "text/html", webpage); }
  void handleNotFound() { server.send(404, "text/plain", "Not found"); }

  void handleSetColor() {
    if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
      int r = server.arg("r").toInt(), g = server.arg("g").toInt(), b = server.arg("b").toInt();
      eye.setColor((uint8_t)r, (uint8_t)g, (uint8_t)b);
      int cs = irisGen.cacheSize();
      if (cs <= 0) cs = 96;
      irisGen.generateCache(cs);
      server.send(200, "application/json", "{\"ok\":true}");
      Serial.printf("Color set %d,%d,%d\n", r,g,b);
    } else server.send(400, "application/json", "{\"ok\":false}");
  }

  void handleSetGaze() {
    if (server.hasArg("x") && server.hasArg("y")) {
      float x = server.arg("x").toFloat(), y = server.arg("y").toFloat();
      eye.setGaze(x,y);
      server.send(200, "application/json", "{\"ok\":true}");
    } else server.send(400, "application/json", "{\"ok\":false}");
  }

  void handleSetExpression() {
    if (server.hasArg("e")) {
      int e = server.arg("e").toInt();
      if (e >= 0 && e <= 7) {
        eye.setExpression((Expression)e);
        server.send(200, "application/json", "{\"ok\":true}");
      } else server.send(400, "application/json", "{\"ok\":false}");
    } else server.send(400, "application/json", "{\"ok\":false}");
  }

  void handleSetPupil() {
    if (server.hasArg("t")) {
      int t = server.arg("t").toInt();
      float tf = clampf((float)t / 100.0f, 0.0f, 1.0f);
      eye.setPupil(tf);
      server.send(200, "application/json", "{\"ok\":true}");
    } else server.send(400, "application/json", "{\"ok\":false}");
  }

  void handleToggle() {
    if (server.hasArg("auto")) animation.setAutoGaze(server.arg("auto").toInt() != 0);
    if (server.hasArg("blink")) animation.setBlinking(server.arg("blink").toInt() != 0);
    if (server.hasArg("micro")) animation.setMicroSaccades(server.arg("micro").toInt() != 0);
    server.send(200, "application/json", "{\"ok\":true}");
  }
};

static WebImpl gweb;

void WebServerClass::init() { gweb.begin(); }
void WebServerClass::handle() { gweb.handleClient(); }
