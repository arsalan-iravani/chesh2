#include "web.h"
#include "display.h"
#include "eye.h"
#include "animation.h"
#include <ESP8266WebServer.h>
#include "webpage.h"

static ESP8266WebServer server(80);

namespace WebServerSetup {

static void handleRoot() {
  server.send_P(200, "text/html", webpage);
}

static void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

static void handleSetColor() {
  if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
    int r = server.arg("r").toInt();
    int g = server.arg("g").toInt();
    int b = server.arg("b").toInt();
    Eye::setColor(r,g,b);
    server.send(200, "application/json", "{\"ok\":true}\n");
  } else {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"missing params\"}\n");
  }
}

static void handleSetExpression() {
  if (server.hasArg("e")) {
    int e = server.arg("e").toInt();
    Animation::setBlinking(true);
    server.send(200, "application/json", "{\"ok\":true}\n");
  } else {
    server.send(400, "application/json", "{\"ok\":false}\n");
  }
}

static void handleSetGaze() {
  if (server.hasArg("x") && server.hasArg("y")) {
    float x = server.arg("x").toFloat();
    float y = server.arg("y").toFloat();
    Eye::setGaze(x,y);
    server.send(200, "application/json", "{\"ok\":true}\n");
  } else {
    server.send(400, "application/json", "{\"ok\":false}\n");
  }
}

void init() {
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.on("/setColor", handleSetColor);
  server.on("/setGaze", handleSetGaze);
  server.begin();
  Serial.println("Web server started");
}

void handle() {
  server.handleClient();
}

}
