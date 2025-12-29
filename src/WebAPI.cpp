#include "WebAPI.h"

WebAPI webApi;
char WebAPI::bodyBuf[512] = {0};
size_t WebAPI::bodyLen = 0;

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
  
  // PATCH endpoints
  server.on("/api/duty", HTTP_PATCH, handleSetDuty, nullptr, onBody);
  server.on("/api/mode", HTTP_PATCH, handleSetMode, nullptr, onBody);
  server.on("/api/curvePoints", HTTP_PATCH, handleSetCurvePoints, nullptr, onBody);
  
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
  
  if (!LittleFS.exists(p)) {
    if (p.startsWith("/api")) return false;
    p = "/index.html";
    if (!LittleFS.exists(p)) return false;
  }
  
  auto* response = req->beginResponse(LittleFS, p, getContentType(p.c_str()));
  response->addHeader("Cache-Control", "max-age=86400");
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
  
  int len = snprintf(buf, sizeof(buf),
    "{\"wifi\":true,\"ip\":\"%s\",\"tempC\":%.2f,\"fans\":[", ip, temp);
  
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
  int id = -1;
  float duty = -1.0f;

  if (bodyLen > 0) {
    id = parseIntParam(bodyBuf, "id");
    duty = parseFloatParam(bodyBuf, "duty");
    bodyLen = 0;
  } else {
    // Fallback: read params parsed by ESPAsyncWebServer
    if (req->hasParam("id", true)) id = req->getParam("id", true)->value().toInt();
    if (req->hasParam("duty", true)) duty = req->getParam("duty", true)->value().toFloat();
  }
  
  if (id < 0 || id >= FAN_COUNT || duty < 0) { req->send(400, "text/plain", "Invalid"); return; }
  fanCtrl.setManualDuty(id, duty);
  req->send(200, "application/json", "{\"ok\":true}");
}

void WebAPI::handleSetMode(AsyncWebServerRequest* req) {
  int id = -1;
  int manual = -1;

  if (bodyLen > 0) {
    id = parseIntParam(bodyBuf, "id");
    manual = parseIntParam(bodyBuf, "manual");
    bodyLen = 0;
  } else {
    // Fallback: read params parsed by ESPAsyncWebServer
    if (req->hasParam("id", true)) id = req->getParam("id", true)->value().toInt();
    if (req->hasParam("manual", true)) manual = req->getParam("manual", true)->value().toInt();
  }
  
  if (id < 0 || id >= FAN_COUNT || manual < 0) { req->send(400, "text/plain", "Invalid"); return; }
  fanCtrl.setManualMode(id, manual == 1);
  req->send(200, "application/json", "{\"ok\":true}");
}

void WebAPI::handleSetCurvePoints(AsyncWebServerRequest* req) {
  int id = -1;
  static char pts[256];

  if (bodyLen > 0) {
    id = parseIntParam(bodyBuf, "id");
    if (!parseStrParam(bodyBuf, "points", pts, sizeof(pts))) {
      bodyLen = 0;
      req->send(400, "text/plain", "Invalid");
      return;
    }
    bodyLen = 0;
  } else {
    // Fallback: read params parsed by ESPAsyncWebServer
    if (req->hasParam("id", true)) id = req->getParam("id", true)->value().toInt();
    if (req->hasParam("points", true)) {
      String v = req->getParam("points", true)->value();
      size_t n = v.length();
      if (n >= sizeof(pts)) n = sizeof(pts) - 1;
      memcpy(pts, v.c_str(), n);
      pts[n] = 0;
    } else {
      req->send(400, "text/plain", "Invalid");
      return;
    }
  }
  
  CurvePoint points[MAX_CURVE_POINTS];
  uint8_t count = 0;
  char* token = strtok(pts, ",");
  while (token && count < MAX_CURVE_POINTS) {
    char* colon = strchr(token, ':');
    if (colon) {
      *colon = 0;
      points[count++] = {(float)atof(token), (float)atof(colon + 1)};
    }
    token = strtok(nullptr, ",");
  }
  
  fanCtrl.setCurvePoints(id, points, count);
  req->send(200, "application/json", "{\"ok\":true}");
}

// ─────────────────────────────────────────────────────────────────────────────
// Body Parsing
// ─────────────────────────────────────────────────────────────────────────────

void WebAPI::onBody(AsyncWebServerRequest*, uint8_t* data, size_t len, size_t index, size_t total) {
  if (total >= sizeof(bodyBuf)) return; // Too large
  if (index == 0) bodyLen = 0;
  
  size_t copyLen = min(len, sizeof(bodyBuf) - index - 1);
  memcpy(bodyBuf + index, data, copyLen);
  bodyLen = index + copyLen;
  bodyBuf[bodyLen] = 0;
}

int WebAPI::parseIntParam(const char* body, const char* key) {
  char search[32];
  snprintf(search, sizeof(search), "%s=", key);
  const char* p = strstr(body, search);
  return p ? atoi(p + strlen(search)) : -1;
}

float WebAPI::parseFloatParam(const char* body, const char* key) {
  char search[32];
  snprintf(search, sizeof(search), "%s=", key);
  const char* p = strstr(body, search);
  return p ? atof(p + strlen(search)) : -1.0f;
}

const char* WebAPI::parseStrParam(const char* body, const char* key, char* out, size_t outLen) {
  char search[32];
  snprintf(search, sizeof(search), "%s=", key);
  const char* p = strstr(body, search);
  if (!p) return nullptr;
  
  p += strlen(search);
  const char* end = strchr(p, '&');
  size_t len = end ? (size_t)(end - p) : strlen(p);
  if (len >= outLen) len = outLen - 1;
  // Copy and URL-decode into out
  size_t oi = 0;
  for (size_t i = 0; i < len && oi + 1 < outLen; ++i) {
    char c = p[i];
    if (c == '%' && i + 2 < len) {
      char h1 = p[i + 1];
      char h2 = p[i + 2];
      auto hex = [](char ch) -> int {
        if (ch >= '0' && ch <= '9') return ch - '0';
        if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
        if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
        return -1;
      };
      int v1 = hex(h1), v2 = hex(h2);
      if (v1 >= 0 && v2 >= 0) {
        out[oi++] = (char)((v1 << 4) | v2);
        i += 2;
      } else {
        out[oi++] = c;
      }
    } else if (c == '+') {
      out[oi++] = ' ';
    } else {
      out[oi++] = c;
    }
  }
  out[oi] = 0;
  return out;
}
