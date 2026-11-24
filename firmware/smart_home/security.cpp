#include "security.h"
#include "settings.h"
#include "display.h"

enum PanelState : uint8_t { PANEL_DISARMED, PANEL_MUTE, PANEL_WINDOW_CLOSED, PANEL_WINDOW_OPEN };

static bool armed = false;
static bool alarmActive = false;
static bool lastWindowOpen = false;
static bool pirMotion = false;
static unsigned long motionStartedAt = 0;
static unsigned long lastMotionAt = 0;
static bool extLightOn = false;

static bool windowStable = false, windowPending = false;
static unsigned long windowPendingSince = 0;
static PanelState panelState = PANEL_DISARMED, pendingState = PANEL_DISARMED;
static unsigned long pendingSince = 0;

/** Sets pin modes; arms immediately when ALWAYS_ARMED. */
void securityInit() {
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_REED, INPUT_PULLUP);
  pinMode(PIN_LED_OUT, OUTPUT);
  digitalWrite(PIN_LED_OUT, LOW);
  armed = ALWAYS_ARMED;
}

/**
 * Arms or disarms security. Disarming clears the alarm and the exterior light.
 * @param on true to arm
 */
void setArmed(bool on) {
  if (on == armed) return;
  armed = on;
  lastWindowOpen = false;  // an already-open window triggers right after arming
  if (!on) {
    alarmActive = false;
    extLightOn = false;
    showMessage("SECURITY OFF");
  } else {
    showMessage("SECURITY ON");
  }
}

/**
 * Silences a sounding alarm; it sounds again only when the window is opened again.
 * @return true if an alarm was silenced, false if none was active
 */
bool muteAlarm() {
  if (!alarmActive) return false;
  alarmActive = false;
  showMessage("ALARM MUTED");
  return true;
}

/**
 * Debounced window state from the reed, so loose wires cannot restart a silenced alarm.
 * @return true when the window is open and stable for WINDOW_DEBOUNCE_MS
 */
static bool readWindowOpen() {
  bool raw = digitalRead(PIN_REED) == WINDOW_OPEN_LEVEL;
  if (raw != windowPending) {
    windowPending = raw;
    windowPendingSince = millis();
  } else if (raw != windowStable && millis() - windowPendingSince >= WINDOW_DEBOUNCE_MS) {
    windowStable = raw;
  }
  return windowStable;
}

/**
 * Converts the J12 panel analog value (ARM + reed + MUTE ladder) into a state.
 * @param raw analogRead of the reed pin with the internal pull-up enabled
 * @return decoded panel state
 */
static PanelState decodePanel(int raw) {
  if (raw < PANEL_DISARMED_MAX) return PANEL_DISARMED;
  if (raw < PANEL_MUTE_MAX) return PANEL_MUTE;
  if (raw < PANEL_CLOSED_MAX) return PANEL_WINDOW_CLOSED;
  return PANEL_WINDOW_OPEN;
}

/** Panel mode: accepts a new panel state only after it is stable for PANEL_DEBOUNCE_MS. */
static void updatePanel() {
  PanelState now = decodePanel(analogRead(PIN_REED));
  if (now != pendingState) {
    pendingState = now;
    pendingSince = millis();
  } else if (now != panelState && millis() - pendingSince >= PANEL_DEBOUNCE_MS) {
    panelState = now;
    if (panelState == PANEL_MUTE) muteAlarm();
  }
}

/** Reads the window and the PIR; latches the alarm and drives the exterior light. */
void updateSecurity() {
  unsigned long now = millis();
  bool motion = digitalRead(PIN_PIR) == HIGH;
  if (motion && !pirMotion) motionStartedAt = now;
  pirMotion = motion;

  bool windowKnown = true, windowOpen = false;
  if (USE_SECURITY_PANEL) {
    updatePanel();
    setArmed(ALWAYS_ARMED || panelState != PANEL_DISARMED);
    windowKnown = panelState != PANEL_MUTE;  // window state is unknown while MUTE is held
    windowOpen = panelState == PANEL_WINDOW_OPEN;
  } else {
    windowOpen = readWindowOpen();
  }

  if (windowKnown) {
    if (armed && windowOpen && !lastWindowOpen) alarmActive = true;
    lastWindowOpen = windowOpen;
  }

  if (armed && motion) {
    lastMotionAt = now;
    extLightOn = true;
  }
  if (extLightOn && now - lastMotionAt >= EXT_LIGHT_MS) extLightOn = false;
  if (!armed) extLightOn = false;
  digitalWrite(PIN_LED_OUT, extLightOn ? HIGH : LOW);
}

/** @return true while security is armed */
bool isArmed() { return armed; }

/** @return true while the alarm is sounding */
bool isAlarmActive() { return alarmActive; }

/** @return true while the PIR reports motion */
bool isPirMotion() { return pirMotion; }

/** @return true while the exterior light is on */
bool isExtLightOn() { return extLightOn; }

/** @return millis() when the latest motion started (0 if none yet) */
unsigned long pirMotionStartedAt() { return motionStartedAt; }
