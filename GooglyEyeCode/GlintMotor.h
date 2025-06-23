/*

  Glint motor specific functions.

*/

#ifndef GlintMotor_h
#define GlintMotor_h

#include "local_config.h"
#include "Arduino.h"
#include "Motor.h"

class GlintMotor : public Motor
{
public:
  GlintMotor()
      : Motor(
            "Glint",
            glintStepperPin1,
            glintStepperPin2,
            glintStepperPin3,
            glintStepperPin4) {
          // Serial.println("GlintMotor constructor called.");
        };
};

#endif
