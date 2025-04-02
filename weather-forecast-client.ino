/****************************************************************
*
*   INCLUDES
*
*****************************************************************/
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>     // For Arduino Uno Wifi rev2
//#include <WiFiS3.h>     // For Arduino Uno Wifi rev4

#include "arduino_secrets.h"
#include "connection.h"
#include "led_status.h"


/****************************************************************
*
*   CONFIG
*
*****************************************************************/
// Wifi
const char wifiNetwork[] = WIFI_SSID;
const char wifiPassword[] = WIFI_PASSWORD;

// Adafruit IO credentials
const char mqttServer[] = "io.adafruit.com";
const int mqttPort = 1883;
const char mqttUser[] = MQTT_USER;
const char mqttPassword[] = MQTT_AIO_KEY;

const char topic[]  = "TimVdWalle/feeds/welcome-feed";
const char topic2[]  = "TimVdWalle/feeds/welcome-feed";
const char topic3[]  = "TimVdWalle/feeds/welcome-feed";

// Loop helper vars
const long interval = 8000;
unsigned long previousMillis = 0;


/****************************************************************
*
*   GLOBAL CLIENTS
*
*****************************************************************/
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);


/****************************************************************
*
*   SETUP
*
*****************************************************************/
void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Starting up...");
  initStatusLED();

  connectToWiFi();
  connectToMQTT();
}

// Loop
void loop() {
  ensureWiFiConnected();
  ensureMQTTConnected();

  mqttClient.poll(); // maintain MQTT connection

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read sensor values
    int Rvalue  = analogRead(A0);
    int Rvalue2 = analogRead(A1);
    int Rvalue3 = analogRead(A2);

    // Debug print
    Serial.print("Publishing to ");
    Serial.print(topic); Serial.print(": "); Serial.println(Rvalue);
    Serial.print(topic2); Serial.print(": "); Serial.println(Rvalue2);
    Serial.print(topic3); Serial.print(": "); Serial.println(Rvalue3);

    // Publish messages
    mqttClient.beginMessage(topic);
    mqttClient.print(Rvalue);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic2);
    mqttClient.print(Rvalue2);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic3);
    mqttClient.print(Rvalue3);
    mqttClient.endMessage();

    Serial.println("Messages sent.\n");
  }

  // Update LED to reflect current connection state
  updateStatusLED();
}
