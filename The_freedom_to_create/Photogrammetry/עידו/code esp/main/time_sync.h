#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "camera_handler.h"
#include "network_handler.h"
#include "esp_timer.h"

extern esp_timer_handle_t capture_timer;
extern double timeOffset;  // We'll define it in main.ino

// === Capture Timing ===

inline void subscribeToTimeSyncEvents() {
  wsClient.onMessage([](WebsocketsMessage message) {
    String data = message.data();
    // Serial.println("Message received: " + data);

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      Serial.print("JSON parse failed: ");
      Serial.println(error.c_str());
      return;
    }

    String type = doc["type"];

    if (type == "sync") {
      double serverTime = doc["time"];
      double localTime = millis() / 1000.0;
      timeOffset = serverTime - localTime;
      // Serial.print("Synced time offset: ");
      // Serial.println(timeOffset, 6);

    } else if (type == "capture") {
      double targetTime = doc["time"];
      double now = millis() / 1000.0 + timeOffset;
      double delaySec = targetTime - now;

      if (delaySec <= 0) {
        Serial.println("Target time passed, capturing immediately.");
        triggerCapture();
        return;
      }

      unsigned long delayMs = (unsigned long)(delaySec * 1000);
      Serial.print("Scheduling capture in ");
      Serial.print(delayMs);
      Serial.println(" ms");

      esp_timer_stop(capture_timer);
      esp_timer_start_once(capture_timer, delayMs * 1000);
    }
  });
}
