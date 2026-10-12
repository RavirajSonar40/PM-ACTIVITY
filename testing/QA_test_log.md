# QA Test Log — Automatic Gate Barrier Control System

Bench-test evidence supporting each GitHub issue's conclusion. Each test was executed before and after the
referenced fix commit, on the hardware below.

**Environment:** USB 5 V bench power · ambient ~26 °C · foam-block mock vehicle · Arduino IDE Serial Monitor @ 9600 baud.

**Tester:** Raviraj Sonar (PRN 202301070167) · **Activity 2, CO2 · Project Management (2307476T)**

## Test Setup

| Item | Value |
|---|---|
| Board | Arduino UNO R3 (ATmega328P) |
| Firmware under test | `gate_barrier_system.ino` (baseline `65de664` → production) |
| Sensors | PIR (D2), HC-SR04 ultrasonic (D3/D4) |
| Actuators | SG90 servo (D9), LEDs (D5/D6), 16x2 I2C LCD |
| Serial telemetry | 9600 baud |

## Test 1 — Issue #1: Main-Loop Blocking (delay → millis)

| Run | Firmware | Test | Measured | Result |
|---|---|---|---|---|
| 1.1 | baseline `65de664` | Count LCD state changes over 10 s with vehicle parked | 5 updates in 10 s (5 Hz, frozen feel) | ❌ FAIL |
| 1.2 | baseline `65de664` | Servo sweep motion 0→90° | Visible jerky steps | ❌ FAIL |
| 1.3 | post-fix `43b846c` | LCD state changes over 10 s | 20 updates in 10 s (20 Hz) | ✅ PASS |
| 1.4 | post-fix `43b846c` | Servo sweep motion 0→90° | Smooth continuous ramp | ✅ PASS |

**Conclusion:** non-blocking `millis()` scheduling restored responsiveness; confirmed fix for Issue #1.

## Test 2 — Issue #2: Threshold Hysteresis (open/close stutter)

| Run | Firmware | Test | Measured | Result |
|---|---|---|---|---|
| 2.1 | baseline `65de664` | Hold mock vehicle at 29–31 cm, observe barrier state over 60 s | 14 spurious open/close toggles | ❌ FAIL |
| 2.2 | post-fix `e4cb6d2` | Same test, 60 s | 0 toggles | ✅ PASS |
| 2.3 | post-fix `e4cb6d2` | Target sweep 26→34 cm and back, 5 repetitions | 1 state change per direction (expected behaviour) | ✅ PASS |

**Conclusion:** 5 cm dead-band eliminated boundary chatter; confirmed fix for Issue #2.

## Test 3 — Issue #3: PIR False Trigger (debounce)

| Run | Firmware | Test | Measured | Result |
|---|---|---|---|---|
| 3.1 | baseline `65de664` | Direct warm-air draft at PIR dome for 2 s | Barrier opened (false trigger) | ❌ FAIL |
| 3.2 | baseline `65de664` | Sunlight transient across PIR window | Barrier opened (false trigger) | ❌ FAIL |
| 3.3 | post-fix `ef917c4` | 5-minute thermal-interference soak (heater + sunlight) | 0 false opens | ✅ PASS |
| 3.4 | post-fix `ef917c4` | Real vehicle approach at walking speed | Confirmed in ~0.5 s (10 × 50 ms) | ✅ PASS |

**Conclusion:** 10-sample confirmation window filters transients without delaying real detection; confirmed fix for Issue #3.

## Test 4 — Issue #4: Ultrasonic No-Echo Hang (pulseIn timeout)

| Run | Firmware | Test | Measured | Result |
|---|---|---|---|---|
| 4.1 | baseline `65de664` | Aim HC-SR04 at open air (> 4 m), watch loop for 30 s | LCD frozen, servo stalled (never recovers) | ❌ FAIL |
| 4.2 | baseline `65de664` | Soft foam target absorbing echo | Same indefinite hang | ❌ FAIL |
| 4.3 | post-fix `109c782` | Same open-air test, 30 s | Distance reads `999` (lane clear), state machine continues | ✅ PASS |
| 4.4 | post-fix `109c782` | Soft foam target | Graceful "lane clear", LCD keeps refreshing | ✅ PASS |

**Conclusion:** explicit 30 ms timeout + `999` fallback removed the hang; confirmed fix for Issue #4.

## Test 5 — Issue #5: LCD Backlight Power-Save (enhancement)

| Run | Firmware | Test | Measured | Result |
|---|---|---|---|---|
| 5.1 | baseline `65de664` | Idle current draw after 2 min (multimeter, series USB) | 48 mA, backlight always on | ❌ Baseline |
| 5.2 | post-fix `061affc` | Idle current after > 10 s inactivity | 23 mA (backlight off) | ✅ PASS |
| 5.3 | post-fix `061affc` | Vehicle approaches after idle | Backlight restores instantly with first PIR sample | ✅ PASS |

**Conclusion:** ~52 % idle-power saving with no usability loss; enhancement confirmed for Issue #5.

## Regression Sweep (all fixes together, post-merge `4578a9c`)

| # | Scenario | Expected | Result |
|---|---|---|---|
| R1 | Normal vehicle pass (approach → open → clear → close) | One smooth open/close cycle | ✅ |
| R2 | Vehicle parked straddling 30 cm for 90 s | Barrier stays open, no stutter | ✅ |
| R3 | Empty lane for 5 min | Backlight times out, current drops | ✅ |
| R4 | Sunlight + heater interference | No false trigger | ✅ |
| R5 | Sensor aimed at open air | "Lane clear", loop keeps running | ✅ |

**Overall verdict:** all five issues verified fixed; no regressions observed. System declared production-ready at
commit `4578a9c`.
