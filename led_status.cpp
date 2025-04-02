#include "led_status.h"

unsigned long previousBlinkMillis = 0;
bool ledState = LOW;

void initStatusLED() {
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
}

void blinkStatusLED(unsigned long interval) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkMillis >= interval) {
    previousBlinkMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(STATUS_LED, ledState);
  }
}

void setStatusLED(bool on) {
  digitalWrite(STATUS_LED, on ? HIGH : LOW);
}
