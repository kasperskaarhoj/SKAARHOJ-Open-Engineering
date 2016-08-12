/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Addition to original blink sketch also turns on and off camera 1's tally indicator.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.
*/

#include <BMDSDIControl.h>                                // need to include the library

BMD_SDITallyControl_I2C sdiTallyControl(0x6E);            // define the Tally object using I2C using the default shield address

// the setup function runs once when you press reset or power the board
void setup()
{
  sdiTallyControl.begin();                                 // initialize tally control
  sdiTallyControl.setOverride(true);                       // enable tally override

  pinMode(13, OUTPUT);                                     // initialize digital pin 13 as an output
}

// the loop function runs over and over again forever
void loop()
{
  digitalWrite(13, HIGH);                                  // turn the LED ON

  sdiTallyControl.setCameraTally(                          // turn tally ON
    1,                                                     // Camera Number
    true,                                                  // Program Tally
    false                                                  // Preview Tally
  );

  delay(1000);                                             // leave it ON for 1 second

  digitalWrite(13, LOW);                                   // turn the LED OFF

  sdiTallyControl.setCameraTally(                          // turn tally OFF
    1,                                                      // Camera Number
    false,                                                 // Program Tally
    false                                                  // Preview Tally
  );

  delay(1000);                                             // leave it OFF for 1 second
}
