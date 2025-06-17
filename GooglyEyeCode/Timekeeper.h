/*

  Abstraction of the timekeeping.

*/

#ifndef Timekeeper_h
#define Timekeeper_h

#include "Arduino.h"
#include "RTClib.h"

RTC_DS3231 rtc;

class Timekeeper {
public:

  Timekeeper(){};

  void setup() {

    // FIXME - what to do if there is no RTC?
    if (!rtc.begin()) {
      Serial.println("Couldn't find RTC");
      Serial.flush();
      while (1) delay(10);
    }

    if (rtc.lostPower()) {
      Serial.println("RTC lost power, let's set the time!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  };

  int hours() {
    DateTime now = rtc.now();
    return now.hour();
  };

  int mins() {
    DateTime now = rtc.now();
    return now.minute();
  };

private:
};

#endif
