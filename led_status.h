#ifndef LED_STATUS_H
#define LED_STATUS_H

#include <Arduino.h>

// Use pin 13 for onboard LED
#define STATUS_LED LED_BUILTIN

// Call this once in setup()
void initStatusLED();

// Call this repeatedly for blinking (non-blocking)
void blinkStatusLED(unsigned long interval);

// Set LED ON/OFF (e.g., for connection status)
void setStatusLED(bool on);

#endif
