
// Test that the pupils can move the components- may need to change the gearing a few times

#include "config.h"
#include <AccelStepper.h>



AccelStepper pupilStepper(
  AccelStepper::HALF4WIRE,
  pupilStepperPin1,
  pupilStepperPin3,
  pupilStepperPin2,
  pupilStepperPin4);

AccelStepper glintStepper(
  AccelStepper::HALF4WIRE,
  glintStepperPin1,
  glintStepperPin3,
  glintStepperPin2,
  glintStepperPin4);


void setup() {
  pupilStepper.setMaxSpeed(1000);
  pupilStepper.setAcceleration(1000.0);
  pupilStepper.moveTo(20480);

  glintStepper.setMaxSpeed(1000);
  glintStepper.setAcceleration(1000.0);
  glintStepper.moveTo(20480);

  pinMode(pupilLightGatePin, INPUT_PULLUP);
  pinMode(glintHallEffectPin, INPUT_PULLUP);
  pinMode(13, OUTPUT);

  Serial.begin(115200);
}


void loop() {
  // Check to see if we are at midday (lightgate triggers)
  int isMidday = !digitalRead(pupilLightGatePin);

  int isOnTheHour = !digitalRead(glintHallEffectPin);
  digitalWrite(13, isOnTheHour);

  // When we see midday save the position and go back there
  if (isMidday)
    pupilStepper.moveTo(pupilStepper.currentPosition());

  // when pupil at home position then home the glint
  if (pupilStepper.distanceToGo() == 0) {
    if (isOnTheHour) {
      glintStepper.moveTo(glintStepper.currentPosition());
    }
    //Serial.print("curr:");
    //Serial.print(glintStepper.currentPosition());
    //Serial.print(",target:");
    //Serial.println(glintStepper.targetPosition());
  }

  pupilStepper.run();
  glintStepper.run();

  // When we have reached the end power down
  if (
    pupilStepper.distanceToGo() == 0
    && glintStepper.distanceToGo() == 0) {
    pupilStepper.disableOutputs();
    glintStepper.disableOutputs();
  }
}
