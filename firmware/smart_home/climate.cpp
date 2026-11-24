#include <dht.h>          // DHTlib by Rob Tillaart (Lab 4)
#include "climate.h"
#include "settings.h"

static dht DHT;
static float temperature = NAN;
static float humidity = NAN;
static float threshold = 30.0;
static bool fanOn = false;
static bool dhtOk = false;
static unsigned long lastDhtRead = 0;

/**
 * Lab 6 L293D truth table for channel 2 on the shield:
 *   EN1,2 is wired to 5V, IN1 (pin 2) is wired to GND, IN2 (pin 7) = PIN_FAN.
 *   IN1 LOW + IN2 HIGH -> motor spins | IN1 LOW + IN2 LOW -> motor OFF.
 * @param on true to run the fan
 */
static void driveFan(bool on) {
  digitalWrite(PIN_FAN, on ? HIGH : LOW);
}

/** Configures the fan output (off). The DHT11 needs no setup with DHTlib. */
void climateInit() {
  pinMode(PIN_FAN, OUTPUT);
  driveFan(false);
}

/**
 * Lab 4 ADC conversion: Vout = analogRead * Vref / (2^n - 1).
 * @param pin analog pin to read
 * @return voltage on the pin in volts
 */
static float readVolts(uint8_t pin) {
  return analogRead(pin) * ADC_REF_V / ADC_MAX;
}

/**
 * Reads the knob (Lab 4 potentiometer: 0-5 V on the wiper) and maps it to a temperature threshold.
 * @return threshold in deg C between TEMP_MIN and TEMP_MAX
 */
static float readThreshold() {
  return TEMP_MIN + (TEMP_MAX - TEMP_MIN) * readVolts(PIN_KNOB) / ADC_REF_V;
}

/**
 * Reads the DHT11 with DHTlib (Lab 4). read11() returns 0 = OK, 1 = checksum error, 2 = timeout.
 * Keeps the last good values when a read fails.
 */
static void readDht() {
  int chk = DHT.read11(PIN_DHT);
  dhtOk = (chk == DHTLIB_OK);
  if (dhtOk) {
    temperature = DHT.temperature;
    humidity = DHT.humidity;
  }
}

/** Closed loop (Lab 2): sensor = DHT11, setpoint = knob, actuator = fan, with hysteresis. */
void updateClimate() {
  threshold = readThreshold();
  if (millis() - lastDhtRead >= DHT_PERIOD_MS) {
    lastDhtRead = millis();
    readDht();
  }
  if (!isnan(temperature)) {
    if (temperature > threshold) fanOn = true;
    else if (temperature < threshold - FAN_HYSTERESIS) fanOn = false;
  }
  driveFan(fanOn);
}

/** @return last valid temperature in deg C, or NAN before the first reading */
float getTemperature() { return temperature; }

/** @return last valid relative humidity in %, or NAN before the first reading */
float getHumidity() { return humidity; }

/** @return fan threshold in deg C set by the knob */
float getThreshold() { return threshold; }

/** @return true while the fan is running */
bool isFanOn() { return fanOn; }

/** @return true if the latest DHT11 read succeeded */
bool isDhtOk() { return dhtOk; }
