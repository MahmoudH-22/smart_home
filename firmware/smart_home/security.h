/**
 * security.h - security system (project function 2): window reed and PIR inputs,
 * buzzer and exterior LEDs as alerts (Lab 3 style multi-level alerts).
 * Window opening while armed latches the alarm until SECURITY_CODE is entered.
 */
#pragma once
#include <Arduino.h>

void securityInit();
void updateSecurity();
void setArmed(bool armed);
bool muteAlarm();
bool isArmed();
bool isAlarmActive();
bool isPirMotion();
bool isExtLightOn();
unsigned long pirMotionStartedAt();
