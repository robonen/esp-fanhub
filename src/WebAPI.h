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
  
  // Request body handling (static buffer)
  static char bodyBuf[512];
  static size_t bodyLen;
  
  // File serving
  static const char* getContentType(const char* path);
  static bool serveFile(AsyncWebServerRequest* req, const String& path);
  
  // API handlers
  static void handleStatus(AsyncWebServerRequest* req);
  static void handleCurvePoints(AsyncWebServerRequest* req);
  static void handleSetDuty(AsyncWebServerRequest* req);
  static void handleSetMode(AsyncWebServerRequest* req);
  static void handleSetCurvePoints(AsyncWebServerRequest* req);
  
  // Body parsing
  static void onBody(AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total);
  static int parseIntParam(const char* body, const char* key);
  static float parseFloatParam(const char* body, const char* key);
  static const char* parseStrParam(const char* body, const char* key, char* out, size_t outLen);
};

extern WebAPI webApi;
