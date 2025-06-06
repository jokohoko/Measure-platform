/*
   Measure platform  AKA  Prusa spare hacks

   The code below is the first implemenation based on the spare prusa Parts
   Based on the spares of an old MK2 printer
   Using the Mini Rambo (the one with plug in power connectors)
   Using the X axis with end stop
   Start button connected to pin Z_MIN
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
#define movement_length 245
long movement_length_steps;

//AD5206 pot = AD5206(38, -1, -1, &SPI); //select, reset, shutdown, &SPI

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  movement_length_steps = (long) movement_length * STEPSPERMM;
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(X_MS1_PIN, OUTPUT);
  pinMode(X_MS2_PIN, OUTPUT);
  digitalWrite(X_MS1_PIN, HIGH);
  digitalWrite(X_MS2_PIN, HIGH);
  pinMode(X_MIN_PIN, INPUT_PULLUP);
  pinMode(Z_MIN_PIN, INPUT_PULLUP);
  Serial.println("Distance");
  Serial.println("mm");
  Serial.println("0");
  /*SPI.begin();
    pot.begin(4);
    pot.setValue(0, 100);
  */
  analogWrite(MOTOR_CURRENT_PWM_XY_PIN, 100); //255=2A, 100 = 0.8A?
  //use 166 at most, this gives 1.3A to the motors (with a spec of 1A).
  //100 is a safer usage current of 0.8A to allow for errors
  //25 is enough to move without real strength.
  digitalWrite(X_ENABLE_PIN, ACTIVE);
}

void loop() {
  homingsequence();
  if (starttrigger()) { //trigger minimaal 1sec inhouden om te starten, anders reset
    for (int i = 0; i < movement_length; i++) {
      Serial.println(i);
      analogWrite(MOTOR_CURRENT_PWM_E_PIN, i);
      movedistance(1, GOOD_SPEED); // move 225mm up
    }
    delay(5000);
  }
}

boolean starttrigger() {
  while (digitalRead(Z_MIN_PIN)); delay(100); //Wachten op start
  long starttijd = millis();
  while (!digitalRead(Z_MIN_PIN)); delay(100); //timen hoe lang ingedrukt
  if ((millis() - starttijd) > 1000) {
    return true;
  }
  return false;
}

void movedistance(float distance, int speed) {
  long steps = (float) distance * STEPSPERMM;
  movesteps(steps, speed);
}

void movesteps(long steps, int speed) {
  analogWrite(MOTOR_CURRENT_PWM_XY_PIN, 100);
  check_safety(steps);
  digitalWrite(X_DIR_PIN, (steps > 0) ? HIGH : LOW);
  for (long i = 0; i < abs(steps); i++) { //400 steps / mm
    digitalWrite(X_STEP_PIN, HIGH);
    digitalWrite(X_STEP_PIN, LOW);
    delayMicroseconds(speed);
  }
  position += steps;
  analogWrite(MOTOR_CURRENT_PWM_XY_PIN, 50);
}

void check_safety(long steps) {
  if ((long) (position + steps) > movement_length_steps) {
    Serial.println("ALARM, this code goes above the end limit");
    while (true);
  }
}

void movetohome(int speed) { //move towards home until endstop is triggered
  Serial.println("Homing");
  digitalWrite(X_DIR_PIN, LOW);
  while (!digitalRead(X_MIN_PIN)) {
    digitalWrite(X_STEP_PIN, HIGH);
    digitalWrite(X_STEP_PIN, LOW);
    delayMicroseconds(speed);
  }
  position = -STEPSPERMM;
  analogWrite(MOTOR_CURRENT_PWM_E_PIN, 0);
}

void homingsequence() {
  digitalWrite(X_ENABLE_PIN, ACTIVE);
  movetohome(FAST_SPEED);        delay(100); // home
  movedistance(2, GOOD_SPEED);   delay(100); // backup a tad, Move up 2mm
  movetohome(SLOW_SPEED);        delay(100); // slow home
  movedistance(1, GOOD_SPEED);   delay(100); // move up 1mm again
}
