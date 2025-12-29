#pragma once

#include "Secrets.h"

// WiFi
#ifndef WIFI_SSID
#error "WIFI_SSID is not defined. Create src/Secrets.h based on Secrets.sample.h"
#endif
#ifndef WIFI_PASS
#error "WIFI_PASS is not defined. Create src/Secrets.h based on Secrets.sample.h"
#endif

constexpr char HOSTNAME[] = "esp32-fans";

// Fans
constexpr uint8_t FAN_COUNT = 3;
constexpr uint8_t MAX_CURVE_POINTS = 8;
constexpr int FAN_PINS[FAN_COUNT] = {17, 16, 5};
constexpr uint8_t FAN_CHANNELS[FAN_COUNT] = {0, 1, 2};

// PWM
constexpr uint32_t PWM_FREQ = 25000;
constexpr uint8_t PWM_RES_BITS = 10;
constexpr uint16_t PWM_MAX = (1U << PWM_RES_BITS) - 1;

// Control
constexpr float MIN_DUTY_PCT = 30.0f;
constexpr unsigned long CONTROL_INTERVAL_MS = 500;
constexpr unsigned long BLINK_INTERVAL_MS = 300;
