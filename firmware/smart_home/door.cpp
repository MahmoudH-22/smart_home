#include <Servo.h>
#include "door.h"
#include "settings.h"
#include "display.h"

static Servo doorServo;
static bool doorOpen = false;
static unsigned long doorOpenedAt = 0;

/** Attaches the servo and starts with the door closed. */
void doorInit() {
  doorServo.attach(PIN_SERVO);
  closeDoor();
}

/** Opens the door and starts the auto-close timer. */
void openDoor() {
  doorServo.write(SERVO_OPEN);
  doorOpen = true;
  doorOpenedAt = millis();
  showMessage("ACCESS GRANTED");
}

/** Closes the door. */
void closeDoor() {
  doorServo.write(SERVO_CLOSED);
  doorOpen = false;
}

/** Closes the door automatically after DOOR_OPEN_MS. */
void updateDoor() {
  if (doorOpen && millis() - doorOpenedAt >= DOOR_OPEN_MS) closeDoor();
}

/**
 * @return true while the door is open
 */
bool isDoorOpen() {
  return doorOpen;
}
