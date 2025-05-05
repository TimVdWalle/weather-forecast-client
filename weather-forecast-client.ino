/****************************************************************
*
*   INCLUDES
*
*****************************************************************/
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h> 
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

// Dummy display function
void displayForecast(JsonObject& now, JsonObject& shortTerm, JsonObject& midTerm, JsonObject& longTerm, JsonObject& xlongTerm) {
  Serial.println("=== WEATHER FORECAST ===");

  auto printEntry = [](const char* label, JsonObject& entry) {
    Serial.print(label);
    Serial.print(": ");
    Serial.print(entry["temp"].as<float>());
    Serial.print("°C, ");
    Serial.print(entry["sky"].as<const char*>());
    Serial.print(", Wind ");
    Serial.print(entry["wind"]["speed"].as<float>());
    Serial.print(" km/h ");
    Serial.println(entry["wind"]["dir"].as<const char*>());
  };

  printEntry("Now", now);
  printEntry("0–3h", shortTerm);
  printEntry("3–6h", midTerm);
  printEntry("6–12h", longTerm);
  printEntry("12–24h", xlongTerm);
}

// lets start by sending the data in json
// if that turns out to be asking too much resources to parse (RAM and/or CPU), then switch to something like "23.5,65,1012" or "T=23.5;H=65", 
// which can be parsed with sscanf() or strtok(); those methods are more resource-friendly, but the data format becomes less human-readable and less flexible
void handleIncomingData(String data) {
  Serial.println("Parsing incoming data:");
  Serial.println(data);

  StaticJsonDocument<512> doc;  // Adjust size as needed

  DeserializationError error = deserializeJson(doc, data);
  if (error) {
    Serial.print("JSON parsing failed: ");
    Serial.println(error.f_str());
    return;
  }

  // Extract forecast objects
  JsonObject now       = doc["now_0h"];
  JsonObject shortTerm = doc["short_0-3h"];
  JsonObject midTerm   = doc["mid_3-6h"];
  JsonObject longTerm  = doc["long_6-12h"];
  JsonObject xlongTerm = doc["xlong_12-24h"];

  if (now.isNull() || shortTerm.isNull() || midTerm.isNull() || longTerm.isNull() || xlongTerm.isNull()) {
    Serial.println("One or more forecast sections are missing.");
    return;
  }

  // Call dummy display function
  displayForecast(now, shortTerm, midTerm, longTerm, xlongTerm);
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


/*
example json expected format : 

{
  "now_0h": {
    "temp": 21.8,
    "sky": "partly cloudy",
    "wind": {
      "speed": 4.5,
      "dir": "NE"
    }
  },
  "short_0-3h": {
    "temp": 22.4,
    "sky": "sunny",
    "wind": {
      "speed": 5.1,
      "dir": "E"
    }
  },
  "mid_3-6h": {
    "temp": 23.0,
    "sky": "mostly sunny",
    "wind": {
      "speed": 6.3,
      "dir": "SE"
    }
  },
  "long_6-12h": {
    "temp": 19.6,
    "sky": "cloudy",
    "wind": {
      "speed": 7.8,
      "dir": "S"
    }
  },
  "xlong_12-24h": {
    "temp": 17.2,
    "sky": "rainy",
    "wind": {
      "speed": 9.0,
      "dir": "SW"
    }
  }
}

*/