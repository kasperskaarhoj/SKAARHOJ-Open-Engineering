/*****************
 * Example: Video Hub Controller via TCP
 * This will connect to and allow keyboard based control (through the serial monitor) of a Videohub
 * Notice: This is a "free" implementation of part of the "VideohubController" found  as a part of the ClientBMDVideohubTCP class! It basically demonstrates how to create the same in the main sketch if you don't want to make a full class to encapsulate it.
 *
 * The point is how SkaarhojSerialServer and SkaarhojTCPClient works together
 *   SkaarhojSerialServer is taking and parsing input from the serial monitor - the stuff you type. Then it acts upon that (forwards commands to Videohub)
 *   SkaarhojTCPClient is connected to the Videohub and handles communication and heartbeat signals forth and back there.
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

// Including libraries: 
#include <SPI.h>
#include <Ethernet.h>
#include <utility/w5100.h>


// MAC address and IP address for this *particular* Ethernet Shield!
// MAC address is printed on the shield
// IP address is an available address you choose on your subnet where the videohub is also present:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192, 168, 10, 99);              // <= SETUP!  IP address of the Arduino


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
SkaarhojTCPClient telnetClient;



#include <SkaarhojSerialServer.h>
SkaarhojSerialServer SerialServer(Serial);



/**
 * These functions are callback functions from the telnet class, talking to the videohub
 */
uint8_t videohubRoutes[16];
bool isInSection = false;
void handleTelnetIncoming()  {
  telnetClient.setInitialized();  // Make sure a response triggers that the connection is "initialized"
  
  if (!strcmp(telnetClient.getRemainingBuffer(),""))	{
    isInSection = false;
  } else if (telnetClient.isBufferEqualTo_P(PSTR("VIDEO OUTPUT ROUTING:")))  {
    isInSection = true;
  } else if (isInSection)  {
    uint8_t output = telnetClient.parseInt();
    telnetClient.isNextPartOfBuffer_P(PSTR(" "));
    uint8_t input = telnetClient.parseInt();
    if (output < 16)  {
      videohubRoutes[output] = input;
    }
  }
}
void handleTelnetPing()  {
  telnetClient._client << F("PING:\n\n");
}
void handleTelnetStatus()  {
  telnetClient._client << F("VIDEO OUTPUT ROUTING:\n\n");
}


/**
 * These function is a callback from the serial SERVER class (taking in our commands from serial monitor)
 */
void handleSerialIncoming()  {
      // "list"
      if (SerialServer.isBufferEqualTo_P(PSTR("list")))  {
        Serial << F("Listing:\n");

        // Output labels:
        Serial << F("Outputs:\n");
        for(uint8_t i=0; i<16; i++)  {
          Serial << (i+1)
            << F(" = Input ") << (videohubRoutes[i]+1) 
               << F("\n");
        }
        Serial << F("\n");
      } 
      else {  // Single letter:
        uint16_t output;
        uint16_t input;
        uint8_t inputLen;
        
        char* serialBuffer = SerialServer.getRemainingBuffer();

        switch((char)serialBuffer[0])  {
        case 'r':
          input = atoi(serialBuffer+2);
          inputLen = log10(input)+1;
          output = atoi(serialBuffer+2+inputLen+1);

          // Routing:
          Serial << F("Routing input ") << input << F(" to output ") << output << F("\n");
          telnetClient._client << F("VIDEO OUTPUT ROUTING:\n") << (output-1) << F(" ") << (input-1) << F("\n\n");
          break; 
        case 'g':
          output = atoi(serialBuffer+2);
          Serial << F("Output ") << output << F(" has input ") << (videohubRoutes[output-1]+1) << F(" routed to it\n");
          break;  
        case 'q':
          Serial << F("Disconnecting...\n");
          telnetClient.disconnect();
          break; 
        }
      }
 }



void setup() { 

  delay(2000);
   
  // start the Ethernet connection:
  Ethernet.begin(mac,ip);

  // start the serial SERVER library (taking in commands from serial monitor):
  SerialServer.begin(115200);
  SerialServer.setHandleIncomingLine(handleSerialIncoming);  // Put only the name of the function
  SerialServer.serialOutput(3);
  SerialServer.enableEOLTimeout();  // Calling this without parameters sets EOL Timeout to 2ms which is enough for 9600 baud and up. Use only if the serial client (like Arduinos serial monitor) doesn't by itself send a <cr> token.
  Serial << F("Type stuff in the serial monitor to send over telnet...\n");  

  
  // Setting short timeout on IP connections:
  // May result in "connection failed" messages, in particular on the first connection attempt
  W5100.setRetransmissionTime(3000);  // Milli seconds. NOTICE: Experiments show that this value cannot be set much lower, otherwise a connection will be impossible.
  W5100.setRetransmissionCount(2);
  
  delay(1000);

    // start the telnet Client library (talking to videohub):
  Serial.println("connecting...");
  telnetClient.begin(IPAddress(192, 168, 10, 210), 9990);
  telnetClient.serialOutput(3);
  telnetClient.setHandleIncomingLine(handleTelnetIncoming);
  telnetClient.setHandlePing(handleTelnetPing,5000);
  telnetClient.setHandleStatus(handleTelnetStatus,60000);
  telnetClient.connect();
  
  
    // Instructions:
  Serial << F("Ready to control Videohub. Type any of these commands + Enter in the serial monitor:\n");

  Serial << F("r <input> <output>: Routes <input> to <output>\n");
  Serial << F("g <output>: Get input assigned to <output>\n");
  Serial << F("list: Complete list\n");
  Serial << F("q : quit (and reconnect automatically)\n");
}

void loop() {
  SerialServer.runLoop();
  telnetClient.runLoop();
}

