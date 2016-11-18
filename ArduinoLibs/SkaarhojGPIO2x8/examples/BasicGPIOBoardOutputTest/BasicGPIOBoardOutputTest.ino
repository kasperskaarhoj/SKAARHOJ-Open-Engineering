/*****************
 * Test of the GPIO2x16 board
 * It will start out by flipping each relay during initialization
 * In the runloop it will flip a relay if you activate the corresponding input.
 * On the serial monitor you can also read out the state of the pins
 * 
 * - kasper
 */



#include <Wire.h>
#include <MCP23017.h>
#include <SkaarhojGPIO2x8.h>

SkaarhojGPIO2x8 GPIOboard;

void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Serial.begin(115200);  
  Serial.println("Serial started");

  Wire.begin(); // Start the wire library for communication with the GPIO chip.

  GPIOboard.begin(0,1);
  
  // Set:
  for (int i=1; i<=16; i++)  {
   GPIOboard.setOutput(i,HIGH);
   delay(100); 
  }
  // Clear:
  for (int i=1; i<=16; i++)  {
   GPIOboard.setOutput(i,LOW);
   delay(100); 
  }
}

void loop() {
  
   for (int i=1; i<=16; i++)  {
   GPIOboard.setOutput(i,HIGH);
   delay(100); 
   GPIOboard.setOutput(i,LOW);
  }
}

