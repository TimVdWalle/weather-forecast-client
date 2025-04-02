#ifndef CONNECTION_H
#define CONNECTION_H

#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>     // For Arduino Uno Wifi rev2
//#include <WiFiS3.h>     // For Arduino Uno Wifi rev4
#include "led_status.h"

// Extern declarations (defined in .ino)
extern const char wifiNetwork[];   // TODO
extern const char wifiPassword[];   // TODO

extern const char mqttServer[];
extern const int mqttPort;
extern const char mqttUser[];
extern const char mqttPassword[];

extern WiFiClient wifiClient;
extern MqttClient mqttClient;

// Connection functions
void connectToWiFi();
void connectToMQTT();
void ensureWiFiConnected();
void ensureMQTTConnected();
void updateStatusLED();

#endif
