/*****************
 * Basic T-bar example with a potentiometer or real T-bar
 * Works with ethernet-enabled arduino devices (Arduino Ethernet or a model with Ethernet shield)
 * Make sure to configure IP and addresses! Look for all instances of "<= SETUP" in the code below!
 *
 * This example also uses SkaarhojUtils library. 
 * Download it from http://skaarhoj.com/wiki/index.php/Libraries_for_Arduino
 *
 * Connect the outer pins of a 10K potentiometer to GND and 5V. Connect the middle pin to A0. Connect also a 10nF capacitor between GND and middle pin (stabilizes the analog readout)
 * See also http://skaarhoj.com/wiki/index.php/SKAARHOJ_Slider_board
 * 
 * - kasper
 */



// Including libraries: 
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>


// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192, 168, 10, 99);              // <= SETUP!  IP address of the Arduino


// Include ATEM library and make an instance:
// Connect to an ATEM switcher on this address and using this local port:
// The port number is chosen randomly among high numbers.
#include <ATEM.h>
ATEM AtemSwitcher(IPAddress(192, 168, 10, 240), 56417);  // <= SETUP (the IP address of the ATEM switcher)



// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}



// All related to library "SkaarhojUtils". Used for "T-bar" potentiometer:
#include "SkaarhojUtils.h"
SkaarhojUtils utils;



void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(9600);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  

  // Initializing the slider:
  utils.uniDirectionalSlider_init(10, 25, 25, A0);  // These 4 arguments are the default arguments given, if you call the function without any. See the library SkaarhojUtils for more info on what they are for.

  // Initialize a connection to the switcher:
  AtemSwitcher.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher.connect();
}



bool AtemOnline = false;

void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  // If connection is gone, try to reconnect:
  if (AtemSwitcher.isConnectionTimedOut())  {
    if (AtemOnline)  {
      AtemOnline = false;

      Serial << F("ATEM switcher went offline\n");
    }

    Serial << F("Connection to ATEM Switcher has timed out - reconnecting!\n");
    AtemSwitcher.connect();
  }

  // If the switcher has been initialized:
  if (AtemSwitcher.hasInitialized())  {
    if (!AtemOnline)  {
      AtemOnline = true;
      Serial << F("ATEM switcher went online\n");
    }


    commandDispatch();
  }
}





/*************************
 * Commands handling
 *************************/
void commandDispatch()  {
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

