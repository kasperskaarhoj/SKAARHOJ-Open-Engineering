/*****************
 * Basic control for the SKAARHOJ 1UVJ model
 * Works with Ethernet enabled arduino devices (Arduino Ethernet or a model with Ethernet shield)
 * Make sure to configure IP and addresses! Look for "<= SETUP" in the code below!
 * 
 * - kasper
 */


#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>


// MAC address and IP address for this *particular* Ethernet Shield!
// MAC address is printed on the shield
// IP address is an available address you choose on your subnet where the switcher is also present:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xF2, 0x16 };      // <= SETUP!
IPAddress ip(192, 168, 0, 22);              // <= SETUP!


// Include ATEM library and make an instance:
#include <ATEM.h>

// Connect to an ATEM switcher on this address and using this local port:
// The port number is chosen randomly among high numbers.
ATEM AtemSwitcher;


// All related to library "SkaarhojBI8":
#include "Wire.h"
#include "MCP23017.h"
#include "PCA9685.h"
#include "SkaarhojBI8.h"

SkaarhojBI8 inputSelect1;
SkaarhojBI8 inputSelect2;


#include "SkaarhojUtils.h"
SkaarhojUtils utils;

void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(9600);  

  // Always initialize Wire before setting up the SkaarhojBI8 class!
  Wire.begin(); 

  // Set up the SkaarhojBI8 boards:
  inputSelect1.begin(0,false);
  inputSelect2.begin(1,false);

  inputSelect1.setDefaultColor(0);  // Off by default
  inputSelect2.setDefaultColor(0);  // Off by default

  inputSelect1.testSequence();
  inputSelect2.testSequence();


  // Initializing the slider:
  utils.uniDirectionalSlider_init();

  // Initialize a connection to the switcher:
  AtemSwitcher.begin(IPAddress(192, 168, 0, 50), 56417);    // <= SETUP!
 // AtemSwitcher.serialOutput(true);
  AtemSwitcher.connect();
}

bool AtemOnline = false;
bool programLeft = true;  // This flag indicates if the program bus is selected on the left selector (inputSelect1) or not.

uint8_t programInput = 255;
uint8_t previewInput = 255;

void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  // If connection is gone, try to reconnect:
  if (AtemSwitcher.isConnectionTimedOut())  {
    if (AtemOnline)  {
      AtemOnline = false;

      Serial.println("Turning off buttons light");
      inputSelect1.setDefaultColor(0);  // Off by default
      inputSelect2.setDefaultColor(0);  // Off by default
      inputSelect1.setButtonColorsToDefault();
      inputSelect2.setButtonColorsToDefault();
    }

    Serial.println("Connection to ATEM Switcher has timed out - reconnecting!");
    AtemSwitcher.connect();
  }

  // If the switcher has been initialized, check for button presses as reflect status of switcher in button lights:
  if (AtemSwitcher.hasInitialized())  {
    if (!AtemOnline)  {
      AtemOnline = true;
      Serial.println("Turning on buttons");

      inputSelect1.setDefaultColor(5);  // Dimmed by default
      inputSelect2.setDefaultColor(5);  // Dimmed by default
      inputSelect1.setButtonColorsToDefault();
      inputSelect2.setButtonColorsToDefault();
    }


    // Setting colors of input select buttons:
    if (programLeft)  {

      int programTally = 0;
      bool programTallyWasSet = false;
      for (uint8_t i=1;i<=8;i++)  {
          // Left panel:
        if (AtemSwitcher.getProgramTally(i))  {
          programTally = i;
          if (!AtemSwitcher.getPreviewTally(i))  {
            inputSelect1.setButtonColor(i, 2);
            programTallyWasSet = true;
          }
        } 
        else {
          inputSelect1.setButtonColor(i, 5);   
        }

          // Right panel:
        if (AtemSwitcher.getPreviewTally(i))  {
          if (AtemSwitcher.getProgramTally(i))  {
            inputSelect2.setButtonColor(i, 2);
          } else {
            inputSelect2.setButtonColor(i, 3);
          }
        } 
        else {
          inputSelect2.setButtonColor(i, 5);   
        }
      }
      
      if (!programTallyWasSet && programTally>0)  {
          inputSelect1.setButtonColor(programTally, 2);
      }

      AtemSwitcher.runLoop();

      // Sending commands:
      uint8_t busSelection = inputSelect1.buttonDownAll();
      if (inputSelect1.isButtonIn(1, busSelection))  { AtemSwitcher.changeProgramInput(1); }
      if (inputSelect1.isButtonIn(2, busSelection))   { AtemSwitcher.changeProgramInput(2); }
      if (inputSelect1.isButtonIn(3, busSelection))   { AtemSwitcher.changeProgramInput(3); }
      if (inputSelect1.isButtonIn(4, busSelection))  { AtemSwitcher.changeProgramInput(4); }
      if (inputSelect1.isButtonIn(5, busSelection))  { AtemSwitcher.changeProgramInput(5); }
      if (inputSelect1.isButtonIn(6, busSelection))   { AtemSwitcher.changeProgramInput(6); }
      if (inputSelect1.isButtonIn(7, busSelection))  { AtemSwitcher.changeProgramInput(7); }
      if (inputSelect1.isButtonIn(8, busSelection))  { AtemSwitcher.changeProgramInput(8); }

      // Sending commands:
      busSelection = inputSelect2.buttonDownAll();
      if (inputSelect2.isButtonIn(1, busSelection))  { AtemSwitcher.changePreviewInput(1); }
      if (inputSelect2.isButtonIn(2, busSelection))   { AtemSwitcher.changePreviewInput(2); }
      if (inputSelect2.isButtonIn(3, busSelection))   { AtemSwitcher.changePreviewInput(3); }
      if (inputSelect2.isButtonIn(4, busSelection))  { AtemSwitcher.changePreviewInput(4); }
      if (inputSelect2.isButtonIn(5, busSelection))  { AtemSwitcher.changePreviewInput(5); }
      if (inputSelect2.isButtonIn(6, busSelection))   { AtemSwitcher.changePreviewInput(6); }
      if (inputSelect2.isButtonIn(7, busSelection))  { AtemSwitcher.changePreviewInput(7); }

    } else {

      int programTally = 0;
      bool programTallyWasSet = false;
      for (uint8_t i=1;i<=8;i++)  {
          // Left panel:
        if (AtemSwitcher.getProgramTally(i))  {
          programTally = i;
          if (!AtemSwitcher.getPreviewTally(i))  {
            inputSelect2.setButtonColor(i, 2);
            programTallyWasSet = true;
          }
        } 
        else {
          inputSelect2.setButtonColor(i, 5);   
        }

          // Right panel:
        if (AtemSwitcher.getPreviewTally(i))  {
          if (AtemSwitcher.getProgramTally(i))  {
            inputSelect1.setButtonColor(i, 2);
          } else {
            inputSelect1.setButtonColor(i, 3);
          }
        } 
        else {
          inputSelect1.setButtonColor(i, 5);   
        }
      }
      
      if (!programTallyWasSet && programTally>0)  {
          inputSelect2.setButtonColor(programTally, 2);
      }
      
      AtemSwitcher.runLoop();

      // Sending commands:
      uint8_t busSelection = inputSelect2.buttonDownAll();
      if (inputSelect2.isButtonIn(1, busSelection))  { AtemSwitcher.changeProgramInput(1); }
      if (inputSelect2.isButtonIn(2, busSelection))   { AtemSwitcher.changeProgramInput(2); }
      if (inputSelect2.isButtonIn(3, busSelection))   { AtemSwitcher.changeProgramInput(3); }
      if (inputSelect2.isButtonIn(4, busSelection))  { AtemSwitcher.changeProgramInput(4); }
      if (inputSelect2.isButtonIn(5, busSelection))  { AtemSwitcher.changeProgramInput(5); }
      if (inputSelect2.isButtonIn(6, busSelection))   { AtemSwitcher.changeProgramInput(6); }
      if (inputSelect2.isButtonIn(7, busSelection))  { AtemSwitcher.changeProgramInput(7); }
      if (inputSelect2.isButtonIn(8, busSelection))  { AtemSwitcher.changeProgramInput(8); }

      // Sending commands:
      busSelection = inputSelect1.buttonDownAll();
      if (inputSelect1.isButtonIn(1, busSelection))  { AtemSwitcher.changePreviewInput(1); }
      if (inputSelect1.isButtonIn(2, busSelection))   { AtemSwitcher.changePreviewInput(2); }
      if (inputSelect1.isButtonIn(3, busSelection))   { AtemSwitcher.changePreviewInput(3); }
      if (inputSelect1.isButtonIn(4, busSelection))  { AtemSwitcher.changePreviewInput(4); }
      if (inputSelect1.isButtonIn(5, busSelection))  { AtemSwitcher.changePreviewInput(5); }
      if (inputSelect1.isButtonIn(6, busSelection))   { AtemSwitcher.changePreviewInput(6); }
      if (inputSelect1.isButtonIn(7, busSelection))  { AtemSwitcher.changePreviewInput(7); }
      if (inputSelect1.isButtonIn(8, busSelection))  { AtemSwitcher.changePreviewInput(8); }      


    }
    
    
    
      // "T-bar" slider:
    if (utils.uniDirectionalSlider_hasMoved())  {
      AtemSwitcher.changeTransitionPosition(utils.uniDirectionalSlider_position());
      Serial.println(utils.uniDirectionalSlider_position(), DEC);
      AtemSwitcher.delay(20);
      if (utils.uniDirectionalSlider_isAtEnd())  {
	programLeft = analogRead(0) > 500 ? true : false;

	AtemSwitcher.changeTransitionPositionDone();
        previewInput=255;
        programInput=255;
	Serial.println("Transition Done");
	AtemSwitcher.delay(5);  
      }
    }
    


  }
}
