/*****************
 * This example will first run the LED test sequence on SkaarhojBI8 board with address 0 on the I2C bus 
 * You should see all color combinations of the buttons shown
 * Then, in the run loop it will look for button presses and a) cycle the color of the button pressed 
 * and b) print the button number to Serial-out
 * 
 * - kasper
 */

// All related to library "SkaarhojBI8":
#include "Wire.h"
#include "MCP23017.h"
#include "PCA9685.h"
#include "SkaarhojBI8.h"

// Setting up a board:
SkaarhojBI8 board;


void setup() { 
  Serial.begin(115200); // set up serial
  Serial.println("Serial Started");
  
  // Always initialize Wire before setting up the SkaarhojBI8 class!
  Wire.begin(); 
  
  // Set up the board:
  board.begin(0,false,true);  // Address 0, dip-switch on board is OFF for both 1 and 2
  board.testSequence();  // Runs LED test sequence
}

uint8_t buttonColorTracking[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void loop() {
  for(int i=1;i<=16;i++)  {
    if (board.buttonDown(i))  {
      Serial.print("Button #");
      Serial.print(i);
      Serial.print(" was pressed down");

      uint8_t nextColor = nextButtonColor(i);
      board.setButtonColor(i, nextColor);

      Serial.print(" - new color index: ");
      Serial.print(nextColor);
      Serial.println();
    }  
    if (board.buttonUp(i))  {
      Serial.print("Button #");
      Serial.print(i);
      Serial.println(" was released");
    }  
  }  
  
  if (board.buttonIsHeldFor(1, 1000))  {
    Serial.println("1 was held");
  }
  if (board.buttonIsHeldFor(5, 3000))  {
    Serial.println("5 was held");
  }
  if (board.buttonIsReleasedAgo(1, 1000))  {
    Serial.println("1 was released");
  }
  if (board.buttonIsReleasedAgo(2, 2000))  {
    Serial.println("2 was released");
  }
 
}


uint8_t nextButtonColor(int i) {
  buttonColorTracking[i-1]++;
  if (buttonColorTracking[i-1]> (board.isRGBboard()?9:4))  {
    buttonColorTracking[i-1]=0;
  }
  return buttonColorTracking[i-1];
}
