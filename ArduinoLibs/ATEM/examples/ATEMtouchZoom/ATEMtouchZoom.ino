/*****************
 * Touch Control for ATEM panel
 * Requires connection to a touch screen via the SKAARHOJ utils library.
 * For the ATEM 1M/E
 * You must display the ATEM multiviewer on the screen through an AUX output (configurable). Do this by routing the MV output back into an input.
 * The sketch will let you select Preview inputs by touching the input source thumbnails
 * By touching Preview you make a cut, swiping from Preview to Program makes an Auto transion.
 * Touch and hold the Program or Preview window to enter special fullscreen mode.
 * When in "Program" full screen you can drag your finger diagonally over an area to zoom in on it (requres the DVE to be properly configured).
 *
 * Digital zoom requires that the DVE is configured for Upstream Key 1, having borders disabled.
 *
 * This example also uses several custom libraries which you must install first. 
 * Search for "#include" in this file to find the libraries. Then download the libraries from http://skaarhoj.com/wiki/index.php/Libraries_for_Arduino
 *
 * Works with ethernet-enabled arduino devices (Arduino Ethernet or a model with Ethernet shield)
 * Make sure to configure IP and addresses! Look for all instances of "<= SETUP" in the code below!
 * 
 * - kasper
 */


uint8_t AUXchannelForScreen = 3;
uint8_t MVinputChannel = 4;


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
  utils.touch_setExtended();

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
  // AtemSwitcher.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher.connect();

  // Set Bi-color LED orange - indicates "connecting...":
  digitalWrite(3,true);
  digitalWrite(2,true);
}


bool AtemOnline = false;
uint8_t MVFieldMapping[] = {
  1,2,3,4, 5,6,7,8}; // Maps the multiviewer fields 1-4,5-8 in two rows to input source numbers. This needs to correspond to the switcher connected
int screenMode = 0;  // 0=MV, 1=Preview, 2=Program



bool digitalZoomMode = false;

int xFact = 7950;
int yFact = 4450;

long    cur_centerXscr = 0;
long    cur_centerYscr = 0;
long    cur_theSize = 1000;


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

      AtemSwitcher.changeAuxState(AUXchannelForScreen, MVinputChannel);
      screenMode = 0;
    }

    AtemSwitcher.delay(15);
    manageTouch();
    AtemSwitcher.delay(15);
    manageTouch();
    AtemSwitcher.delay(15);
    manageTouch();

    // Set Bi-color LED to red or green depending on mode:
    digitalWrite(3,screenMode==2);
    digitalWrite(2,!(screenMode==2));
  }
}


void manageTouch()  {

  if (screenMode==0)  {  
    uint8_t tState = utils.touch_state();  // See what the state of touch is:
    if (tState !=0)  {  // Different from zero? Then a touch even has happened:

      // Calculate which Multiviewer field on an ATEM we pressed:
      uint8_t MVfieldPressed = touchArea(utils.touch_getEndedXCoord(),utils.touch_getEndedYCoord());

      if (tState==9 && MVfieldPressed==16)  {  // held down in program area
        screenMode=2;
        AtemSwitcher.changeAuxState(AUXchannelForScreen, 16);  // Program on AUX channel
        AtemSwitcher.delay(15);
        AtemSwitcher.changeUpstreamKeyFillSource(1,AtemSwitcher.getProgramInput());
        AtemSwitcher.delay(15);
        setDigitalZoom(true);
      } 
      else
        if (tState==9 && MVfieldPressed==17)  {  // held down in program area
          screenMode=1;
          AtemSwitcher.changeAuxState(AUXchannelForScreen, 17);  // Program on AUX channel
        } 
        else
          if (tState==1)  {  // Single tap somewhere:
            if (MVfieldPressed == AtemSwitcher.getPreviewInput() || MVfieldPressed==17)  {  // If Preview or a source ON preview is touched, bring in on.
              AtemSwitcher.doCut();
            } 
            else if (MVfieldPressed>=1 && MVfieldPressed<=8)  {  // If any other source is touched, bring it to preview or program depending on mode:
              AtemSwitcher.changePreviewInput(MVFieldMapping[MVfieldPressed-1]);
            }
          } 
          else
            if (tState==10) {  // Left -> Right swipe
              if (MVfieldPressed==16 && touchArea(utils.touch_getStartedXCoord(),utils.touch_getStartedYCoord())==17)  {
                Serial << F("Swipe from Preview to Program\n"); 
                AtemSwitcher.doAuto();
              }
            }
    }
  } 
  else if (screenMode==1)  {
    uint8_t tState = utils.touch_state();  // See what the state of touch is:
    if (tState !=0)  {  // Different from zero? Then a touch even has happened:
      AtemSwitcher.changeAuxState(AUXchannelForScreen, MVinputChannel);
      screenMode = 0;
    }
  } 
  else if (screenMode==2)  {
    uint8_t tState = utils.touch_state();  // See what the state of touch is:

    if (tState>=10) {
      if (digitalZoomMode)  {
        int xMax = utils.touch_coordX(utils.touch_getRawXValMax());
        int xMin = utils.touch_coordX(utils.touch_getRawXValMin());
        int yMax = utils.touch_coordY(utils.touch_getRawYValMax());
        int yMin = utils.touch_coordY(utils.touch_getRawYValMin());

        // Calculate size:
        int sizeX = (float)1280/(xMax-xMin)*10;
        int sizeY = (float)720/(yMax-yMin)*10;
        int theSize= (sizeX>sizeY ? sizeY : sizeX)*100;
        if (theSize<1200) theSize=1000;
        if (theSize>3000) theSize=3000;

        // Calculating center:
        int centerX = (long)xMin+(xMax-xMin)/2;
        int centerY = (long)yMin+(yMax-yMin)/2;

        long coordRangeX = (long)xFact * theSize / 1000 * 4;
        long coordRangeY = (long)yFact * theSize / 1000 * 4;

        long centerXscr = -((long)centerX*coordRangeX/1280-coordRangeX/2);
        long centerYscr = -((long)centerY*coordRangeY/720-coordRangeY/2);

        long maxCenterXscr = (long)(theSize-1000)*xFact*2/1000;
        long maxCenterYscr = (long)(theSize-1000)*yFact*2/1000;

        if (centerXscr>maxCenterXscr) centerXscr = maxCenterXscr;
        if (centerXscr<-maxCenterXscr) centerXscr = -maxCenterXscr;
        if (centerYscr>maxCenterYscr) centerYscr = maxCenterYscr;
        if (centerYscr<-maxCenterYscr) centerYscr = -maxCenterYscr;

        /* 
         Serial.print("Max: ");
         Serial.print(xMax);
         Serial.print(",");
         Serial.print(yMax);
         Serial.print("  - Min: ");
         Serial.print(xMin);
         Serial.print(",");
         Serial.print(yMin);
         Serial.print("  - Size: ");
         Serial.print(sizeX);
         Serial.print(",");
         Serial.print(sizeY);
         Serial.print("=>");
         Serial.print(theSize);
         Serial.print("  - Center: ");
         Serial.print(centerX);
         Serial.print(",");
         Serial.print(centerY);
         Serial.print("  - MaxCenterScr: ");
         Serial.print(maxCenterXscr);
         Serial.print(",");
         Serial.print(maxCenterYscr);
         Serial.print("  - CenterScr: ");
         Serial.print(centerXscr);
         Serial.print(",");
         Serial.print(centerYscr);
         Serial.println("");
         
         */
        for(int i=1;i<=100;i++)  {
          float factor = (float)(sin(-PI/2+PI/100*i)+1)/2;
          AtemSwitcher.changeDVESettingsTemp(
          (float)cur_centerXscr+(centerXscr-cur_centerXscr)*factor, 
          (float)cur_centerYscr+(centerYscr-cur_centerYscr)*factor, 
          (float)cur_theSize+(theSize-cur_theSize)*factor, 
          (float)cur_theSize+(theSize-cur_theSize)*factor
            );

          AtemSwitcher.delay(10);
        }

        cur_centerXscr = centerXscr;
        cur_centerYscr = centerYscr;
        cur_theSize = theSize;

        if (cur_theSize==1000)  {
          setDigitalZoom(false); 
          AtemSwitcher.changeAuxState(AUXchannelForScreen, MVinputChannel);
          screenMode = 0;
        }

        //          AtemSwitcher.changeDVESettingsTemp(unsigned long Xpos,unsigned long Ypos,unsigned long Xsize,unsigned long Ysize);
      }
    } 
    else if (tState !=0)  {  // Different from zero? Then a touch even has happened:
      setDigitalZoom(false); 
      AtemSwitcher.changeAuxState(AUXchannelForScreen, MVinputChannel);
      screenMode = 0;
    }
  }
}


void setDigitalZoom(bool enableDZ)  {
  if (enableDZ && !digitalZoomMode)  {
    cur_centerXscr = 0;
    cur_centerYscr = 0;
    cur_theSize = 1000;

    digitalZoomMode=true;
    AtemSwitcher.changeDVESettingsTemp(cur_centerXscr, cur_centerYscr, cur_theSize, cur_theSize);
    AtemSwitcher.delay(10);
    AtemSwitcher.changeUpstreamKeyOn(1, true);
  }

  if (!enableDZ && digitalZoomMode)  {
    digitalZoomMode=false;
    AtemSwitcher.changeUpstreamKeyOn(1, false);
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





