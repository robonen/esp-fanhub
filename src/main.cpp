#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ArduinoOTA.h>

#include "Config.h"
#include "FanController.h"
#include "WebAPI.h"

static bool wifiConnected = false;
static unsigned long lastBlinkMs = 0;
static unsigned long lastControlMs = 0;
static bool ledState = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  LittleFS.begin(true);
  fanCtrl.begin();
  
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(HOSTNAME);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {
  unsigned long now = millis();
  
  if (!wifiConnected) {
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
      digitalWrite(LED_BUILTIN, HIGH);
      webApi.begin();
      ArduinoOTA.setHostname(HOSTNAME);
      ArduinoOTA.begin();
    } else if (now - lastBlinkMs >= BLINK_INTERVAL_MS) {
      lastBlinkMs = now;
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState);
    }
  }
  
  if (wifiConnected) ArduinoOTA.handle();
  
  if (now - lastControlMs >= CONTROL_INTERVAL_MS) {
    lastControlMs = now;
    fanCtrl.update();
  }
}