
#include "Arduino.h"
#include "local_config.h"
#include "Display.h"
#include "Timekeeper.h"

Display display;
Timekeeper timekeeper;

void setup() {

  Serial.begin(115200);
  for (int i = 0; i < 20; i++)
    if (!Serial)
      delay(200);  // wait for serial port to connect. Needed for native USB

  Serial.println("------------------------------------");
  Serial.println("Starting setup");

  display.setup();
  timekeeper.setup();
}


void loop() {

  // for (int hours = 0; hours <= 11; hours++) {
  //   for (int minutes = 0; minutes <= 59; minutes++) {
  //     display.displayTime(hours, minutes);
  //     // delay(1000);
  //   }
  // }

  // update time every minute
  display.displayTime(timekeeper);
  delay(1000 * (61 - timekeeper.secs()));

  // update time continuously
  // Serial.println();
  // display.displayTime(timekeeper);

  // delay(10 * 1000);
}
