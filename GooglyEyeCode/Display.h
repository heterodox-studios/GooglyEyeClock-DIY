/*

This module manages the display. It tracks where the angles of pupil and glint. It calculates movement needed to reach certain angles and executes them by stepping the motors.

For now we are ignoring acceleration.

*/

#ifndef Display_h
#define Display_h

#include "local_config.h"
#include "Arduino.h"
#include "PupilMotor.h"
#include "GlintMotor.h"
#include "Timekeeper.h"

class Display
{
public:
  Display()
  {
    _pupilMotor = PupilMotor();
    _glintMotor = GlintMotor();
  };

  void calibrate()
  {

    while (_pupilReachedLightGate == false)
    {
      _pupilMotor.step();
      delay(2);
    }

    delay(1000);
    _pupilMotor.sleep();
    _glintMotor.sleep();
  };

  void PupilLightGateChangeISR(bool state)
  {
    Serial.println("PupilLightGateChangeISR: " + String(state));
    _pupilReachedLightGate = true;
  }

private:
  Motor _pupilMotor;
  Motor _glintMotor;

  bool _pupilReachedLightGate = false;
};

// calibrate();

// _pupilMotor.slow_mode();
// _glintMotor.slow_mode();

// void displayTime(Timekeeper now)
// {
//   displayTime(now.hours(), now.mins(), now.secs());
// };

//   void displayTime(int hours, int mins, int secs = 0)
//   {
//     Serial.println("\n");
//     Serial.println(
//         "Displaying time: " + String(hours) + ":" + String(mins) + ":" + String(secs));

//     float fractional_minutes = mins + (secs / 60.0);
//     float mins_angle = fractional_minutes / 60.0 * 360.0;

//     while (mins_angle >= 360)
//       mins_angle -= 360;

//     float fractional_hours = hours + (fractional_minutes / 60.0);
//     float hours_angle = fractional_hours / 12.0 * 360.0;
//     while (hours_angle >= 360)
//       hours_angle -= 360;

//     // Serial.println("hours_angle:" + String(hours_angle));
//     // Serial.println("mins_angle:" + String(mins_angle));

//     // // wake up motors
//     _pupilMotor.wake();
//     _glintMotor.wake();
//     delay(50);

//     // track how much pupil rotates so we can adjust the glint afterwards
//     float delta = 0.0;
//     delta += _pupilMotor.gotoAngleClockwise(hours_angle);
//     // delta += _pupilMotor.gotoAngleClockwise(hours_angle); // apply any adjustments after calibration

//     // adjust the angle glint thinks it is at
//     _glintMotor.adjust_angle(delta);
//     // _glintMotor.adjust_angle(delta * _glint_correction_per_pupil_degree);

//     _glintMotor.gotoAngleClockwise(mins_angle);
//     // _glintMotor.gotoAngleClockwise(mins_angle); // apply any adjustments after calibration

//     // put motors to sleep
//     delay(50);
//     _pupilMotor.sleep();
//     _glintMotor.sleep();

//     Serial.println("pupil.currentAngle:    " + String(_pupilMotor.getCurrentAngle()));
//     Serial.println("glint.currentAngle:    " + String(_glintMotor.getCurrentAngle()));
//     Serial.println("glint.unadjustedAngle: " + String(_glintMotor.getUnadjustedAngle()));
//   };

//   void calibrate()
//   {
//     // dump in known values to speed up calibration
//     // _pupilMotor._stepsPerRotation = 14271;
//     // _pupilMotor._stepsAcrossHome = 594;
//     // _pupilMotor._stepsAcrossHomeHasBeenCalibrated = true;
//     // _pupilMotor._stepsPerRotationHasBeenCalibrated = true;

//     // _glintMotor._stepsPerRotation = 14789;
//     // _glintMotor._stepsAcrossHome = 375;
//     // _glintMotor._stepsAcrossHomeHasBeenCalibrated = true;
//     // _glintMotor._stepsPerRotationHasBeenCalibrated = true;
//     calibrateMotors();

//     // _glint_correction_per_pupil_degree = 0.30;
//     calibrateGlintDrift();

//     // displayTime(6, 0);

//     // Serial.println(_glintMotor.debug());
//     // displayTime(11, 55);
//     // displayTime(11, 56);
//     // displayTime(11, 57);
//     // displayTime(11, 58);
//     // displayTime(11, 59);
//     // displayTime(12, 0);
//     // displayTime(12, 01);
//     // displayTime(12, 02);

//     // delay(2000);
//     // displayTime(6, 0);

//     // Serial.println("\n\n");
//     // Serial.println(_pupilMotor.debug());
//     // Serial.println(_glintMotor.debug());

//     // Serial.println(_glintMotor.debug());
//     // displayTime(11, 55);
//     // displayTime(12, 0);
//     // displayTime(12, 05);
//     // Serial.println(_glintMotor.debug());

//     // _pupilMotor.gotoAngleDirect(0);
//     // _glintMotor.gotoAngleDirect(0);

//     // delay(10000000);

//     // int gotoAngle = 60;
//     // while (1)
//     // {
//     //   Serial.println("\n");
//     //   _glintMotor.gotoAngleDirect(gotoAngle);
//     //   gotoAngle += 120;
//     //   // gotoAngle *= -1;

//     //   delay(1000);
//     // }
//   }

//   void calibrateMotors()
//   {
//     Serial.println("Calibrating full rotations...");
//     _pupilMotor.disableAutoCalibration();
//     _glintMotor.disableAutoCalibration();

//     Serial.println("Calibrating pupil motor...");
//     _pupilMotor.calibrateUsingInterupts();
//     _pupilMotor.gotoAngleDirect(0);

//     Serial.println("Calibrating glint motor...");
//     _glintMotor.calibrateUsingInterupts();
//   }

//   void calibrateGlintDrift()
//   {

//     // _glintMotor.disableAutoCalibration();

//     Serial.println("Calibrating drift of glint when pupil rotates...");
//     _pupilMotor.gotoAngleDirect(0);
//     _glintMotor.gotoAngleDirect(0);

//     // Do one full rotation of the pupil
//     _pupilMotor.gotoAngleClockwise(180);
//     _pupilMotor.gotoAngleClockwise(360);

//     // Get adjustment angle for glint and store it sensibly
//     float glint_drift = _glintMotor.measure_angle_to_home();
//     if (glint_drift > 180)
//       glint_drift -= 360.0; // we want the shortest path to home
//     _glint_correction_per_pupil_degree = -(glint_drift / 360.0);

//     Serial.print("_glint_correction_per_pupil_degree: ");
//     Serial.println(_glint_correction_per_pupil_degree);

//     // _glintMotor.enableAutoCalibration();
//   };

//   void passOnPupilISR(bool state)
//   {
//     // Serial.println("passOnPupilISR received ISR with state: " + String(state));
//     _pupilMotor.autoCalibrateFromInterupt(state);
//   }

//   void passOnGlintISR(bool state)
//   {
//     // Serial.println("passOnGlintISR received ISR with state: " + String(state));
//     _glintMotor.autoCalibrateFromInterupt(state);
//   }

// private:
//   float _glint_correction_per_pupil_degree = 0.0; // how much the glint moves per degree of pupil movement

#endif
