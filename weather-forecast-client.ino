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

const char topicSending[]  = "TimVdWalle/feeds/weather-forecast.temperatures";
const char topicReceiving[]  = "TimVdWalle/feeds/weather-forecast.forecast";

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
  Serial.begin(31250);
  while (!Serial);

  Serial.println("Starting up...");
  initStatusLED();
  connectToWiFi();
  connectToMQTT();
  mqttClient.subscribe(topicReceiving);
}

// Loop
void loop() {
  ensureWiFiConnected();
  ensureMQTTConnected();

  // maintain MQTT connection
  mqttClient.poll();  

  
  handleReceiving();      // handle incoming mqtt data

  //unsigned long currentMillis = millis();
  //if (currentMillis - previousMillis >= interval) {
  //  previousMillis = currentMillis;
  //  handleSending();
  //}

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

  Serial.println("Messages sent.\n");
  setStatusLED(true);  
}


// check this https://docs.arduino.cc/tutorials/uno-wifi-rev2/uno-wifi-r2-mqtt-device-to-device/
void handleReceiving(){
  int messageSize = mqttClient.parseMessage();

  if (messageSize) {
    // we received a message, print out the topic and contents
    Serial.print("Received a message with topic '");
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
}

// void handleReceiving(){
//   while (mqttClient.parseMessage()) {

//     int messageSize = mqttClient.parseMessage();
//     Serial.print("Received a message with topic '");
//     Serial.print(mqttClient.messageTopic());
//     Serial.print("', length ");
//     Serial.print(messageSize);
//     Serial.println(" bytes:");

//     // use the Stream interface to print the contents
//     while (mqttClient.available()) {
//       Serial.print((char)mqttClient.read());
//     }
//     Serial.println();
//     Serial.println();
//   }
// }
