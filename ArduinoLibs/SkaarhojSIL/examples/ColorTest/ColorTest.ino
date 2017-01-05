/*****************

 * - kasper
 */

// All related to library "SkaarhojSIL":
#include "Wire.h"
#include "MCP23017.h"
#include "PCA9685.h"
#include "SkaarhojSIL.h"
#include "Streaming.h"

// Setting up a board:
SkaarhojSIL board;


void setup() {
  Serial.begin(115200); // set up serial
  Serial.println("Serial Started");
  
  randomSeed(analogRead(2));

  // Always initialize Wire before setting up the SkaarhojSIL class!
  Wire.begin();

  // Set up the board:
  board.begin(7); // Address 7, solder jumpers all ON
  board.testSequence();  // Runs LED test sequence

  for (uint8_t i = 1; i <= 10; i++)  {
    board.setButtonColor(i, i);
  }
}

void loop() {
  for (uint8_t i = 1; i <= 10; i++)  {
    if (board.buttonDown(i))  {
      uint8_t R = random(0, 101);
      uint8_t G = random(0, 101);
      uint8_t B = random(0, 101);
      Serial << F("Button ") << i << F(" = ") << R << F(",") << G << F(",") << B << F("\n");
      board.setColorBalanceRGB(0, R, G, B);
      board.setButtonColor(i, 1); // Clear previous flag
      board.setButtonColor(i, 0);
    }
  }
}

