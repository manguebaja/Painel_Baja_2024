#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "soft_defs.h"

#define GRAUS_SYM   "\u00b0"
#define WIFI_SYM    "\u00f7"

#define Receiver Serial2 // Defines a Serial port instance called 'Receiver' using serial port 2

// Four digits display
TM1637Display Four(Fourdigits_CLK, Fourdigits_DIO);

// Six digits display
TM1637TinyDisplay6 Six(Sixdigits_CLK, Sixdigits_DIO);

// OLED
U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, SCLK_OLED, SDA_OLED, U8X8_PIN_NONE); // ESP32 Thing, pure SW emulated I2C

#endif