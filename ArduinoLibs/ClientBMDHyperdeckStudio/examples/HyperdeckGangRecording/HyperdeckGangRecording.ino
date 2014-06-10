/*****************
 * Example: Hyperdeck Controller
 * Demonstrates how to gang-record with 4 hyperdecks
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
IPAddress ip(10, 66, 197, 99);				// <= SETUP (Arduino IP)


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
#include <ClientBMDHyperdeckStudio.h>

// Connect to a Hyperdeck Studio recorder with this object
ClientBMDHyperdeckStudio hyperDeck[] = {
  ClientBMDHyperdeckStudio(), ClientBMDHyperdeckStudio(), ClientBMDHyperdeckStudio(), ClientBMDHyperdeckStudio()}; 


void setup() {

  // start the Ethernet connection:
  Ethernet.begin(mac, ip);

  // start the serial library:
  Serial.begin(115200);
  Serial << "\n-------------\n";

  // Setting short timeout on IP connections:
//  W5100.setRetransmissionTime(2000);  // Milli seconds
//  W5100.setRetransmissionCount(1);

  // give the Ethernet shield a second to initialize:
  delay(1000);

  // Start Hyperdeck connection:
  for(uint8_t a=0; a<4; a++)  {
    uint8_t theAdr = 63+a;
    hyperDeck[a].begin(IPAddress(10, 66, 197, theAdr));	 // <= SETUP (the IP address of the Hyperdeck Studio)
    hyperDeck[a].serialOutput(1);  // 1= normal, 2= medium verbose, 3=Super verbose
    hyperDeck[a].connect();  // For some reason the first connection attempt seems to fail, but in the runloop it will try to reconnect.
    while(!hyperDeck[a].hasInitialized())  {
      hyperDeck[a].runLoop();
    }
  }

  Serial << "Ready to control HyperDecks.\n";

  Serial << "k : stop\n";
  Serial << "r : record\n";
  Serial << "q : disconnect and quit\n";
}

void loop() {
  for(uint8_t a=0; a<4; a++)  {
    hyperDeck[a].runLoop();
  }

  // Tracking state changes:
  static uint8_t HDtransportStatus[4];
  for(uint8_t a=0; a<4; a++)  {
    if (hyperDeck[a].getTransportStatus()!=HDtransportStatus[a])  {
      HDtransportStatus[a] = hyperDeck[a].getTransportStatus();
      Serial << "Transport status: ";
      switch(HDtransportStatus[a])  {
      case ClientBMDHyperdeckStudio_TRANSPORT_PREVIEW:
        Serial << "Preview\n";
        break; 
      case ClientBMDHyperdeckStudio_TRANSPORT_STOPPED:
        Serial << "Stopped\n";
        break; 
      case ClientBMDHyperdeckStudio_TRANSPORT_PLAY:
        Serial << "Playing\n";
        break; 
      case ClientBMDHyperdeckStudio_TRANSPORT_RECORD:
        Serial << "Recording\n";
        break; 
      case ClientBMDHyperdeckStudio_TRANSPORT_JOG:
        Serial << "Jog\n";
        break; 
      case ClientBMDHyperdeckStudio_TRANSPORT_FORWARD:
        Serial << "Forward\n";
        break; 
      case ClientBMDHyperdeckStudio_TRANSPORT_REWIND:
        Serial << "Rewind\n";
        break; 
      }
    }
  }
  // Detect commands:
  if (hyperDeck[0].hasInitialized() && hyperDeck[1].hasInitialized() && hyperDeck[2].hasInitialized() && hyperDeck[3].hasInitialized())  {
    if (Serial.available())  {
      Serial << "Keypress\n";
      switch(Serial.read())  {
      case 'k':
        for(uint8_t a=0; a<4; a++)  {
          hyperDeck[a].stop();
        }
        break;      
      case 'r':
        for(uint8_t a=0; a<4; a++)  {
          hyperDeck[a].recordWithName("GangR");
        }
        break;      
      case 'q':  // Disconnect and quit:
        for(uint8_t a=0; a<4; a++)  {
          hyperDeck[a].disconnect();
        }
        for(uint8_t a=0; a<4; a++)  {
          hyperDeck[a].runLoop(500);
        }
        while(true);
        break;      
      }
    }
  }
}




