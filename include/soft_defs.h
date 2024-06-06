#ifndef SOFT_DEFS_H
#define SOFT_DEFS_H

#include "hard_defs.h"

typedef struct {
  uint16_t seconds = 0;
  uint8_t minutes = 0;
  uint8_t hours = 0;

  unsigned long time_current = 0;
  unsigned long time_lap_ms = 0;

} Time;

Time Endurance;
Time Lap;

typedef struct {
  uint8_t Pin = SWITCH;
  //uint8_t buttonState;                 // The current reading from the input pin
  uint8_t lastButtonState = LOW;       // Tthe previous reading from the input pin
  unsigned long lastDebounceTime = 0;  // The last time the output pin was toggled
  //uint8_t mode = 0;

} Peripherals;

Peripherals Switch;

// Struct to receive all panel data
// If you want to add data to the panel, change it not only in the variable but also in the front ECU
typedef struct {
  uint16_t speed;     // 2by
  uint16_t rpm;       // 2by
  uint8_t temp_motor; // 1by
  uint16_t level;     // 2by
  uint8_t battery;    // 1by
  uint8_t temp_cvt;   // 1by
  uint8_t sot;        // 1by

} Txtmng;

unsigned long lastDebounceTime = 0;
const int debounceDelay = 200;          // Time in milliseconds for button debounce

typedef enum PROGMEM {ENDURANCE_TIMER, STOPWATCH, DELTA_CRONOMETRO} tempo_six_digits;

uint8_t six_digits_state = ENDURANCE_TIMER;

#endif