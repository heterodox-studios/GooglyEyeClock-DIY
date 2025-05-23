/*
  Code that abstracts the motors of the two arms
*/

#include "Arduino.h"
#include "Motor.h"

Motor::Motor(int pin1, int pin2, int pin3, int pin4, int homeSensorPin) {
  _pin1 = pin1;
  _pin2 = pin2;
  _pin3 = pin3;
  _pin4 = pin4;
  _homeSensorPin = homeSensorPin;

  AccelStepper ourStepper(
    AccelStepper::HALF4WIRE,
    _pin1,
    _pin3,  // Note 2 and 3 swapped for 28BYJ motor wiring
    _pin2,
    _pin4);

  _stepper = ourStepper;
  _stepper.setMaxSpeed(1000);
  _stepper.setAcceleration(1000.0);

  pinMode(_homeSensorPin, INPUT_PULLUP);

  // starting values that should be corrected by calibration
  _steps_per_rotation = 2048 * 2;  // basic value for a 28BYJ stepper
}

bool Motor::_home_detected() {
  return !digitalRead(_homeSensorPin);
}

void Motor::calibrate() {
  // rotate detecting when the home position is passed and store the steps per rotation and current angle

  // forget where we think we are and set a target a long way away
  _stepper.setCurrentPosition(0);
  _stepper.moveTo(_steps_per_rotation * 100);  // 100 revolutions of motor

  // keep going until we reach home
  while (_home_detected()) _stepper.run();
  while (!_home_detected()) _stepper.run();

  long first_stop = _stepper.currentPosition();
  Serial.println(first_stop);

  // go again until we stop again
  while (_home_detected()) _stepper.run();
  while (!_home_detected()) _stepper.run();

  long second_stop = _stepper.currentPosition();
  Serial.println(second_stop);

  // stop the motion and return to the home position gently
  _stepper.moveTo(_stepper.currentPosition());
  while (_stepper.run()) 1;

  // store the calibration results
  _steps_per_rotation = second_stop - first_stop;
  Serial.println(_steps_per_rotation);
}





String Motor::debug() {
  String out = "";
  out += _pin1;
  return out;
}
