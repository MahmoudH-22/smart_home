/** door.h - main door access (project function 1): hobby servo with <Servo.h> (Lab 6), auto-close. */
#pragma once
#include <Arduino.h>

void doorInit();
void openDoor();
void closeDoor();
void updateDoor();
bool isDoorOpen();
