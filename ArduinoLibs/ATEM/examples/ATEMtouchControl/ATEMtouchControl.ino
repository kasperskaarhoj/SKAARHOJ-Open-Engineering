/*****************
 * Touch Control for ATEM panel
 * Requires connection to a touch screen via the SKAARHOJ utils library.
 * You must display the ATEM multiviewer on the screen in the default configuration
 * The sketch will let you select Preview inputs by touching the input source thumbnails
 * By touching Preview you make a cut, swiping from Preview to Program makes an Auto transion.
 * Touch and hold the Program window to enter special "Directly to Program" mode where selection of input source 
 * goes directly to Program.
 *
 * This example also uses several custom libraries which you must install first. 
 * Search for "#include" in this file to find the libraries. Then download the libraries from http://skaarhoj.com/wiki/index.php/Libraries_for_Arduino
 *
 * Works with ethernet-enabled arduino devices (Arduino Ethernet or a model with Ethernet shield)
 * Make sure to configure IP and addresses! Look for all instances of "<= SETUP" in the code below!
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




// Include the SkaarhojUtils library. 
#include "SkaarhojUtils.h"
SkaarhojUtils utils;
int touchCal[8];    // Will hold the Arduino Ethernet shield/board MAC address (loaded from EEPROM memory, set with ConfigEthernetAddresses example sketch)
uint8_t checksumByte;

// If this flag is set true, a tap on a source brings it directly to program
// If a Bi-color LED is connected to digital output 2 (green) and 3 (red) (both active high) it will light red when this functionality is on and green when preview select is on. It will light orange if it's in the process of connecting to the ATEM Switcher.
bool directlyToProgram = false;  


void setup() { 

    // Touch Screen power-up. Should not affect touch screen solution where it is not wired up.
    // Uses D4 to trigger the "on" signal (through a 15K res), first set it in Tri-state mode.
    // Uses Analog pin A5 to detect from the LED voltage levels whether it is already on or not
  pinMode(4, INPUT);
  delay(3000);
  if (analogRead(A5)>770)  {  // 700 is the threshold value found by trying. If off, it's around 790, if on, it's around 650
    // Now, trying to turn the screen on:
    digitalWrite(4,0);  // First, set the next output of D4 to low
    pinMode(4, OUTPUT);  // Change D4 from an high impedance input to an output, effectively triggering a button press
    delay(500);  // Wait a while.
    pinMode(4, INPUT);  // Change D4 back to high impedance input.
  }
  
  
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

    // Initialize touch library
  utils.touch_init();


  // Touch Screen calibration:
  touchCal[0] = (EEPROM.read(50) << 8) | EEPROM.read(51);
  touchCal[1] = (EEPROM.read(52) << 8) | EEPROM.read(53);
  touchCal[2] = (EEPROM.read(54) << 8) | EEPROM.read(55);
  touchCal[3] = (EEPROM.read(56) << 8) | EEPROM.read(57);
  touchCal[4] = (EEPROM.read(58) << 8) | EEPROM.read(59);
  touchCal[5] = (EEPROM.read(60) << 8) | EEPROM.read(61);
  touchCal[6] = (EEPROM.read(62) << 8) | EEPROM.read(63);
  touchCal[7] = (EEPROM.read(64) << 8) | EEPROM.read(65);
  checksumByte = ((int)(touchCal[0]
    + touchCal[1]
    + touchCal[2]
    + touchCal[3]
    + touchCal[4]
    + touchCal[5]
    + touchCal[6]
    + touchCal[7]))  & 0xFF;  // checksum
  if (checksumByte == EEPROM.read(66))  {
    Serial << F("TouchScreen Calibration Data: \n    (") 
      << touchCal[0] << "," 
      << touchCal[1] << "," 
      << touchCal[2] << "," 
      << touchCal[3] << "," 
      << touchCal[4] << "," 
      << touchCal[5] << "," 
      << touchCal[6] << "," 
      << touchCal[7] << ")" 
      << " - Checksum: " << checksumByte << " => OK\n";
    utils.touch_calibrationPointRawCoordinates(touchCal[0],touchCal[1],touchCal[2],touchCal[3],touchCal[4], touchCal[5],touchCal[6],touchCal[7]);
  } 
  else {
    Serial << F("Calibration not found in memory!\n") << 
      F("Please run the Touchscreen Calibration sketch first\n") <<
      F("Now, using default calibration"); 

    // The line below is calibration numbers for a specific monitor. 
    // Substitute this with calibration for YOUR monitor (see example "Touch_Calibrate")
    utils.touch_calibrationPointRawCoordinates(330,711,763,716,763,363,326,360);
  }
    

  
  // Sets the Bi-color LED to off = "no connection"
  digitalWrite(2,false);
  digitalWrite(3,false);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);


  // Initialize a connection to the switcher:
  AtemSwitcher.begin(IPAddress(atem_ip[0],atem_ip[1],atem_ip[2],atem_ip[3]), 56417);
  AtemSwitcher.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher.connect();
  
    // Set Bi-color LED orange - indicates "connecting...":
  digitalWrite(3,true);
  digitalWrite(2,true);
}


bool AtemOnline = false;
uint8_t MVFieldMapping[] = {1,2,3,4, 5,6,7,8}; // Maps the multiviewer fields 1-4,5-8 in two rows to input source numbers. This needs to correspond to the switcher connected

void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  // If connection is gone, try to reconnect:
  if (AtemSwitcher.isConnectionTimedOut())  {
    if (AtemOnline)  {
      AtemOnline = false;
      
        // Set Bi-color LED off = "no connection"
      digitalWrite(3,false);
      digitalWrite(2,false);
    }

    Serial.println("Connection to ATEM Switcher has timed out - reconnecting!");
    AtemSwitcher.connect();

    // Set Bi-color LED orange - indicates "connecting...":
    digitalWrite(3,true);
    digitalWrite(2,true);
  }

  // If the switcher has been initialized, check for button presses as reflect status of switcher in button lights:
  if (AtemSwitcher.hasInitialized())  {
    if (!AtemOnline)  {
      AtemOnline = true;
      Serial << "ATEM model: " << AtemSwitcher.getATEMmodel() << "\n";
      if (AtemSwitcher.getATEMmodel()==0)  {  // TVS:
        MVFieldMapping[3] = 10;  // Media Player 1
        MVFieldMapping[4] = 4;
        MVFieldMapping[5] = 5;
        MVFieldMapping[6] = 6;
        MVFieldMapping[7] = 12;  // Media Player 2
      }
    }

    AtemSwitcher.delay(15);	// This determines how smooth you will see the graphic move.
    manageTouch();

    // Set Bi-color LED to red or green depending on mode:
    digitalWrite(3,directlyToProgram);
    digitalWrite(2,!directlyToProgram);
  }
}


void manageTouch()  {

  uint8_t tState = utils.touch_state();  // See what the state of touch is:
  if (tState !=0)  {  // Different from zero? Then a touch even has happened:
  
      // Calculate which Multiviewer field on an ATEM we pressed:
    uint8_t MVfieldPressed = touchArea(utils.touch_getEndedXCoord(),utils.touch_getEndedYCoord());

    if (tState==9 && MVfieldPressed==16)  {  // held down in program area
      directlyToProgram=!directlyToProgram;
      Serial.print("directlyToProgram=");
      Serial.println(directlyToProgram);
    } else
    if (tState==1)  {  // Single tap somewhere:
      if (MVfieldPressed == AtemSwitcher.getPreviewInput() || MVfieldPressed==17)  {  // If Preview or a source ON preview is touched, bring in on.
        AtemSwitcher.doCut();
      } 
      else if (MVfieldPressed>=1 && MVfieldPressed<=8)  {  // If any other source is touched, bring it to preview or program depending on mode:
        if (directlyToProgram)  {
          AtemSwitcher.changeProgramInput(MVFieldMapping[MVfieldPressed-1]);
        } else {
          AtemSwitcher.changePreviewInput(MVFieldMapping[MVfieldPressed-1]);
        }
      }
    } else
    if (tState==10) {  // Left -> Right swipe
      if (MVfieldPressed==16 && touchArea(utils.touch_getStartedXCoord(),utils.touch_getStartedYCoord())==17)  {
         Serial << F("Swipe from Preview to Program\n"); 
         AtemSwitcher.doAuto();
      }
    }
  }
}

// Returns which field in MV was pressed:
uint8_t touchArea(int x, int y)  {
  if (y<=720/2)  {  // lower than middel
    if (x<=1280/4)  {
      if (y>=720/4)  {
        // 1: 
        return 1;
      } 
      else {
        // 5:
        return 5;
      }
    } 
    else if (x<=1280/2) {
      if (y>=720/4)  {
        // 2: 
        return 2;
      } 
      else {
        // 6:
        return 6;
      }
    } 
    else if (x<=1280/4*3) {
      if (y>=720/4)  {
        // 3: 
        return 3;
      } 
      else {
        // 7:
        return 7;
      }
    } 
    else {
      if (y>=720/4)  {
        // 4: 
        return 4;
      } 
      else {
        // 8:
        return 8;
      }
    }
  } 
  else {  // Preview or program:
    if (x<=1280/2)  {
      // Preview:
      return 17;
    } 
    else {
      // Program:
      return 16;
    }
  }
}

