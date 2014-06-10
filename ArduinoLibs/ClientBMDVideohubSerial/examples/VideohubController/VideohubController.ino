/*****************
 * Example: Video Hub Controller, using RS422 serial communication
 * This will connect to and allow keyboard based control (through the serial monitor) of a Videohub
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have a TTL Serial to RS-422 adaptor hooked up to your Arduino's Serial1 RX/TX
 * - You must have an BlackMagic Design Videohub (Micro for instance) connected to this RS422 connection
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
#include <ClientBMDVideohubSerial.h>
ClientBMDVideohubSerial Videohub(Serial1); 







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

  // start the serial library:
  Serial.begin(115200);
  Serial << "\n-------------\n";

  Videohub.begin();
  Videohub.serialOutput(1);
  Videohub.connect();

  while(!Videohub.hasInitialized())  {
    Videohub.runLoop();
  }

  Serial << F("Ready to control Videohub. Type any of these commands + Enter in the serial monitor:\n");

  Serial << F("r <input> <output>: Routes <input> to <output>\n");
  Serial << F("g <output>: Get input assigned to <output> (NOTICE: May be inaccurate since it depends on periodic status updates.\n");
  Serial << F("list: Complete list <output>\n");

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

        // Output labels:
        Serial << F("Outputs:\n");
        for(uint8_t i=0; i<ClientBMDVideohubSerial_NUMOUTPUTS; i++)  {
          Serial << (i+1)
                << F(" = Input ") << Videohub.getRoute(i+1) 
                << F("\n");
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
        }
      }
    }
  }  
}






