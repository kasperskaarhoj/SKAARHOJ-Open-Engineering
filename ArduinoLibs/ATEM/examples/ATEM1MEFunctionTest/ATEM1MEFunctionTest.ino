/*****************
 * Example: ATEM Library Functional test
 * Connects to the ATEM Switcher and sends a new command everytime digital pin 7 is HIGH on the arduino (connect a button with pull-down res.)
 * This is mostly an example for debugging a development of the library. However, it may serve as a reference for how to operate various functions in the Switcher.
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an Atem Switcher connected to the same network as the Arduino - and you should have it working with the desktop software
 * - You must make specific set ups in the below lines where the comment "// SETUP" is found!
 */





#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>


// MAC address and IP address for this *particular* Ethernet Shield!
// MAC address is printed on the shield
// IP address is an available address you choose on your subnet where the switcher is also present:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xE8, 0xE9 };		// <= SETUP
IPAddress ip(192, 168, 10, 99);				// <= SETUP


// Include ATEM library and make an instance:
#include <ATEM.h>

// Connect to an ATEM switcher on this address and using this local port:
// The port number is chosen randomly among high numbers.
ATEM AtemSwitcher(IPAddress(192, 168, 10, 240), 56417);  // <= SETUP (the IP address of the ATEM switcher)



void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(9600);  
  Serial.println("Serial started.");

  pinMode(7, INPUT);

  // Initialize a connection to the switcher:
  AtemSwitcher.serialOutput(true);
  AtemSwitcher.connect();
}

boolean buttonState = false;
int c = 0;

void loop() {
  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  // Detech press of a button on digital input 7:
  if (digitalRead(7))  {
    if (buttonState == false)  {
      c++;

      switch(c)  {
        // Program select
      case 1: 
        AtemSwitcher.changeProgramInput(0); 
        break;
      case 2: 
        AtemSwitcher.changeProgramInput(1); 
        break;
      case 3: 
        AtemSwitcher.changeProgramInput(2); 
        break;
      case 4: 
        AtemSwitcher.changeProgramInput(3); 
        break;
      case 5: 
        AtemSwitcher.changeProgramInput(4); 
        break;
      case 6: 
        AtemSwitcher.changeProgramInput(5); 
        break;
      case 7: 
        AtemSwitcher.changeProgramInput(6); 
        break;
      case 8: 
        AtemSwitcher.changeProgramInput(7); 
        break;
      case 9: 
        AtemSwitcher.changeProgramInput(8); 
        break;
      case 10: 
        AtemSwitcher.changeProgramInput(9); 
        break;
      case 11: 
        AtemSwitcher.changeProgramInput(10); 
        break;
      case 12: 
        AtemSwitcher.changeProgramInput(11); 
        break;
      case 13: 
        AtemSwitcher.changeProgramInput(12); 
        break;
      case 14: 
        AtemSwitcher.changeProgramInput(14); 
        break;
        // Preview select:
      case 15: 
        AtemSwitcher.changePreviewInput(0); 
        break;
      case 16: 
        AtemSwitcher.changePreviewInput(1); 
        break;
      case 17: 
        AtemSwitcher.changePreviewInput(2); 
        break;
      case 18: 
        AtemSwitcher.changePreviewInput(3); 
        break;
      case 19: 
        AtemSwitcher.changePreviewInput(4); 
        break;
      case 20: 
        AtemSwitcher.changePreviewInput(5); 
        break;
      case 21: 
        AtemSwitcher.changePreviewInput(6); 
        break;
      case 22: 
        AtemSwitcher.changePreviewInput(7); 
        break;
      case 23: 
        AtemSwitcher.changePreviewInput(8); 
        break;
      case 24: 
        AtemSwitcher.changePreviewInput(9); 
        break;
      case 25: 
        AtemSwitcher.changePreviewInput(10); 
        break;
      case 26: 
        AtemSwitcher.changePreviewInput(11); 
        break;
      case 27: 
        AtemSwitcher.changePreviewInput(12); 
        break;
      case 28: 
        AtemSwitcher.changePreviewInput(14); 
        break;
        // Cut / Transitions
      case 29: 
        AtemSwitcher.changeProgramInput(3); 
        AtemSwitcher.changePreviewInput(4);
        break;
      case 30: 
        AtemSwitcher.doCut(); 
        break;
      case 31: 
        AtemSwitcher.doAuto(); 
        break;
      case 32: 
        AtemSwitcher.fadeToBlackActivate(); 
        break;
      case 33: 
        AtemSwitcher.fadeToBlackActivate(); 
        break;
      case 34: 
        AtemSwitcher.changeTransitionPosition(500); 
        break;
      case 35: 
        AtemSwitcher.changeTransitionPosition(1000); 
        AtemSwitcher.changeTransitionPositionDone();
        break;
      case 36: 
        AtemSwitcher.changeTransitionPreview(true); 
        break;
      case 37: 
        AtemSwitcher.changeTransitionPreview(false); 
        break;
      case 38: 
        AtemSwitcher.changeTransitionType(1); 
        break;
      case 39: 
        AtemSwitcher.changeTransitionType(2); 
        break;
      case 40: 
        AtemSwitcher.changeTransitionType(3); 
        break;
      case 41: 
        AtemSwitcher.changeTransitionType(4); 
        break;
      case 42: 
        AtemSwitcher.changeTransitionType(0);   // Back to mix
        break;
      case 43: 
        AtemSwitcher.changeUpstreamKeyOn(1, true); 
        break;
      case 44: 
        AtemSwitcher.changeUpstreamKeyOn(2, true); 
        break;
      case 45: 
        AtemSwitcher.changeUpstreamKeyOn(3, true); 
        break;
      case 46: 
        AtemSwitcher.changeUpstreamKeyOn(4, true); 
        break;
      case 47: 
        AtemSwitcher.changeUpstreamKeyOn(1, false); 
        break;
      case 48: 
        AtemSwitcher.changeUpstreamKeyOn(2, false); 
        break;
      case 49: 
        AtemSwitcher.changeUpstreamKeyOn(3, false); 
        break;
      case 50: 
        AtemSwitcher.changeUpstreamKeyOn(4, false); 
        break;
      case 51: 
        AtemSwitcher.changeUpstreamKeyNextTransition(1, true); 
        break;
      case 52: 
        AtemSwitcher.changeUpstreamKeyNextTransition(2, true); 
        break;
      case 53: 
        AtemSwitcher.changeUpstreamKeyNextTransition(3, true); 
        break;
      case 54: 
        AtemSwitcher.changeUpstreamKeyNextTransition(4, true); 
        break;
      case 55: 
        AtemSwitcher.changeUpstreamKeyNextTransition(1, false); 
        break;
      case 56: 
        AtemSwitcher.changeUpstreamKeyNextTransition(2, false); 
        break;
      case 57: 
        AtemSwitcher.changeUpstreamKeyNextTransition(3, false); 
        break;
      case 58: 
        AtemSwitcher.changeUpstreamKeyNextTransition(4, false); 
        break;
      case 59: 
        AtemSwitcher.changeDownstreamKeyOn(1, true); 
        break;
      case 60: 
        AtemSwitcher.changeDownstreamKeyOn(2, true); 
        break;
      case 61: 
        AtemSwitcher.changeDownstreamKeyOn(1, false); 
        break;
      case 62: 
        AtemSwitcher.changeDownstreamKeyOn(2, false); 
        break;
      case 63: 
        AtemSwitcher.changeDownstreamKeyTie(1, true); 
        break;
      case 64: 
        AtemSwitcher.changeDownstreamKeyTie(2, true); 
        break;
      case 65: 
        AtemSwitcher.changeDownstreamKeyTie(1, false); 
        break;
      case 66: 
        AtemSwitcher.changeDownstreamKeyTie(2, false); 
        break;

      case 67: 
        AtemSwitcher.doAutoDownstreamKeyer(1);
        break;
      case 68: 
        AtemSwitcher.doAutoDownstreamKeyer(2);
        break;
      case 69: 
        AtemSwitcher.doAutoDownstreamKeyer(1);
        break;
      case 70: 
        AtemSwitcher.doAutoDownstreamKeyer(2);
        break;
  
      case 71:
        AtemSwitcher.changeAuxState(1, 1);
        break;
      case 72:
        AtemSwitcher.changeAuxState(1, 16);
        break;
      case 73:
        AtemSwitcher.changeAuxState(2, 1);
        break;
      case 74:
        AtemSwitcher.changeAuxState(2, 16);
        break;
      case 75:
        AtemSwitcher.changeAuxState(3, 1);
        break;
      case 76:
        AtemSwitcher.changeAuxState(3, 16);
        break;
        
      case 77:
        AtemSwitcher.changeColorValue(1, 2011, 246, 535);
        break;
      case 78:
        AtemSwitcher.changeColorValue(2, 54, 792, 497);
        break;
      case 79:
        AtemSwitcher.changeColorValue(1, 0, 0, 30);
        AtemSwitcher.changeColorValue(2, 0, 0, 70);
        break;
        
      case 80:
        AtemSwitcher.settingsMemorySave();
        Serial.println("SAVED SETTINGS");
        break;
      case 81:
        AtemSwitcher.settingsMemoryClear();
        Serial.println("CLEARED SETTINGS");
        break;
  
      default: 
        c=0; 
        break;
      }
    }
    buttonState = true;
  } 
  else {
    buttonState = false;
  }
}
