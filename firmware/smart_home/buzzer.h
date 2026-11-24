/** buzzer.h - active buzzer on D7 (Lab 3): key beeps and the pulsing alarm sound. */
#pragma once
#include <Arduino.h>

void buzzerInit();
void beep(unsigned int ms);
void updateBuzzer(bool alarmActive);
