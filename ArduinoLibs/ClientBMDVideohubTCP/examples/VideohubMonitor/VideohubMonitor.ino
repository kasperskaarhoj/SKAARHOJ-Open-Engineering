/*****************
 * Example: Video Hub Monitor over TCP
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an BlackMagic Design Videohub (Micro for instance) connected to the same network as the Arduino - and you should have it working with the desktop software
 * - You must make specific set ups in the below lines where the comment "// SETUP" is found!
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
// IP address is an available address you choose on your subnet where the videohub is also present:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xE8, 0xE9 };		// <= SETUP
IPAddress ip(192, 168, 10, 99);				// <= SETUP


// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}



/* MEMORY USAGE:
 * This can be used to track free memory. 
 * Include "MemoryFree.h" and use the following line
 *     Serial << F("freeMemory()=") << freeMemory() << "\n";  
 * in your code to see available memory.
 */
 #include <MemoryFree.h>
 // Without anything, on a Mega, the memory available will report to 7464, with ethernet libraries 7402 and with video hub library 6748. So... using 650 bytes of memory. If se set inputs/outputs/string length to 1, it would be 7096.
 
 
#include <SkaarhojBufferTools.h>
#include <SkaarhojASCIIClient.h>
#include <SkaarhojTCPClient.h>
#include <ClientBMDVideohubTCP.h>
ClientBMDVideohubTCP Videohub; 


void setup() {

  delay(2000);
  
  // start the Ethernet connection:
  Ethernet.begin(mac, ip);

  // start the serial library:
  Serial.begin(115200);

  // Setting short timeout on IP connections:
  // May result in "connection failed" messages, in particular on the first connection attempt
  W5100.setRetransmissionTime(3000);  // Milli seconds. NOTICE: Experiments show that this value cannot be set much lower, otherwise a connection will be impossible.
  W5100.setRetransmissionCount(2);

  // give the Ethernet shield a second to initialize:
  delay(1000);

  Serial.println("connecting...");
  Videohub.begin(IPAddress(192, 168, 10, 210));  // <= SETUP (the IP address of the Videohub)
  Videohub.connect();
  Videohub.serialOutput(3);
}

void loop() {
  Videohub.runLoop(1000);
  
  Serial << F("freeMemory()=") << freeMemory() << "\n";
}





