
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
