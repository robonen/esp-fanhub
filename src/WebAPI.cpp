#include "WebAPI.h"

WebAPI webApi;

void WebAPI::begin() {
  // CORS preflight
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PATCH, OPTIONS");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");
  
  // GET endpoints
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    if (!serveFile(req, "/")) req->send(404, "text/plain", "Not found");
  });
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/curvePoints", HTTP_GET, handleCurvePoints);
  
  // OPTIONS preflight for PATCH
  server.on("/api/duty", HTTP_OPTIONS, [](AsyncWebServerRequest* req) { req->send(204); });
  server.on("/api/mode", HTTP_OPTIONS, [](AsyncWebServerRequest* req) { req->send(204); });
  server.on("/api/curvePoints", HTTP_OPTIONS, [](AsyncWebServerRequest* req) { req->send(204); });
  
  // PATCH endpoints - library auto-parses application/x-www-form-urlencoded body
  server.on("/api/duty", HTTP_PATCH, handleSetDuty);
  server.on("/api/mode", HTTP_PATCH, handleSetMode);
  server.on("/api/curvePoints", HTTP_PATCH, handleSetCurvePoints);
  
  server.onNotFound([](AsyncWebServerRequest* req) {
    if (!serveFile(req, req->url())) req->send(404, "text/plain", "Not found");
  });
  
  server.begin();
}

// ─────────────────────────────────────────────────────────────────────────────
// File Serving
// ─────────────────────────────────────────────────────────────────────────────

const char* WebAPI::getContentType(const char* path) {
  if (strstr(path, ".html")) return "text/html";
  if (strstr(path, ".css")) return "text/css";
  if (strstr(path, ".js")) return "application/javascript";
  if (strstr(path, ".json")) return "application/json";
  if (strstr(path, ".png")) return "image/png";
  if (strstr(path, ".jpg")) return "image/jpeg";
  if (strstr(path, ".svg")) return "image/svg+xml";
  if (strstr(path, ".ico")) return "image/x-icon";
  return "application/octet-stream";
}

bool WebAPI::serveFile(AsyncWebServerRequest* req, const String& path) {
  String p = path.endsWith("/") ? path + "index.html" : path;
  
  // Add .gz extension for gzipped files
  String gzPath = p + ".gz";
  
  if (!LittleFS.exists(gzPath)) {
    if (p.startsWith("/api")) return false;
    gzPath = "/index.html.gz";
    if (!LittleFS.exists(gzPath)) return false;
    p = "/index.html";
  }
  
  auto* response = req->beginResponse(LittleFS, gzPath, getContentType(p.c_str()));
  response->addHeader("Cache-Control", "max-age=86400");
  response->addHeader("Content-Encoding", "gzip");
  req->send(response);
  return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// API Handlers
// ─────────────────────────────────────────────────────────────────────────────

void WebAPI::handleStatus(AsyncWebServerRequest* req) {
  static char buf[384];
  float temp = fanCtrl.getTemp();
  const char* ip = WiFi.localIP().toString().c_str();
  int rssi = WiFi.RSSI();
  
  int len = snprintf(buf, sizeof(buf),
    "{\"wifi\":true,\"ip\":\"%s\",\"rssi\":%d,\"tempC\":%.2f,\"fans\":[", ip, rssi, temp);
  
  for (uint8_t i = 0; i < FAN_COUNT; ++i) {
    const auto& f = fanCtrl.getFan(i);
    len += snprintf(buf + len, sizeof(buf) - len,
      "%s{\"index\":%d,\"dutyPct\":%.1f,\"manualDuty\":%.1f,\"manual\":%s}",
      i ? "," : "", i, f.currentDuty, f.manualDuty, f.manual ? "true" : "false");
  }
  snprintf(buf + len, sizeof(buf) - len, "]}");
  req->send(200, "application/json", buf);
}

void WebAPI::handleCurvePoints(AsyncWebServerRequest* req) {
  static char buf[512];
  int len = snprintf(buf, sizeof(buf), "[");
  
  for (uint8_t f = 0; f < FAN_COUNT; ++f) {
    const auto& fan = fanCtrl.getFan(f);
    len += snprintf(buf + len, sizeof(buf) - len, "%s[", f ? "," : "");
    for (uint8_t i = 0; i < fan.curveCount; ++i) {
      len += snprintf(buf + len, sizeof(buf) - len,
        "%s{\"t\":%.1f,\"d\":%.1f}", i ? "," : "", fan.curve[i].tempC, fan.curve[i].dutyPct);
    }
    len += snprintf(buf + len, sizeof(buf) - len, "]");
  }
  snprintf(buf + len, sizeof(buf) - len, "]");
  req->send(200, "application/json", buf);
}

void WebAPI::handleSetDuty(AsyncWebServerRequest* req) {
  if (!req->hasParam("id", true) || !req->hasParam("duty", true)) {
    req->send(400, "text/plain", "Missing id or duty");
    return;
  }
  
  int id = req->getParam("id", true)->value().toInt();
  float duty = req->getParam("duty", true)->value().toFloat();
  
  if (id < 0 || id >= FAN_COUNT || duty < 0) {
    req->send(400, "text/plain", "Invalid");
    return;
  }
  
  fanCtrl.setManualDuty(id, duty);
  req->send(200, "application/json", "{\"ok\":true}");
}

void WebAPI::handleSetMode(AsyncWebServerRequest* req) {
  if (!req->hasParam("id", true) || !req->hasParam("manual", true)) {
    req->send(400, "text/plain", "Missing id or manual");
    return;
  }
  
  int id = req->getParam("id", true)->value().toInt();
  int manual = req->getParam("manual", true)->value().toInt();
  
  if (id < 0 || id >= FAN_COUNT) {
    req->send(400, "text/plain", "Invalid");
    return;
  }
  
  fanCtrl.setManualMode(id, manual == 1);
  req->send(200, "application/json", "{\"ok\":true}");
}

void WebAPI::handleSetCurvePoints(AsyncWebServerRequest* req) {
  if (!req->hasParam("id", true) || !req->hasParam("points", true)) {
    req->send(400, "text/plain", "Missing id or points");
    return;
  }
  
  int id = req->getParam("id", true)->value().toInt();
  String pointsStr = req->getParam("points", true)->value();
  
  if (id < 0 || id >= FAN_COUNT) {
    req->send(400, "text/plain", "Invalid fan id");
    return;
  }
  
  CurvePoint points[MAX_CURVE_POINTS];
  uint8_t count = 0;
  
  int start = 0;
  while (count < MAX_CURVE_POINTS) {
    int comma = pointsStr.indexOf(',', start);
    String pair = (comma >= 0) ? pointsStr.substring(start, comma) : pointsStr.substring(start);
    
    int colon = pair.indexOf(':');
    if (colon > 0) {
      points[count].tempC = pair.substring(0, colon).toFloat();
      points[count].dutyPct = pair.substring(colon + 1).toFloat();
      count++;
    }
    
    if (comma < 0) break;
    start = comma + 1;
  }
  
  if (count == 0) {
    req->send(400, "text/plain", "No valid points");
    return;
  }
  
  fanCtrl.setCurvePoints(id, points, count);
  req->send(200, "application/json", "{\"ok\":true}");
}
