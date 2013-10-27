/*****************
 * Basis control for the SKAARHOJ C50 models
 * This example is programmed for ATEM 1M/E versions
 *
 * This example also uses a number of custom libraries which you must install first. 
 * Search for "#include" in this file to find the libraries. Then download the libraries from http://skaarhoj.com/wiki/index.php/Libraries_for_Arduino
 *
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
  0x90, 0xA2, 0xDA, 0x00, 0xE6, 0x73 };      // <= SETUP!
IPAddress ip(192, 168, 0, 20);              // <= SETUP!


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

SkaarhojBI8 inputSelect;
SkaarhojBI8 cmdSelect;



void setup() { 
  
  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(9600);  
  
    // Always initialize Wire before setting up the SkaarhojBI8 class!
  Wire.begin(); 
  
    // Set up the SkaarhojBI8 boards:
  inputSelect.begin(0,false);
  cmdSelect.begin(1,false);
  
  inputSelect.setDefaultColor(0);  // Off by default
  cmdSelect.setDefaultColor(0);  // Off by default
  
  inputSelect.testSequence();
  cmdSelect.testSequence();


  // Initialize a connection to the switcher:
  AtemSwitcher.begin(IPAddress(192, 168, 0, 50), 56417);    // <= SETUP!
//  AtemSwitcher.serialOutput(true);
  AtemSwitcher.connect();
}

// These variables are used to track state, for instance when the VGA+PIP button has been pushed.
bool preVGA_active = false;
bool preVGA_UpstreamkeyerStatus = false;
int preVGA_programInput = 0;
bool AtemOnline = false;

uint8_t inputMapping[] = {0,4,5,6,8,1,2,3,7}; 
uint8_t reverseInputMapping[] = {0,5,6,7,1,2,3,8,4};
bool busDirectlyToProgramState = false;

void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();
  
  // If connection is gone, try to reconnect:
  if (AtemSwitcher.isConnectionTimedOut())  {
      if (AtemOnline)  {
        AtemOnline = false;

        Serial.println("Turning off buttons light");
        inputSelect.setDefaultColor(0);  // Off by default
        cmdSelect.setDefaultColor(0);  // Off by default
        inputSelect.setButtonColorsToDefault();
        cmdSelect.setButtonColorsToDefault();
      }

     Serial.println("Connection to ATEM Switcher has timed out - reconnecting!");
     AtemSwitcher.connect();
  }

    // If the switcher has been initialized, check for button presses as reflect status of switcher in button lights:
  if (AtemSwitcher.hasInitialized())  {
    if (!AtemOnline)  {
      AtemOnline = true;
      Serial.println("Turning on buttons");

      inputSelect.setDefaultColor(5);  // Dimmed by default
      cmdSelect.setDefaultColor(5);  // Dimmed by default
      inputSelect.setButtonColorsToDefault();
      cmdSelect.setButtonColorsToDefault();
    }
    
    
    // Setting colors of input select buttons:
    for (uint8_t i=1;i<=8;i++)  {
      uint8_t idx = inputMapping[i]>4 ? inputMapping[i]-4 : inputMapping[i]+4;  // Mirroring because of buttons on PCB
      if (AtemSwitcher.getProgramTally(i))  {
        inputSelect.setButtonColor(idx, 2);
      } else if (AtemSwitcher.getPreviewTally(i))  {
        inputSelect.setButtonColor(idx, 3);
      } else {
        inputSelect.setButtonColor(idx, 5);   
      }
    }
    
    // Setting colors of the command buttons:
    cmdSelect.setButtonColor(3, AtemSwitcher.getTransitionPosition()>0 ? 4 : 5);     // Auto button
    cmdSelect.setButtonColor(8, busDirectlyToProgramState ? 2 : 5);     // VGA+PIP button
    cmdSelect.setButtonColor(4, AtemSwitcher.getAuxState(2)==5 ? 2 : 5);     // PIP button
    cmdSelect.setButtonColor(7, AtemSwitcher.getDownstreamKeyerStatus(1) ? 4 : 5);    // DSK1 button
    if (!cmdSelect.buttonIsPressed(1))  {
      cmdSelect.setButtonColor(1, 5);   // de-highlight CUT button
    }
  
  
    // Sending commands:
    uint8_t busSelection = inputSelect.buttonDownAll();
    for (uint8_t i=1;i<=8;i++)  {
      uint8_t idx = i>4 ? i-4 : i+4;  // Mirroring because of buttons on PCB
      if (inputSelect.isButtonIn(i, busSelection))  { 
        if (i==3)  {
          nameTag();
        } else {
          if (busDirectlyToProgramState)  {
            AtemSwitcher.changeProgramInput(reverseInputMapping[idx]); 
          } else {
            AtemSwitcher.changePreviewInput(reverseInputMapping[idx]); 
          }
        }
      }
    }    
  
    AtemSwitcher.delay(50);
    uint8_t cmdSelection = cmdSelect.buttonDownAll();
    if (cmdSelection & (B1 << 0))  { 
      cmdSelect.setButtonColor(1, 4);    // Highlight CUT button
      AtemSwitcher.doCut(); 
    }
    if (cmdSelection & (B1 << 2))  { AtemSwitcher.doAuto(); }
    if (cmdSelection & (B1 << 3))  {
      
       AtemSwitcher.changeAuxState(2, AtemSwitcher.getAuxState(2)!=5 ? 5 : 16);
    }  // PIP
    
    if (cmdSelection & (B1 << 6))  { AtemSwitcher.doAutoDownstreamKeyer(1); }  // DSK1
    if (cmdSelection & (B1 << 7))  { // Check Preview/ProgramCut
        busDirectlyToProgramState = !busDirectlyToProgramState;
    }
  }
}





void nameTag()  {
      cmdSelect.setDefaultColor(0);
      cmdSelect.setButtonColorsToDefault();
      inputSelect.setDefaultColor(0);
      inputSelect.setButtonColorsToDefault();
  
      inputSelect.setButtonColor(3, 2);     // PIP button
      
      

        // For Picture-in-picture, do an "auto" transition:
      unsigned long timeoutTime = millis()+15000;

        // First, store original preview input:
      uint8_t tempPreviewInput = AtemSwitcher.getPreviewInput();  
        
        // Then, set preview=program (so auto doesn't change input)
      AtemSwitcher.changePreviewInput(AtemSwitcher.getProgramInput());  
      while(AtemSwitcher.getProgramInput()!=AtemSwitcher.getPreviewInput())  {
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }
        
        // Then set transition status:
      bool tempOnNextTransitionStatus = AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(2);
      AtemSwitcher.changeUpstreamKeyNextTransition(2, true);  // Set upstream key next transition
      while(!AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(2))  {  
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }
        
        // Start Mediaplayer 2:
      AtemSwitcher.mediaPlayerClipStart(1);
      
        // Wait 0.5 sec before starting the transition on keyer one:
      AtemSwitcher.delay(500);

        // Make Auto Transition:      
      AtemSwitcher.doAuto();
      while(AtemSwitcher.getTransitionPosition()==0)  {
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }
      while(AtemSwitcher.getTransitionPosition()>0)  {
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }
      
        // Leave the name on for some time:
      AtemSwitcher.delay(5000);
      
        // Make another Auto Transition back:
      AtemSwitcher.doAuto();
      while(AtemSwitcher.getTransitionPosition()==0)  {
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }
      while(AtemSwitcher.getTransitionPosition()>0)  {
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }

        // Then reset transition status:
      AtemSwitcher.changeUpstreamKeyNextTransition(2, false);
      while(false!=AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(2))  {  
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }
        // Reset preview bus:
      AtemSwitcher.changePreviewInput(tempPreviewInput);  
      while(tempPreviewInput!=AtemSwitcher.getPreviewInput())  {
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }
        // Finally, tell us how we did:
     if (timeoutTime<millis()) {
       Serial.println("Timed out during operation!");
     } else {
      Serial.println("DONE!");
     }
     
     
      cmdSelect.setDefaultColor(5);
      inputSelect.setDefaultColor(5);
}
