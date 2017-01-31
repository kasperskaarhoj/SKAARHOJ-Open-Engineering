/*****************
 * This example will run the LED test sequence on all SkaarhojSIL boards on the I2C bus
 * You should see all color combinations of the buttons shown
 * - kasper
 */

// All related to library "SkaarhojSIL":
#include "Wire.h"
#include "MCP23017.h"
#include "PCA9685.h"
#include "SkaarhojSIL.h"

// Setting up four boards, addresses 0-7 (solder jumpers):
SkaarhojSIL boards[8];
bool boardArrayEnabled[8];

void setup() { 
  
  Wire.begin(); // Always initialize Wire before setting up the SkaarhojSIL class!
  
	// Set up each board:
  for(uint8_t i=0; i<8; i++) {
    boardArrayEnabled[i] = boards[i].begin(i);
    boards[i].setDefaultColor(9); // Default color fully ON
  }
}


void loop() {
	// Run the LED test sequence for each board:
  for(uint8_t i=0; i<8; i++) {
    if(boardArrayEnabled[i]) {
      boards[i].testSequence(50);
    }
  }
  delay(5000);
}
