#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* ============================================================
 * gate_barrier_system.ino  --  BASELINE VERSION (pre-QA build)
 * Automatic Gate Barrier Control System
 * Raviraj Sonar | PRN 202301070167 | MIT AOE
 *
 * NOTE: This baseline intentionally contains the happy-path
 * logic only. QA issues #1-#5 are logged in GitHub Issues and
 * resolved on feature branches (see commit history).
 * ============================================================ */

Servo barrierServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int PIR_PIN   = 2;   // PIR motion sensor
const int TRIG_PIN  = 3;   // HC-SR04 trigger
const int ECHO_PIN  = 4;   // HC-SR04 echo
const int GREEN_LED = 5;
const int RED_LED   = 6;
const int SERVO_PIN = 9;

const int ANGLE_CLOSED      = 0;
const int ANGLE_OPEN        = 90;
const int CLEAR_DISTANCE_CM = 30;  // clearance threshold under boom

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
  lcd.print("Gate Barrier Ready");

  Serial.begin(9600);
}

int readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(4);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  // BUG #4: no timeout on pulseIn() -- if no echo returns,
  // this call hangs forever and the whole state machine stalls.
  unsigned long echo = pulseIn(ECHO_PIN, HIGH);
  return (int)(echo / 58);
}

void loop() {
  // BUG #1: delay(200) blocks the CPU every cycle -- the LCD
  // cannot refresh faster than 5 Hz, servo stepping is jerky,
  // and future features (buzzer, alarms) cannot run concurrently.
  delay(200);

  int pir = digitalRead(PIR_PIN);
  int distanceCm = readDistanceCm();

  // BUG #2: single hard threshold with no hysteresis -- jitter of
  // +/-1 cm around 30 makes the barrier/LED stutter open&closed.
  if (pir == HIGH && distanceCm < CLEAR_DISTANCE_CM) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    lcd.clear();
    lcd.print("Opening...");
    barrierServo.write(ANGLE_OPEN);   // instant jump, no ramping
    delay(2500);                      // guard-down dwell (blocking)
    lcd.clear();
    lcd.print("Go!");
  } else {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    lcd.clear();
    lcd.print("Lane blocked");
    barrierServo.write(ANGLE_CLOSED); // instant jump, no ramping
  }

  // BUG #3: any single PIR HIGH counts as a vehicle -- stray
  // thermal pulses (sunlight / heater) falsely open the barrier.

  // BUG #5: backlight is switched on once in setup() and never
  // powered down -- constant ~25 mA drain even when idle.
}
