#include "web.h"
#include "display.h"
#include "eye.h"
#include "animation.h"
#include <ESP8266WebServer.h>
#include "webpage.h"
#include <Arduino.h>
#include <functional>

WebServerClass webServer;

class WebServerImpl {
public:
  ESP8266WebServer server{80};
  void begin() {
    server.on("/", std::bind(&WebServerImpl::handleRoot, this));
    server.onNotFound(std::bind(&WebServerImpl::handleNotFound, this));
    server.on("/setColor", std::bind(&WebServerImpl::handleSetColor, this));
    server.on("/setGaze", std::bind(&WebServerImpl::handleSetGaze, this));
    server.on("/setExpression", std::bind(&WebServerImpl::handleSetExpression, this));
    server.on("/toggle", std::bind(&WebServerImpl::handleToggle, this));
    server.begin();
    Serial.println("Web server started");
  }
  void handleClient() { server.handleClient(); }
private:
  void handleRoot() {
    server.send_P(200, "text/html", webpage);
  }
  void handleNotFound() {
    server.send(404, "text/plain", "Not found");
  }
  void handleSetColor() {
    if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
      int r = server.arg("r").toInt();
      int g = server.arg("g").toInt();
      int b = server.arg("b").toInt();
      eye.setColor(r,g,b);
      server.send(200, "application/json", "{\"ok\":true}\n");
    } else {
      server.send(400, "application/json", "{\"ok\":false,\"error\":\"missing params\"}\n");
    }
  }
  void handleSetGaze() {
    if (server.hasArg("x") && server.hasArg("y")) {
      float x = server.arg("x").toFloat();
      float y = server.arg("y").toFloat();
      eye.setGaze(x,y);
      server.send(200, "application/json", "{\"ok\":true}\n");
    } else {
      server.send(400, "application/json", "{\"ok\":false}\n");
    }
  }
  void handleSetExpression() {
    if (server.hasArg("e")) {
      int e = server.arg("e").toInt();
      if (e >= 0 && e <= 7) {
        eye.setExpression((Expression)e);
        server.send(200, "application/json", "{\"ok\":true}\n");
      } else {
        server.send(400, "application/json", "{\"ok\":false}\n");
      }
    } else {
      server.send(400, "application/json", "{\"ok\":false}\n");
    }
  }
  void handleToggle() {
    // /toggle?auto=1&blink=0&micro=1
    if (server.hasArg("auto")) animation.setAutoGaze(server.arg("auto").toInt()!=0);
    if (server.hasArg("blink")) animation.setBlinking(server.arg("blink").toInt()!=0);
    if (server.hasArg("micro")) animation.setMicroSaccades(server.arg("micro").toInt()!=0);
    server.send(200, "application/json", "{\"ok\":true}\n");
  }
};

static WebServerImpl g_serverImpl;

void WebServerClass::init() {
  g_serverImpl.begin();
}

void WebServerClass::handle() {
  g_serverImpl.handleClient();
}
