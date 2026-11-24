#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "display.h"
#include "settings.h"
#include "access.h"
#include "door.h"
#include "security.h"
#include "lighting.h"
#include "climate.h"

static LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);
static char shownLine[2][17] = {"", ""};
static char msgText[17] = "";
static unsigned long msgUntil = 0;
static uint8_t page = 0;
static unsigned long pageChangedAt = 0;
static unsigned long manualPageUntil = 0;

/** Starts the I2C LCD (Lab 4 further activities: SDA = A4, SCL = A5) and shows the start-up message. */
void displayInit() {
  lcd.init();
  lcd.backlight();
  showMessage("SMART HOME");
}

/**
 * Shows a temporary message on line 1 for MSG_MS.
 * @param text message, up to 16 characters
 */
void showMessage(const char *text) {
  strncpy(msgText, text, 16);
  msgText[16] = '\0';
  msgUntil = millis() + MSG_MS;
}

/** Jumps to the next status page and holds it for MANUAL_PAGE_MS. */
void nextPageManual() {
  page = (page + 1) % LCD_PAGES;
  pageChangedAt = millis();
  manualPageUntil = millis() + MANUAL_PAGE_MS;
}

/**
 * Writes one LCD line only when its text changed (avoids flicker).
 * @param row 0 or 1
 * @param text up to 16 characters
 */
static void lcdLine(uint8_t row, const char *text) {
  char padded[17];
  snprintf(padded, sizeof(padded), "%-16s", text);
  if (strcmp(padded, shownLine[row]) == 0) return;
  strcpy(shownLine[row], padded);
  lcd.setCursor(0, row);
  lcd.print(padded);
}

/**
 * Builds line 1 from the alarm, message, lockout and door/security state.
 * @param out buffer of 17 chars
 */
static void buildLine1(char *out) {
  unsigned long now = millis();
  unsigned long locked = lockoutRemainingMs();
  if (isAlarmActive()) strcpy(out, "!! INTRUSION !!");
  else if ((long)(msgUntil - now) > 0) strcpy(out, msgText);
  else if (locked > 0) snprintf(out, 17, "LOCKED %lus", locked / 1000 + 1);
  else snprintf(out, 17, "Door:%s Sec:%s", isDoorOpen() ? "OPN" : "CLS", isArmed() ? "ON" : "OFF");
}

/**
 * Builds line 2: stars while typing, otherwise the current status page
 * (0 temperature/humidity, 1 threshold/fan, 2 lamp/LDR voltage, 3 PIR).
 * Pages advance every LCD_PAGE_MS unless one was picked with '*'; new motion shows the PIR page.
 * @param out buffer of 17 chars
 */
static void buildLine2(char *out) {
  unsigned long now = millis();
  uint8_t typed = typedLength();
  if (typed > 0) {
    char stars[9] = "";
    for (uint8_t i = 0; i < typed && i < 8; i++) stars[i] = '*';
    stars[typed < 8 ? typed : 8] = '\0';
    snprintf(out, 17, "Code: %s", stars);
    return;
  }
  bool manual = (long)(manualPageUntil - now) > 0;
  if (!manual && now - pageChangedAt >= LCD_PAGE_MS) {
    page = (page + 1) % LCD_PAGES;
    pageChangedAt = now;
  }
  bool pirForced = !manual && pirMotionStartedAt() != 0 && now - pirMotionStartedAt() < PIR_PAGE_MS;
  uint8_t show = pirForced ? 3 : page;

  char t[6] = "--.-", v[5] = "-.--";
  if (!isnan(getTemperature())) dtostrf(getTemperature(), 4, 1, t);
  dtostrf(getLdrVolts(), 4, 2, v);
  switch (show) {
    case 0:
      if (isnan(getHumidity())) snprintf(out, 17, "T:%sC H:--%%", t);
      else snprintf(out, 17, "T:%sC H:%d%%", t, (int)getHumidity());
      break;
    case 1: snprintf(out, 17, "Set:%dC Fan:%s", (int)getThreshold(), isFanOn() ? "ON" : "OFF"); break;
    case 2: snprintf(out, 17, "Lamp:%s L:%sV", isLampOn() ? "ON " : "OFF", v); break;
    default: snprintf(out, 17, "PIR:%s Ext:%s", isPirMotion() ? "MOVE" : "----", isExtLightOn() ? "ON" : "OFF"); break;
  }
}

/** Refreshes both LCD lines. */
void updateDisplay() {
  char l1[17], l2[17];
  buildLine1(l1);
  buildLine2(l2);
  lcdLine(0, l1);
  lcdLine(1, l2);
}
