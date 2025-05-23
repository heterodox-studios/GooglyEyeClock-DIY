
#include "local_config.h"
#include "Motor.h"


Motor pupilMotor(
  pupilStepperPin1,
  pupilStepperPin2,
  pupilStepperPin3,
  pupilStepperPin4);

// AccelStepper glintStepper(
//   AccelStepper::HALF4WIRE,
//   glintStepperPin1,
//   glintStepperPin3,
//   glintStepperPin2,
//   glintStepperPin4);


void setup() {
  Serial.begin(115200);


  pupilMotor.moveTo(20480);
  // Serial.println("moveTo");

  // glintStepper.setMaxSpeed(1000);
  // glintStepper.setAcceleration(1000.0);
  // glintStepper.moveTo(20480);

  // pinMode(pupilLightGatePin, INPUT_PULLUP);
  // pinMode(glintHallEffectPin, INPUT_PULLUP);
  // pinMode(13, OUTPUT);
}


void loop() {
  // Check to see if we are at midday (lightgate triggers)
  // int isMidday = !digitalRead(pupilLightGatePin);

  // int isOnTheHour = !digitalRead(glintHallEffectPin);
  // digitalWrite(13, isOnTheHour);

  // When we see midday save the position and go back there
  // if (isMidday)
  //   pupilStepper.moveTo(pupilStepper.currentPosition());

  // when pupil at home position then home the glint
  // if (pupilStepper.distanceToGo() == 0) {
  //   if (isOnTheHour) {
  //     glintStepper.moveTo(glintStepper.currentPosition());
  //   }
  //   //Serial.print("curr:");
  //   //Serial.print(glintStepper.currentPosition());
  //   //Serial.print(",target:");
  //   //Serial.println(glintStepper.targetPosition());
  // }

  // Serial.print("run ");
  // Serial.println(millis());
  // Serial.println(pupilMotor.debug());

  pupilMotor.run();
  // glintStepper.run();

  // When we have reached the end power down
  // if (
  //   !pupilStepper.isRunning()
  //   && !glintStepper.isRunning()) {
  //   pupilStepper.disableOutputs();
  //   glintStepper.disableOutputs();
  // }
}
