/*****************
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an BlackMagic Design Teranex or SmartScope connected to the same network as the Arduino - and you should have it working with the desktop software
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
// IP address is an available address you choose on your subnet where the Teranex unit is also present:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xE8, 0xE9 };		// <= SETUP
IPAddress ip(192, 168, 177, 100);				// <= SETUP (Arduino IP)

#include <Streaming.h>
#include <SkaarhojBufferTools.h>
#include <SkaarhojASCIIClient.h>
#include <SkaarhojTCPClient.h>
#include <ClientBMDTeranex.h>
ClientBMDTeranex Teranex;


// Set to 3, it will cyclicly connect/disconnect to 192.168.10.220, then 192.168.10.221, then 192.168.10.222 and over again
uint8_t numberOfTeranexs = 3;

void setup() {

  // start the Ethernet connection:
  Ethernet.begin(mac, ip);

  // start the serial library:
  Serial.begin(115200);
  Serial << "\n-------------\n";

  // Setting short timeout on IP connections:
  // May result in "connection failed" messages, in particular on the first connection attempt
  W5100.setRetransmissionTime(1000);  // Milli seconds
  W5100.setRetransmissionCount(2);
  
  
  // give the Ethernet shield a second to initialize:
  delay(1000);

}


void loop() {
  
  static uint8_t mon = 0;
  static uint8_t border = 0;

  for(uint8_t a=0; a<numberOfTeranexs; a++)  {
    uint8_t lastDigit = 220+a;
    Teranex.begin(IPAddress(192, 168, 10, lastDigit));	 // <= SETUP (the IP address of the Teranex)
    Teranex.serialOutput(1);
    Teranex.connect();
    Teranex.waitForReady(10000);
    if (Teranex.isConnected())  {
      Teranex.setBorder(mon%2,(border%3)+1);
      Teranex.waitForReady(10000);
      Teranex.disconnect();
    }
  }
  border++;
  mon++;
}

