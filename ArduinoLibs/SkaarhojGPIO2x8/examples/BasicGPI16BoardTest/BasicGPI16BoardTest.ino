/*****************
 * Test of the GPIO2x8 board
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

  GPIOboard.begin(0, 2);
}

void loop() {

     // Read GPI states:
  uint16_t inputs = GPIOboard.inputIsActiveAll();
  Serial.println(inputs, BIN);  

  delay(100);
}

