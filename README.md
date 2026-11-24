# Smart Home - MCT331s Project 1

Arduino Uno smart-home prototype for **MCT331s Mechatronic System Integration** (Ain Shams University, Fall 2026).
Runs on the **Smart Home Shield Rev 3.1** (single-sided PCB stacked on the Uno, see `hardware/`).

## Functions (project brief)

| # | Function | Behaviour |
|---|---|---|
| 1 | Main door access | `1234#` on the keypad opens the servo door; it closes after 5 s. 3 wrong codes lock the keypad for 10 s |
| 2 | Security system | Always armed. Opening the window sounds the alarm until `2580#` is entered. Motion near the door turns the exterior LEDs on |
| 3 | Indoor lighting | On when the LDR voltage drops below 2.9 V (off above 3.4 V) or when the LIGHT button toggles it on |
| 4 | Climate control | Fan runs when the DHT11 temperature exceeds the knob threshold (20-40 °C, 1 °C hysteresis) |
| 5 | LCD feedback | Line 1: door/security/messages. Line 2: temperature & humidity, threshold & fan, lamp & LDR voltage, PIR. `*` picks the page |

## Code structure (one subsystem per file)

```
smart_home/
├── smart_home.ino     setup() + loop() only
├── settings.h         every pin and tunable value
├── access.h/.cpp      keypad: codes, lockout, LCD page key
├── door.h/.cpp        servo door + auto-close
├── security.h/.cpp    window reed alarm, PIR exterior light
├── lighting.h/.cpp    LDR divider + LIGHT button, indoor LEDs
├── climate.h/.cpp     DHT11, knob threshold, fan via L293D
├── display.h/.cpp     16x2 I2C LCD
└── buzzer.h/.cpp      key beeps, alarm sound
shield_test/           device-by-device test sketch (serial menu)
hardware/              PCB Gerbers, schematic, workshop PDF, BOM
history/               daily work log
```

## Course methods used

| Lab | Method | Where |
|---|---|---|
| Lab 1 - PCB prototyping | Schematic, PCB layout, Gerber + drill output, DRC | `hardware/` (Rev 3.1 shield) |
| Lab 2 - Arduino basics | `pinMode`/`digitalWrite` outputs, closed-loop sensor → controller → actuator | LEDs, buzzer, fan loop in `climate.cpp` |
| Lab 3 - Parking alert | Digital sensor inputs + buzzer/LED alerts | `security.cpp`, `buzzer.cpp` |
| Lab 4 - Weather station | ADC formula `V = reading × 5 / 1023`, potentiometer, DHT11 with **DHTlib** `read11()`, I2C LCD | `climate.cpp`, `lighting.cpp`, `display.cpp` |
| Lab 5 - Git | Repository, commits, GitHub remote | this repository |
| Lab 6 - Light tracking | LDR voltage divider, hobby servo with `<Servo.h>`, L293D H-bridge truth table | `lighting.cpp`, `door.cpp`, `climate.cpp` |

## Pin map (Rev 3.1 shield)

| Function | Arduino | Connector |
|---|---|---|
| Keypad rows R1-R4 / cols C1-C3 | D6-D3 / D2-D0 | J1 |
| Buzzer | D7 | BZ1 (on board) |
| Indoor LEDs / outdoor LEDs | D8 / D9 | J6 / J5 |
| PIR | D10 | J4 |
| Servo (door) | D11 | J3 |
| LIGHT button | D12 | J2 |
| Fan (L293D IN2; EN tied to 5 V, IN1 to GND) | D13 | J7, motor supply on J8 |
| LDR (+ 10 kΩ to GND) | A0 | J9 |
| Knob (threshold) | A1 | J10 |
| DHT11 | A2 | J11 |
| Window reed | A3 | J12 |
| LCD I2C | A4 SDA, A5 SCL | J13 |

## Build and upload

1. Libraries (Library Manager): **Keypad** (Mark Stanley), **LiquidCrystal I2C** (Frank de Brabander), **DHTlib** (Rob Tillaart). `Servo` is built in.
   If the Adafruit "DHT sensor library" is installed, remove it: its `DHT.h` clashes with DHTlib's `dht.h` on Windows.
2. **Unplug the keypad** (it uses D0/D1, the USB serial lines), upload `smart_home/smart_home.ino`, plug the keypad back in and press RESET.
3. Serial is not used by the firmware for the same reason; use `shield_test/` for serial debugging with the keypad unplugged.

All behaviour (codes, servo angles, timings, thresholds) is set in `smart_home/settings.h`.

## Git workflow (Lab 5)

```bash
git config --global user.name "Mahmoud Hisham Mahmoud"
git config --global user.email mahmoud0hisham22@gmail.com
git init
git add .
git commit -m "Smart home firmware split into subsystems"
git branch -M main
git remote add origin https://github.com/<username>/mct331-smart-home.git
git push -u origin main
```
Teammates: `git clone <url>`, then `git pull` before working and `git push` after committing.

