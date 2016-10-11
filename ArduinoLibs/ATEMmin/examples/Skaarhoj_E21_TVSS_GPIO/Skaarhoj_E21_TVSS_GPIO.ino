/*****************
 * Basis control for the SKAARHOJ E21-TVSS series devices. It's based on a C90 standard sketch, but for BI16 board with small one-color bottons and GPIO module.
 * This example is programmed for ATEM TVS versions
 *
 * This example also uses several custom libraries which you must install first. 
 * Search for "#include" in this file to find the libraries. Then download the libraries from http://skaarhoj.com/wiki/index.php/Libraries_for_Arduino
 *
 * Works with ethernet-enabled arduino devices (Arduino Ethernet or a model with Ethernet shield)
 * Make sure to configure IP and addresses first using the sketch "ConfigEthernetAddresses"
 * 
 * - kasper
 */



// Including libraries: 
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EEPROM.h>      // For storing IP numbers
#include <SkaarhojPgmspace.h>
#include <SkaarhojTools.h>
#include <SkaarhojPgmspace.h>

SkaarhojTools sTools(1);    // 0=No runtime serial logging, 1=Moderate runtime serial logging, 2=more verbose... etc.

// Include ATEM library and make an instance:
#include <ATEMbase.h>
#include <ATEMmin.h>
ATEMmin AtemSwitcher;

#include <MemoryFree.h>
#include <Streaming.h>

// Configure the IP addresses and MAC address with the sketch "ConfigEthernetAddresses":
uint8_t ip[4];        // Will hold the Arduino IP address
uint8_t atem_ip[4];  // Will hold the ATEM IP address
uint8_t mac[6];    // Will hold the Arduino Ethernet shield/board MAC address (loaded from EEPROM memory, set with ConfigEthernetAddresses example sketch)



// All related to library "SkaarhojBI8":
#include "Wire.h"
#include "MCP23017.h"
#include "PCA9685.h"
#include "SkaarhojBI16.h"
#include "SkaarhojUtils.h"
#include "SkaarhojGPIO2x8.h"

// Setting up a board:
SkaarhojBI16 board;
// Setting up utilities:
SkaarhojUtils utils;
// Setting the SkaarhojGPIO2x8 board:
SkaarhojGPIO2x8 GPIOboard;

uint8_t greenLED = 2;
uint8_t redLED = 3;





/*************************************************************
 *
 *
 *                     MAIN PROGRAM CODE AHEAD
 *
 *
 **********************************************************/



void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  


 // *********************************
  // INITIALIZE EEPROM memory:
  // *********************************
  // Check if EEPROM has ever been initialized, if not, install default IP
  if (EEPROM.read(0) != 12 ||  EEPROM.read(1) != 232)  {  // Just randomly selected values which should be unlikely to be in EEPROM by default.
    // Set these random values so this initialization is only run once!
    EEPROM.write(0,12);
    EEPROM.write(1,232);

    // Set default IP address for Arduino/C100 panel (172.16.99.57)
    EEPROM.write(2,172);
    EEPROM.write(3,16);
    EEPROM.write(4,99);
    EEPROM.write(5,57);  // Just some value I chose, probably below DHCP range?

    // Set default IP address for ATEM Switcher (172.16.99.54):
    EEPROM.write(6,172);
    EEPROM.write(7,16);
    EEPROM.write(8,99);
    EEPROM.write(9,54);
  }


  // *********************************
  // Setting up IP addresses, starting Ethernet
  // *********************************

  ip[0] = EEPROM.read(0+2);
  ip[1] = EEPROM.read(1+2);
  ip[2] = EEPROM.read(2+2);
  ip[3] = EEPROM.read(3+2);
  

  // Setting the ATEM IP address:
  atem_ip[0] = EEPROM.read(0+2+4);
  atem_ip[1] = EEPROM.read(1+2+4);
  atem_ip[2] = EEPROM.read(2+2+4);
  atem_ip[3] = EEPROM.read(3+2+4);

  Serial << F("SKAARHOJ Device IP Address: ") << ip[0] << "." << ip[1] << "." << ip[2] << "." << ip[3] << "\n";
  Serial << F("ATEM Switcher IP Address: ") << atem_ip[0] << "." << atem_ip[1] << "." << atem_ip[2] << "." << atem_ip[3] << "\n";

  // Setting MAC address:
  mac[0] = EEPROM.read(10);
  mac[1] = EEPROM.read(11);
  mac[2] = EEPROM.read(12);
  mac[3] = EEPROM.read(13);
  mac[4] = EEPROM.read(14);
  mac[5] = EEPROM.read(15);
  char buffer[18];
  sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial << F("SKAARHOJ Device MAC address: ") << buffer << F(" - Checksum: ")
    << ((mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5]) & 0xFF) << "\n";
  if ((uint8_t)EEPROM.read(16)!=((mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5]) & 0xFF))  {
    Serial << F("MAC address not found in EEPROM memory!\n") <<
      F("Please load example sketch ConfigEthernetAddresses to set it.\n") <<
      F("The MAC address is found on the backside of your Ethernet Shield/Board\n (STOP)");
    while(true);
  }

  Ethernet.begin(mac, ip);





  // Always initialize Wire before setting up the SkaarhojBI16 class!
  Wire.begin(); 

  // Set up the board:
  board.begin(1);  // Address 1
  board.testSequence();  // Runs LED test sequence
  board.setDefaultColor(3);  // Dimmed by default
  board.setButtonColorsToDefault();

  // Initializing the slider:
  utils.uniDirectionalSlider_init();
  utils.uniDirectionalSlider_hasMoved();
  
  GPIOboard.begin();
  
   // Set GPIO:
  for (int i=1; i<=8; i++)  {
    GPIOboard.setOutput(i,HIGH);
    delay(100); 
  }
  // Clear:
  for (int i=1; i<=8; i++)  {
    GPIOboard.setOutput(i,LOW);
    delay(100); 
  }
  
  board.setDefaultColor(0);  // Off by default
  board.setButtonColorsToDefault();
  
 // Sets the Bi-color LED to off = "no connection"
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  digitalWrite(greenLED,false);
  digitalWrite(redLED,false);
 

    // Initialize a connection to the switcher:
    AtemSwitcher.begin(IPAddress(atem_ip[0], atem_ip[1], atem_ip[2], atem_ip[3]), 56417);    // <= SETUP!
//    AtemSwitcher.serialOutput(true);
    AtemSwitcher.connect();

    // Set Bi-color LED orange - indicates "connecting...":
    digitalWrite(redLED,true);
    digitalWrite(greenLED,true);
}

// Some pre-defined variables:
bool AtemOnline = false;
uint8_t buttonColorTracking[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void loop() {

    // Check for packets, respond to them etc. Keeping the connection alive!
    lDelay(0);
  
      // If the switcher has been initialized, check for button presses as reflect status of switcher in button lights:
      if (AtemSwitcher.hasInitialized())  {
        if (!AtemOnline)  {
          AtemOnline = true;
          Serial.println("Turning on buttons");
          
          // Set Bi-color LED to red or green depending on mode:
          digitalWrite(redLED,false);
          digitalWrite(greenLED,true);
    
          board.setDefaultColor(2);  // Dimmed by default
          board.setButtonColorsToDefault();
        }
    
    
        setButtonColors();
        commandDispatch();
        slider();
        // Selecting output mode: Let only ONE of the functions below be run - the others must be commented out:
        setTallyProgramOutputs();    // This will reflect inputs 1-8 Program tally on GPO 1-8
        checkGPI_setProgramBus();  // This will select the inputs 1-8 to be on the Program bus.

    } else { // If connection is gone, try to reconnect:
      if (AtemOnline)  {
        AtemOnline = false;
        
        // Set Bi-color LED off = "no connection"
        digitalWrite(redLED,true);
        digitalWrite(greenLED,false);
  
        Serial.println("Turning off buttons light");
        board.setDefaultColor(0);  // Off by default
        board.setButtonColorsToDefault();
      }
  
      Serial.println("Connection to ATEM Switcher has timed out - reconnecting!");
      
      // Set Bi-color LED orange - indicates "connecting...":
      digitalWrite(redLED,true);
      digitalWrite(greenLED,true);
      }
}






/*************************
 * Set button colors
 *************************/
void setButtonColors()  {
  // Setting colors of input select buttons:
  for (uint8_t i=1;i<=6;i++)  {
    if (AtemSwitcher.getProgramInputVideoSource(0)==i)  {
      board.setButtonColor(i+6, 1);
    }
    else {
       board.setButtonColor(i+6, 3);
    }
    if (AtemSwitcher.getPreviewInputVideoSource(0)==i)  {
      board.setButtonColor(i, 1);
    } 
    else {
      board.setButtonColor(i, 3);
    }
  }

  // Setting colors of the command buttons:
  if (!board.buttonIsPressed(13))  {
    board.setButtonColor(13, 2);   // de-highlight CUT button
  }
  board.setButtonColor(14, AtemSwitcher.getTransitionPosition(0)>0 ? 1 : 2);     // Auto button
  board.setButtonColor(15, AtemSwitcher.getKeyerOnAirEnabled(0, 0) ? 1 : 3);     // USK button

  if (AtemSwitcher.getProgramInputVideoSource(0)==3010)  {
      board.setButtonColor(16, 1);
    }
    else if (AtemSwitcher.getPreviewInputVideoSource(0)==3010)  {
       board.setButtonColor(16, 2);
    } else {
      board.setButtonColor(16, 3);
    }
}




/*************************
 * Commands handling
 *************************/
void commandDispatch()  {

  // Sending commands, bus input selection:
  uint16_t busSelection = board.buttonDownAll();
  if (board.isButtonIn(1, busSelection))  { 
    AtemSwitcher.setPreviewInputVideoSource(0, 1); 
  }
  if (board.isButtonIn(2, busSelection))  { 
    AtemSwitcher.setPreviewInputVideoSource(0, 2); 
  }
  if (board.isButtonIn(3, busSelection))  { 
    AtemSwitcher.setPreviewInputVideoSource(0, 3); 
  }
  if (board.isButtonIn(4, busSelection))  { 
    AtemSwitcher.setPreviewInputVideoSource(0, 4); 
  }
  if (board.isButtonIn(5, busSelection))  { 
    AtemSwitcher.setPreviewInputVideoSource(0, 5); 
  }
  if (board.isButtonIn(6, busSelection))  { 
    AtemSwitcher.setPreviewInputVideoSource(0, 6); 
  }

  if (board.isButtonIn(7, busSelection))  { 
    AtemSwitcher.setProgramInputVideoSource(0, 1); 
  }
  if (board.isButtonIn(8, busSelection))  { 
    AtemSwitcher.setProgramInputVideoSource(0, 2); 
  }
  if (board.isButtonIn(9, busSelection))  { 
    AtemSwitcher.setProgramInputVideoSource(0, 3); 
  }
  if (board.isButtonIn(10, busSelection))  { 
    AtemSwitcher.setProgramInputVideoSource(0, 4); 
  }
  if (board.isButtonIn(11, busSelection))  { 
    AtemSwitcher.setProgramInputVideoSource(0, 5); 
  }
  if (board.isButtonIn(12, busSelection))  { 
    AtemSwitcher.setProgramInputVideoSource(0, 6); 
  }

  if (board.isButtonIn(13, busSelection))  { 
    board.setButtonColor(13, 1);    // Highlight CUT button
    AtemSwitcher.performCutME(0);  // Cut
  }
  if (board.isButtonIn(14, busSelection))  { 
    AtemSwitcher.performAutoME(0);  // Auto
  }
  if (board.isButtonIn(15, busSelection))  {
    AtemSwitcher.setKeyerOnAirEnabled(0, 0, !AtemSwitcher.getKeyerOnAirEnabled(0, 0));
  }
  if (board.isButtonIn(16, busSelection))  { 
    AtemSwitcher.setPreviewInputVideoSource(0, 3010);
  } 
}


/*************************
 * Slider handling
 *************************/
void slider()  {
  // "T-bar" slider:
  if (utils.uniDirectionalSlider_hasMoved())  {
    AtemSwitcher.setTransitionPosition(0, 10*utils.uniDirectionalSlider_position());
    lDelay(20);
    if (utils.uniDirectionalSlider_isAtEnd())  {
      AtemSwitcher.setTransitionPosition(0, 0);
      lDelay(5);
    }
  }
}


/*************************
 * GPIO
 *************************/
void setTallyProgramOutputs()  {
   // Setting colors of input select buttons:
  if (AtemSwitcher.getProgramInputVideoSource(0)==1)  {
    GPIOboard.setOutput(1,HIGH);
  } else {
    GPIOboard.setOutput(1,LOW);
  }
  if (AtemSwitcher.getProgramInputVideoSource(0)==2)  {
    GPIOboard.setOutput(2,HIGH);
  } else {
    GPIOboard.setOutput(2,LOW);
  }
  if (AtemSwitcher.getProgramInputVideoSource(0)==3)  {
    GPIOboard.setOutput(3,HIGH);
  } else {
    GPIOboard.setOutput(3,LOW);
  }
  if (AtemSwitcher.getProgramInputVideoSource(0)==4)  {
    GPIOboard.setOutput(4,HIGH);
  } else {
    GPIOboard.setOutput(4,LOW);
  }
  if (AtemSwitcher.getProgramInputVideoSource(0)==5)  {
    GPIOboard.setOutput(5,HIGH);
  } else {
    GPIOboard.setOutput(5,LOW);
  }
  if (AtemSwitcher.getProgramInputVideoSource(0)==6)  {
    GPIOboard.setOutput(6,HIGH);
  } else {
    GPIOboard.setOutput(6,LOW);
  }
  if (AtemSwitcher.getProgramInputVideoSource(0)==3010)  {
    GPIOboard.setOutput(7,HIGH);
  } else {
    GPIOboard.setOutput(7,LOW);
  }
  if (AtemSwitcher.getProgramInputVideoSource(0)==3020)  {
    GPIOboard.setOutput(8,HIGH);
  } else {
    GPIOboard.setOutput(8,LOW);
  }
  
}

void checkGPI_setProgramBus()  {
  if (GPIOboard.inputDown(1))  {
    AtemSwitcher.setProgramInputVideoSource(0, 1);
    lDelay(100);  // to catch any rebounce from the switch
  }
  if (GPIOboard.inputDown(2))  {
    AtemSwitcher.setProgramInputVideoSource(0, 2);
    lDelay(100);  // to catch any rebounce from the switch
  }
  if (GPIOboard.inputDown(3))  {
    AtemSwitcher.setProgramInputVideoSource(0, 3);
    lDelay(100);  // to catch any rebounce from the switch
  }
  if (GPIOboard.inputDown(4))  {
    AtemSwitcher.setProgramInputVideoSource(0, 4);
    lDelay(100);  // to catch any rebounce from the switch
  }
  if (GPIOboard.inputDown(5))  {
    AtemSwitcher.setProgramInputVideoSource(0, 5);
    lDelay(100);  // to catch any rebounce from the switch
  }
  if (GPIOboard.inputDown(6))  {
    AtemSwitcher.setProgramInputVideoSource(0, 6);
    lDelay(100);  // to catch any rebounce from the switch
  }
  if (GPIOboard.inputDown(7))  {
    AtemSwitcher.setProgramInputVideoSource(0, 3010);
    lDelay(100);  // to catch any rebounce from the switch
  }
  if (GPIOboard.inputDown(8))  {
    AtemSwitcher.setProgramInputVideoSource(0, 3020);
    lDelay(100);  // to catch any rebounce from the switch
  }
}


/**
   Local delay function
*/
void lDelay(unsigned long timeout)  {
  unsigned long thisTime = millis();
  do {
    AtemSwitcher.runLoop();
    Serial << F(".");
    static int k = 1;
    k++;
    if (k > 100) {
      k = 1;
      Serial << F("\n");
    }
  }
  while (!sTools.hasTimedOut(thisTime, timeout));
}
