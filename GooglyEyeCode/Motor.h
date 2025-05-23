/*

  Abstarction of the basic motor functions.

*/

#ifndef Motor_h
#define Motor_h

#include "Arduino.h"
#include <AccelStepper.h>

class Motor {
public:
  Motor(String name, int pin1, int pin2, int pin3, int pin4, int homeSensorPin);
  void calibrate();
  void sleep();
  String debug();
private:
  AccelStepper _stepper;
  String _name;
  int _pin1;
  int _pin2;
  int _pin3;
  int _pin4;
  int _homeSensorPin;
  bool _home_detected();

  float _current_angle;
  float _target_angle;

  int _steps_per_rotation;
  int _steps_across_home_sensor;
};

#endif
