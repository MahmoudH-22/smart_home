/**
 * settings.h - pins (Rev 3.1 shield) and every tunable value of the smart home.
 * Change behaviour here; the subsystem files only read these constants.
 */
#pragma once
#include <Arduino.h>

// ===================== Pins (Rev 3.1) =====================
const uint8_t PIN_BUZZER = 7;
const uint8_t PIN_LED_IN = 8;
const uint8_t PIN_LED_OUT = 9;
const uint8_t PIN_PIR = 10;
const uint8_t PIN_SERVO = 11;
const uint8_t PIN_BUTTON = 12;
const uint8_t PIN_FAN = 13;
const uint8_t PIN_LDR = A0;
const uint8_t PIN_KNOB = A1;
const uint8_t PIN_DHT = A2;
const uint8_t PIN_REED = A3;
// Keypad: rows D6..D3, columns D2..D0 (see access.cpp)

// ===================== Door access =====================
const char DOOR_CODE[] = "1234";              // + '#' opens the door
const uint8_t SERVO_CLOSED = 90;              // arm mounted pointing left at this angle
const uint8_t SERVO_OPEN = 0;                 // opens by turning the other way (-90)
const unsigned long DOOR_OPEN_MS = 5000UL;    // auto-close delay
const uint8_t MAX_WRONG = 3;
const unsigned long LOCKOUT_MS = 10000UL;

// ===================== Security =====================
const char SECURITY_CODE[] = "2580";          // + '#' silences the alarm (the only way to stop it)
const bool ALWAYS_ARMED = true;               // security is ON from power-up and cannot be turned off
const bool USE_SECURITY_PANEL = false;        // true once ARM + MUTE are wired on J12
const uint8_t WINDOW_OPEN_LEVEL = HIGH;       // reed with INPUT_PULLUP, HIGH = window open
const unsigned long WINDOW_DEBOUNCE_MS = 300UL;
const unsigned long EXT_LIGHT_MS = 3000UL;    // exterior light on-time after last motion
const int PANEL_DISARMED_MAX = 12;            // panel mode: A3 below this = ARM switch closed (OFF)
const int PANEL_MUTE_MAX = 145;               // panel mode: A3 below this = MUTE pressed
const int PANEL_CLOSED_MAX = 700;             // panel mode: A3 below this = window closed
const unsigned long PANEL_DEBOUNCE_MS = 60UL;

// ===================== ADC (Lab 4) =====================
const float ADC_REF_V = 5.0;                  // default analog reference
const int ADC_MAX = 1023;                     // 10-bit ADC: 2^n - 1

// ===================== Indoor light (Lab 6: LDR voltage divider) =====================
// LDR from 5V to A0, R5 = 10k from A0 to GND:  Vout = 5V * R / (R + R_LDR)
// Bright -> R_LDR low -> Vout high.  Dark -> R_LDR high -> Vout low.
const float LDR_DARK_V = 2.9;                 // light turns on below this voltage
const float LDR_BRIGHT_V = 3.4;               // light turns off above this voltage (hysteresis)
const unsigned long BUTTON_DEBOUNCE_MS = 50UL;

// ===================== Climate (Lab 4: DHT11 + potentiometer, Lab 6: L293D) =====================
const float TEMP_MIN = 20.0;                  // knob at 0 V
const float TEMP_MAX = 40.0;                  // knob at 5 V
const float FAN_HYSTERESIS = 1.0;
const unsigned long DHT_PERIOD_MS = 2000UL;   // DHT11 gives a new reading about every 2 s

// ===================== Display =====================
const uint8_t LCD_ADDR = 0x27;
const uint8_t LCD_PAGES = 4;                  // temp/humidity, threshold/fan, lamp/LDR, PIR
const unsigned long LCD_PAGE_MS = 5000UL;     // each status page when paging by itself
const unsigned long PIR_PAGE_MS = 5000UL;     // PIR page forced after new motion
const unsigned long MANUAL_PAGE_MS = 20000UL; // page picked with '*' stays this long
const unsigned long MSG_MS = 3000UL;          // line-1 messages (ACCESS GRANTED, ...)
