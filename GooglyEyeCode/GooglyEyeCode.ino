
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

  pupilMotor.goto_angle(45);
  Serial.print(pupilMotor.debug());
  Serial.print(glintMotor.debug());
  delay(1000);

  pupilMotor.goto_angle(180);
  Serial.print(pupilMotor.debug());
  Serial.print(glintMotor.debug());
  delay(1000);

  pupilMotor.goto_angle(0);
  Serial.print(pupilMotor.debug());
  Serial.print(glintMotor.debug());
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
