/*****************
 * Example: ATEM Monitor Dual
 * Connects to two Atem Switcher and outputs changes to Preview and Program on the Serial monitor (at 9600 baud)
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
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <utility/w5100.h>

//#include <MemoryFree.h>


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
ATEM AtemSwitcher1(IPAddress(192, 168, 10, 241), 56417);  // <= SETUP (the IP address of the ATEM switcher)
ATEM AtemSwitcher2(IPAddress(192, 168, 10, 240), 56416);  // <= SETUP (the IP address of the ATEM switcher)



// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}



void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(9600);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  

  // It's absolutely necessary to define a low retransmission time when working with atem connections. 
  // Otherwise the arduino may hang up in various network related issues which can break the connection to working atem switchers.
  // In this sketch for example, if one switcher is not present and this short retransmission time was not set, the connection to the other two would quickly be lost.
  W5100.setRetransmissionTime(200);  // Milli seconds
  W5100.setRetransmissionCount(1);
  
  
  // Initialize a connection to the switcher:
  AtemSwitcher1.serialOutput(2);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher1.connect();

  AtemSwitcher2.serialOutput(2);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher2.connect();

  // Shows free memory:  
//  Serial << F("freeMemory()=") << freeMemory() << "\n";
}

void loop() {
  // Check for packets, respond to them etc. Keeping the connection alive!
  // VERY important that this function is called all the time - otherwise connection might be lost because packets from the switcher is
  // overlooked and not responded to.
    AtemSwitcher1.runLoop();
    AtemSwitcher2.runLoop();

  // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher1.isConnectionTimedOut())  {
    Serial << F("Connection to FIRST ATEM Switcher has timed out - reconnecting!\n");
    AtemSwitcher1.connect();
  }  

  // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher2.isConnectionTimedOut())  {
    Serial << F("Connection to SECOND ATEM Switcher has timed out - reconnecting!\n");
    AtemSwitcher2.connect();
  }  
}
