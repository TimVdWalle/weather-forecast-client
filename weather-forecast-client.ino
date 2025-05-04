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
const char mqttServer[] = MQTT_SERVER;
const int mqttPort = MQTT_PORT;
const char mqttUser[] = MQTT_USER;
const char mqttPassword[] = MQTT_AIO_KEY;

const char topicSending[]  = MQTT_TOPIC_SENDING_ADAFRUIT;
const char topicReceiving[]  = MQTT_TOPIC_RECEIVING_ADAFRUIT;

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
  mqttClient.subscribe(topicReceiving);
  mqttClient.onMessage(onMqttMessage);
}

// Loop
void loop() {
  ensureWiFiConnected();
  ensureMQTTConnected();

  // maintain MQTT connection
  mqttClient.poll();  

  // handle sending mqtt data
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    handleSending();
  }

  // Update LED to reflect current connection state
  updateStatusLED();
}

void handleSending(){
  setStatusLED(false);

  // Read sensor values
  int Rvalue  = analogRead(A0);

  // Debug print
  Serial.print("Publishing to ");
  Serial.print(topicSending); Serial.print(": "); Serial.println(Rvalue);

  // Publish messages
  mqttClient.beginMessage(topicSending);
  mqttClient.print(Rvalue);
  mqttClient.endMessage();

  Serial.println("Message(s) sent.\n");
  setStatusLED(true);  
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
  Serial.println();
  Serial.println();
}