#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* ============================================================
 * gate_barrier_system.ino  --  PRODUCTION VERSION (post-QA)
 * Automatic Gate Barrier Control System
 * Raviraj Sonar | PRN 202301070167 | MIT AOE
 *
 * All five GitHub issues (#1-#5) resolved:
 *   #1 millis() non-blocking scheduler replaces delay(200)
 *   #2 5 cm hysteresis band on lane-clear decisions
 *   #3 10-sample PIR confirmation window
 *   #4 30 ms pulseIn timeout + 999 cm "lane clear" fallback
 *   #5 LCD backlight auto power-save after 10 s idle
 * ============================================================ */

Servo barrierServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int PIR_PIN   = 2;
const int TRIG_PIN  = 3;
const int ECHO_PIN  = 4;
const int GREEN_LED = 5;
const int RED_LED   = 6;
const int SERVO_PIN = 9;

const int ANGLE_CLOSED      = 0;
const int ANGLE_OPEN        = 90;
const int CLEAR_DISTANCE_CM = 30;   // clearance threshold
const int DIST_HYST_CM      = 5;    // #2 hysteresis band
const int PIR_SETTLE_SAMPLE = 10;   // #3 consecutive HIGH samples

const unsigned long SENSOR_TICK_MS  = 50;   // #1 non-blocking tick
const unsigned long SERVO_SWEEP_MS  = 15;   // #1 servo stepping pace
const unsigned long GUARD_DOWN_MS   = 2500; // arm-up hold before closing
const unsigned long LCD_IDLE_MS     = 10000; // #5 backlight power-save

enum BarrierState { IDLE, VEHICLE_DETECTED, BARRIER_OPEN, BARRIER_CLOSING };
BarrierState state = IDLE;

int confirmedSamples        = 0;   // #3 debounce counter
unsigned long lastSensorTick = 0;
unsigned long lastServoStep  = 0;
unsigned long guardTimerStart = 0;
unsigned long lastActivityMs  = 0;  // #5 backlight tracking
bool backlightToggled = true;       // #5 current backlight state

bool lcdBacklightIsOn() {
  // LiquidCrystal_I2C library does not expose backlight state;
  // track it in a variable to keep decisions cheap.
  return backlightToggled;
}

int readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(4);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  // #4: explicit timeout so a missing echo can never hang loop()
  unsigned long echo = pulseIn(ECHO_PIN, HIGH, 30000UL);
  return echo == 0 ? 999 : (int)(echo / 58);
}

void moveServoToward(int target) {
  if (millis() - lastServoStep >= SERVO_SWEEP_MS) {
    lastServoStep = millis();
    int cur = barrierServo.read();
    if (cur < target) barrierServo.write(min(cur + 1, target));
    else if (cur > target) barrierServo.write(max(cur - 1, target));
  }
}

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  barrierServo.attach(SERVO_PIN);
  barrierServo.write(ANGLE_CLOSED);

  lcd.init();
  lcd.backlight();
  backlightToggled = true;
  lcd.print("Gate Barrier Ready");

  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();

  // #5: backlight auto power-save after 10 s of no activity
  bool blIsOn = lcdBacklightIsOn();
  if (now - lastActivityMs >= LCD_IDLE_MS && blIsOn) {
    lcd.noBacklight();
    backlightToggled = false;
  }
  if (state != IDLE || confirmedSamples > 0) {
    if (!blIsOn) { lcd.backlight(); backlightToggled = true; }
    lastActivityMs = now;
  }

  // #1: non-blocking sensor tick replaces delay(200)
  if (now - lastSensorTick < SENSOR_TICK_MS) return;
  lastSensorTick = now;

  int pir = digitalRead(PIR_PIN);
  int distanceCm = readDistanceCm();

  switch (state) {
    case IDLE:
      // #3: require 10 consecutive HIGH samples - stray pulses reset
      if (pir == HIGH) {
        confirmedSamples++;
        if (confirmedSamples < PIR_SETTLE_SAMPLE) break;
        confirmedSamples = 0;
        state = VEHICLE_DETECTED;
      } else {
        confirmedSamples = 0;
      }
      break;

    case VEHICLE_DETECTED:                     // open barrier
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
      lcd.clear();
      lcd.print("Opening...");
      state = BARRIER_OPEN;
      break;

    case BARRIER_OPEN:
      moveServoToward(ANGLE_OPEN);
      if (barrierServo.read() >= ANGLE_OPEN) {
        guardTimerStart = millis();
        lcd.clear();
        lcd.print("Go!  ");
        lcd.print(distanceCm);
        lcd.print(" cm");
        state = BARRIER_CLOSING;
      }
      break;

    case BARRIER_CLOSING:
      // #2: close only after guard time AND lane clear beyond
      // CLEAR_DISTANCE_CM + DIST_HYST_CM -> no open/close stutter
      if (millis() - guardTimerStart >= GUARD_DOWN_MS &&
          distanceCm > CLEAR_DISTANCE_CM + DIST_HYST_CM) {
        moveServoToward(ANGLE_CLOSED);
        if (barrierServo.read() <= ANGLE_CLOSED) {
          digitalWrite(GREEN_LED, LOW);
          digitalWrite(RED_LED, HIGH);
          lcd.clear();
          lcd.print("Lane blocked");
          state = IDLE;
        }
      }
      break;
  }

  Serial.print("State: ");
  Serial.print(state);
  Serial.print(" | Dist: ");
  Serial.print(distanceCm);
  Serial.print(" cm | PIR samples: ");
  Serial.println(confirmedSamples);
}
