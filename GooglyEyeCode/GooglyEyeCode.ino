
// Test that the pupils can move the components- may need to change the gearing a few times


#include <AccelStepper.h>

// pupil pin definitions
#define pupilStepperPin1 3
#define pupilStepperPin2 4
#define pupilStepperPin3 5
#define pupilStepperPin4 6

// glint pin definitions
#define glintStepperPin1 8
#define glintStepperPin2 9
#define glintStepperPin3 10
#define glintStepperPin4 11

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

  glintStepper.setMaxSpeed(1100);
  glintStepper.setAcceleration(1000.0);
  glintStepper.moveTo(-20480/2);
}


  void loop() {

    //Change direction when the stepper reaches the target position
    if (pupilStepper.distanceToGo() == 0)
      pupilStepper.moveTo(-pupilStepper.currentPosition());

    if (glintStepper.distanceToGo() == 0)
      glintStepper.moveTo(-glintStepper.currentPosition());


    pupilStepper.run();
    glintStepper.run();
  }
