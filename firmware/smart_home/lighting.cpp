#include "lighting.h"
#include "settings.h"

static bool lightOverride = false;
static bool indoorDark = false;
static bool lampOn = false;
static float ldrVolts = 0;
static bool lastButton = HIGH;
static unsigned long lastButtonChange = 0;

/** Sets the LIGHT button (internal pull-up, pressed = LOW) and the indoor LED output. */
void lightingInit() {
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED_IN, OUTPUT);
  digitalWrite(PIN_LED_IN, LOW);
}

/** Toggles the manual override on each debounced LIGHT button press. */
static void updateButton() {
  bool b = digitalRead(PIN_BUTTON);
  if (b != lastButton && millis() - lastButtonChange > BUTTON_DEBOUNCE_MS) {
    lastButtonChange = millis();
    lastButton = b;
    if (b == LOW) lightOverride = !lightOverride;
  }
}

/**
 * Lab 6 voltage divider read with the Lab 4 ADC formula: V = analogRead * 5 / 1023.
 * @return LDR divider voltage in volts (higher = brighter)
 */
static float readLdrVolts() {
  return analogRead(PIN_LDR) * ADC_REF_V / ADC_MAX;
}

/** Indoor light = dark (voltage below LDR_DARK_V, off above LDR_BRIGHT_V) OR manual override. */
void updateLighting() {
  updateButton();
  ldrVolts = readLdrVolts();
  if (ldrVolts < LDR_DARK_V) indoorDark = true;
  else if (ldrVolts > LDR_BRIGHT_V) indoorDark = false;
  lampOn = indoorDark || lightOverride;
  digitalWrite(PIN_LED_IN, lampOn ? HIGH : LOW);
}

/** @return true while the indoor light is on */
bool isLampOn() { return lampOn; }

/** @return latest LDR divider voltage in volts */
float getLdrVolts() { return ldrVolts; }
