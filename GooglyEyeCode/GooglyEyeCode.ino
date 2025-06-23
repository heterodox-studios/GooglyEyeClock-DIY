
#include "local_config.h"
#include "Arduino.h"
#include "Display.h"
#include "Timekeeper.h"

Display display;
Timekeeper timekeeper;

bool pupilLightGateLastSeenState = false;

void lightGateISR()
{
    bool state = digitalRead(pupilLightGatePin);
    if (state != pupilLightGateLastSeenState)
    {
        pupilLightGateLastSeenState = state;
        // Serial.println("pupilLightGateISR triggered: " + String(state));
        display.PupilLightGateChangeISR(state);
    }
}

void setup()
{
    pinMode(pupilLightGatePin, INPUT_PULLUP);
    bool pupilLightGateLastSeenState = digitalRead(pupilLightGatePin);
    attachInterrupt(
        digitalPinToInterrupt(pupilLightGatePin),
        lightGateISR,
        CHANGE);

    display.calibrate();
}

void loop() {}

// volatile bool pupilLightGateLastSeenState = LOW;
// volatile bool glintHallEffectLastSeenState = LOW;

// void setup()
// {

// Serial.begin(115200);
// for (int i = 0; i < 20; i++)
//   if (!Serial)
//     delay(200); // wait for serial port to connect. Needed for native USB

// setup_interupts();

// delay(5000); // wait for serial monitor to open

// Serial.println("------------------------------------");
// Serial.println("Starting setup");
// Serial.flush();

// display.setup();
// timekeeper.setup();
// }

// void setup_interupts()
// {
//   // attach interrupt to pupil sensor
//   pinMode(pupilLightGatePin, INPUT_PULLUP);
//   pupilLightGateLastSeenState = digitalRead(pupilLightGatePin);
//   attachInterrupt(
//       digitalPinToInterrupt(pupilLightGatePin),
//       lightGateISR,
//       CHANGE);

//   pinMode(glintHallEffectPin, INPUT_PULLUP);
//   glintHallEffectLastSeenState = digitalRead(glintHallEffectPin);
//   attachInterrupt(
//       digitalPinToInterrupt(glintHallEffectPin),
//       glintHallEffectISR,
//       CHANGE);
// }

// void lightGateISR()
// {
//   bool state = digitalRead(pupilLightGatePin);
//   if (state != pupilLightGateLastSeenState)
//   {
//     pupilLightGateLastSeenState = state;
//     // Serial.println("pupilLightGateISR triggered: " + String(state));
//     display.passOnPupilISR(state);
//   }
// }

// void glintHallEffectISR()
// {
//   // ignore hall sensor if pupil light gate is not triggered
//   if (!pupilLightGateLastSeenState == HIGH)
//   {
//     Serial.println("glintHallEffectISR ignored because pupil light gate is not triggered.");
//     return;
//   }

//   bool state = !digitalRead(glintHallEffectPin);
//   if (state != glintHallEffectLastSeenState)
//   {
//     glintHallEffectLastSeenState = state;
//     // Serial.println("glintHallEffectISR triggered: " + String(state));
//     display.passOnGlintISR(state);
//   }
// }

// void loop()
// {

// display.displayTime(0, 0);

// display.displayTime(11, 50);
// display.displayTime(11, 55);
// display.displayTime(0, 0);
// display.displayTime(0, 5);
// display.displayTime(0, 10);

// delay(1000000);

// swing_past_possible_hall_sensor_trigger_times();

// for (int hours = 0; hours <= 11; hours++) {
//   for (int minutes = 0; minutes <= 59; minutes++) {
//     display.displayTime(hours, minutes);
//     // delay(1000);
//   }
// }

// Display current time every minute
// display.displayTime(timekeeper);

// Wait until the next minute change
// delay(1000 * (60 - timekeeper.secs()));

// QuickTime(60);

// delay(1000000);

// while (1)
// {
//   // Display current time every second
//   display.displayTime(0, 0);
//   delay(1000);
//   display.displayTime(0, 0);
//   delay(1000);
//   display.displayTime(6, 0);
//   delay(1000);
// }
// }

// This function is for testing purposes to swing the display past all possible

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
