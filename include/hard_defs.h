#ifndef HARD_DEFS_H
#define HARD_DEFS_H

#include <Arduino.h>
#include <driver/gpio.h>

// Four digits display
#define Fourdigits_DIO      GPIO_NUM_15     // 4-digit display DATA
#define Fourdigits_CLK      GPIO_NUM_4      // 4-digit display CLOCK

// Six digits display
#define Sixdigits_DIO       GPIO_NUM_18     // 6-digit display DATA
#define Sixdigits_CLK       GPIO_NUM_19     // 6-digit display CLOCK

// OLED
#define SCLK_OLED           GPIO_NUM_21     // OLED CLOCK
#define SDA_OLED            GPIO_NUM_22     // OLED DATA

#define DisplayHight        64 
#define DisplayWidth        124    

// Leds
#define combust_1           GPIO_NUM_14    // 1st fuel LED
#define combust_2           GPIO_NUM_27    // 2st fuel LED
#define combust_3           GPIO_NUM_25    // 3st fuel LED 
#define combust_4           GPIO_NUM_33    // 4st fuel LED
#define combust_5           GPIO_NUM_32    // fuel emergency LED 

#define MotorTemp_led       GPIO_NUM_13    // Motor temperature warning LED
#define CVTtemp_led         GPIO_NUM_12    // CVT temperature warning LED
#define Battery_led         GPIO_NUM_26    // Battery percentage warning LED

// Buttons and keys for interactions with the user in the car
#define BUTTON              GPIO_NUM_23    // Green
#define SWITCH              GPIO_NUM_35    // Purple
#define POTENTIOMETER       GPIO_NUM_34    // Gray

#define Alert_TempCVT       90            // Estimated temperature of when the cvt belt may have a problem
#define Alert_TempMOT       105           // Estimated temperature for engine temperature problems

// Serial Communication with the Front ECU (MMI)
#define Receiver_Rxd_pin    GPIO_NUM_16    // Blue
#define Receiver_Txd_pin    GPIO_NUM_17    // Yellow

#endif
