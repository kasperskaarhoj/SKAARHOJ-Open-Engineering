/*****************
 * Basis control for the SKAARHOJ C50 series devices
 * This example is programmed for ATEM 1M/E versions
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

SkaarhojBI8 inputSelect;
SkaarhojBI8 cmdSelect;



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
  inputSelect.begin(0);
  cmdSelect.begin(1);

  inputSelect.setDefaultColor(0);  // Off by default
  cmdSelect.setDefaultColor(0);  // Off by default

  inputSelect.testSequence();
  cmdSelect.testSequence();

  // Initialize a connection to the switcher:
  AtemSwitcher.begin(IPAddress(atem_ip[0],atem_ip[1],atem_ip[2],atem_ip[3]), 56417);
//  AtemSwitcher.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher.connect();
}

// These variables are used to track state, for instance when the VGA+PIP button has been pushed.
bool preVGA_active = false;
bool preVGA_UpstreamkeyerStatus = false;
int preVGA_programInput = 0;
bool AtemOnline = false;


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


    setButtonColors();
    commandDispatch();
  }
}






/*************************
 * Set button colors
 *************************/
void setButtonColors()  {
  // Setting colors of input select buttons:
  for (uint8_t i=1;i<=8;i++)  {
    uint8_t idx = i>4 ? i-4 : i+4;  // Mirroring because of buttons on PCB
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

  // Setting colors of the command buttons:
  cmdSelect.setButtonColor(3, AtemSwitcher.getTransitionPosition()>0 ? 4 : 5);     // Auto button
  cmdSelect.setButtonColor(8, preVGA_active ? 4 : 5);     // VGA+PIP button
  cmdSelect.setButtonColor(4, AtemSwitcher.getUpstreamKeyerStatus(1) ? 4 : 5);     // PIP button
  cmdSelect.setButtonColor(7, AtemSwitcher.getDownstreamKeyerStatus(1) ? 4 : 5);    // DSK1 button
  if (!cmdSelect.buttonIsPressed(1))  {
    cmdSelect.setButtonColor(1, 5);   // de-highlight CUT button
  }
}




/*************************
 * Commands handling
 *************************/
void commandDispatch()  {

  // Sending commands, bus input selection:
  uint8_t busSelection = inputSelect.buttonDownAll();
  if (inputSelect.isButtonIn(1, busSelection))  { 
    AtemSwitcher.changePreviewInput(5); 
  }
  if (inputSelect.isButtonIn(2, busSelection))   { 
    AtemSwitcher.changePreviewInput(6); 
  }
  if (inputSelect.isButtonIn(3, busSelection))   { 
    AtemSwitcher.changePreviewInput(7); 
  }
  if (inputSelect.isButtonIn(4, busSelection))  { 
    AtemSwitcher.changePreviewInput(8); 
  }
  if (inputSelect.isButtonIn(5, busSelection))  { 
    AtemSwitcher.changePreviewInput(1); 
  }
  if (inputSelect.isButtonIn(6, busSelection))   { 
    AtemSwitcher.changePreviewInput(2); 
  }
  if (inputSelect.isButtonIn(7, busSelection))  { 
    AtemSwitcher.changePreviewInput(3); 
  }
  if (inputSelect.isButtonIn(8, busSelection))  { 
    AtemSwitcher.changePreviewInput(4); 
  }



  // Reading buttons from the Command BI8 board:
  uint8_t cmdSelection = cmdSelect.buttonDownAll();
  if (cmdSelection & (B1 << 0))  { 
    commandCut();
  }
  if (cmdSelection & (B1 << 2))  { 
    commandAuto();
  }
  if (cmdSelection & (B1 << 3))  {
    commandPIP();
  }
  if (cmdSelection & (B1 << 6))  { 
    commandDSK1();
  }
  if (cmdSelection & (B1 << 7))  {
    commandPIPVGA();
  }  
}


