#include "FanController.h"
#include <LittleFS.h>

FanController fanCtrl;

void FanController::begin() {
  for (uint8_t i = 0; i < FAN_COUNT; ++i) {
    ledcSetup(FAN_CHANNELS[i], PWM_FREQ, PWM_RES_BITS);
    ledcAttachPin(FAN_PINS[i], FAN_CHANNELS[i]);
    setDuty(i, MIN_DUTY_PCT);
  }
  initDefaultCurves();
  loadCurves();
}

void FanController::update() {
  lastTemp = (static_cast<float>(temprature_sens_read()) - 32.0f) / 1.8f;
  for (uint8_t i = 0; i < FAN_COUNT; ++i) {
    float duty = fans[i].manual ? fans[i].manualDuty : applyCurve(i, lastTemp);
    setDuty(i, duty);
  }
}

void FanController::setManualDuty(uint8_t idx, float duty) {
  if (idx < FAN_COUNT) fans[idx].manualDuty = duty;
}

void FanController::setManualMode(uint8_t idx, bool manual) {
  if (idx < FAN_COUNT) fans[idx].manual = manual;
}

void FanController::setCurvePoints(uint8_t idx, const CurvePoint* points, uint8_t count) {
  if (idx >= FAN_COUNT || count > MAX_CURVE_POINTS) return;
  for (uint8_t i = 0; i < count; ++i) {
    fans[idx].curve[i] = points[i];
  }
  fans[idx].curveCount = count;
  sortCurve(idx);
  saveCurves();
}

void FanController::initDefaultCurves() {
  for (uint8_t i = 0; i < FAN_COUNT; ++i) {
    fans[i].curveCount = 2;
    fans[i].curve[0] = {35.0f, 35.0f};
    fans[i].curve[1] = {75.0f, 100.0f};
  }
}

bool FanController::saveCurves() {
  File f = LittleFS.open("/curves.txt", "w");
  if (!f) return false;
  for (uint8_t i = 0; i < FAN_COUNT; ++i) {
    for (uint8_t j = 0; j < fans[i].curveCount; ++j) {
      if (j) f.print(',');
      f.print(fans[i].curve[j].tempC, 1);
      f.print(':');
      f.print(fans[i].curve[j].dutyPct, 1);
    }
    if (i + 1 < FAN_COUNT) f.print('\n');
  }
  f.close();
  return true;
}

bool FanController::loadCurves() {
  File f = LittleFS.open("/curves.txt", "r");
  if (!f) return false;

  uint8_t fanIdx = 0;
  while (f && fanIdx < FAN_COUNT) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) { fanIdx++; continue; }

    CurvePoint pts[MAX_CURVE_POINTS];
    uint8_t cnt = 0;
    int start = 0;
    while (start < line.length() && cnt < MAX_CURVE_POINTS) {
      int comma = line.indexOf(',', start);
      String token = (comma == -1) ? line.substring(start) : line.substring(start, comma);
      int colon = token.indexOf(':');
      if (colon > 0) {
        float t = token.substring(0, colon).toFloat();
        float d = token.substring(colon + 1).toFloat();
        pts[cnt++] = {t, d};
      }
      if (comma == -1) break;
      start = comma + 1;
    }
    if (cnt > 0) setCurvePoints(fanIdx, pts, cnt);
    fanIdx++;
  }
  f.close();
  return true;
}

void FanController::sortCurve(uint8_t idx) {
  auto& f = fans[idx];
  for (uint8_t i = 0; i < f.curveCount; ++i) {
    for (uint8_t j = i + 1; j < f.curveCount; ++j) {
      if (f.curve[i].tempC > f.curve[j].tempC) {
        std::swap(f.curve[i], f.curve[j]);
      }
    }
  }
}

float FanController::applyCurve(uint8_t idx, float tempC) {
  auto& f = fans[idx];
  if (f.curveCount == 0) return MIN_DUTY_PCT;
  
  if (tempC <= f.curve[0].tempC) return max(f.curve[0].dutyPct, MIN_DUTY_PCT);
  if (tempC >= f.curve[f.curveCount - 1].tempC) return max(f.curve[f.curveCount - 1].dutyPct, MIN_DUTY_PCT);
  
  for (uint8_t i = 1; i < f.curveCount; ++i) {
    if (tempC <= f.curve[i].tempC) {
      auto& a = f.curve[i - 1];
      auto& b = f.curve[i];
      float range = b.tempC - a.tempC;
      if (range <= 0.001f) return max(b.dutyPct, MIN_DUTY_PCT);
      float alpha = (tempC - a.tempC) / range;
      return max(a.dutyPct + alpha * (b.dutyPct - a.dutyPct), MIN_DUTY_PCT);
    }
  }
  return MIN_DUTY_PCT;
}

void FanController::setDuty(uint8_t idx, float pct) {
  pct = max(pct, MIN_DUTY_PCT);
  ledcWrite(FAN_CHANNELS[idx], pctToPwm(pct));
  fans[idx].currentDuty = pct;
}
