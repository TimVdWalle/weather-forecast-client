#include "connection.h"

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.begin(wifiNetwork, wifiPassword) != WL_CONNECTED) {
    Serial.print(".");
    blinkStatusLED(500); // Slow blink while trying Wi-Fi
  }
  Serial.println(" connected.");
}

void connectToMQTT() {
  Serial.print("Connecting to Adafruit IO...");
  mqttClient.setUsernamePassword(mqttUser, mqttPassword);
  while (!mqttClient.connect(mqttServer, mqttPort)) {
    Serial.print(" failed. Error code: ");
    Serial.println(mqttClient.connectError());
    Serial.println("Retrying in 5 seconds...");

    unsigned long startTime = millis();
    while (millis() - startTime < 5000) {
      blinkStatusLED(150); // Fast blink while trying MQTT
    }
  }
  Serial.println(" connected.");
}

void ensureWiFiConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected! Reconnecting...");
    connectToWiFi();
  }
}

void ensureMQTTConnected() {
  if (!mqttClient.connected()) {
    Serial.println("MQTT disconnected! Reconnecting...");
    connectToMQTT();
  }
}

void updateStatusLED() {
  if (WiFi.status() == WL_CONNECTED && mqttClient.connected()) {
    setStatusLED(true); // Solid ON
  } else {
    setStatusLED(false); // OFF
  }
}
