/*

  Abstraction of the basic motor functions.

*/

#ifndef Motor_h
#define Motor_h

#include "Arduino.h"
#include <AccelStepper.h>

class Motor {
public:

  Motor(){};

  Motor(
    String name,
    int pin1,
    int pin2,
    int pin3,
    int pin4,
    int homeSensorPin,
    bool reverse_direction = false,
    bool homeSensorHomeState = true) {
    _name = name;
    _homeSensorPin = homeSensorPin;
    _homeSensorHomeState = homeSensorHomeState;

    /*
      For the 28BYJ-48 stepper motor, the wiring by default is as follows:
      - IN1   Blue    B- 
      - IN2   Pink    A+
      - IN3   Yellow  B+
      - IN4   Orange  A-
      - Vmot  Red
    
      So for a normal setup order is B-, B+, A+, A-  == 1, 3, 2, 4

      To reverse use A-, A+, B+, B- == 4, 2, 3, 1 
    */

    if (reverse_direction) {
      _pin1 = pin4;
      _pin2 = pin3;
      _pin3 = pin2;
      _pin4 = pin1;
    } else {
      _pin1 = pin1;
      _pin2 = pin2;
      _pin3 = pin3;
      _pin4 = pin4;
    }
  };

  void setup() {

    _stepper = AccelStepper(
      AccelStepper::HALF4WIRE,
      _pin1,
      _pin3,
      _pin2,
      _pin4);


    fast_mode();

    pinMode(_homeSensorPin, INPUT_PULLUP);

    _current_angle = 0;

    // starting values that should be corrected by calibration
    _steps_per_rotation = 2048 * 2;  // basic value for a 28BYJ stepper
    _steps_across_home_sensor = 0;
  };


  void calibrate() {
    // rotate detecting when the home position is passed and store the steps per rotation and current angle

    // forget where we think we are and set a target a long way away
    _stepper.setCurrentPosition(0);
    _stepper.moveTo(_steps_per_rotation * 100);  // 100 revolutions of motor

    // keep going until we reach home
    while (_home_detected())
      _stepper.run();
    while (!_home_detected())
      _stepper.run();

    long first_stop = _stepper.currentPosition();

    // go again until we stop again
    while (_home_detected())
      _stepper.run();
    _steps_across_home_sensor = _stepper.currentPosition() - first_stop;
    while (!_home_detected())
      _stepper.run();

    long second_stop = _stepper.currentPosition();
    _steps_per_rotation = second_stop - first_stop;


    // stop the motion and return to the center of the home position gently. Remember to overshoot so that we return in the correct direction to avoid the effects of backlash in the gearing.
    float overshoot_amount = 10.0 / 360 * _steps_per_rotation;
    float overshoot_pos = second_stop - overshoot_amount;

    _stepper.moveTo(overshoot_pos);
    while (_stepper.isRunning()) _stepper.run();

    _stepper.moveTo(second_stop + _steps_across_home_sensor / 2);
    while (_stepper.run()) 1;

    // reset the current angle with the center of home as 0
    _current_angle = 0;
  };

  float measure_angle_to_home() {
    long starting_position = _stepper.currentPosition();

    _stepper.moveTo(starting_position + _steps_per_rotation * 100);  // 100 revolutions of motor

    while (!_home_detected())
      _stepper.run();

    _stepper.move(_steps_across_home_sensor / 2);
    while (_stepper.run()) 1;


    long found_home = _stepper.currentPosition();
    long steps_taken = found_home - starting_position;
    float angle = steps_taken * (360.0 / _steps_per_rotation);

    _current_angle = 0;

    Serial.print("Angle to home: ");
    Serial.println(angle);

    return angle;
  };

  float goto_angle(float angle) {

    // if angle is less than current add 360, we only want to move in one direction
    while (angle < _current_angle) {
      angle += 360;
    }

    float angle_delta = angle - _current_angle;
    if (angle_delta > 5) {
      Serial.println("largish angle " + String(angle_delta));
    }



    // Sometimes the movement of one part of clock puts us slightly ahead of where we
    // should be. Instead of doing a full loop just wait for the angle wanted to catch
    // up with us
    if (angle_delta > 355.0) {
      Serial.println("Skipping movement for angle " + String(angle_delta));
      return 0.0;
    }

    // calculate the number of steps to move
    int steps_to_move = round(angle_delta / 360.0 * _steps_per_rotation);

    Serial.println("steps_to_take" + _name + ":" + String(steps_to_move));

    if (steps_to_move < 20) {  // FIXME move this to a constant
      return 0.0;
    }

    // move to the target position
    _stepper.move(steps_to_move);
    while (_stepper.run()) 1;

    // update the current angle
    float degrees_per_step = 360.0 / _steps_per_rotation;
    _current_angle += steps_to_move * degrees_per_step;

    // keep the current angle in the range 0-360
    while (_current_angle > 360)
      _current_angle -= 360.0;

    return angle_delta;
  };


  void adjust_angle(float angle) {
    _current_angle += angle;

    while (_current_angle >= 360)
      _current_angle -= 360.0;
  };

  void fast_mode() {
    _stepper.setMaxSpeed(1000);
    _stepper.setAcceleration(1000.0);
  };

  void slow_mode() {
    _stepper.setMaxSpeed(1000);
    _stepper.setAcceleration(200.0);
  };

  void sleep() {
    _stepper.disableOutputs();
  };

  void wake() {
    _stepper.enableOutputs();
  };

  String debug(String note = "") {
    String out = "";

    out += "#### " + _name + " - " + note + " ####\n";

    out += "# _steps_per_rotation: ";
    out += _steps_per_rotation;
    out += "\n";

    out += "# _steps_across_home_sensor: ";
    out += _steps_across_home_sensor;
    out += "\n";

    out += "# current steps position: ";
    out += _stepper.currentPosition();
    out += "\n";

    out += "# current angle: ";
    out += _current_angle;
    out += "\n";

    out += "\n";

    return out;
  };
private:
  AccelStepper _stepper;
  String _name;
  int _pin1;
  int _pin2;
  int _pin3;
  int _pin4;

  int _homeSensorPin;
  bool _homeSensorHomeState;

  bool _home_detected() {
    return digitalRead(_homeSensorPin) == _homeSensorHomeState;
  };

  float _current_angle;

  int _steps_per_rotation;
  int _steps_across_home_sensor;
};

#endif
