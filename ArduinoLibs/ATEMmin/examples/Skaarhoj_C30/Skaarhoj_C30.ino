/*****************
 * Basis control for the SKAARHOJ C30 series devices
 * This example is programmed for ATEM 1M/E & TVS versions
 * The button rows are assumed to be configured as 1-2-3-AUTO / 4-5-6-CUT
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

// Include ATEM library and make an instance:
#include <ATEMmin.h>
ATEMmin AtemSwitcher;

#include <MemoryFree.h>
#include <Streaming.h>
#include <SkaarhojPgmspace.h>
#include "SkaarhojTools.h"
SkaarhojTools sTools(1);

// Configure the IP addresses and MAC address with the sketch "ConfigEthernetAddresses":
uint8_t ip[4];        // Will hold the Arduino IP address
uint8_t atem_ip[4];  // Will hold the ATEM IP address
uint8_t mac[6];    // Will hold the Arduino Ethernet shield/board MAC address (loaded from EEPROM memory, set with ConfigEthernetAddresses example sketch)



// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
//template<class T>
//inline Print &operator <<(Print &obj, T arg)
//{  
//  obj.print(arg); 
//  return obj; 
//}



// All related to library "SkaarhojBI8":
#include "Wire.h"
#include "MCP23017.h"
#include "PCA9685.h"
#include "SkaarhojBI8.h"
#include "SkaarhojUtils.h"

SkaarhojBI8 buttons;
SkaarhojUtils utils;



void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Serial.begin(9600);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  



  // Setting the Arduino IP address:
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




  // Always initialize Wire before setting up the SkaarhojBI8 class!
  Wire.begin(); 

  // Set up the SkaarhojBI8 boards:
  buttons.begin(0);

  buttons.setDefaultColor(0);  // Off by default
  buttons.testSequence();

  // Initializing the slider:
  utils.uniDirectionalSlider_init();
  utils.uniDirectionalSlider_hasMoved();


  // Initialize a connection to the switcher:
  AtemSwitcher.begin(IPAddress(atem_ip[0],atem_ip[1],atem_ip[2],atem_ip[3]), 56417);
  //AtemSwitcher.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher.connect();
}



bool AtemOnline = false;

void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  lDelay(0);

  // If the switcher has been initialized, check for button presses as reflect status of switcher in button lights:
  if (AtemSwitcher.hasInitialized())  {
    if (!AtemOnline)  {
      AtemOnline = true;
      Serial << F("Turning on buttons\n");

      buttons.setDefaultColor(5);  // Dimmed by default
      buttons.setButtonColorsToDefault();
    }


    setButtonColors();
    commandDispatch();
  } else {
    // If connection is gone, try to reconnect:
    if (AtemOnline)  {
      AtemOnline = false;

      Serial << F("Turning off buttons light\n");
      buttons.setDefaultColor(0);  // Off by default
      buttons.setButtonColorsToDefault();
    }

    Serial << F("Connection to ATEM Switcher has timed out - reconnecting!\n");
  }
}




/*************************
 * Set button colors
 *************************/
void setButtonColors()  {

  // Setting colors of input select buttons:
  for (uint8_t i=1;i<=6;i++)  {
      buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(0)==i ? 2 : AtemSwitcher.getPreviewInputVideoSource(0)==i ? 3 : 5);
  }

  // Setting colors of the command buttons:
  buttons.setButtonColor(7, AtemSwitcher.getTransitionPosition(0)>0 ? 2 : 5);     // Auto button
  if (!buttons.buttonIsPressed(8))  {
    buttons.setButtonColor(8, 5);   // de-highlight CUT button
  }

}




/*************************
 * Commands handling
 *************************/
void commandDispatch()  {

  // Sending commands:
  uint8_t busSelection = buttons.buttonDownAll();
  if(busSelection > 0) {
    Serial << (busSelection, BIN) << "\n";
  }
  
  // The following 6 if-clauses detects if a button is pressed for input selection:
  if (buttons.isButtonIn(1, busSelection))  { 
    AtemSwitcher.setPreviewInputVideoSource(0, 1); 
  }
  if (buttons.isButtonIn(2, busSelection))   { 
    AtemSwitcher.setPreviewInputVideoSource(0, 2); 
  }
  if (buttons.isButtonIn(3, busSelection))   { 
    AtemSwitcher.setPreviewInputVideoSource(0, 3); 
  }
  if (buttons.isButtonIn(4, busSelection))  { 
    AtemSwitcher.setPreviewInputVideoSource(0, 4); 
  }
  if (buttons.isButtonIn(5, busSelection))  { 
    AtemSwitcher.setPreviewInputVideoSource(0, 5); 
  }
  if (buttons.isButtonIn(6, busSelection))   { 
    AtemSwitcher.setPreviewInputVideoSource(0, 6); 
  }

  // The following detects if a button is pressed for either AUTO or CUT:
  if (buttons.isButtonIn(7, busSelection))  { 
    AtemSwitcher.performAutoME(0); 
  }
  if (buttons.isButtonIn(8, busSelection))  {
    buttons.setButtonColor(8,4);    // Highlight CUT button
    AtemSwitcher.performCutME(0); 
  }
  
  
  // "T-bar" slider:
  if (utils.uniDirectionalSlider_hasMoved())  {
    AtemSwitcher.setTransitionPosition(0, 10 * utils.uniDirectionalSlider_position());
    lDelay(20);
    if (utils.uniDirectionalSlider_isAtEnd())  {
      AtemSwitcher.setTransitionPosition(0, 0);
      lDelay(5);
    }
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
