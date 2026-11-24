/**
 * access.h - keypad input: door code, security code, lockout and LCD page selection.
 * Named "access" (not "keypad") to avoid clashing with the Keypad library header.
 */
#pragma once
#include <Arduino.h>

void updateAccess();
uint8_t typedLength();
unsigned long lockoutRemainingMs();
