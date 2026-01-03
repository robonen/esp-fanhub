#pragma once
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFi.h>
#include "Config.h"
#include "FanController.h"

class WebAPI {
public:
  void begin();
  
private:
  AsyncWebServer server{80};
  
  // File serving
  static const char* getContentType(const char* path);
  static bool serveFile(AsyncWebServerRequest* req, const String& path);
  
  // API handlers
  static void handleStatus(AsyncWebServerRequest* req);
  static void handleCurvePoints(AsyncWebServerRequest* req);
  static void handleSetDuty(AsyncWebServerRequest* req);
  static void handleSetMode(AsyncWebServerRequest* req);
  static void handleSetCurvePoints(AsyncWebServerRequest* req);
};

extern WebAPI webApi;
