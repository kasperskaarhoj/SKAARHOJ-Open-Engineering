/*****************
 * Example: Video Hub Controller
 * This will connect to and allow keyboard based control (through the serial monitor) of a Videohub
 * Notice: This is a "free" implementation of part of the "VideohubController" found  as a part of the ClientBMDVideohubSerial class! It basically demonstrates how to create the same in the main sketch if you don't want to make a full class to encapsulate it.
 *
 * The point is how SkaarhojSerialServer and SkaarhojSerialClient works together
 *   SkaarhojSerialServer is taking and parsing input from the serial monitor - the stuff you type. Then it acts upon that (forwards commands to Videohub)
 *   SkaarhojSerialClient is connected to the Videohub and handles communication and heartbeat signals forth and back there.
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
#include <SkaarhojSerialClient.h>
SkaarhojSerialClient SerialClient(Serial1);



#include <SkaarhojSerialServer.h>
SkaarhojSerialServer SerialServer(Serial);



/**
 * These functions are callback functions from the serial CLIENT class (which communicates with the videohub)
 */
uint8_t videohubRoutes[16];
bool isInSection = false;
void handleClientIncoming()  {

  if (SerialClient.isNextPartOfBuffer_P(PSTR("@ S?0")))  {
    SerialClient.setInitialized();  // Make sure a response triggers that the connection is "initialized"
  } 
  else if (SerialClient.isNextPartOfBuffer_P(PSTR("S:")))  {
    uint8_t output = SerialClient.parseHex();
    SerialClient.isNextPartOfBuffer_P(PSTR(","));
    uint8_t input = SerialClient.parseHex();
    if (output < 16)  {
      videohubRoutes[output] = input;
    }
  }
}
void handleClientPing()  {
  SerialClient._HardSerial << F("\r");
}
void handleClientStatus()  {
  SerialClient._HardSerial << F("@ S?0\r");
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
      SerialClient._HardSerial << F("@ X:0/") << String(output-1, HEX) << F(",") << String(input-1, HEX) << F("\r");
      break; 
    case 'g':
      output = atoi(serialBuffer+2);
      Serial << F("Output ") << output << F(" has input ") << (videohubRoutes[output-1]+1) << F(" routed to it\n");
      break;  
    }
  }
}







void setup() { 

  delay(1000);


  // start the serial SERVER library (taking in commands from serial monitor):
  SerialServer.begin(115200);
  SerialServer.setHandleIncomingLine(handleSerialIncoming);  // Put only the name of the function
  SerialServer.serialOutput(3);
  SerialServer.enableEOLTimeout();  // Calling this without parameters sets EOL Timeout to 2ms which is enough for 9600 baud and up. Use only if the serial client (like Arduinos serial monitor) doesn't by itself send a <cr> token.
  Serial << F("Type stuff in the serial monitor to send over telnet...\n");  

  delay(1000);

  // start the serial Client library (talking to videohub):
  Serial.println("connecting...");
  SerialClient.begin(9600);
  SerialClient.serialOutput(3);
  SerialClient.setHandleIncomingLine(handleClientIncoming);
  SerialClient.setHandlePing(handleClientPing,5000);
  SerialClient.setHandleStatus(handleClientStatus,60000);
  SerialClient.setEOTChar('>');  // This character signals end-of-transmission from a videohub
  SerialClient.connect();

    // Instructions:
  Serial << F("Ready to control Videohub. Type any of these commands + Enter in the serial monitor:\n");

  Serial << F("r <input> <output>: Routes <input> to <output>\n");
  Serial << F("g <output>: Get input assigned to <output>\n");
  Serial << F("list: Complete list\n");
}

void loop() {
  SerialServer.runLoop();
  SerialClient.runLoop();
}


