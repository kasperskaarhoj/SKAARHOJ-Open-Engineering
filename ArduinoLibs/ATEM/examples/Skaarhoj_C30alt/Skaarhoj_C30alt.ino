/*****************
 * Alternative control for the SKAARHOJ C30 series devices
 * This example is programmed for ATEM 1M/E versions
 * The button rows are assumed to be configured as AUX1 BUS-DSK1-Lower Third-Fade To Black / 1-2-3-CUT
 * When AUX1 is enabled, the bus input selector (1-2-3) will select those sources for the AUX1 output instead of Preview.
 * DSK1 enables downstream keyer 1. Lower Third simply enables upstream keyer 4 (assuming it is configured with a lower third graphic)
 * This example has a more modular approach to programming the buttons, trying to address each button separately when possible. Compare the code to the "Skaarhoj_C30" sketch to see this difference.
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
#include <ATEM.h>
ATEM AtemSwitcher;

//#include <MemoryFree.h>

// Configure the IP addresses and MAC address with the sketch "ConfigEthernetAddresses":
uint8_t ip[4];        // Will hold the Arduino IP address
uint8_t atem_ip[4];  // Will hold the ATEM IP address
uint8_t mac[6];    // Will hold the Arduino Ethernet shield/board MAC address (loaded from EEPROM memory, set with ConfigEthernetAddresses example sketch)



// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}



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

  // Initialize a connection to the switcher:
  AtemSwitcher.begin(IPAddress(atem_ip[0],atem_ip[1],atem_ip[2],atem_ip[3]), 56417);
//  AtemSwitcher.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher.connect();

    // Shows free memory:  
//  Serial << F("freeMemory()=") << freeMemory() << "\n";
}



bool AtemOnline = false;

void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  // If connection is gone, try to reconnect:
  if (AtemSwitcher.isConnectionTimedOut())  {
    if (AtemOnline)  {
      AtemOnline = false;

      Serial << F("Turning off buttons light\n");
      buttons.setDefaultColor(0);  // Off by default
      buttons.setButtonColorsToDefault();
    }

    Serial << F("Connection to ATEM Switcher has timed out - reconnecting!\n");
    AtemSwitcher.connect();
  }

  // If the switcher has been initialized, check for button presses as reflect status of switcher in button lights:
  if (AtemSwitcher.hasInitialized())  {
    if (!AtemOnline)  {
      AtemOnline = true;
      Serial << F("Turning on buttons\n");

      buttons.setDefaultColor(0);  // Dimmed by default
      buttons.setButtonColorsToDefault();
    }

    // Implement the functions of the buttons and slider:
    buttonFunctions();  // Notice: THis function is in another external file, ButtonFunctions.ino. It's open as a tab in the top of the Arduino IDE!
    sliderFunction();
  }
}



/*************************
 * Slider operation
 *************************/
void sliderFunction()  {

  // "T-bar" slider:
  if (utils.uniDirectionalSlider_hasMoved())  {
    AtemSwitcher.changeTransitionPosition(utils.uniDirectionalSlider_position());
    AtemSwitcher.delay(20);
    if (utils.uniDirectionalSlider_isAtEnd())  {
      AtemSwitcher.changeTransitionPositionDone();
      AtemSwitcher.delay(5);  
    }
  }
}


