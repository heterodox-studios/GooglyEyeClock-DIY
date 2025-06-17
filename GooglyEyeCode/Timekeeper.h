/*

  Abstraction of the timekeeping.

*/

#ifndef Timekeeper_h
#define Timekeeper_h

#include "Arduino.h"

class Timekeeper {
public:

  Timekeeper(){};

  int hours() {
    return 6;
  };

  int mins() {
    return 30;
  };

private:
};

#endif
