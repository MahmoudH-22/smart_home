/*
 * Smart Home - MCT331s Project 1 (Rev 3.1 shield on Arduino Uno)
 *
 * One file per subsystem:
 *   settings.h          pins and every tunable value
 *   access.h/.cpp       keypad: door code, security code, lockout, LCD page key
 *   door.h/.cpp         servo door, auto-close
 *   security.h/.cpp     window alarm (latched until SECURITY_CODE), PIR exterior light
 *   lighting.h/.cpp     indoor light: LDR voltage divider or LIGHT button override
 *   climate.h/.cpp      DHT11 temperature/humidity, knob threshold, fan via L293D
 *   display.h/.cpp      16x2 I2C LCD status
 *   buzzer.h/.cpp       key beeps and alarm sound
 *
 * Keypad uses D0/D1: unplug the keypad while uploading. Serial is not used.
 * Libraries: Servo (built-in), Keypad (Mark Stanley), LiquidCrystal I2C (Frank de Brabander),
 *            DHTlib (Rob Tillaart, as in Lab 4).
 */
#include "settings.h"
#include "buzzer.h"
#include "door.h"
#include "security.h"
#include "lighting.h"
#include "climate.h"
#include "access.h"
#include "display.h"

/** Initialises every subsystem; the door starts closed and security armed (ALWAYS_ARMED). */
void setup() {
  buzzerInit();
  lightingInit();
  climateInit();
  securityInit();
  doorInit();
  displayInit();
}

/** Runs every subsystem once per pass; nothing here blocks. */
void loop() {
  updateAccess();
  updateDoor();
  updateSecurity();
  updateLighting();
  updateClimate();
  updateBuzzer(isAlarmActive());
  updateDisplay();
}
