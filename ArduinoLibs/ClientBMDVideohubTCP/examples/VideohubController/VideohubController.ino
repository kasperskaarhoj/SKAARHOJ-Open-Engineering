/*****************
 * Example: Video Hub Controller
 * This will connect to and allow keyboard based control (through the serial monitor) of a Videohub
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
#include <SkaarhojPgmspace.h>

#include <math.h> 

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


#include <SkaarhojBufferTools.h>
#include <SkaarhojASCIIClient.h>
#include <SkaarhojTCPClient.h>
#include <ClientBMDVideohubTCP.h>
ClientBMDVideohubTCP Videohub; 











/****
 * Code for a simple serial monitor command reader:
 ****/
#define SER_BUFFER_SIZE 20
char serialBuffer[SER_BUFFER_SIZE];
uint8_t serialBufferPointer = 255;

bool loadSerialCommand()  {  // Call in loop() to check for commands
  if (serialBufferPointer==255)  {  // Initialization
    serialBuffer[0]=0;  // Null-terminate empty buffer
    serialBufferPointer = 0;
  }
  if (Serial.available())  {  // A line of characters has been sent from the serial monitor - no <lf> termination! Just waiting...
    serialBufferPointer=0;  // so, we can start over again filling the buffer
    delay(10);  // Wait for all characters to arrive.
    while(Serial.available())  {
      char c = Serial.read();
      if (serialBufferPointer < SER_BUFFER_SIZE-1)	{	// one byte for null termination reserved
        serialBuffer[serialBufferPointer] = c;
        serialBuffer[serialBufferPointer+1] = 0;
        serialBufferPointer++;
      } 
      else {
        Serial.println(F("ERROR: Buffer overflow."));
      }    
    }
    //Serial << F("SERIAL COMMAND: ") << serialBuffer << F("\n");
    return true;
  }
  return false;
}











void setup() {
  delay(1000);

  // start the Ethernet connection:
  Ethernet.begin(mac, ip);

  // start the serial library:
  Serial.begin(115200);
  Serial << "\n-------------\n";

  // Setting short timeout on IP connections:
  // May result in "connection failed" messages, in particular on the first connection attempt
  W5100.setRetransmissionTime(3000);  // Milli seconds. NOTICE: Experiments show that this value cannot be set much lower, otherwise a connection will be impossible.
  W5100.setRetransmissionCount(2);

  // give the Ethernet shield a second to initialize:
  delay(1000);

  Serial.println("connecting...");
  Videohub.begin(IPAddress(192, 168, 10, 210));  // <= SETUP (the IP address of the Videohub)
  Videohub.serialOutput(1);
  Videohub.connect();

  while(!Videohub.hasInitialized())  {
    Videohub.runLoop();
  }

  Serial << F("Ready to control Videohub. Type any of these commands + Enter in the serial monitor:\n");

  Serial << F("r <input> <output>: Routes <input> to <output>\n");
  Serial << F("g <output>: Get input assigned to <output>\n");
  Serial << F("list: Complete list <output>\n");
  Serial << F("l <output>: Lock/Unlock <output>\n");
  Serial << F("q : quit\n");


}

void loop() {
  Videohub.runLoop();



  // Detect commands:
  if (Videohub.hasInitialized())  {
    if (loadSerialCommand())  {
      Serial.println(serialBuffer);
      // "list"
      if (!strcmp_P(serialBuffer,PSTR("list")))  {
        Serial << F("Listing:\n");

        // Input labels:
        Serial << F("Inputs:\n");
        for(uint8_t i=0; i<ClientBMDVideohubTCP_NUMINPUTS; i++)  {
          Serial << (i+1) << F(": \"") << Videohub.getInputLabel(i+1) << F("\"\n");
        }
        Serial << F("\n");

        // Output labels:
        Serial << F("Outputs:\n");
        for(uint8_t i=0; i<ClientBMDVideohubTCP_NUMOUTPUTS; i++)  {
          Serial << (i+1) << F(": \"") << Videohub.getOutputLabel(i+1) 
            << F("\" = Input ") << Videohub.getRoute(i+1) 
              << F(" (Locking: ") << Videohub.getLock(i+1) << F(")\n");
        }
        Serial << F("\n");



      } 
      else {  // Single letter:
        uint16_t output;
        uint16_t input;
        uint8_t inputLen;

        switch((char)serialBuffer[0])  {
        case 'r':
          input = atoi(serialBuffer+2);
          inputLen = log10(input)+1;
          output = atoi(serialBuffer+2+inputLen+1);

          // Routing:
          Serial << F("Routing input ") << input << F(" to output ") << output << F("\n");
          Videohub.routeInputToOutput(input, output);
          break; 
        case 'g':
          output = atoi(serialBuffer+2);
          Serial << F("Output ") << output << F(" has input ") << Videohub.getRoute(output) << F(" routed to it\n");
          break; 
        case 'l':
          output = atoi(serialBuffer+2);

          if (Videohub.getLock(output)=='L')  {
            Serial << F("Output ") << output << F(" was locked by another client.\n");
          } 
          else if (Videohub.getLock(output)=='O')  {
            Serial << F("Output ") << output << F(" was locked by you. Unlocking...\n");
            Videohub.setLock(output,'U');
          } 
          else {
            Serial << F("Output ") << output << F(" was unlocked. Locking...\n");
            Videohub.setLock(output,'O');
          }
          break; 
        case 'q':
          Serial << F("Disconnecting...\n");
          Videohub.disconnect();
          break; 
        }
      }
    }
  }  
}






