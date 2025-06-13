
#include "local_config.h"
#include "Motor.h"


Motor pupilMotor(
  "Pupil",
  pupilStepperPin1,
  pupilStepperPin2,
  pupilStepperPin3,
  pupilStepperPin4,
  pupilLightGatePin,
  false);

Motor glintMotor(
  "Glint",
  glintStepperPin1,
  glintStepperPin2,
  glintStepperPin3,
  glintStepperPin4,
  glintHallEffectPin,
  true);

void setup() {
  Serial.begin(115200);
  Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

  pupilMotor.calibrate();
  glintMotor.calibrate();

  float delta = 0;
  delta = pupilMotor.goto_angle(180);
  delta = pupilMotor.goto_angle(360);

  float drift_angle_360 = glintMotor.measure_angle_to_home();
  float adjustment_per_degree = drift_angle_360 / 360;

  pupilMotor.slow_mode();
  glintMotor.slow_mode();

  for (int sec = 0; sec <= 1 * 12 * 60 * 60; sec++) {

    if (sec % 60 != 0)
      continue;

    float mins = sec / 60.0;
    float hours = mins / 60.0;
    float delta = 0.0;

    float hours_angle = hours / 12.0 * 360.0;
    float mins_angle = mins / 60.0 * 360.0;

    while (hours_angle >= 360) hours_angle -= 360;
    while (mins_angle >= 360) mins_angle -= 360;

    // Serial.println(
    //   "hours_angle:" + String(hours_angle) + ", mins_angle:" + String(mins_angle));

    pupilMotor.wake();
    delta = pupilMotor.goto_angle(hours_angle);
    pupilMotor.sleep();

    glintMotor.adjust_angle(delta - delta * adjustment_per_degree);
    glintMotor.wake();
    glintMotor.goto_angle(mins_angle);
    glintMotor.sleep();

     delay(58000);
  }
}


void loop() {
  pupilMotor.sleep();
  glintMotor.sleep();

  delay(1000);
}
