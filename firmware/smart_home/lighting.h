/**
 * lighting.h - automatic indoor lighting (project function 3).
 * LDR voltage divider (Lab 6) read through the ADC (Lab 4), LIGHT button override, indoor LEDs (Lab 2).
 */
#pragma once
#include <Arduino.h>

void lightingInit();
void updateLighting();
bool isLampOn();
float getLdrVolts();
