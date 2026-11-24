#include <Keypad.h>
#include "access.h"
#include "settings.h"
#include "buzzer.h"
#include "door.h"
#include "security.h"
#include "display.h"

static const byte ROWS = 4, COLS = 3;
static char keys[ROWS][COLS] = {{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'}};
static byte rowPins[ROWS] = {6, 5, 4, 3};
static byte colPins[COLS] = {2, 1, 0};
static Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

static char entry[8];
static uint8_t entryLen = 0;
static uint8_t wrongTries = 0;
static unsigned long lockedUntil = 0;

/**
 * Checks the typed code: the door code opens the door; the security code silences a sounding alarm
 * (and toggles security only when ALWAYS_ARMED is false); anything else is a wrong try.
 */
static void submitCode() {
  entry[entryLen] = '\0';
  if (strcmp(entry, DOOR_CODE) == 0) {
    wrongTries = 0;
    openDoor();
  } else if (strcmp(entry, SECURITY_CODE) == 0) {
    wrongTries = 0;
    if (muteAlarm()) {}                               // alarm silenced, security stays ON
    else if (!ALWAYS_ARMED && !USE_SECURITY_PANEL) setArmed(!isArmed());
    else showMessage("NO ALARM");
  } else if (++wrongTries >= MAX_WRONG) {
    wrongTries = 0;
    lockedUntil = millis() + LOCKOUT_MS;
    beep(600);
  } else {
    showMessage("WRONG CODE");
    beep(300);
  }
  entryLen = 0;
}

/**
 * Handles one key: digits are collected, '*' clears (or, with nothing typed, shows the next LCD page),
 * '#' submits the typed code. Ignored during lockout.
 * @param k the pressed key
 */
static void handleKey(char k) {
  if (lockoutRemainingMs() > 0) return;
  beep(30);
  if (k == '*') { if (entryLen) entryLen = 0; else nextPageManual(); }
  else if (k == '#') { if (entryLen) submitCode(); }
  else if (entryLen < sizeof(entry) - 1) entry[entryLen++] = k;
}

/** Reads one key from the keypad and handles it. */
void updateAccess() {
  char k = keypad.getKey();
  if (k) handleKey(k);
}

/** @return number of digits typed so far (shown as stars on the LCD) */
uint8_t typedLength() { return entryLen; }

/** @return milliseconds left in the wrong-code lockout, 0 when not locked */
unsigned long lockoutRemainingMs() {
  long left = (long)(lockedUntil - millis());
  return left > 0 ? (unsigned long)left : 0;
}
