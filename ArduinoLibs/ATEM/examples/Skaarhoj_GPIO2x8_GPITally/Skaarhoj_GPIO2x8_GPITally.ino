/*****************
 * An GPI/Tally box for Blackmagic ATEM Switchers
 * 
 * This example REQUIRES the Skaarhoj GPIO2x8 shield for Arduino. That particular shield
 * is a general purpose GPIO board but with connectors and curcuitry compatible with
 * Blackmagic-Designs own GPI/Tally box.
 *
 * This example also uses a number of custom libraries which you must install first. 
 * Search for "#include" in this file to find the libraries. Then download the libraries from http://skaarhoj.com/wiki/index.php/Libraries_for_Arduino
 *
 * By default the example will reflect the Program tally state on the 8 camera inputs
 * The GPI inputs will set the corresponding input as Program
 * If you call the functions setTallyPreviewProgramOutputs() and checkGPI_setVarious() instead (see code)
 * you will see a demonstration of alternative configurations - how customized you can work with the ATEM Switcher
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
  0x90, 0xA2, 0xDA, 0x00, 0xF2, 0x00 };      // <= SETUP!
IPAddress ip(192, 168, 10, 99);              // <= SETUP!


// Include ATEM library and make an instance:
#include <ATEM.h>

// Connect to an ATEM switcher on this address and using this local port:
// The port number is chosen randomly among high numbers.
ATEM AtemSwitcher;


// Related to SkaarhojGPIO2x8:
#include <Wire.h>
#include <MCP23017.h>
#include <SkaarhojGPIO2x8.h>

SkaarhojGPIO2x8 GPIOboard;





void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(9600);  
  Serial.println("Serial started");

  Wire.begin(); // Start the wire library for communication with the GPIO chip.

  GPIOboard.begin();

  // Set:
  for (int i=1; i<=8; i++)  {
    GPIOboard.setOutput(i,HIGH);
    delay(100); 
  }
  // Clear:
  for (int i=1; i<=8; i++)  {
    GPIOboard.setOutput(i,LOW);
    delay(100); 
  }


  // Initialize a connection to the switcher:
  AtemSwitcher.begin(IPAddress(192, 168, 10, 240), 56417);    // <= SETUP!
//  AtemSwitcher.serialOutput(true);
  AtemSwitcher.connect();
}

bool AtemOnline = false;
void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

    // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher.isConnectionTimedOut())  {
     Serial.println("Connection to ATEM Switcher has timed out - reconnecting!");
     AtemSwitcher.connect();
  }  

  // Selecting output mode: Let only ONE of the functions below be run - the others must be commented out:
  setTallyProgramOutputs();    // This will reflect inputs 1-8 Program tally on GPO 1-8
//  setTallyPreviewProgramOutputs();    // This will reflect inputs 1-4 Program/Preview tally on GPO 1-8 (in pairs)


  checkGPI_setProgramBus();  // This will select the inputs 1-8 to be on the Program bus.
  //checkGPI_setVarious();  // This will call all sorts of switcher functions as a demonstration of how much crazy stuff can be done. See the function for more info.
}


void setTallyProgramOutputs()  {
   // Setting colors of input select buttons:
  for (uint8_t i=1;i<=8;i++)  {
    if (AtemSwitcher.getProgramTally(i))  {
      GPIOboard.setOutput(i,HIGH);
    }       
    else {
      GPIOboard.setOutput(i,LOW);
    }
  }
}

void setTallyPreviewProgramOutputs()  {
   // Setting colors of input select buttons:
  for (uint8_t i=1;i<=4;i++)  {
    if (AtemSwitcher.getProgramTally(i))  {
      GPIOboard.setOutput(i*2-1,HIGH);
    }       
    else {
      GPIOboard.setOutput(i*2-1,LOW);
    }

    if (AtemSwitcher.getPreviewTally(i))  {
      GPIOboard.setOutput(i*2,HIGH);
    }       
    else {
      GPIOboard.setOutput(i*2,LOW);
    }
  }
}

void checkGPI_setProgramBus()  {
  for(int i=1; i<=8; i++)  {
    if (GPIOboard.inputDown(i))  {
      AtemSwitcher.changeProgramInput(i);

      AtemSwitcher.delay(3);  // to catch any rebounce from the switch
      break;  // Only send one command at a time!
    }
  }
}

void checkGPI_setVarious()  {
  for(int i=1; i<=8; i++)  {
    if (GPIOboard.inputDown(i))  {
      
      switch(i){
        case 1:
          AtemSwitcher.doCut();  // Cut on GPI 1
        break; 
        case 2:
          AtemSwitcher.doAuto();   // Auto transition on GPI 2
        break; 
        case 3:
          AtemSwitcher.fadeToBlackActivate();   // Fade to black on GPI 3
        break; 
        case 4:
          AtemSwitcher.changeTransitionType(2); // Transition type "Wipe"  on GPI 4
        break; 
        case 5:
          AtemSwitcher.changeUpstreamKeyOn(1, AtemSwitcher.getUpstreamKeyerStatus(1)?false:true);   // Toggle Upstream keyer 1 on GPI 5
        break; 
        case 6:
          AtemSwitcher.changeAuxState(2, 16);  // Change AUX2 to Program on GPI 6
        break; 
        case 7:
          AtemSwitcher.changeColorValue(1, 2011, 246, 535);  // Set color value of Color generator 1 to a light blue plus set Color 1 on preview on GPI 7
          AtemSwitcher.delay(3);  // Allow command to go over ethernet and respond to reply of command before sending the next.
          AtemSwitcher.changePreviewInput(10);
        break; 
        case 8:
          AtemSwitcher.changeTransitionType(0);   // Change transition type to Mix on GPI 8
        break; 
      }

      AtemSwitcher.delay(10);  // to catch any rebounce from the switch
      break;  // Only send one command at a time!
    }
  }
}

