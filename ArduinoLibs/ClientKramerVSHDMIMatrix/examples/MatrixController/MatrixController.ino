/*****************
 * Example: Kramer HDMI matrix Monitor, using RS-232(S) serial communication
 * This will connect to and allow keyboard based control (through the serial monitor) of a Kramer VS-4*HN
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have a TTL Serial to RS-232 adaptor hooked up to your Arduino's Serial1 RX/TX
 * - You must have a Kramer HDMI Matrix, VS-4*HN connected to this RS-232 connection (Straight Cable)
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
#include <ClientKramerVSHDMIMatrix.h>
ClientKramerVSHDMIMatrix HDMImatrix(Serial1); 







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

  HDMImatrix.begin();
  HDMImatrix.serialOutput(1);
  HDMImatrix.connect();

  while(!HDMImatrix.hasInitialized())  {
    HDMImatrix.runLoop();
  }

  Serial << F("Ready to control HDMImatrix. Type any of these commands + Enter in the serial monitor:\n");

  Serial << F("r <input> <output>: Routes <input> to <output>\n");
  Serial << F("g <output>: Get input assigned to <output> (NOTICE: May be inaccurate since it depends on periodic status updates.\n");
  Serial << F("s <input>: Get signal status on <input> (NOTICE: May be inaccurate since it depends on periodic status updates.\n");
  Serial << F("list: Complete list\n");

}

void loop() {
  HDMImatrix.runLoop();



  // Detect commands:
  if (HDMImatrix.hasInitialized())  {
    if (loadSerialCommand())  {
      Serial.println(serialBuffer);
      // "list"
      if (!strcmp_P(serialBuffer,PSTR("list")))  {
        Serial << F("Listing:\n");

        // Output labels:
        Serial << F("Outputs:\n");
        for(uint8_t i=0; i<ClientKramerVSHDMIMatrix_NUMOUTPUTS; i++)  {
          Serial << (i+1)
                << F(" = Input ") << HDMImatrix.getRoute(i+1) 
                << F("\n");
        }
        Serial << F("\n");
        // Output labels:
        Serial << F("Signals on inputs:\n");
        for(uint8_t i=0; i<ClientKramerVSHDMIMatrix_NUMINPUTS; i++)  {
          Serial << F("Input ") << (i+1)
                << F(": ") << (HDMImatrix.getSignal(i+1)?"YES":"NO") 
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
          HDMImatrix.routeInputToOutput(input, output);
          break; 
        case 'g':
          output = atoi(serialBuffer+2);
          Serial << F("Output ") << output << F(" has input ") << HDMImatrix.getRoute(output) << F(" routed to it\n");
          break; 
        case 's':
          input = atoi(serialBuffer+2);
          Serial << F("Input ") << input << F(" has signal: ") << (HDMImatrix.getSignal(input)?F("YES"):F("NO")) << F("\n");
          break; 
        }
      }
    }
  }  
}






