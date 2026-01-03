#pragma once
#include <Arduino.h>
#include "Config.h"

extern "C" uint8_t temprature_sens_read();

struct CurvePoint {
  float tempC;
  float dutyPct;
};

struct FanState {
  CurvePoint curve[MAX_CURVE_POINTS];
  uint8_t curveCount = 2;
  float currentDuty = MIN_DUTY_PCT;
  float manualDuty = 50.0f;
  bool manual = false;
};

class FanController {
public:
  void begin();
  void update();
  
  float getTemp() const { return lastTemp; }
  const FanState& getFan(uint8_t idx) const { return fans[idx]; }
  
  void setManualDuty(uint8_t idx, float duty);
  void setManualMode(uint8_t idx, bool manual);
  void setCurvePoints(uint8_t idx, const CurvePoint* points, uint8_t count);
  bool saveState();
  bool loadState();
  
private:
  FanState fans[FAN_COUNT];
  float lastTemp = 0;
  
  void initDefaultCurves();
  void sortCurve(uint8_t idx);
  float applyCurve(uint8_t idx, float tempC);
  void setDuty(uint8_t idx, float pct);
  
  static float clamp(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
  static uint16_t pctToPwm(float pct) { return static_cast<uint16_t>(roundf(clamp(pct, 0, 100) / 100.0f * PWM_MAX)); }
};

extern FanController fanCtrl;
