/*****************
 * Example: Smart View multiple simultaneous connections
 * Will connect to 3 SmartView monitors at the same time, blinking the border color
 * This is of course much faster than the other example sketch doing slightly the same. 
 * This sketch proves that we can keep a connection to three monitors at a time (and probably 4 in total).
 * The othe sketch aims to prove at which speed we can expect to temporarily connect to a monitor in order to do settings, then disconnect again.
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an BlackMagic Design SmartView or SmartScope connected to the same network as the Arduino - and you should have it working with the desktop software
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
// IP address is an available address you choose on your subnet where the SmartView unit is also present:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xE8, 0xE9 };		// <= SETUP
IPAddress ip(192, 168, 10, 99);				// <= SETUP (Arduino IP)


// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}

#include <SkaarhojBufferTools.h>
#include <SkaarhojASCIIClient.h>
#include <SkaarhojTCPClient.h>
#include <ClientBMDSmartView.h>

// Connect to a number of SmartView monitors
uint8_t numberOfSmartViews = 3;
ClientBMDSmartView SmartViews[] = {ClientBMDSmartView(), ClientBMDSmartView(), ClientBMDSmartView()};

void setup() {

  // start the Ethernet connection:
  Ethernet.begin(mac, ip);

  // start the serial library:
  Serial.begin(115200);
  Serial << "\n-------------\n";

  // Setting short timeout on IP connections:
  // May result in "connection failed" messages, in particular on the first connection attempt
  W5100.setRetransmissionTime(1000);  // Milli seconds
  W5100.setRetransmissionCount(1);
  

  for(uint8_t a=0; a<numberOfSmartViews; a++)  {
    uint8_t lastDigit = 220+a;
    SmartViews[a].begin(IPAddress(192, 168, 10, lastDigit));	 // <= SETUP (the IP address of the SmartView)
    SmartViews[a].serialOutput(1);
    SmartViews[a].connect();
  }
  
  // give the Ethernet shield a second to initialize:
  delay(1000);

}


void loop() {
    
  static uint8_t mon = 0;
  static uint8_t border = 0;

  for(uint8_t a=0; a<numberOfSmartViews; a++)  {
    SmartViews[a].runLoop();

    if (SmartViews[a].isConnected() && SmartViews[a].isReady())  {
      SmartViews[a].setBorder(mon%2,(border%3)+1);
    }
  }
  
  border++;
  mon++;
}

