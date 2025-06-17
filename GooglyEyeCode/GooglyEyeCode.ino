
#include "Arduino.h"
#include "local_config.h"
#include "Display.h"

Display display;


void setup() {

  Serial.begin(115200);
  while (!Serial) delay(10);  // wait for serial port to connect. Needed for native USB

  Serial.println("------------------------------------");
  Serial.println("Starting setup");
  display.setup();
}


void loop() {
  display.displayTime(3, 15);
  display.displayTime(6, 30);
  display.displayTime(9, 45);
  display.displayTime(0, 0);
  delay(1000000);
}
