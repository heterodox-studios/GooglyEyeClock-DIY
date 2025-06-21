
#include "Arduino.h"
#include "local_config.h"
#include "Display.h"
#include "Timekeeper.h"

Display display;
Timekeeper timekeeper;

void setup()
{

  Serial.begin(115200);
  for (int i = 0; i < 20; i++)
    if (!Serial)
      delay(200); // wait for serial port to connect. Needed for native USB

  // setup_interupts();

  delay(5000); // wait for serial monitor to open

  Serial.println("------------------------------------");
  Serial.println("Starting setup");
  Serial.flush();

  display.setup();
  timekeeper.setup();
}

// void setup_interupts()
// {
// // attach interrupt to pupil sensor
// pinMode(pupilLightGatePin, INPUT_PULLUP);
// attachInterrupt(
//     digitalPinToInterrupt(pupilLightGatePin),
//     lightGateISR,
//     CHANGE);

//   pinMode(glintHallEffectPin, INPUT_PULLUP);
//   attachInterrupt(
//       digitalPinToInterrupt(glintHallEffectPin),
//       glintHallEffectISR,
//       CHANGE);
// }

// volatile bool pupilLightGateLastSeenState = LOW;
// void lightGateISR()
// {
//   bool state = digitalRead(pupilLightGatePin);
//   if (state != pupilLightGateLastSeenState)
//   {
//     pupilLightGateLastSeenState = state;
//     // Serial.println("pupilLightGateISR triggered: " + String(state));
//   }
// }

// volatile bool glintHallEffectLastSeenState = LOW;
// void glintHallEffectISR()
// {
//   bool state = digitalRead(glintHallEffectPin);
//   if (state != glintHallEffectLastSeenState)
//   {
//     glintHallEffectLastSeenState = state;
//     // Serial.println("glintHallEffectISR triggered: " + String(state));
//   }
// }

void loop()
{

  // swing_past_possible_hall_sensor_trigger_times();
  // monitor_interrupts();

  // for (int hours = 0; hours <= 11; hours++) {
  //   for (int minutes = 0; minutes <= 59; minutes++) {
  //     display.displayTime(hours, minutes);
  //     // delay(1000);
  //   }
  // }

  // update time every minute
  display.displayTime(timekeeper);
  delay(1000 * (60 - timekeeper.secs()));

  // update time continuously
  // Serial.println();
  // display.displayTime(timekeeper);

  // delay(10 * 1000);
}

// void swing_past_possible_hall_sensor_trigger_times()
// {
//   // display.displayTime(0, 0);
//   // delay(5000);

//   // go to start time to avoid false reporting of the interupts
//   // display.displayTime(10, 40);

//   for (int hour = 11; hour <= 11; hour++)
//   {
//     for (int minute = 52; minute <= 68; minute++)
//     {
//       int m = minute % 60;
//       int h = m == minute ? hour : hour + 1;

//       bool pupil_before = pupilLightGateLastSeenState;
//       bool glint_before = glintHallEffectLastSeenState;

//       display.displayTime(h, m);

//       if (pupil_before != pupilLightGateLastSeenState)
//       {
//         Serial.print("Pupil light gate changed to " + String(pupilLightGateLastSeenState) + " at ");
//         Serial.print(h);
//         Serial.print(":");
//         Serial.println(m);
//       }
//       if (glint_before != glintHallEffectLastSeenState)
//       {
//         Serial.print("Glint hall effect changed to " + String(glintHallEffectLastSeenState) + " at ");
//         Serial.print(h);
//         Serial.print(":");
//         Serial.println(m);
//       }
//     }
//     Serial.println("");
//   }

//   // display.displayTime(0, 0);
//   // delay(5000);
// }

// void monitor_interrupts()
// {
//   display.displayTime(6, 0);

//   Serial.println("");

//   Serial.println("Monitoring interrupts...");

//   bool pupil_before = pupilLightGateLastSeenState;
//   bool glint_before = glintHallEffectLastSeenState;

//   while (1)
//   {
//     if (pupil_before != pupilLightGateLastSeenState)
//     {
//       Serial.println("Pupil light gate changed to " + String(pupilLightGateLastSeenState));
//       pupil_before = pupilLightGateLastSeenState;
//     }

//     if (glint_before != glintHallEffectLastSeenState)
//     {
//       Serial.println("Glint hall effect changed to " + String(glintHallEffectLastSeenState));
//       glint_before = glintHallEffectLastSeenState;
//     }
//   }
// }
