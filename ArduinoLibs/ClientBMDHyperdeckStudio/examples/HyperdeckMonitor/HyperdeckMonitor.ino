/*****************
 * Example: Hyperdeck Monitor
 * This will connect to a HyperDeck Studio and show verbose connection related information.
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have a BlackMagic Design Hyperdeck Studio (Pro) connected to the same network as the Arduino
 * - You must make specific setups in the below lines where the comment "// SETUP" is found!
 */


/*
	IMPORTANT: If you want to use this library in your own projects and/or products,
	please play a fair game and heed the license rules! See our web page for a Q&A so
	you can keep a clear conscience: http://skaarhoj.com/about/licenses/
*/


#include <SPI.h>
#include <Ethernet.h>
#include <utility/w5100.h>


// MAC address and IP address for this *particular* Ethernet Shield!
// MAC address is printed on the shield
// IP address is an available address you choose on your subnet where the Hyperdeck Studio is also present:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xE8, 0xE9 };		// <= SETUP
IPAddress ip(192, 168, 177, 99);				// <= SETUP (Arduino IP)


// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}

#include <SkaarhojPgmspace.h>
#include <SkaarhojBufferTools.h>
#include <SkaarhojASCIIClient.h>
#include <SkaarhojTCPClient.h>
#include <ClientBMDHyperdeckStudio.h>

/* MEMORY USAGE:
 * This can be used to track free memory. 
 * Include "MemoryFree.h" and use the following line
 *     Serial << F("freeMemory()=") << freeMemory() << "\n";  
 * in your code to see available memory.
 */
#include <MemoryFree.h>

// Connect to a Hyperdeck Studio recorder with this object
ClientBMDHyperdeckStudio hyperDeck; 


void setup() {

  // start the Ethernet connection:
  Ethernet.begin(mac, ip);

  // start the serial library:
  Serial.begin(115200);
  Serial << "\n-------------\n";

  // Setting short timeout on IP connections:
  W5100.setRetransmissionTime(2000);  // Milli seconds
  W5100.setRetransmissionCount(2);

  // give the Ethernet shield a second to initialize:
  delay(1000);

  // Start Hyperdeck connection:
  hyperDeck.begin(IPAddress(192, 168, 177, 239));	 // <= SETUP (the IP address of the Hyperdeck Studio)
  hyperDeck.serialOutput(3);  // 1= normal, 2= medium verbose, 3=Super verbose
  hyperDeck.connect();  // For some reason the first connection attempt seems to fail, but in the runloop it will try to reconnect.
}

void loop() {

  hyperDeck.runLoop(1000);

  Serial << F("freeMemory()=") << freeMemory() << "\n";
}

