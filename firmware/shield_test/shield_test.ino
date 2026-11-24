/*
 * Smart Home Shield Rev 3.1 - device test sketch
 * Open Serial Monitor at 9600 baud, line ending "Newline", and type a test number.
 * Keep the keypad UNPLUGGED for tests 1-10 (D0/D1 are the USB serial lines).
 *
 * Libraries (Library Manager): LiquidCrystal I2C (Frank de Brabander), Servo,
 * DHTlib (Rob Tillaart), Keypad (Mark Stanley).
 */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <dht.h>          // DHTlib by Rob Tillaart (Lab 4)
#include <Keypad.h>

// ---- Rev 3.1 pin map ----
const uint8_t PIN_BUZZER = 7;
const uint8_t PIN_LED_IN = 8;
const uint8_t PIN_LED_OUT = 9;
const uint8_t PIN_PIR = 10;
const uint8_t PIN_SERVO = 11;
const uint8_t PIN_BUTTON = 12;
const uint8_t PIN_FAN = 13;
const uint8_t PIN_LDR = A0;
const uint8_t PIN_KNOB = A1;
const uint8_t PIN_DHT = A2;
const uint8_t PIN_REED = A3;

// ---- test durations (change these to test longer or shorter) ----
const unsigned long TEST_SECONDS = 30;   // button, reed, PIR, LDR/knob
const uint8_t DHT_READINGS = 10;         // one reading every 2 s

const uint8_t SERVO_CLOSED = 90;  // arm mounted pointing LEFT at this angle
const uint8_t SERVO_OPEN = 0;     // opens by turning the other way (-90)

const byte ROWS = 4, COLS = 3;
char keys[ROWS][COLS] = {{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'}};
byte rowPins[ROWS] = {6, 5, 4, 3};
byte colPins[COLS] = {2, 1, 0};

LiquidCrystal_I2C *lcd = nullptr;
Servo door;
dht DHT;

/**
 * Scans the I2C bus for the LCD backpack.
 * @return 7-bit I2C address, or 0 if nothing answers
 */
uint8_t findLcdAddress() {
  for (uint8_t a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) return a;
  }
  return 0;
}

/**
 * Writes two lines on the LCD if it was found.
 * @param l1 text for line 1
 * @param l2 text for line 2
 */
void lcdShow(const char *l1, const char *l2) {
  if (!lcd) return;
  lcd->clear();
  lcd->setCursor(0, 0); lcd->print(l1);
  lcd->setCursor(0, 1); lcd->print(l2);
}

/** Prints the test menu. No params, no return. */
void printMenu() {
  Serial.println(F("\n=== Smart Home Shield test ==="));
  Serial.println(F(" 1  LCD (J13)"));
  Serial.println(F(" 2  LEDs outdoor (J5, D9) + indoor (J6, D8)"));
  Serial.println(F(" 3  Buzzer (BZ1, D7)"));
  Serial.println(F(" 4  Button (J2, D12)"));
  Serial.println(F(" 5  Window reed (J12, A3)"));
  Serial.println(F(" 6  PIR (J4, D10)"));
  Serial.println(F(" 7  LDR (J9, A0) + knob (J10, A1)"));
  Serial.println(F(" 8  DHT11 (J11, A2)"));
  Serial.println(F(" 9  Servo door (J3, D11)"));
  Serial.println(F("10  Fan (J7 + J8 supply, L293D in socket)"));
  Serial.println(F(" k  Keypad (J1) - ends Serial, shows keys on the LCD"));
  Serial.println(F(" p  Security panel on J12: ARM + window reed + MUTE"));
  Serial.print(F("Choose: "));
}

/** Test 1: finds the LCD and prints a message. No params, no return. */
void testLcd() {
  uint8_t addr = findLcdAddress();
  if (!addr) { Serial.println(F("FAIL: no I2C device. Check J13 wiring (SDA/SCL swapped?)")); return; }
  Serial.print(F("LCD found at 0x")); Serial.println(addr, HEX);
  if (!lcd) { lcd = new LiquidCrystal_I2C(addr, 16, 2); lcd->init(); lcd->backlight(); }
  lcdShow("SMART HOME", "LCD OK");
  Serial.println(F("If the screen is blank, turn the blue contrast screw on the back."));
}

/** Test 2: blinks the outdoor then the indoor LEDs three times. No params, no return. */
void testLeds() {
  const uint8_t pins[2] = {PIN_LED_OUT, PIN_LED_IN};
  const char *names[2] = {"Outdoor (J5)", "Indoor (J6)"};
  for (uint8_t i = 0; i < 2; i++) {
    Serial.print(names[i]); Serial.println(F(" blinking..."));
    for (uint8_t n = 0; n < 3; n++) {
      digitalWrite(pins[i], HIGH); delay(400);
      digitalWrite(pins[i], LOW); delay(400);
    }
  }
  Serial.println(F("No light? LED reversed: long leg goes to L, short leg to G."));
}

/** Test 3: three short beeps. No params, no return. */
void testBuzzer() {
  for (uint8_t n = 0; n < 3; n++) {
    digitalWrite(PIN_BUZZER, HIGH); delay(200);
    digitalWrite(PIN_BUZZER, LOW); delay(200);
  }
  Serial.println(F("Buzzer beeped 3 times? If silent, it may be a passive buzzer."));
}

/**
 * Prints a digital input every time it changes, for a fixed time.
 * @param pin input pin to watch
 * @param ms how long to watch, in milliseconds
 * @param lowText text when the pin reads LOW
 * @param highText text when the pin reads HIGH
 */
void watchInput(uint8_t pin, unsigned long ms, const char *lowText, const char *highText) {
  int last = -1;
  unsigned long end = millis() + ms;
  while ((long)(end - millis()) > 0) {
    int v = digitalRead(pin);
    if (v != last) { Serial.println(v == LOW ? lowText : highText); last = v; }
    delay(20);
  }
}

/** Test 7: prints LDR and knob readings for TEST_SECONDS. No params, no return. */
void testAnalog() {
  for (unsigned long n = 0; n < TEST_SECONDS * 2; n++) {
    Serial.print(F("LDR A0 = ")); Serial.print(analogRead(PIN_LDR));
    Serial.print(F("   knob A1 = ")); Serial.println(analogRead(PIN_KNOB));
    delay(500);
  }
  Serial.println(F("Cover the LDR and turn the knob: both numbers must change."));
}

/** Test 8: reads the DHT11 DHT_READINGS times. No params, no return. */
void testDht() {
  for (uint8_t n = 0; n < DHT_READINGS; n++) {
    delay(2000);
    int chk = DHT.read11(PIN_DHT);   // 0 = OK, 1 = checksum error, 2 = timeout
    if (chk != DHTLIB_OK) { Serial.print(F("FAIL: DHT11 status ")); Serial.print(chk); Serial.println(F(". Check S/G/V on J11.")); continue; }
    float t = DHT.temperature, h = DHT.humidity;
    Serial.print(F("Temp = ")); Serial.print(t, 1); Serial.print(F(" C   Humidity = ")); Serial.print(h, 0); Serial.println(F(" %"));
  }
}

/**
 * Waits until the user sends a line on Serial.
 * @return the trimmed line that was typed
 */
String waitForLine() {
  while (!Serial.available()) {}
  String in = Serial.readStringUntil('\n');
  in.trim();
  return in;
}

/**
 * Test 9: moves the servo to the closed angle and holds it so the arm can be mounted.
 * Any input (e.g. ok) moves it to the open angle and holds; the next input closes it again.
 */
void testServo() {
  door.attach(PIN_SERVO);
  door.write(SERVO_CLOSED);
  Serial.print(F("Servo CLOSED at ")); Serial.print(SERVO_CLOSED);
  Serial.println(F(" and holding. Mount the arm pointing LEFT."));
  Serial.println(F("Type ok + Enter to open."));
  waitForLine();
  door.write(SERVO_OPEN);
  Serial.print(F("Servo OPEN at ")); Serial.print(SERVO_OPEN);
  Serial.println(F(" and holding. Type ok + Enter to close."));
  waitForLine();
  door.write(SERVO_CLOSED);
  Serial.println(F("Servo CLOSED and holding."));
}

/** Test 10: runs the fan for 3 s. No params, no return. */
void testFan() {
  Serial.println(F("Fan ON for 3 s (needs L293D in socket and 5-6 V on J8)"));
  digitalWrite(PIN_FAN, HIGH); delay(3000);
  digitalWrite(PIN_FAN, LOW);
  Serial.println(F("Fan OFF"));
}

/** Keypad test (k): stops Serial and shows keypad presses on the LCD for 60 s. No params, no return. */
void testKeypad() {
  if (!lcd) testLcd();
  Serial.println(F("Serial stops now. Plug the keypad into J1 and press keys; watch the LCD."));
  Serial.flush();
  Serial.end();
  Keypad kp = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
  lcdShow("KEYPAD TEST", "Press keys...");
  char line[17] = "";
  uint8_t len = 0;
  unsigned long end = millis() + 60000UL;
  while ((long)(end - millis()) > 0) {
    char k = kp.getKey();
    if (!k) continue;
    if (len >= 16) len = 0;
    line[len++] = k; line[len] = '\0';
    lcdShow("KEYPAD TEST", line);
  }
  lcdShow("KEYPAD DONE", "Press RESET");
  while (true) {}
}

/**
 * Security panel test (p): ARM switch, window reed + 10k and MUTE button + 2.2k are all
 * in parallel on J12 (S and G). Prints the raw A3 value and the decoded state whenever it changes.
 */
void testPanel() {
  Serial.println(F("Flip ARM, move the magnet, press MUTE..."));
  const char *names[4] = {"DISARMED (ARM off)", "MUTE pressed", "ARMED - window CLOSED", "ARMED - window OPEN"};
  int lastState = -1;
  unsigned long end = millis() + TEST_SECONDS * 1000UL;
  while ((long)(end - millis()) > 0) {
    int raw = analogRead(PIN_REED);
    int st = raw < 12 ? 0 : raw < 145 ? 1 : raw < 700 ? 2 : 3;
    if (st != lastState) {
      lastState = st;
      Serial.print(F("A3 = ")); Serial.print(raw);
      Serial.print(F("  ->  ")); Serial.println(names[st]);
    }
    delay(60);
  }
}

/** Sets pin modes and starts serial. No params, no return. */
void setup() {
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_IN, OUTPUT);
  pinMode(PIN_LED_OUT, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_REED, INPUT_PULLUP);
  digitalWrite(PIN_FAN, LOW);
  Serial.begin(9600);
  Wire.begin();
  printMenu();
}

/** Waits for a test number on Serial and runs that test. No params, no return. */
void loop() {
  if (!Serial.available()) return;
  String in = Serial.readStringUntil('\n');
  in.trim();
  if (in.length() == 0) return;
  Serial.println(in);
  if (in.equalsIgnoreCase("k")) { testKeypad(); return; }
  if (in.equalsIgnoreCase("p")) { testPanel(); printMenu(); return; }
  int choice = in.toInt();
  switch (choice) {
    case 1: testLcd(); break;
    case 2: testLeds(); break;
    case 3: testBuzzer(); break;
    case 4: Serial.println(F("Press the button...")); watchInput(PIN_BUTTON, TEST_SECONDS * 1000UL, "PRESSED", "released"); break;
    case 5: Serial.println(F("Move the magnet...")); watchInput(PIN_REED, TEST_SECONDS * 1000UL, "window CLOSED (magnet near)", "window OPEN"); break;
    case 6: Serial.println(F("Wait still, then move in front of the PIR...")); watchInput(PIN_PIR, TEST_SECONDS * 1000UL, "no motion", "MOTION"); break;
    case 7: testAnalog(); break;
    case 8: testDht(); break;
    case 9: testServo(); break;
    case 10: testFan(); break;
    default: Serial.println(F("Unknown test"));
  }
  printMenu();
}
