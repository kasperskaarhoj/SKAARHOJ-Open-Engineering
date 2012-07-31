/*****************
 * Basis control for the SKAARHOJ C100 models
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

#include <MemoryFree.h>

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>


// MAC address and IP address for this *particular* Ethernet Shield!
// MAC address is printed on the shield
// IP address is an available address you choose on your subnet where the switcher is also present:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0x63 };      // <= SETUP!
IPAddress ip(192,168,10,101);              // <= SETUP!


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
  AtemSwitcher.begin(IPAddress(192,168,10,240), 56417);    // <= SETUP!
  //  AtemSwitcher.serialOutput(true);
  AtemSwitcher.connect();
}





// State: If the ATEM Switcher is connected.
bool AtemOnline = false;

// These variables are used to track state, for instance when the VGA+PIP button has been pushed.
bool preVGA_active = false;
bool preVGA_UpstreamkeyerStatus = false;
int preVGA_programInput = 0;

// State variables: Auto pilot
bool functionAutoPilot = false;  // Running or not
unsigned int functionAutoPilotStepLengthMS[] = {
  10000, 7000, 8000, 10000, 6000};  // Step lengths in ms. Max 65535 (65 seconds)
int functionAutoPilotStepNum = 5;  // Number of steps (much match # of entries in functionAutoPilotStepLengthMS)
int functionAutoPilotStep = 0;    // Step counter for auto pilot, private
unsigned long functionAutoPilotTime = 0;  // Time tracking for auto pilot, private

// State variables: Zoom function
bool functionZoom = false;  // Running or not
int functionZoomActiveInput = 0;  // Tracking which program input has been processed.




// MAIN Arduino Loop
void loop() {
    //Serial.print("freeMemory()=");
    //Serial.println(freeMemory());
    
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


    setButtonColors();
    commandDispatch();

    runloopAutoPilot();
    runloopZoom();
  }
}





/*************************
 * Set button colors
 *************************/
void setButtonColors()  {
   
  // Setting colors of input select buttons:
    for (uint8_t i=1;i<=8;i++)  {
      uint8_t idx = i>4 ? i-4 : i+4;  // Mirroring because of buttons on PCB
      if (i==3)  {
        inputSelect.setButtonColor(idx, functionZoom ? 4 : (AtemSwitcher.getUpstreamKeyerStatus(1)?0:5));
      } 
      else if (i==4)  {  // Button 3 and 4 are used for Zoom and Auto Pilot in this special case
        inputSelect.setButtonColor(idx, functionAutoPilot ? (millis()%2000<1000?2:0) : 
        (AtemSwitcher.getUpstreamKeyerStatus(1)?0:5));
      } 
      else {
        if (AtemSwitcher.getProgramTally(i))  {
          inputSelect.setButtonColor(idx, 2);
        } 
        else if (AtemSwitcher.getPreviewTally(i))  {
          inputSelect.setButtonColor(idx, 3);
        } 
        else {
          inputSelect.setButtonColor(idx, 5);   
        }
      }
    }

    // Setting colors of the command buttons:
    cmdSelect.setButtonColor(3, AtemSwitcher.getTransitionPosition()>0 ? 4 : 5);     // Auto button
    cmdSelect.setButtonColor(8, functionAutoPilot||functionZoom?0:(preVGA_active ? 4 : 5));     // VGA+PIP button
    cmdSelect.setButtonColor(4, functionAutoPilot||functionZoom?0:(AtemSwitcher.getUpstreamKeyerStatus(1) ? 4 : 5));     // PIP button
    cmdSelect.setButtonColor(7, AtemSwitcher.getDownstreamKeyerStatus(1) ? 4 : 5);    // DSK1 button
    if (!cmdSelect.buttonIsPressed(1))  {
      cmdSelect.setButtonColor(1, 5);   // de-highlight CUT button
    }
}





/*************************
 * Commands handling
 *************************/
void commandDispatch()  {
    // Sending commands:
    uint8_t busSelection = inputSelect.buttonDownAll();
    if (inputSelect.isButtonIn(1, busSelection))  { 
      AtemSwitcher.changePreviewInput(5); 
      stopAutoPilot(); 
    }
    if (inputSelect.isButtonIn(2, busSelection))   { 
      AtemSwitcher.changePreviewInput(6); 
      stopAutoPilot(); 
    }
    if (inputSelect.isButtonIn(3, busSelection))   { 
      AtemSwitcher.changePreviewInput(7); 
      stopAutoPilot(); 
    }
    if (inputSelect.isButtonIn(4, busSelection))  { 
      AtemSwitcher.changePreviewInput(8); 
      stopAutoPilot(); 
    }
    if (inputSelect.isButtonIn(5, busSelection))  { 
      AtemSwitcher.changePreviewInput(1); 
      stopAutoPilot(); 
    }
    if (inputSelect.isButtonIn(6, busSelection))   { 
      AtemSwitcher.changePreviewInput(2); 
      stopAutoPilot(); 
    }

    // These buttons are Zoom and Auto Pilot:
    if (inputSelect.isButtonIn(7, busSelection))  { 
      commandZoom();
    }
    if (inputSelect.isButtonIn(8, busSelection))  { 
      commandAutoPilot();
    }


      // Reading buttons from the Command BI8 board:
    uint8_t cmdSelection = cmdSelect.buttonDownAll();
    if (cmdSelection & (B1 << 0))  { 
      commandCut();
    }
    if (cmdSelection & (B1 << 2))  { 
      commandAuto();
    }
    if (!functionAutoPilot && !functionZoom && (cmdSelection & (B1 << 3)))  {
      commandPIP();
    }
    if (cmdSelection & (B1 << 6))  { 
      commandDSK1();
    }
    if (!functionAutoPilot && !functionZoom && (cmdSelection & (B1 << 7)))  {
      commandPIPVGA();
    }  
}
void commandCut()  {
      stopAutoPilot(); 

      cmdSelect.setButtonColor(1, 4);    // Highlight CUT button

      // If VGA is the one source, make Auto instead!
      if (AtemSwitcher.getProgramInput()==1 || AtemSwitcher.getPreviewInput()==1)  {
        AtemSwitcher.doAuto(); 
      } 
      else {
        AtemSwitcher.doCut(); 
      }
      preVGA_active = false;
      AtemSwitcher.delay(20);

      if (AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1)) {
        Serial.println("disable next trans....");
        AtemSwitcher.changeUpstreamKeyNextTransition(1, false);  // Set upstream key next transition
        AtemSwitcher.delay(20);
      }  
}
void commandAuto()  {
      stopAutoPilot(); 
      
      AtemSwitcher.doAuto(); 
      preVGA_active = false;
      AtemSwitcher.delay(20);

      if (AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1)) {
        Serial.println("disable next trans....");
        AtemSwitcher.changeUpstreamKeyNextTransition(1, false);  // Set upstream key next transition
        AtemSwitcher.delay(20);
      }  
}
void commandPIP()  {
      // For Picture-in-picture, do an "auto" transition:
      unsigned long timeoutTime = millis()+5000;

      // First, store original preview input:
      uint8_t tempPreviewInput = AtemSwitcher.getPreviewInput();  

      // Then, set preview=program (so auto doesn't change input)
      AtemSwitcher.changePreviewInput(AtemSwitcher.getProgramInput());  
      while(AtemSwitcher.getProgramInput()!=AtemSwitcher.getPreviewInput())  {
        AtemSwitcher.runLoop();
        if (timeoutTime<millis()) {
          break;
        }
      }

      // Then set transition status:
      bool tempOnNextTransitionStatus = AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1);
      AtemSwitcher.changeUpstreamKeyNextTransition(1, true);  // Set upstream key next transition
      while(!AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1))  {  
        AtemSwitcher.runLoop();
        if (timeoutTime<millis()) {
          break;
        }
      }

      // Make Auto Transition:      
      AtemSwitcher.doAuto();
      while(AtemSwitcher.getTransitionPosition()==0)  {
        AtemSwitcher.runLoop();
        if (timeoutTime<millis()) {
          break;
        }
      }
      while(AtemSwitcher.getTransitionPosition()>0)  {
        AtemSwitcher.runLoop();
        if (timeoutTime<millis()) {
          break;
        }
      }

      // Then reset transition status:
      AtemSwitcher.changeUpstreamKeyNextTransition(1, tempOnNextTransitionStatus);
      while(tempOnNextTransitionStatus!=AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1))  {  
        AtemSwitcher.runLoop();
        if (timeoutTime<millis()) {
          break;
        }
      }
      // Reset preview bus:
      AtemSwitcher.changePreviewInput(tempPreviewInput);  
      while(tempPreviewInput!=AtemSwitcher.getPreviewInput())  {
        AtemSwitcher.runLoop();
        if (timeoutTime<millis()) {
          break;
        }
      }
      // Finally, tell us how we did:
      if (timeoutTime<millis()) {
        Serial.println("Timed out during operation!");
      } 
      else {
        Serial.println("DONE!");
      }  
}
void commandPIPVGA()  {
      if (!preVGA_active)  {
        preVGA_active = true;
        preVGA_UpstreamkeyerStatus = AtemSwitcher.getUpstreamKeyerStatus(1);
        preVGA_programInput = AtemSwitcher.getProgramInput();

        AtemSwitcher.changeProgramInput(1);
        AtemSwitcher.changeUpstreamKeyOn(1, true); 
      } 
      else {
        preVGA_active = false;
        AtemSwitcher.changeProgramInput(preVGA_programInput);
        AtemSwitcher.changeUpstreamKeyOn(1, preVGA_UpstreamkeyerStatus); 
      }
}
void commandDSK1()  {
  AtemSwitcher.changeDownstreamKeyOn(1, !AtemSwitcher.getDownstreamKeyerStatus(1));
}
void commandZoom()  {
  if (!functionZoom) {
        if (!AtemSwitcher.getUpstreamKeyerStatus(1))  {  // Do not allow to start the auto pilot if the keyer is already active with something else (like PIP)!
          startZoom();
        }
  } 
  else {
    stopZoom();
  } 
}
void commandAutoPilot()  {
      if (!functionAutoPilot) {
        if (!AtemSwitcher.getUpstreamKeyerStatus(1))  {  // Do not allow to start the auto pilot if the keyer is already active with something else (like PIP)!
          startAutoPilot();
        }
      } 
      else {
        stopAutoPilot();
      } 
}





/*************************
 * Auto Pilot functions
 *************************/
void startAutoPilot()  {  // Starting the auto pilot
  functionAutoPilot = true;
  functionZoom = false;

  // Select last step and 0 time so that first runloopAutoPilot() call will increment to step 0:
  functionAutoPilotTime = 0;
  functionAutoPilotStep = functionAutoPilotStepNum-1;  

  // Prepare first step:
  AtemSwitcher.changePreviewInput(8);
  AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change
}
void stopAutoPilot()  {  // Stopping it. More actions might need to be combined in, for instance if the keyer is enabled at this time.
  functionAutoPilot = false;
}
void runloopAutoPilot()  {  // Auto Pilot "runloop". This must be called in the runloop of the main program and this function will fire any events the auto pilot controls
  if (functionAutoPilot)  {

    // Checking timing; If a new auto pilot event has come.
    if ((unsigned long)millis()-functionAutoPilotTime > functionAutoPilotStepLengthMS[functionAutoPilotStep])  {
      functionAutoPilotStep = (functionAutoPilotStep+1)%functionAutoPilotStepNum;  // Circulates the step numbers.
      functionAutoPilotTime = millis();

      Serial.print("Next autopilot step: ");
      Serial.println(functionAutoPilotStep);

      // The action to take when a new autopilot step arises:
      switch(functionAutoPilotStep)  {
        case 0:
            // Preview should be input 8!
          AtemSwitcher.doCut();
          AtemSwitcher.delay(20);
          
          // No zoom: 
          AtemSwitcher.changeUpstreamKeyNextTransition(1, false);  // Set upstream key next transition
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
          
          // Prepare for next:
          AtemSwitcher.delay(1000);  // This delay is just here to make the next selection of preview source seem a bit less rushed when watching the auto pilot at work. It can be removed.
          AtemSwitcher.changePreviewInput(5);
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
        break; 
        case 1:
          AtemSwitcher.doCut();
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
  
          // Zoom:
          startDveBasedKenBurns(5);

          // Prepare for next:
          AtemSwitcher.delay(1000);  // This delay is just here to make the next selection of preview source seem a bit less rushed when watching the auto pilot at work. It can be removed.
          AtemSwitcher.changePreviewInput(7);
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
        break; 
        case 2:
          AtemSwitcher.doCut();
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
          
          // No zoom: 
          AtemSwitcher.changeUpstreamKeyNextTransition(1, false);  // Set upstream key next transition
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
  
          // Prepare for next:
          AtemSwitcher.delay(1000);  // This delay is just here to make the next selection of preview source seem a bit less rushed when watching the auto pilot at work. It can be removed.
          AtemSwitcher.changePreviewInput(8);
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
        break; 
        case 3:
          AtemSwitcher.doCut();
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
          
          // No zoom:
          AtemSwitcher.changeUpstreamKeyNextTransition(1, false);  // Set upstream key next transition
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
  
          // Prepare for next:
          AtemSwitcher.delay(1000);  // This delay is just here to make the next selection of preview source seem a bit less rushed when watching the auto pilot at work. It can be removed.
          AtemSwitcher.changePreviewInput(6);
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
        break; 
        case 4:
          AtemSwitcher.doCut();
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
  
          // Zoom:
          startDveBasedKenBurns(6);
          
          // Prepare for next:
          AtemSwitcher.delay(1000);  // This delay is just here to make the next selection of preview source seem a bit less rushed when watching the auto pilot at work. It can be removed.
          AtemSwitcher.changePreviewInput(8);
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
        break; 
      }
    }
  }
}


/*************************
 * Zoom functions
 *************************/
void startZoom()  {
  functionZoom = true;
  functionAutoPilot = false;
  functionZoomActiveInput = -1;
}
void stopZoom()  {
  functionZoom = false;
}
void runloopZoom()  {  // Auto Pilot "runloop". This must be called in the runloop of the main program and this function will fire any events the auto pilot controls
  if (functionZoom)  {
    if (AtemSwitcher.getProgramInput() != functionZoomActiveInput)  {
      functionZoomActiveInput = AtemSwitcher.getProgramInput();
      Serial.println("Change in program input");
      switch(functionZoomActiveInput)  {
        case 5:
        case 6:
          startDveBasedKenBurns(functionZoomActiveInput);
        break;
      }
    }
  }
}




/*************************
 * Various Helper Functions
 *************************/
void startDveBasedKenBurns(uint8_t inputNum)  {
          AtemSwitcher.changeDVESettingsTemp_Rate(1);  // 1 frame
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
          AtemSwitcher.changeDVESettingsTemp_RunKeyFrame(3);  // Run to full - quickly!
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
          AtemSwitcher.changeUpstreamKeyFillSource(1, inputNum);  // Set the Fill Source of upstr. keyer 1 to 6 - like what is set at program...
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
          AtemSwitcher.changeDVESettingsTemp_Rate(250);  // 10 seconds runtime...
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
          AtemSwitcher.changeUpstreamKeyOn(1,true);
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
          AtemSwitcher.changeDVESettingsTemp_RunKeyFrame(1);
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change 
          AtemSwitcher.changeUpstreamKeyNextTransition(1, true);  // Set upstream key next transition
          AtemSwitcher.delay(20);  // Workaround for the fact that we are not looking for a value change   
}





