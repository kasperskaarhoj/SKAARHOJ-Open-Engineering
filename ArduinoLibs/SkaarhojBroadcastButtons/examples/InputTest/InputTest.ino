/*****************
 * This example will first run the LED test sequence on Skaarhoj Broadcast Button Shield
 * You should see all color combinations of the buttons shown
 * Then, in the run loop it will look for button presses and a) cycle the color of the button pressed 
 * and b) print the button number to Serial-out
 * Notice that you need to call .begin() with "true" if you are using a four button configuration (B3A-B3D)
 * 
 * - kasper
 */

#include "SkaarhojBroadcastButtons.h"

// Setting up a board:
SkaarhojBroadcastButtons board;


void setup() { 
  Serial.begin(115200); // set up serial
  Serial.println("Serial Started");
      
  // Set up the board:
  board.begin(false);  // B2A/B Configuration (two-buttons). If the four buttons B3A-D is used, send "true" instead. Relevant only for LED lighting.
  board.testSequence(50);  // Runs LED test sequence
}

uint8_t buttonColorTracking[] = {0,0,0,0};

void loop() {
  for(int i=1;i<=4;i++)  {
    if (board.buttonDown(i))  {
      Serial.print("Button #");
      Serial.print(i);
      Serial.println(" was pressed down");

      board.setButtonColor(i, nextButtonColor(i));
    }  
    if (board.buttonUp(i))  {
      Serial.print("Button #");
      Serial.print(i);
      Serial.println(" was released");
    }  
  }
}


uint8_t nextButtonColor(int i) {
  buttonColorTracking[i-1]++;
  if (buttonColorTracking[i-1]>5)  {
    buttonColorTracking[i-1]=0;
  }
  return buttonColorTracking[i-1];
}
