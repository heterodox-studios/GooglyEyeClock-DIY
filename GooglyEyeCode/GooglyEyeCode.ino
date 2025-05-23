
#include "local_config.h"
#include "Motor.h"


Motor pupilMotor(
  "Pupil",
  pupilStepperPin1,
  pupilStepperPin2,
  pupilStepperPin3,
  pupilStepperPin4,
  pupilLightGatePin);

Motor glintMotor(
  "Glint",
  glintStepperPin1,
  glintStepperPin2,
  glintStepperPin3,
  glintStepperPin4,
  glintHallEffectPin);

void setup() {
  Serial.begin(115200);
  Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

  pupilMotor.calibrate();
  Serial.print(pupilMotor.debug());

  glintMotor.calibrate();
  Serial.print(glintMotor.debug());

  float delta = 0;
  delta = pupilMotor.goto_angle(180);
  delta = pupilMotor.goto_angle(360);

  float drift_angle_360 = glintMotor.measure_angle_to_home();
  float adjustment_per_degree = drift_angle_360 / 360;

  Serial.println(drift_angle_360);
  Serial.println(adjustment_per_degree);
  delay(1000);

  // delta = pupilMotor.goto_angle(180);
  // delta = pupilMotor.goto_angle(0);


  delta = pupilMotor.goto_angle(180);
  glintMotor.adjust_angle(180);
  glintMotor.adjust_angle(-delta * adjustment_per_degree);
  glintMotor.goto_angle(0);

  delay(1000);


  delta = pupilMotor.goto_angle(0);
  Serial.println(delta);
  Serial.print(pupilMotor.debug());

  Serial.print(glintMotor.debug("pre adjust"));
  glintMotor.adjust_angle(180);
  glintMotor.adjust_angle(-delta * adjustment_per_degree);

  Serial.print(glintMotor.debug("post adjust"));

  glintMotor.goto_angle(0);
  Serial.print(glintMotor.debug("post goto"));

  delay(1000);



  pupilMotor.sleep();
  glintMotor.sleep();
}


void loop() {
  // When we have reached the end power down
  // if (
  //   !pupilStepper.isRunning()
  //   && !glintStepper.isRunning()) {
  //   pupilStepper.disableOutputs();
  //   glintStepper.disableOutputs();
  // }
}
