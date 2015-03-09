/*****************

 * - kasper
 */

// All related to library "SkaarhojBI8":
#include "Wire.h"
#include "MCP23017.h"
#include "PCA9685.h"
#include "SkaarhojBI8.h"
#include "Streaming.h"

// Setting up a board:
SkaarhojBI8 board;


void setup() {
  Serial.begin(115200); // set up serial
  Serial.println("Serial Started");
  
  randomSeed(analogRead(2));

  // Always initialize Wire before setting up the SkaarhojBI8 class!
  Wire.begin();

  // Set up the board:
  board.begin(0, false, true); // Address 0, dip-switch on board is OFF for both 1 and 2
  board.testSequence();  // Runs LED test sequence

  for (uint8_t i = 1; i <= 8; i++)  {
    board.setButtonColor(i, i);
  }
}

void loop() {
  for (uint8_t i = 1; i <= 8; i++)  {
    if (board.buttonDown(i))  {
      uint8_t R = random(0, 101);
      uint8_t G = random(0, 101);
      uint8_t B = random(0, 101);
      Serial << F("Button ") << i << F(" = ") << R << F(",") << G << F(",") << B << F("\n");
      board.setColorBalanceRGB(i, R, G, B);
      board.setButtonColor(i, i-1);  // To clear previous...
      board.setButtonColor(i, i);
    }
  }
}

