#include "buzzer.h"
#include "settings.h"

/** Sets the buzzer pin as an output, silent. */
void buzzerInit() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
}

/**
 * Short blocking beep used for key feedback and error tones.
 * @param ms beep length in milliseconds
 */
void beep(unsigned int ms) {
  digitalWrite(PIN_BUZZER, HIGH);
  delay(ms);
  digitalWrite(PIN_BUZZER, LOW);
}

/**
 * Pulses the buzzer while the alarm is active, keeps it silent otherwise.
 * @param alarmActive true while the intrusion alarm is sounding
 */
void updateBuzzer(bool alarmActive) {
  if (alarmActive) digitalWrite(PIN_BUZZER, (millis() / 300UL) % 2 ? HIGH : LOW);
  else digitalWrite(PIN_BUZZER, LOW);
}
