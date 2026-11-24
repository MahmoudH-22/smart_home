/**
 * climate.h - climate control (project function 4).
 * DHT11 temperature/humidity (Lab 4, DHTlib), knob threshold through the ADC (Lab 4),
 * fan through the L293D H-bridge (Lab 6).
 */
#pragma once
#include <Arduino.h>

void climateInit();
void updateClimate();
float getTemperature();
float getHumidity();
float getThreshold();
bool isFanOn();
bool isDhtOk();
