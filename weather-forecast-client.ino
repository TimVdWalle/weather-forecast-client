#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

// Replace with your network credentials
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;

// Adafruit IO credentials
const char mqttServer[] = "io.adafruit.com";
const int mqttPort = 1883; // Use 8883 for SSL
const char mqttUser[] = MQTT_USER;
const char mqttPassword[] = MQTT_AIO_KEY;

const char topic[]  = "TimVdWalle/feeds/welcome-feed";
const char topic2[]  = "TimVdWalle/feeds/welcome-feed";
const char topic3[]  = "TimVdWalle/feeds/welcome-feed";

//set interval for sending messages (milliseconds)
const long interval = 8000;
unsigned long previousMillis = 0;

int count = 0;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" connected.");

  // Connect to Adafruit IO MQTT broker
  Serial.print("Connecting to Adafruit IO...");
  mqttClient.setUsernamePassword(mqttUser, mqttPassword);

  if (!mqttClient.connect(mqttServer, mqttPort)) {
    Serial.print(" failed. Error code: ");
    Serial.println(mqttClient.connectError());
    while (1);
  }
  Serial.println(" connected.");
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    //record random value from A0, A1 and A2
    int Rvalue = analogRead(A0);
    int Rvalue2 = analogRead(A1);
    int Rvalue3 = analogRead(A2);

    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.println(Rvalue);

    Serial.print("Sending message to topic: ");
    Serial.println(topic2);
    Serial.println(Rvalue2);

    Serial.print("Sending message to topic: ");
    Serial.println(topic2);
    Serial.println(Rvalue3);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    mqttClient.print(Rvalue);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic2);
    mqttClient.print(Rvalue2);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic3);
    mqttClient.print(Rvalue3);
    mqttClient.endMessage();

    Serial.println();
  }
}
