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

//const char topicSending[]  = MQTT_TOPIC_SENDING_ADAFRUIT;
const char topicReceiving[]  = MQTT_TOPIC_RECEIVING_ADAFRUIT;

// Loop helper vars
const long interval = 650;
int statusMessageLineStars = 100;


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

  Serial.println("WEATHER FORECAST CLIENT V0.2");
  Serial.println("****************************");
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
  // handling of incoming data is done in onMqttMessage
  // mqttClient.poll keeps connection alive and listens for incoming data on the channel
  mqttClient.poll();  

  // send regular status messages to serial monitor output
  handleStatusMessage();

  // Update LED to reflect current connection state
  updateStatusLED();
}

// handle sending mqtt data
unsigned long previousMillis = 0;
int charCount = 0;
void handleStatusMessage() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if(charCount > statusMessageLineStars) {
      Serial.println();
      resetCharCount();
      }

      Serial.print("*");
      charCount++;
  }
}

void resetCharCount() {
  charCount = 0;
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  resetCharCount();
  Serial.println();
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  String data = "";
  while (mqttClient.available()) {
    char c = (char)mqttClient.read();
    data += c;
  }

  Serial.println();
  Serial.println();

    // Only handle if it's a complete, non-empty message
  if (data.length() > 0) {
    handleIncomingData(data);
  } else {
    Serial.println("Warning: Received empty or invalid MQTT message.");
  }
}

void handleIncomingData(String data) {
  Serial.println("Parsing incoming data:");
  Serial.println(data);
}

/*
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
*/