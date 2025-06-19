/*

  Abstraction of the display of the time.

  Given a certain time to display it moves the pupil and glint to display it.

  Keeps track of how many steps are needed for a full rotation of each part and what the drift is between pupil and glint.

  No timekeeping is done here, just the display of a time passed in.

*/

#ifndef Display_h
#define Display_h

#include "Arduino.h"
#include "Motor.h"
#include "local_config.h"

class Display {
public:
  Display(){};


  void setup() {

    _pupilMotor = Motor(
      "Pupil",
      pupilStepperPin1,
      pupilStepperPin2,
      pupilStepperPin3,
      pupilStepperPin4,
      pupilLightGatePin,
      false,
      true);

    _glintMotor = Motor(
      "Glint",
      glintStepperPin1,
      glintStepperPin2,
      glintStepperPin3,
      glintStepperPin4,
      glintHallEffectPin,
      false,
      false);

    // For faster startup let's use values we captured earlier
    if (false) {

      _pupilMotor._steps_per_rotation = 14275;
      _pupilMotor._steps_across_home_sensor = 192;

      _glintMotor._steps_per_rotation = 21896;
      _glintMotor._steps_across_home_sensor = 528;

      _glint_correction_per_pupil_degree = 0.2;

      // go to noon position
      _pupilMotor.findNoon();
      _glintMotor.findNoon();

    } else {
      calibrate();
    }


    Serial.println("Setting slow mode for motors...");
    _pupilMotor.slow_mode();
    _glintMotor.slow_mode();
  };

  void displayTime(int hours, int mins) {
    Serial.println("Displaying time: " + String(hours) + ":" + String(mins));


    float delta = 0.0;

    float fraction_of_hour = mins / 60.0;

    float hours_angle = (hours + fraction_of_hour) / 12.0 * 360.0;
    float mins_angle = mins / 60.0 * 360.0;

    while (hours_angle >= 360) hours_angle -= 360;
    while (mins_angle >= 360) mins_angle -= 360;

    // Serial.println(
    //   "hours_angle:" + String(hours_angle) + ", mins_angle:" + String(mins_angle));

    // wake up motors
    _pupilMotor.wake();
    _glintMotor.wake();
    delay(50);

    // rotate pupil first
    delta = _pupilMotor.goto_angle(hours_angle);

    // adjust glint and then rotate it
    _glintMotor.adjust_angle(delta + delta * _glint_correction_per_pupil_degree);
    _glintMotor.goto_angle(mins_angle);

    // put motors to sleep
    delay(50);
    _pupilMotor.sleep();
    _glintMotor.sleep();
  };

  void calibrate() {
    Serial.println("Calibrating full rotations...");
    _pupilMotor.calibrate();
    Serial.println(_pupilMotor.debug());

    _glintMotor.calibrate();
    Serial.println(_glintMotor.debug());

    Serial.println("Calibrating drift of glint when pupil rotates...");

    // Home both motors
    _pupilMotor.goto_angle(0);
    _glintMotor.goto_angle(0);

    // Do one full rotation of the pupil
    _pupilMotor.goto_angle(180);
    _pupilMotor.goto_angle(360);

    // Get adjustment angle for glint and store it sensibly
    float glint_drift = _glintMotor.measure_angle_to_home();
    if (glint_drift > 180)
      glint_drift -= 360.0;  // we want the shortest path to home
    _glint_correction_per_pupil_degree = -(glint_drift / 360.0);

    Serial.print("_glint_correction_per_pupil_degree: ");
    Serial.println(_glint_correction_per_pupil_degree);
  };

private:
  Motor _pupilMotor;
  Motor _glintMotor;
  float _glint_correction_per_pupil_degree = 0.0;  // how much the glint moves per degree of pupil movement
};

#endif
