/*****************
 * Basic ATEM Connection
 * Connects to the ATEM Switcher and outputs keep-alive package information
 * This is for debugging / test
 * By running this script, and if successful, you should see something like this on the serial monitor:
 * (This is for an ATEM 2 M/E 4K switcher):
 
 
Serial Started
Sending connect packet to ATEM switcher on IP 192.168.10.240 from port 56540
freeMemory()=7107
rpID: 0, Head: 0x2, Len: 20 bytes
rpID: 0, Head: 0x6, Len: 20 bytes
rpID: 1, Head: 0x1, Len: 1392 bytes
rpID: 5, Head: 0x1, Len: 268 bytes
rpID: 10, Head: 0x1, Len: 220 bytes
_initPayloadSent=TRUE @rpID 21
Session ID: 33151
rpID: 21, Head: 0x1, Len: 12 bytes - ACK!
rpID: 1, Head: 0x5, Len: 1392 bytes
rpID: 5, Head: 0x5, Len: 268 bytes
rpID: 10, Head: 0x5, Len: 220 bytes
rpID: 21, Head: 0x5, Len: 12 bytes
Asking for package 2
rpID: 2, Head: 0x5, Len: 1416 bytes
Asking for package 3
rpID: 3, Head: 0x5, Len: 1420 bytes
Asking for package 4
rpID: 4, Head: 0x5, Len: 1420 bytes
Asking for package 6
rpID: 6, Head: 0x5, Len: 1420 bytes
Asking for package 7
rpID: 7, Head: 0x5, Len: 1420 bytes
Asking for package 8
rpID: 8, Head: 0x5, Len: 1420 bytes
Asking for package 9
rpID: 9, Head: 0x5, Len: 1420 bytes
Asking for package 11
rpID: 11, Head: 0x5, Len: 1420 bytes
Asking for package 12
rpID: 12, Head: 0x5, Len: 1420 bytes
Asking for package 13
rpID: 13, Head: 0x5, Len: 1420 bytes
Asking for package 14
rpID: 14, Head: 0x5, Len: 1420 bytes
Asking for package 15
rpID: 15, Head: 0x5, Len: 1420 bytes
Asking for package 16
rpID: 16, Head: 0x5, Len: 1420 bytes
Asking for package 17
rpID: 17, Head: 0x5, Len: 1420 bytes
Asking for package 18
rpID: 18, Head: 0x5, Len: 1420 bytes
Asking for package 19
rpID: 19, Head: 0x5, Len: 1420 bytes
Asking for package 20
rpID: 20, Head: 0x5, Len: 1368 bytes
ATEM _hasInitialized = TRUE
rpID: 22, Head: 0x1, Len: 12 bytes - ACK!
rpID: 23, Head: 0x1, Len: 12 bytes - ACK!
rpID: 24, Head: 0x1, Len: 12 bytes - ACK!
rpID: 25, Head: 0x1, Len: 12 bytes - ACK!
rpID: 26, Head: 0x1, Len: 12 bytes - ACK!
rpID: 27, Head: 0x1, Len: 12 bytes - ACK!
rpID: 28, Head: 0x1, Len: 12 bytes - ACK!
rpID: 29, Head: 0x1, Len: 12 bytes - ACK!
... (etc)
 
 
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an Atem Switcher connected to the same network as the Arduino - and you should have it working with the desktop software
 * - You must make specific set ups in the below lines where the comment "// SETUP" is found!
 */



// Including libraries: 
#include <SPI.h>
#include <Ethernet.h>
#include <Streaming.h>
#include <MemoryFree.h>
#include <SkaarhojPgmspace.h>

// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9 };      // <= SETUP!  MAC address of the Arduino
IPAddress clientIp(192, 168, 10, 99);        // <= SETUP!  IP address of the Arduino
IPAddress switcherIp(192, 168, 10, 240);     // <= SETUP!  IP address of the ATEM Switcher

// Include ATEMbase library and make an instance:
// The port number is chosen randomly among high numbers.
#include <ATEMbase.h>
ATEMbase AtemSwitcher;




void setup() { 

  randomSeed(analogRead(5));  // For random port selection
  
  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,clientIp);
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  

  // Initialize a connection to the switcher:
  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(0x80);
  AtemSwitcher.connect();

  // Shows free memory:  
  Serial << F("freeMemory()=") << freeMemory() << "\n";
}

void loop() {
  // Check for packets, respond to them etc. Keeping the connection alive!
  // VERY important that this function is called all the time - otherwise connection might be lost because packets from the switcher is
  // overlooked and not responded to.
    AtemSwitcher.runLoop();
}
