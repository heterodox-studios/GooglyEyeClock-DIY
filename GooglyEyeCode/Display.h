/*

  Abstraction of the display of the time.

  Given a certain time to display it moves the pupil and glint to display it.

  Keeps track of how many steps are needed for a full rotation of each part and what the drift is between pupil and glint.

  No timekeeping is done here, just the display of a time passed in.

*/

#ifndef Display_h
#define Display_h

#include "Arduino.h"
#include "PupilMotor.h"
#include "GlintMotor.h"
#include "local_config.h"
#include "Timekeeper.h"

class Display
{
public:
  Display() {};

  void setup()
  {
    Serial.println("Setting up display...");
    _pupilMotor = PupilMotor();
    _glintMotor = GlintMotor();

    calibrate();

    _pupilMotor.slow_mode();
    _glintMotor.slow_mode();
  };

  void displayTime(Timekeeper now)
  {
    displayTime(now.hours(), now.mins(), now.secs());
  };

  void displayTime(int hours, int mins, int secs = 0)
  {
    Serial.println("\n");
    Serial.println(
        "Displaying time: " + String(hours) + ":" + String(mins) + ":" + String(secs));

    float fractional_minutes = mins + (secs / 60.0);
    float mins_angle = fractional_minutes / 60.0 * 360.0;

    while (mins_angle >= 360)
      mins_angle -= 360;

    float fractional_hours = hours + (fractional_minutes / 60.0);
    float hours_angle = fractional_hours / 12.0 * 360.0;
    while (hours_angle >= 360)
      hours_angle -= 360;

    // Serial.println("hours_angle:" + String(hours_angle));
    // Serial.println("mins_angle:" + String(mins_angle));

    // // wake up motors
    _pupilMotor.wake();
    _glintMotor.wake();
    delay(50);

    // track how much pupil rotates so we can adjust the glint afterwards
    float delta = _pupilMotor.gotoAngleClockwise(hours_angle);

    // adjust the angle glint thinks it is at
    _glintMotor.adjust_angle(delta);
    _glintMotor.adjust_angle(delta * _glint_correction_per_pupil_degree);

    // _glintMotor.gotoAngleClockwise(mins_angle);

    // put motors to sleep
    delay(50);
    _pupilMotor.sleep();
    _glintMotor.sleep();

    Serial.println("pupil_angle: " + String(_pupilMotor.getCurrentAngle()));
    Serial.println("glint_angle: " + String(_glintMotor.getCurrentAngle()));
  };

  void calibrate()
  {
    calibrateMotors();
    // calibrateGlintDrift();
  }

  void calibrateMotors()
  {
    Serial.println("Calibrating full rotations...");
    _pupilMotor.disableAutoCalibration();
    _glintMotor.disableAutoCalibration();

    Serial.println("Calibrating pupil motor...");
    _pupilMotor.calibrateUsingInterupts();
    _pupilMotor.gotoAngleDirect(-2);

    // Serial.println(_pupilMotor.debug());

    Serial.println("Calibrating glint motor...");
    _glintMotor.calibrateUsingInterupts();

    int gotoAngle = 60;
    while (1)
    {
      Serial.println("\n");
      _glintMotor.gotoAngleDirect(gotoAngle);
      gotoAngle += 120;
      // gotoAngle *= -1;

      delay(1000);
    }
  }

  void calibrateGlintDrift()
  {
    Serial.println("Calibrating drift of glint when pupil rotates...");
    _pupilMotor.gotoAngleDirect(0);
    _glintMotor.gotoAngleDirect(0);

    // Do one full rotation of the pupil
    _pupilMotor.gotoAngleClockwise(180);
    _pupilMotor.gotoAngleClockwise(360);

    // Get adjustment angle for glint and store it sensibly
    float glint_drift = _glintMotor.measure_angle_to_home();
    if (glint_drift > 180)
      glint_drift -= 360.0; // we want the shortest path to home
    _glint_correction_per_pupil_degree = -(glint_drift / 360.0);

    Serial.print("_glint_correction_per_pupil_degree: ");
    Serial.println(_glint_correction_per_pupil_degree);
  };

  void passOnPupilISR(bool state)
  {
    // Serial.println("passOnPupilISR received ISR with state: " + String(state));
    _pupilMotor.autoCalibrateFromInterupt(state);
  }

  void passOnGlintISR(bool state)
  {
    // Serial.println("passOnGlintISR received ISR with state: " + String(state));
    _glintMotor.autoCalibrateFromInterupt(state);
  }

private:
  Motor _pupilMotor;
  Motor _glintMotor;
  float _glint_correction_per_pupil_degree = 0.0; // how much the glint moves per degree of pupil movement
};

#endif
