/**
 * display.h - LCD feedback (project function 5), 16x2 LCD with I2C adapter (Lab 4).
 * Line 1: alarm, temporary message, lockout, or door/security status.
 * Line 2: typed code, or status pages (temperature/humidity, threshold/fan, lamp/LDR, PIR).
 */
#pragma once
#include <Arduino.h>

void displayInit();
void showMessage(const char *text);
void nextPageManual();
void updateDisplay();
