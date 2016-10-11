/*****************
 * This example return events from the encoders on SKAARHOJ Rotary Encoder Breakout Board
 * If the value is 0 (zero), nothing happened on the encoder
 * Values 1 and -1 indicates a rotation clockwise and counter-clockwise
 * Value 2 indicates a button press (on down)
 * Values larger than 2 indicates the release time of a button
 * If the second parameter (milliseconds) is sent when asking for state of the button, it indicates the time
 * when an automated release-event is detected on the buttons. 
 * 
 * - kasper
 *
 * This example code is under GNU GPL license
 */


#include "SkaarhojUtils.h"
SkaarhojUtils utils;



#include <Streaminh.h>



void setup() {

  // Start the Serial (debugging) and UDP:
  Serial.begin(115200);  
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  
  utils.encoders_init();  // Init encoders
  utils.debugMode();
  
    // Here we attach hardware interrups to functions that call the encoder object (see below). Trigger pins must be soldered to D2 and D3:
    // We can choose not to use interrupts (just comment these lines out) and the utils-library will fall back to standard detection, but that is usually not satisfactory.
  attachInterrupt(0, _enc1active, RISING);  // Lower encoder (1), pin 2
  attachInterrupt(1, _enc0active, RISING);  // Higher encoder (0), pin 3
}

  // The following two functions are called for hardware interrupt 0 and 1. They in turn call the encoder object to detect the direction pin state.
  // Using interrupts for this secures that we detect the rotation direction of the encoders right. 
void _enc0active()  {
  utils.encoders_interrupt(0);
}
void _enc1active()  {
  utils.encoders_interrupt(1);
}



void loop() {
  int encValue = utils.encoders_state(0,1000);
  if (encValue)  {
     Serial << F("Encoder 0: ") << encValue << "\n"; 
  }

  encValue = utils.encoders_state(1,1000);
  if (encValue)  {
     Serial << F("Encoder 1: ") << encValue << "\n"; 
  }
}

