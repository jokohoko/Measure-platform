/*
 * Measure platform  AKA  Prusa spare hacks
 * 
 * The code below is the first implemenation based on the spare prusa Parts
 * Based on the spares of an old MK2 printer
 * Using the Mini Rambo (the one with plug in power connectors)
 * Using the X axis with end stop
 * Start button connected to pin Z_MIN
 */

//#include "AD520X.h"
//#include "configuration_einsy.h";
#include "configuration_mini_rambo.h";
//digipots of type AD5206 for motor current
#define ACTIVE LOW
#define RELEASED HIGH
#define SLOW_SPEED 200
#define GOOD_SPEED 100
#define FAST_SPEED 50
#define STEPSPERMM 400
long position = -1;

//AD5206 pot = AD5206(38, -1, -1, &SPI); //select, reset, shutdown, &SPI

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(X_MS1_PIN, OUTPUT);
  pinMode(X_MS2_PIN, OUTPUT);
  digitalWrite(X_MS1_PIN, HIGH);
  digitalWrite(X_MS2_PIN, HIGH);
  pinMode(X_MIN_PIN, INPUT_PULLUP);
  pinMode(Z_MIN_PIN, INPUT_PULLUP);
  /*SPI.begin();
    pot.begin(4);
    pot.setValue(0, 100);
  */
  analogWrite(MOTOR_CURRENT_PWM_XY_PIN, &00); //255=2A, 100 = 0.8A?
}

void loop() {
  homingsequence();
  while (digitalRead(Z_MIN_PIN));//Wachten op start
  movedistance(225, GOOD_SPEED); // move 225mm up
}

void movedistance(float distance, int speed) {
  long steps = (float) distance * STEPSPERMM;
  check_safety(steps);
  digitalWrite(X_DIR_PIN, (steps > 0) ? HIGH : LOW);
  for (long i = 0; i < abs(distance); i++) { //400 steps / mm
    digitalWrite(X_STEP_PIN, HIGH);
    digitalWrite(X_STEP_PIN, LOW);
    delayMicroseconds(speed);
  }
  position += steps;
}

void movesteps(long steps, int speed) {
  check_safety(steps);
  digitalWrite(X_DIR_PIN, (steps > 0) ? LOW : HIGH);
  for (long i = 0; i < abs(steps); i++) { //400 steps / mm
    digitalWrite(X_STEP_PIN, HIGH);
    digitalWrite(X_STEP_PIN, LOW);
    delayMicroseconds(speed);
  }
  position += steps;
}

void check_safety(long steps) {
  if ((position + steps) > (275 * STEPSPERMM)) {
    Serial.println("ALARM, this code goes above the end limit");
    while (true);
  }
}

void movetohome(int speed) { //move towards home until endstop is triggered
  digitalWrite(X_DIR_PIN, LOW);
  while (!digitalRead(X_MIN_PIN)) {
    digitalWrite(X_STEP_PIN, HIGH);
    digitalWrite(X_STEP_PIN, LOW);
    delayMicroseconds(SLOW_SPEED);
  }
}

void homingsequence() {
  digitalWrite(X_ENABLE_PIN, ACTIVE);
  movetohome(GOOD_SPEED);        delay(100); // home
  movedistance(2, GOOD_SPEED);   delay(100); // backup a tad, Move up 2mm
  movetohome(SLOW_SPEED);        delay(100); // slow home
  movedistance(2.5, GOOD_SPEED); delay(100); // move up 2.5mm again
  position = 0;
}
