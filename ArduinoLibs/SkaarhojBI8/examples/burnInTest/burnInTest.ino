/*****************
 * This example will run the LED test sequence on 4 SkaarhojBI8 boards on the I2C bus (if they exist)
 * You should see all color combinations of the buttons shown
 * If you press a button on the board which is currently running the sequence test, you enter a special input mode.
 * In the input mode, all buttons will initially have the dimmed background color except one, which is red. 
 * Press the red button and it turns green plus another button now becomes red. Press that red button again etc. until all are pressed and it starts over.
 * If you leave the buttons for long enough, the LED sequence cycle will start again.
 * If you cannot press a button (for instance if it's not mounted), just wait 4 seconds and it will be auto pressed during the first run of the input mode.
 * If you press another button than the red one, it will become orange/amber. This can be useful to test for switch bounce: that is, if a single button press results in two registered presses by the arduino.
 * - kasper
 */

// All related to library "SkaarhojBI8":
#include "Wire.h"
#include "MCP23017.h"
#include "PCA9685.h"
#include "SkaarhojBI8.h"

//#include <MemoryFree.h>

// Setting up four boards, addresses 0-3 (onboard dip switch):
SkaarhojBI8 boardArray[] = {
  SkaarhojBI8(), SkaarhojBI8(), SkaarhojBI8(), SkaarhojBI8(), SkaarhojBI8(), SkaarhojBI8(), SkaarhojBI8(), SkaarhojBI8()};
bool boardArrayEnabled[8];

#include <Streaming.h>

void setup() { 
  Serial.begin(115200); // set up serial
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  // Shows free memory:  
  //Serial << F("freeMemory()=") << freeMemory() << "\n";

  // Always initialize Wire before setting up the SkaarhojBI8 class!
  Wire.begin();

  // Set up each board, test if it exists (looking for MCP23017 chip).
  for(uint8_t i=0;i<=7;i++)  {  
    boardArray[i].debugMode();
    boardArrayEnabled[i] = boardArray[i].begin(i);
    Serial << F("Board ") << i << " is " << (boardArrayEnabled[i]?"present":"NOT present") << "\n";
    boardArray[i].setDefaultColor(1);
  }

  // Random Seed:
  randomSeed(millis());
}

void loop() {
  // Run the LED test sequence for each board:
  for(uint8_t i=0;i<=7;i++)  {  
    if (boardArrayEnabled[i]) buttonPressSession(boardArray[i].testSequence(50), boardArray[i]);
  }
  delay(5000);
//  Serial << F("freeMemory()=") << freeMemory() << "\n";
}


/**
 * Starts a button press session if bDown is different from 0.
 */
void buttonPressSession(uint8_t bDown, SkaarhojBI8 &board)  {
  if (bDown)  {
    // Set buttons to faded colors:
    board.setDefaultColor(5);
    board.setButtonColorsToDefault();

    // Shuffle button numbers:
    Serial << F("================\n");  
    uint8_t buttonOrder[8];
    randomSequence(buttonOrder);
    uint8_t buttonIndex = 0;

    // 5 seconds of non activity and it will go back to running the auto sequence:
    unsigned long time = millis();
    bool onceThrough = false;
    while(time+5000 > (unsigned long)millis()) {  
      board.setButtonColor(buttonOrder[buttonIndex], 2);  // Set the red button up...
      bDown = board.buttonDownAll();
      if (time+3000 < (unsigned long)millis() && !onceThrough)  {
        bDown |=B1<<(buttonOrder[buttonIndex]-1);
      }
      if (bDown)  {
        time = millis();  // If a button was pushed, re-set the time.
        Serial << bDown << "\n";
        for(uint8_t i=0; i<=7;i++)  {  // Traverse all buttons
          if (board.isButtonIn(i+1, bDown))  {  // If button was pressed...
             if (i+1==buttonOrder[buttonIndex])  {  // If the red button was pushed, go on to the next button:
               board.setButtonColor(i+1, 3);  // For red button, set to green
               buttonIndex++;
               if (buttonIndex>=8)  {
                 board.setButtonColorsToDefault();
                 randomSequence(buttonOrder);
                 buttonIndex=0;
                 onceThrough = true;
               }
             } else {
               board.setButtonColor(i+1, 4);  // Set amber color, indicating a double press!               
             }
          }
        }
      }
    }
    board.setDefaultColor(1);
  } 
}

/**
 * Creates a random sequence of the numbers 1-8 in the array passed by reference:
 */
void randomSequence(uint8_t buttonOrder[])  {
  bool buttonNumberTaken[] = {
    0, 0, 0, 0, 0, 0, 0, 0    };
  uint8_t randPointer;
  for(uint8_t i=0;i<=7;i++)  {  
    randPointer = random(1, 9-i);

    for(uint8_t j=0;j<=7;j++)  {
      if (!buttonNumberTaken[j]) {
        randPointer--;
      }
      if (randPointer==0)  {
        buttonNumberTaken[j] = true;     
        buttonOrder[i] = j+1;
        Serial << F("Button ") << (j+1)  << "\n";
        break;
      }
    }
  }
}


