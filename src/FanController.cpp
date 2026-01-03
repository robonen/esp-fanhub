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
  loadState();
}

void FanController::update() {
  lastTemp = (static_cast<float>(temprature_sens_read()) - 32.0f) / 1.8f;
  for (uint8_t i = 0; i < FAN_COUNT; ++i) {
    float duty = fans[i].manual ? fans[i].manualDuty : applyCurve(i, lastTemp);
    setDuty(i, duty);
  }
}

void FanController::setManualDuty(uint8_t idx, float duty) {
  if (idx < FAN_COUNT) {
    fans[idx].manualDuty = duty;
    saveState();
  }
}

void FanController::setManualMode(uint8_t idx, bool manual) {
  if (idx < FAN_COUNT) {
    fans[idx].manual = manual;
    saveState();
  }
}

void FanController::setCurvePoints(uint8_t idx, const CurvePoint* points, uint8_t count) {
  if (idx >= FAN_COUNT || count > MAX_CURVE_POINTS) return;
  for (uint8_t i = 0; i < count; ++i) {
    fans[idx].curve[i] = points[i];
  }
  fans[idx].curveCount = count;
  sortCurve(idx);
  saveState();
}

void FanController::initDefaultCurves() {
  for (uint8_t i = 0; i < FAN_COUNT; ++i) {
    fans[i].curveCount = 2;
    fans[i].curve[0] = {35.0f, 35.0f};
    fans[i].curve[1] = {75.0f, 100.0f};
  }
}

bool FanController::saveState() {
  File f = LittleFS.open("/fans.txt", "w");
  if (!f) return false;
  
  for (uint8_t i = 0; i < FAN_COUNT; ++i) {
    // Line format: mode|manualDuty|temp:duty,temp:duty,...
    f.print(fans[i].manual ? '1' : '0');
    f.print('|');
    f.print(fans[i].manualDuty, 1);
    f.print('|');
    for (uint8_t j = 0; j < fans[i].curveCount; ++j) {
      if (j) f.print(',');
      f.print(fans[i].curve[j].tempC, 1);
      f.print(':');
      f.print(fans[i].curve[j].dutyPct, 1);
    }
    f.print('\n');
  }
  f.close();
  return true;
}

bool FanController::loadState() {
  File f = LittleFS.open("/fans.txt", "r");
  if (!f) return false;

  uint8_t idx = 0;
  while (f.available() && idx < FAN_COUNT) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) { idx++; continue; }

    // Parse: mode|manualDuty|curves
    int sep1 = line.indexOf('|');
    int sep2 = line.indexOf('|', sep1 + 1);
    if (sep1 < 0 || sep2 < 0) { idx++; continue; }

    fans[idx].manual = (line[0] == '1');
    fans[idx].manualDuty = line.substring(sep1 + 1, sep2).toFloat();

    // Parse curves
    String curvesStr = line.substring(sep2 + 1);
    uint8_t cnt = 0;
    int start = 0;
    while (start < (int)curvesStr.length() && cnt < MAX_CURVE_POINTS) {
      int comma = curvesStr.indexOf(',', start);
      String token = (comma < 0) ? curvesStr.substring(start) : curvesStr.substring(start, comma);
      int colon = token.indexOf(':');
      if (colon > 0) {
        fans[idx].curve[cnt].tempC = token.substring(0, colon).toFloat();
        fans[idx].curve[cnt].dutyPct = token.substring(colon + 1).toFloat();
        cnt++;
      }
      if (comma < 0) break;
      start = comma + 1;
    }
    if (cnt > 0) {
      fans[idx].curveCount = cnt;
      sortCurve(idx);
    }
    idx++;
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
