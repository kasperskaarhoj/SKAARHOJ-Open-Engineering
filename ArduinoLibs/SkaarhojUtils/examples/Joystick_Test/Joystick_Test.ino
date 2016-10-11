/*****************
 * This example returns state of a joystick from a SKAARHOJ Joystick board connected to 
 * an Arduino Mega Extended Breakout Shield
 * 
 * - kasper
 */


#include "SkaarhojUtils.h"
SkaarhojUtils utils;

//#include <MemoryFree.h>

#include <Streaming.h>


void setup() { 

  // Start the Serial (debugging) and UDP:
  Serial.begin(115200);  
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  // Initializing the joystick at A12(H) and A13(V) + 35 (Joystick 1). A14 and A15, 36 is also possible for Joystick 2
  utils.joystick_init(10, A12, A13, 35);
  
    // Shows free memory:  
//  Serial << F("freeMemory()=") << freeMemory() << "\n";
}

void loop() {

  // Joystick test:
  if (utils.joystick_hasMoved(0) || utils.joystick_hasMoved(1))  {
    Serial << F("New joystick position:") << utils.joystick_position(0) << "," << utils.joystick_position(1) << "\n";
  }
  if (utils.joystick_buttonUp())  {
    Serial << F("Joystick Button Up") << "\n";
  }
  if (utils.joystick_buttonDown())  {
    Serial << F("Joystick Button Down") << "\n";
  }
  if (utils.joystick_buttonIsPressed())  {
    Serial << F("Joystick Button Pressed") << "\n";
  }
}


