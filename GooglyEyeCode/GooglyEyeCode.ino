
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
      delay(100);  // wait for serial port to connect. Needed for native USB




  Serial.println("------------------------------------");
  Serial.println("Starting setup");

  display.setup();
  timekeeper.setup();
}


void loop() {
  display.displayTime(timekeeper.hours(), timekeeper.mins());

  delay(10 * 1000);
}
