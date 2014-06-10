/*****************
 * Example: Smart View cyclic settings demo
 * Cycles settings on a SmartScope.
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



/* MEMORY USAGE:
 * This can be used to track free memory. 
 * Include "MemoryFree.h" and use the following line
 *     Serial << F("freeMemory()=") << freeMemory() << "\n";  
 * in your code to see available memory.
 */
 #include <MemoryFree.h>

#include <SkaarhojBufferTools.h>
#include <SkaarhojASCIIClient.h>
#include <SkaarhojTCPClient.h>
#include <ClientBMDSmartView.h>
ClientBMDSmartView SmartView; 


void setup() {

  // start the Ethernet connection:
  Ethernet.begin(mac, ip);

  // start the serial library:
  Serial.begin(115200);
  Serial << "\n-------------\n";

  // Setting short timeout on IP connections:
  // May result in "connection failed" messages, in particular on the first connection attempt
  W5100.setRetransmissionTime(200);  // Milli seconds
  W5100.setRetransmissionCount(2);
  
  
  // give the Ethernet shield a second to initialize:
  delay(1000);

  SmartView.begin(IPAddress(192, 168, 10, 220));	 // <= SETUP (the IP address of the SmartView)
  SmartView.serialOutput(1);
  SmartView.connect();
}


void loop() {

  SmartView.runLoop(1000);
  
    // Begin demo mode:
  if (SmartView.isConnected())  {
    
      // Both screens set to "Picture" mode:
    SmartView.setScopeMode(0,0);
    SmartView.waitForReady(100);
    
    SmartView.setScopeMode(1,0);
    SmartView.waitForReady(100);
    
      // For each monitor, walk through settings:
    for(uint8_t i=0; i<=1; i++)  {
    
      SmartView.setIdentify(i,true);
      SmartView.runLoop(2000);
      
      for(uint8_t j=0; j<=8; j++)  {
        Serial << "SENDING: SmartView.setScopeMode(" << i << "," << ((j+1)%9) << ");\n";
        SmartView.setScopeMode(i,(j+1)%9);
        SmartView.runLoop(2000);
        if (j==0)  {  // Audio
           for(uint8_t k=0; k<=7; k++)  {
            Serial << "SENDING: SmartView.setAudioChannel(" << i << "," << ((k+1)%8) << ");\n";
            SmartView.setAudioChannel(i,(k+1)%8);
            SmartView.runLoop(2000);
           }
        }
      }
      
      for(uint16_t j=0; j<=255; j=j+2)  {
        Serial << "SENDING: SmartView.setBrightness(" << i << "," << j << ");\n";
        SmartView.setBrightness(i,j);
        SmartView.waitForReady(100);
      }
      SmartView.setBrightness(i,255);
      SmartView.waitForReady(100);
      
      for(uint16_t j=0; j<=255; j=j+2)  {
        Serial << "SENDING: SmartView.setContrast(" << i << "," << j << ");\n";
        SmartView.setContrast(i,j);
        SmartView.waitForReady(100);
      }
      SmartView.setContrast(i,127);
      SmartView.waitForReady(100);
      
      for(uint16_t j=0; j<=255; j=j+2)  {
        Serial << "SENDING: SmartView.setSaturation(" << i << "," << j << ");\n";
        SmartView.setSaturation(i,j);
        SmartView.waitForReady(100);
      }
      SmartView.setSaturation(i,127);
      SmartView.waitForReady(100);


      SmartView.setIdentify(i,false);
      SmartView.runLoop(2000);

      for(uint8_t j=0; j<=3; j++)  {
        Serial << "SENDING: SmartView.setBorder(" << i << "," << ((j+1)%4) << ");\n";
        SmartView.setBorder(i,(j+1)%4);
        SmartView.runLoop(2000);
      }
    }
  }
  
 Serial << F("freeMemory()=") << freeMemory() << "\n";
}

