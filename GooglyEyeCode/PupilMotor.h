/*

  Pupil motor specific functions.

*/

#ifndef PupilMotor_h
#define PupilMotor_h

#include "Arduino.h"
#include "Motor.h"
#include "local_config.h"

class PupilMotor : public Motor
{
public:
  PupilMotor()
      : Motor(
            "Pupil",
            pupilStepperPin1,
            pupilStepperPin2,
            pupilStepperPin3,
            pupilStepperPin4,
            pupilLightGatePin,
            false,
            true) {
          // Serial.println("PupilMotor constructor called.");
        };
};

#endif
