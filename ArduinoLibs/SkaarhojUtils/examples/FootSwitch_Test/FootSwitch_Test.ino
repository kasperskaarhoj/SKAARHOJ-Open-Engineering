/*****************
 * Foot Pedal Test
 * (Arduino Mega Extended Breakout Shield)
 * 
 * - kasper
 */
 
 
#include "SkaarhojUtils.h"
SkaarhojUtils utils;
 

#include <Streaming.h>

void setup() { 
    
  // Start the Ethernet, Serial (debugging) and UDP:
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  
  

  // Initializing the footswitch, telling digital pin numbers for tip and ring:
  utils.switch_init(33,34);
}

// the loop routine runs over and over again forever:
void loop() {
  
  if (utils.switch_buttonUp(0))  {
    Serial << "Tip Up" << "\n";
  }
  if (utils.switch_buttonDown(0))  {
    Serial << "Tip Down" << "\n";
  }

  if (utils.switch_buttonUp(1))  {
    Serial << "Ring Up" << "\n";
  }
  if (utils.switch_buttonDown(1))  {
    Serial << "Ring Down" << "\n";
  }
  delay(10);
}


