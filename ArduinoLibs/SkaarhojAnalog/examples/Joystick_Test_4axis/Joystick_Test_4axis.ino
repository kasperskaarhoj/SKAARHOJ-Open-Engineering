

#include "SkaarhojAnalog.h"
#include "ADS7828.h"
#include "Wire.h"
SkaarhojAnalog joystick;

//#include <MemoryFree.h>

#include <Streaming.h>



void setup() { 

  // Start the Serial (debugging) and UDP:
  Serial.begin(115200);  
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  // Initializing the joystick at A12(H) and A13(V) + 35 (Joystick 1). A14 and A15, 36 is also possible for Joystick 2
  joystick.joystick_init(10, 0);
  
    // Shows free memory:  
//  Serial << F("freeMemory()=") << freeMemory() << "\n";
}

void loop() {

  // Joystick test:
  if (joystick.joystick_hasMoved(0) || joystick.joystick_hasMoved(1) || joystick.joystick_hasMoved(2))  {
    Serial << F("New joystick position:") << joystick.joystick_position(0) << "," << joystick.joystick_position(1) << "," << joystick.joystick_position(2) << "\n";
  }
  if (joystick.joystick_buttonUp())  {
    Serial << F("Joystick Button Up") << "\n";
  }
  if (joystick.joystick_buttonDown())  {
    Serial << F("Joystick Button Down") << "\n";
  }
  if (joystick.joystick_buttonIsPressed())  {
    Serial << F("Joystick Button Pressed") << "\n";
  }
}

