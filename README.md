# Automatic Gate Barrier Control System 🚧

Arduino UNO based boom-barrier controller: **PIR motion sensor** flags an approaching vehicle, **HC-SR04 ultrasonic sensor** measures the clear distance under the boom arm, and an **SG90 servo** raises/lowers the barrier. A **16x2 I2C LCD** shows live state and a green/red LED pair indicates lane status.

Developed for the *Project Management (2307476T)* course, MIT AOE — Activity 2: GitHub-Based QA Documentation and Problem Solving.

**Author:** Raviraj Sonar · PRN 202301070167 · Batch B4 · Division C

## Hardware Pin Configuration

| Component | Pin Type | Arduino Pin | Description |
|---|---|---|---|
| PIR Motion Sensor | Digital Input | D2 | Detects vehicle presence at entry |
| HC-SR04 TRIG | Digital Output | D3 | 10 µs trigger pulse per sensing cycle |
| HC-SR04 ECHO | Digital Input | D4 | Echo pulse width → distance in cm |
| Green / Red LEDs | Digital Output | D5 / D6 | Lane-passable / lane-blocked indication |
| SG90 Servo Motor | PWM Output | D9 | Barrier arm: 0° closed, 90° open |
| 16x2 I2C LCD (0x27) | I2C SDA/SCL | A4 / A5 | State, distance and PIR sample count |

## Firmware Architecture

- **Non-blocking cooperative scheduling** — `millis()` timers (`SENSOR_TICK_MS = 50` sensing, `SERVO_SWEEP_MS = 15` servo stepping); no `delay()` in the main loop.
- **Finite state machine** — `IDLE → VEHICLE_DETECTED → BARRIER_OPEN → BARRIER_CLOSING → IDLE`, with a 2.5 s guard-down dwell.
- **Debounced input** — PIR must read HIGH for 10 consecutive samples before a vehicle is confirmed.
- **Hysteresis** — 5 cm distance dead-band prevents open/close stutter at the clearance threshold.
- **Timeout-safe ultrasonic read** — 30 ms `pulseIn` timeout; a missing echo is treated as "lane clear" (999 cm fallback) instead of hanging the firmware.

## QA Protocol

All defects found during bench testing are logged as [Issues](../../issues) using the repository issue template:
expected vs. actual behaviour, reproduction steps, 5-Why root cause analysis, severity, and the commit that
closes the issue (`fixes #N`).

| Issue | Title | Severity | Status | Fix |
|---|---|---|---|---|
| #1 | Blocking `delay()` stalls the main loop | High | ✅ Closed | `fixes #1` — millis() scheduler |
| #2 | Hysteresis missing → barrier/LED stutter at threshold | High | ✅ Closed | `fixes #2` — 5 cm dead-band |
| #3 | PIR false triggers from thermal transients | Medium | ✅ Closed | `fixes #3` — 10-sample debounce |
| #4 | Ultrasonic no-echo freezes state machine | High | ✅ Closed | `fixes #4` — pulseIn timeout |
| #5 | LCD backlight on permanently at idle | Low | ✅ Closed | `fixes #5` — idle backlight power mode |

## Build

Open `gate_barrier_system.ino` in the Arduino IDE, select Arduino UNO, install the **LiquidCrystal_I2C** library, and upload over USB. Serial monitor at **9600 baud** streams live telemetry.
