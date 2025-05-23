/*
  Code that abstracts the motors of the two arms
*/

#include "Arduino.h"
#include "Motor.h"

Motor::Motor(int pin1, int pin2, int pin3, int pin4) {
  _pin1 = pin1;
  _pin2 = pin2;
  _pin3 = pin3;
  _pin4 = pin4;

  AccelStepper ourStepper(
    AccelStepper::HALF4WIRE,
    _pin1,
    _pin3,  // Note 2 and 3 swapped for 28BYJ motor wiring
    _pin2,
    _pin4);

  _stepper = ourStepper;

  _stepper.setMaxSpeed(1000);
  _stepper.setAcceleration(1000.0);
}

void Motor::moveTo(long position) {
  _stepper.moveTo(position);
}

void Motor::run() {
  _stepper.run();
}

String Motor::debug() {
  String out = "";
  out += _pin1;
  return out;
}
