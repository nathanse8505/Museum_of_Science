#include <Arduino.h>
#include "camera_handler.h"
#include "network_handler.h"
#include "time_sync.h"
#include "esp_timer.h"

esp_timer_handle_t capture_timer;
double timeOffset = 0;

// Reconnect variables
unsigned long lastWsReconnectAttempt = 0;
const unsigned long WS_RECONNECT_INTERVAL = 5000;

void setup() {

  pinMode(LED_GPIO_NUM, OUTPUT);
  digitalWrite(LED_GPIO_NUM, LOW);  // Make sure it's off initially

  Serial.begin(115200);
  delay(500);  // Let USB and peripherals settle
  Serial.println("\n");

  bool connection_status = connectToWiFi();
  
  bool camera_status = startCamera();


  // Prepare timer
  esp_timer_create_args_t timer_args = {
    .callback = [](void*) { triggerCapture(); },
    .name = "capture_timer"
  };
  esp_timer_create(&timer_args, &capture_timer);
  
  connectToWebSocket();
  subscribeToTimeSyncEvents();
}

// --- Loop ---
void loop() {
  wsClient.poll();
  // Check if WebSocket is disconnected and try reconnecting
  if (!wsClient.available()) {
    unsigned long now = millis();
    if (now - lastWsReconnectAttempt >= WS_RECONNECT_INTERVAL) {
      Serial.println("WebSocket disconnected, trying to reconnect...");
      wsClient.close();
      delay(100); // Give some time to fully close
      connectToWebSocket();
      lastWsReconnectAttempt = now;
    }
  }
}
