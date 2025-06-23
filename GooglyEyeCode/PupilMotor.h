/*

  Pupil motor specific functions.

*/

#ifndef PupilMotor_h
#define PupilMotor_h

#include "local_config.h"
#include "Arduino.h"
#include "Motor.h"

class PupilMotor : public Motor
{
public:
  PupilMotor()
      : Motor(
            "Pupil",
            pupilStepperPin1,
            pupilStepperPin2,
            pupilStepperPin3,
            pupilStepperPin4) {
          // Serial.println("PupilMotor constructor called.");
        };
};

#endif
