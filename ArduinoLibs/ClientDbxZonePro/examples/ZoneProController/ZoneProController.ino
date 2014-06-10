/*****************
 * Example: dbx ZonePro Controller
 * This will connect to and allow keyboard based control (through the serial monitor) of a dbx ZonePro 1260
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an BlackMagic Design ZonePro (Micro for instance) connected to the same network as the Arduino - and you should have it working with the desktop software
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


#include <math.h> 

// MAC address and IP address for this *particular* Ethernet Shield!
// MAC address is printed on the shield
// IP address is an available address you choose on your subnet where the ZonePro is also present:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xE8, 0xE9 };		// <= SETUP
IPAddress ip(169,254,61,21);				// <= SETUP


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
#include <ClientDbxZonePro.h>
ClientDbxZonePro ZonePro; 











/****
 * Code for a simple serial monitor command reader:
 ****/
#define SER_BUFFER_SIZE 40
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
  ZonePro.begin(IPAddress(169,254,61,16));  // <= SETUP (the IP address of the ZonePro 1260m - 169,254,61,16 is the default!)
  ZonePro.serialOutput(2);

    // To receive states from the ZonePro you have to register WHICH state variables you want to be able to read out. It's made this way to save memory by storing only up to 10 such variables.
    // Registering should happen before connecting - otherwise the initial data payload will not enter data into these registers  
  ZonePro.registerSV(36,0,5,1,13);  // Output 1, mute
  ZonePro.registerSV(17,5,2,1,4);  // Mix 6 EQ, Band 1 level
  ZonePro.registerSV(1,1,1,1,0);  // Mic 2, gain
  
  ZonePro.connect();

  while(!ZonePro.hasInitialized())  {
    ZonePro.runLoop();
  }

  Serial << F("Ready to control ZonePro 1260m. Type any of these commands + Enter in the serial monitor:\n");
  Serial << F("(You must derive <index, b0> <row, b1> from using the ZonePro software on a PC: select an object, shift+ctrl+o and you will see it's b0 and b1 values.)\n");
  Serial << F("\n");
  Serial << F("inputgain <index, b0> <row, b1> <value 0-221>: Sets input gain\n");
  Serial << F("inputEQenable <index, b0> <row, b1> <0/1>: Enables/Disables EQ\n");
  Serial << F("inputEQflat <index, b0> <row, b1> <0/1>: Enables/Disables flat EQ\n");
  Serial << F("inputEQtype <index, b0> <row, b1> <1-4>: Bell curve, High shelf, Low shelf, High/Low shelf\n");
  Serial << F("inputEQFrequency <index, b0> <row, b1> <band 1-4> <freq 0-240>: EQ frequency of band\n");
  Serial << F("inputEQWidth <index, b0> <row, b1> <band 1-4> <width 0-39>: EQ width of band\n");
  Serial << F("inputEQLevel <index, b0> <row, b1> <band 1-4> <level 0-48>: EQ level of band\n");
  Serial << F("outputMute <index, b0> <row, b1> <0/1>: Enables/Disables output\n");
  Serial << F("outputMix <index, b0> <row, b1> <input> <level 0-221>: Level of input on given output\n");
  Serial << F("outputMaster <index, b0> <row, b1> <level 0-221>: Level of output master\n");
  Serial << F("recallScene <scene 0-49>: Recall settings of a scene\n");
  Serial << F("q : quit\n");
}

bool muted = false;

void loop() {
  ZonePro.runLoop();


  // Detect commands:
  if (ZonePro.hasInitialized())  {
    if (loadSerialCommand())  {
      Serial.println(serialBuffer);

      // Parsing input:      
      uint16_t b0;
      uint16_t b1;
      uint16_t value1;
      uint16_t value2;
      int inputOffset = strstr_P(serialBuffer,PSTR(" "))-serialBuffer;

      if (inputOffset>0)  {  // If a space-character is found:
        // Parse out integers:
        inputOffset++;
        b0 = atoi(serialBuffer+inputOffset);
        inputOffset+=(uint8_t)log10(b0)+2;
        b1 = atoi(serialBuffer+inputOffset);
        inputOffset+=(uint8_t)log10(b1)+2;
        value1 = atoi(serialBuffer+inputOffset);
        inputOffset+=(uint8_t)log10(value1)+2;
        value2 = atoi(serialBuffer+inputOffset);

        /* 
         Serial << F("DEBUG Values: ") << b0 << F(" ")
         << b1 << F(" ")
         << value1 << F(" ")
         << value2 << F("\n");
         */

        if (!strncmp_P(serialBuffer,PSTR("inputgain "),10))  {
          Serial << F("Input Gain of index ") << b0 
            << F(", row ") << b1 
            << F(" = ") << value1 
            << F("\n");
          ZonePro.setInputGain(b0, b1, value1);
        } 
        else if (!strncmp_P(serialBuffer,PSTR("inputEQenable "),14))  {
          Serial << F("Input EQ of index ") << b0 
            << F(", row ") << b1 
            << F(": ") << (value1?F("Enable"):F("Disable"))
            << F("\n");
          ZonePro.setInputEQEnable(b0, b1, value1);
        } 
        else if (!strncmp_P(serialBuffer,PSTR("inputEQflat "),12))  {
          Serial << F("Input EQ of index ") << b0 
            << F(", row ") << b1 
            << F(": ") << (value1?F("Flat"):F("Restore"))
            << F("\n");
          ZonePro.setInputEQFlat(b0, b1, value1);
        } 
        else if (!strncmp_P(serialBuffer,PSTR("inputEQtype "),12))  {
          Serial << F("Input EQ type of index ") << b0 
            << F(", row ") << b1 
            << F(": ") << value1
            << F("\n");
          ZonePro.setInputEQType(b0, b1, value1);
        } 
        else if (!strncmp_P(serialBuffer,PSTR("inputEQFrequency "),17))  {
          Serial << F("Input EQ frequency of index ") << b0 
            << F(", row ") << b1 
            << F(", band ") << value1
            << F(": ") << value2
            << F("\n");
          ZonePro.setInputEQFreq(b0, b1, value1, value2);
        } 
        else if (!strncmp_P(serialBuffer,PSTR("inputEQWidth "),13))  {
          Serial << F("Input EQ width of index ") << b0 
            << F(", row ") << b1 
            << F(", band ") << value1
            << F(": ") << value2
            << F("\n");
          ZonePro.setInputEQWidth(b0, b1, value1, value2);
        } 
        else if (!strncmp_P(serialBuffer,PSTR("inputEQLevel "),13))  {
          Serial << F("Input EQ level of index ") << b0 
            << F(", row ") << b1 
            << F(", band ") << value1
            << F(": ") << value2
            << F("\n");
          ZonePro.setInputEQLevel(b0, b1, value1, value2);
        } 
        else if (!strncmp_P(serialBuffer,PSTR("outputMute "),11))  {
          Serial << F("Output Mute of index ") << b0 
            << F(", row ") << b1 
            << F(": ") << (value1?F("Muted"):F("Unmuted"))
            << F("\n");
          ZonePro.setOutputMute(b0, b1, value1);
        } 
        else if (!strncmp_P(serialBuffer,PSTR("outputMix "),10))  {
          Serial << F("Output Mix of index ") << b0 
            << F(", row ") << b1 
            << F(", input ") << value1
            << F(": ") << value2
            << F("\n");
          ZonePro.setOutputMix(b0, b1, value1, value2);
        } 
        else if (!strncmp_P(serialBuffer,PSTR("outputMaster "),13))  {
          Serial << F("Output Master level of index ") << b0 
            << F(", row ") << b1 
            << F(": ") << value1
            << F("\n");
          ZonePro.setOutputMaster(b0, b1, value1);
        } 
        else if (!strncmp_P(serialBuffer,PSTR("recallScene "),12))  {
          Serial << F("Recall Scene:") << b0 
            << F("\n");
          ZonePro.recallScene(b0);
        } 
      } 
      else {
        switch((char)serialBuffer[0])  {
        case 'q':
          Serial << F("Disconnecting...\n");
          ZonePro.disconnect();
          break; 
        }
      }
    }
  }  
  
  
  
  // Output registered get states, if changed:
  static uint16_t tmp1, tmp2, tmp3 = 0;
  if (tmp1 != ZonePro.getSVValue(36,0,5,1,13))  {
    tmp1 = ZonePro.getSVValue(36,0,5,1,13);
    Serial << F("Output 1, mute: ") << tmp1 << F("\n");
  }
  if (tmp2 != ZonePro.getSVValue(17,5,2,1,4))  {
    tmp2 = ZonePro.getSVValue(17,5,2,1,4);
    Serial << F("Mix 6 EQ, Band 1 level: ") << tmp2 << F("\n");
  }
  if (tmp3 != ZonePro.getSVValue(1,1,1,1,0))  {
    tmp3 = ZonePro.getSVValue(1,1,1,1,0);
    Serial << F("Mic 2, gain: ") << tmp3 << F("\n");
  }
}










