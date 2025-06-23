/*

  Abstraction of the basic motor functions.

*/

#ifndef Motor_h
#define Motor_h

#include "Arduino.h"
#include <AccelStepper.h>

// float normalizeAngle(float angle)
// {
//   while (angle < 0)
//     angle += 360.0;
//   while (angle >= 360)
//     angle -= 360.0;
//   return angle;
// }

class Motor
{
public:
  Motor() {};

  Motor(
      String name,
      int pin1,
      int pin2,
      int pin3,
      int pin4,
      int homeSensorPin,
      bool reverse_direction = false,
      bool homeSensorHomeState = true)
  {
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

    if (reverse_direction)
    {
      _pin1 = pin4;
      _pin2 = pin3;
      _pin3 = pin2;
      _pin4 = pin1;
    }
    else
    {
      _pin1 = pin1;
      _pin2 = pin2;
      _pin3 = pin3;
      _pin4 = pin4;
    }

    _stepper = AccelStepper(
        AccelStepper::HALF4WIRE,
        _pin1,
        _pin3,
        _pin2,
        _pin4);

    fast_mode();

    // pinMode(_homeSensorPin, INPUT_PULLUP);

    // _current_angle = 0;
    // _unadjusted_angle = 0;

    // // starting values that should be corrected by calibration
    // _stepsPerRotation = 0;
    // _stepsAcrossHome = 0;
  };

  //   void calibrateUsingInterupts()
  //   {
  //     // Serial.println("Calibrating motor using interrupts...");
  //     // printCalibrationStatus();

  //     // _resetCalibration();
  //     enableAutoCalibration();

  //     // printCalibrationStatus();

  //     fast_mode();
  //     _stepper.move(1000000);

  //     while (!_hasBeenCalibrated)
  //       _stepper.run();
  //   };

  //   // FIXME - should be private, but made public to enable faster start up when developing
  //   int _stepsPerRotation = 0;
  //   int _stepsAcrossHome = 0;

  //   bool _autoCalibrationEnabled = false;
  //   bool _hasBeenCalibrated = false;
  //   bool _stepsAcrossHomeHasBeenCalibrated = false;
  //   bool _stepsPerRotationHasBeenCalibrated = false;
  //   bool _currentAngleHasBeenCalibrated = false;

  //   int _previous_entering_home = 0;
  //   int _previous_leaving_home = 0;
  //   int _most_recent_calibration_position = 0; // use to track no movement or backwards movement

  //   void enableAutoCalibration()
  //   {
  //     _autoCalibrationEnabled = true;
  //   };

  //   void disableAutoCalibration()
  //   {
  //     _autoCalibrationEnabled = false;
  //   };

  //   void autoCalibrateFromInterupt(bool state)
  //   {
  //     // // Serial.println("\nMotor autoCalibrateFromInterupt called with state: " + String(state));
  //     int current_position = _stepper.currentPosition();

  //     if (_autoCalibrationEnabled == false)
  //     {
  //       // // Serial.println("Auto calibration is disabled, skipping ISR handling.");
  //       return;
  //     }

  //     // Serial.println("### autoCalibrate " + _name + "###");

  //     if (!_autoCalibrateIsMovementSane(current_position))
  //     {
  //       // // Serial.println("Auto calibration sanity check failed, skipping ISR handling.");
  //       return;
  //     }

  //     if (state == true)
  //     {
  //       _autoCalibrateEnterHome(current_position);
  //     }
  //     else
  //     {
  //       _autoCalibrateLeaveHome(current_position);
  //     }

  //     if (_stepsAcrossHomeHasBeenCalibrated && _stepsPerRotationHasBeenCalibrated && _currentAngleHasBeenCalibrated)
  //     {
  //       _hasBeenCalibrated = true;
  //     }

  //     // printCalibrationStatus();
  //   }

  //   bool _autoCalibrateIsMovementSane(int current_position)
  //   {
  //     int recent_calibration_position = _most_recent_calibration_position;

  //     // no movement or backwards or too small to be realistic
  //     if (current_position <= recent_calibration_position)
  //     {
  //       // Serial.println("No movement or backwards - skipping calibration.");
  //       return false;
  //     }
  //     if (abs(current_position - recent_calibration_position) < 10)
  //     {
  //       // Serial.println("Movement too small to be realistic - skipping calibration.");
  //       return false;
  //     }

  //     _most_recent_calibration_position = current_position;
  //     return true;
  //   }

  //   void _autoCalibrateCurrentAngle(int angleModifier // -1 for entry, +1 for exit
  //   )
  //   {
  //     // if we have calibrated both steps across home and steps per rotation, we
  //     // can calculate the angle across home - the center of the home sensor is at 0 degrees.
  //     if (_stepsAcrossHomeHasBeenCalibrated && _stepsPerRotationHasBeenCalibrated)
  //     {

  //       // We don't need to worry about number of rotation, we know we are at true north here

  //       float angle_across_home = float(_stepsAcrossHome) / float(_stepsPerRotation) * 360.0;
  //       float angleAtRelativeToHome = normalizeAngle(angleModifier * angle_across_home / 2.0);

  //       float angle_adjustment = angleAtRelativeToHome - _current_angle;
  //       while (angle_adjustment > 180.0)
  //         angle_adjustment -= 360.0;
  //       while (angle_adjustment < -180.0)
  //         angle_adjustment += 360.0;

  //       Serial.println(
  //           String(_name) +                                      //
  //           " Calibrating  angle from " + String(_current_angle) //
  //           + " to " + String(angleAtRelativeToHome)             //
  //           + ": delta of " + String(angle_adjustment)           //
  //       );

  //       _current_angle = angleAtRelativeToHome;
  //       _unadjusted_angle = angleAtRelativeToHome;
  //       _currentAngleHasBeenCalibrated = true;
  //     }
  //     else
  //     {
  //       Serial.println(
  //           String(_name) + " Didn't calibrate " + String(_name) //
  //           + " angle"                                           //
  //       );
  //     }
  //   }

  //   /*
  //   ______________#home#____________________________#home#______
  //   case A                                    *---->
  //   case B           *----------------------------->
  //   case C  *-------------------------------------->
  //   */
  //   void _autoCalibrateEnterHome(int current_position)
  //   {
  //     int last_entering_position = _previous_entering_home;
  //     _previous_entering_home = current_position;

  //     // Serial.println(String(_name) + " _previous_leaving_home  " + String(_previous_leaving_home));
  //     // Serial.println(String(_name) + " _previous_entering_home " + String(last_entering_position));
  //     // Serial.println(String(_name) + " current_position        " + String(current_position));

  //     // case A and case B
  //     if (!last_entering_position)
  //     {
  //       // Serial.println(String(_name) + " Case A or B: skipping calibration.");
  //       return;
  //     }

  //     // case C
  //     // if (last_entering_position)
  //     // {

  //     //   float number_of_rotations_since_last_calibration = _autoCalibrateRotationsSinceLastCalibration(+1);

  //     //   _stepsPerRotation = (current_position - last_entering_position) /
  //     //                       number_of_rotations_since_last_calibration;

  //     //   _stepsPerRotationHasBeenCalibrated = true;
  //     //   Serial.println(String(_name) + " Case C: calibrated _stepsPerRotation to " + String(_stepsPerRotation));

  //     //   // only do this here after we have calibrated the steps per rotation so it can't mess up out rotation count
  //     //   _autoCalibrateCurrentAngle(-1); // -1 for entry
  //     // }
  //   }

  //   float _autoCalibrateRotationsSinceLastCalibration(int rotationDirection)
  //   {
  //     // We can't assume we've rotated just once since the last calibration, so we need to
  //     // calculate the number of rotations since the last calibration.

  //     float rotation_angle = _unadjusted_angle;

  //     if (rotation_angle)
  //     {
  //       // If non-zero adjust with the 1/2 angle of home
  //       rotation_angle += rotationDirection * 0.5 * 360 * float(_stepsAcrossHome) / float(_stepsPerRotation);
  //     }
  //     else
  //     {
  //       rotation_angle = 360.0; // assumption
  //     }

  //     float number_of_rotations_since_last_calibration = rotation_angle / 360.0;

  //     // Serial.println(String(_name) + "   Current angle:       " + String(_current_angle));
  //     // Serial.println(String(_name) + "   Unadjusted angle:    " + String(_unadjusted_angle));
  //     // Serial.println(String(_name) + "   Rotation angle:      " + String(rotation_angle));
  //     // Serial.println(String(_name) + "   Number of rotations: " + String(number_of_rotations_since_last_calibration));

  //     return number_of_rotations_since_last_calibration;
  //   };

  //   /*
  //   ______________#home#____________________________#home#______
  //   case D                                             *->
  //   case E                                    *---------->
  //   case F           *----------------------------------->
  //   */
  //   void _autoCalibrateLeaveHome(int current_position)
  //   {
  //     // leaving home - store values
  //     int last_leaving_position = _previous_leaving_home;
  //     _previous_leaving_home = current_position;

  //     // Serial.println(String(_name) + " _previous_leaving_home  " + String(last_leaving_position));
  //     // Serial.println(String(_name) + " _previous_entering_home " + String(_previous_entering_home));
  //     // Serial.println(String(_name) + " current_position        " + String(current_position));

  //     // case D
  //     if (!_previous_entering_home)
  //     {
  //       // Serial.println(String(_name) + " Case D: skipping calibration.");
  //       return;
  //     }

  //     // case E
  //     if (_previous_entering_home)
  //     {
  //       _stepsAcrossHome = current_position - _previous_entering_home;
  //       _stepsAcrossHomeHasBeenCalibrated = true;
  //       Serial.println(String(_name) + " Case E: calibrated _stepsAcrossHome to " + String(_stepsAcrossHome));
  //     }

  //     // case F
  //     if (last_leaving_position)
  //     {

  //       float number_of_rotations_since_last_calibration = _autoCalibrateRotationsSinceLastCalibration(-1);

  //       _stepsPerRotation = (current_position - last_leaving_position) /
  //                           number_of_rotations_since_last_calibration;
  //       _stepsPerRotationHasBeenCalibrated = true;
  //       Serial.println(String(_name) + " Case F: calibrated _stepsPerRotation to " + String(_stepsPerRotation));

  //       // only do this here after we have calibrated the steps per rotation so it can't mess up out rotation count
  //       _autoCalibrateCurrentAngle(+1); // +1 for exit
  //     }
  //   };

  //   void printCalibrationStatus()
  //   {
  //     // Serial.println(String(_name) + " Steps per rotation: " + String(_stepsPerRotation));
  //     // Serial.println(String(_name) + " Steps across home sensor: " + String(_stepsAcrossHome));
  //     // Serial.println(String(_name) + " _previous_entering_home: " + String(_previous_entering_home));
  //     // Serial.println(String(_name) + " _previous_leaving_home: " + String(_previous_leaving_home));
  //     // Serial.println(String(_name) + " Steps per rotation calibrated: " + String(_stepsPerRotationHasBeenCalibrated));
  //     // Serial.println(String(_name) + " Steps across home sensor calibrated: " + String(_stepsAcrossHomeHasBeenCalibrated));
  //     // Serial.println(String(_name) + " Current angle: " + String(_current_angle));
  //     // Serial.println(String(_name) + " Unadjusted angle: " + String(_unadjusted_angle));
  //     // Serial.println(String(_name) + " Current position: " + String(_stepper.currentPosition()));
  //     // Serial.println(String(_name) + " Has been calibrated: " + String(_hasBeenCalibrated));
  //     // Serial.println(String(_name) + " Auto calibration enabled: " + String(_autoCalibrationEnabled));
  //     Serial.print("\n\n");
  //   };

  //   void _resetCalibration()
  //   {
  //     _autoCalibrationEnabled = false;

  //     _stepsAcrossHomeHasBeenCalibrated = false;
  //     _stepsPerRotationHasBeenCalibrated = false;
  //     _currentAngleHasBeenCalibrated = false;
  //     _hasBeenCalibrated = false;

  //     _previous_entering_home = 0;
  //     _previous_leaving_home = 0;
  //   };

  //   float measure_angle_to_home()
  //   {
  //     long starting_position = _stepper.currentPosition();

  //     findNoon();

  //     long found_home = _stepper.currentPosition();
  //     long steps_taken = found_home - starting_position;
  //     float angle = steps_taken * (360.0 / _stepsPerRotation);

  //     // Serial.print("Angle to home: ");
  //     // Serial.println(angle);

  //     return angle;
  //   };

  //   int findNoon()
  //   {
  //     // go to the noon position by rotating until the sensor triggers and then continuing half the sensor distance. Returns number of steps taken to find noon.

  //     int starting_position = _stepper.currentPosition();

  //     _stepper.move(_stepsPerRotation * 100); // 100 revolutions of motor

  //     // while (_home_detected())
  //     //   _stepper.run();
  //     while (!_home_detected())
  //       _stepper.run();

  //     _stepper.move(_stepsAcrossHome / 2);
  //     while (_stepper.run())
  //       1;

  //     _current_angle = 0;
  //     _unadjusted_angle = 0;

  //     return _stepper.currentPosition() - starting_position;
  //   };

  //   float gotoAngleDirect(float angle)
  //   {
  //     return _gotoAngle(angle, false);
  //   }

  //   float gotoAngleClockwise(float angle)
  //   {
  //     return _gotoAngle(angle, true);
  //   }

  //   float _gotoAngle(
  //       float angle,
  //       bool clockwiseOnly,
  //       float ignoreAngleLargerThan = 358.0)
  //   {
  //     angle = normalizeAngle(angle);

  //     // Serial.println(
  //     //     String(_name) +                                    //
  //     //     " _gotoAngle from " + String(_current_angle)       //
  //     //     + " (" + String(fmod(_current_angle, 360.0)) + ")" //
  //     //     + " to " + String(angle)                           //
  //     // );

  //     float angle_delta = angle - _current_angle;

  //     if (clockwiseOnly)
  //     {
  //       while (angle_delta < 0)
  //         angle_delta += 360.0;
  //       while (angle_delta > 360.0)
  //         angle_delta -= 360.0;

  //       if (angle_delta > ignoreAngleLargerThan)
  //       {
  //         // Sometimes the movement of one part of clock puts us slightly ahead of where we
  //         // should be. Instead of doing a full loop just wait for the angle wanted to catch
  //         // up with us
  //         // Serial.println("Skipping movement for angle " + String(angle_delta));
  //         return 0.0;
  //       }
  //     }
  //     else
  //     {
  //       // go directly, even if it means going backwards
  //       while (angle_delta > 180.0)
  //         angle_delta -= 360.0;
  //       while (angle_delta < -180.0)
  //         angle_delta += 360.0;
  //     }

  //     // // Serial.println("angle_delta: " + String(angle_delta));

  //     // if (angle_delta > 5) {
  //     //   // Serial.println("largish angle " + String(angle_delta));
  //     // }

  //     // calculate the number of steps to move
  //     float degrees_per_step = 360.0 / _stepsPerRotation;
  //     int steps_to_move = floor(angle_delta / 360.0 * _stepsPerRotation);
  //     // // Serial.println("steps_to_move_" + _name + ": " + String(steps_to_move));
  //     if (steps_to_move == 0)
  //       return 0.0;

  //     // if (steps_to_move < 20) {  // FIXME move this to a constant
  //     //   return 0.0;
  //     // }

  //     // move to the target position
  //     _stepper.move(steps_to_move);

  //     int steps_taken_recorded = 0;
  //     while (_stepper.isRunning())
  //     {
  //       _stepper.run();
  //       int steps_taken = steps_to_move - _stepper.distanceToGo();

  //       if (steps_taken != steps_taken_recorded)
  //       {
  //         int steps_taken_this_iteration = steps_taken - steps_taken_recorded;
  //         _current_angle += degrees_per_step * steps_taken_this_iteration;
  //         _unadjusted_angle += degrees_per_step * steps_taken_this_iteration;
  //         steps_taken_recorded += steps_taken_this_iteration;
  //       }
  //     }

  //     // // Serial.println("steps_to_move: " + String(steps_to_move));
  //     // // Serial.println("_stepsPerRotation: " + String(_stepsPer_rotation));
  //     // // Serial.println("_current_angle += " + String(steps_to_move * degrees_per_step));
  //     // // Serial.println("_unadjusted_angle += " + String(steps_to_move * degrees_per_step));

  //     // // Serial.println("Current angle after move: " + String(_current_angle));
  //     // // Serial.println("Unadjusted angle after move: " + String(_unadjusted_angle));

  //     return angle_delta;
  //   };

  //   void adjust_angle(float angle)
  //   {
  //     _current_angle += angle;
  //     // deliberately don't change _unadjusted_angle here
  //   };

  void fast_mode()
  {
    _stepper.setMaxSpeed(_maxSpeed);
    _stepper.setSpeed(_maxSpeed);
    _stepper.setAcceleration(1000.0);
  };

  void slow_mode()
  {
    _stepper.setMaxSpeed(_maxSpeed);
    _stepper.setSpeed(_maxSpeed);
    _stepper.setAcceleration(200.0);
  };

  //   void sleep()
  //   {
  //     _stepper.disableOutputs();
  //   };

  //   void wake()
  //   {
  //     _stepper.enableOutputs();
  //   };

  //   String debug(String note = "")
  //   {
  //     String out = "";

  //     out += "#### " + _name + " - " + note + " ####\n";

  //     out += "# _stepsPerRotation: ";
  //     out += _stepsPerRotation;
  //     out += "\n";

  //     out += "# _stepsAcrossHome: ";
  //     out += _stepsAcrossHome;
  //     out += "\n";

  //     out += "# angle across home sensor: ";
  //     out += 1.0 * _stepsAcrossHome / _stepsPerRotation * 360.0;
  //     out += " degrees\n";
  //     out += "\n";

  //     out += "# current steps position: ";
  //     out += _stepper.currentPosition();
  //     out += "\n";

  //     out += "# current angle: ";
  //     out += _current_angle;
  //     out += "\n";

  //     out += "# unadjusted angle: ";
  //     out += _unadjusted_angle;
  //     out += "\n";

  //     out += "\n";

  //     return out;
  //   };

  //   float getCurrentAngle()
  //   {
  //     return _current_angle;
  //   };

  //   float getUnadjustedAngle()
  //   {
  //     return _unadjusted_angle;
  //   }

private:
  AccelStepper _stepper;
  String _name;
  int _pin1;
  int _pin2;
  int _pin3;
  int _pin4;

  int _maxSpeed = 800; // default max speed in steps per second

  int _homeSensorPin;
  bool _homeSensorHomeState;

  //   bool _home_detected()
  //   {
  //     return digitalRead(_homeSensorPin) == _homeSensorHomeState;
  //   };

  //   float _unadjusted_angle;
  //   float _current_angle;
};

#endif
