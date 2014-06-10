/*****************
 * This example will run the LED test sequence on all SkaarhojBI8 boards on the I2C bus
 * You should see all color combinations of the buttons shown
 * - kasper
 */

// All related to library "SkaarhojBI8":
#include "Wire.h"
#include "MCP23017.h"
#include "PCA9685.h"
#include "SkaarhojBI8.h"

// Setting up four boards, addresses 0-3 (onboard dip switch):
SkaarhojBI8 board0;
SkaarhojBI8 board1;
SkaarhojBI8 board2;
SkaarhojBI8 board3;



void setup() { 
  
  Wire.begin(); // Always initialize Wire before setting up the SkaarhojBI8 class!
  
	// Set up each board:
  board0.begin(0,false);
  board1.begin(1,false);
  board2.begin(2,false);
  board3.begin(3,false);
  
	// Set default color to full "On":
  board0.setDefaultColor(1);
  board1.setDefaultColor(1);
  board2.setDefaultColor(1);
  board3.setDefaultColor(1);
}


void loop() {
	// Run the LED test sequence for each board:
  board0.testSequence(50);
  board1.testSequence(50);
  board2.testSequence(50);
  board3.testSequence(50);
  delay(5000);
}
