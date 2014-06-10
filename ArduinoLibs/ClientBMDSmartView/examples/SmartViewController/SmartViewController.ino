/*****************
 * Example: Smart View Controller
 * Allows you to control a SmartView monitor with commands over the serial monitor
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
ClientBMDSmartView SmartView; 











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
  W5100.setRetransmissionTime(200);  // Milli seconds
  W5100.setRetransmissionCount(2);


  // give the Ethernet shield a second to initialize:
  delay(1000);

  Serial.println("connecting...");
  SmartView.begin(IPAddress(192, 168, 10, 220));	 // <= SETUP (the IP address of the SmartView)
  SmartView.serialOutput(1);
  SmartView.connect();

  while(!SmartView.hasInitialized())  {
    SmartView.runLoop();
  }


  Serial << F("Ready to control SmartView. Type any of these commands + Enter in the serial monitor:\n");

  Serial << F("[A/B] [brightness/contrast/saturation] <0-255>: Brightness, Contrast or Saturation settings\n");
  Serial << F("[A/B] [picture/audio/histogram/paradergb/paradeyuv/vector100/vector75/waveform]: Set scope type\n");
  Serial << F("[A/B] border [red/green/blue/none]: Set border color\n");
  Serial << F("list: Complete settings\n");
  Serial << F("q : quit\n");
  Serial << F("\n");
  Serial << F("Example to set contrast on left monitor:\n");
  Serial << F("   A contrast 50\n");
}


void loop() {

  SmartView.runLoop();

  // Begin demo mode:
  if (SmartView.hasInitialized())  {
    if (loadSerialCommand())  {
      //Serial.println(serialBuffer);

      // "list"
      if (!strcmp_P(serialBuffer,PSTR("list")))  {
        Serial << F("Listing:\n");

        for(uint8_t i=0; i<2; i++)  {
          Serial << (i==0?F("MONITOR A:\n"):F("MONITOR B:\n"));
          Serial << F("Scope = ");
          switch(SmartView.getScopeMode(i))  {
             case 0:
             Serial << F("picture\n");
            break; 
             case 1:
             Serial << F("audio\n");
            break; 
             case 3:
             Serial << F("histogram\n");
            break; 
             case 4:
             Serial << F("paradergb\n");
            break; 
             case 5:
             Serial << F("paradeyuv\n");
            break; 
             case 6:
             Serial << F("vector100\n");
            break; 
             case 7:
             Serial << F("vector75\n");
            break; 
             case 8:
             Serial << F("waveform\n");
            break; 
          }
          
          Serial << F("Brightness = ") << SmartView.getBrightness(i) << F("\n");
          Serial << F("Contrast = ") << SmartView.getContrast(i) << F("\n");
          Serial << F("Saturation = ") << SmartView.getSaturation(i) << F("\n");
          Serial << F("Border = ") << SmartView.getBorder(i) << F("\n");
          Serial << F("\n");
        }



      } 
      else {  // Single letter:
        uint8_t mon=0;

        switch((char)serialBuffer[0])  {
        case 'A':
          mon=1;
          break; 
        case 'B':
          mon=2;
          break; 
        case 'q':
          Serial << F("Disconnecting...\n");
          SmartView.disconnect();
          break; 
        }

        // A or B was chosen:        
        if (mon)  {
          if (!strncmp_P(serialBuffer+2, PSTR("brightness "),10))  {
            SmartView.setBrightness(mon-1,atoi(serialBuffer+2+10));
          } 
          if (!strncmp_P(serialBuffer+2, PSTR("contrast "),9))  {
            SmartView.setContrast(mon-1,atoi(serialBuffer+2+9));
          } 
          if (!strncmp_P(serialBuffer+2, PSTR("saturation "),11))  {
            SmartView.setSaturation(mon-1,atoi(serialBuffer+2+11));
          } 

          if (!strcmp_P(serialBuffer+2, PSTR("picture")))  {
            SmartView.setScopeMode(mon-1,0);
          } 
          if (!strcmp_P(serialBuffer+2, PSTR("audio")))  {
            SmartView.setScopeMode(mon-1,1);
          } 
          if (!strcmp_P(serialBuffer+2, PSTR("histogram")))  {
            SmartView.setScopeMode(mon-1,3);
          } 
          if (!strcmp_P(serialBuffer+2, PSTR("paradergb")))  {
            SmartView.setScopeMode(mon-1,4);
          } 
          if (!strcmp_P(serialBuffer+2, PSTR("paradeyuv")))  {
            SmartView.setScopeMode(mon-1,5);
          } 
          if (!strcmp_P(serialBuffer+2, PSTR("vector100")))  {
            SmartView.setScopeMode(mon-1,6);
          } 
          if (!strcmp_P(serialBuffer+2, PSTR("vector75")))  {
            SmartView.setScopeMode(mon-1,7);
          } 
          if (!strcmp_P(serialBuffer+2, PSTR("waveform")))  {
            SmartView.setScopeMode(mon-1,8);
          } 


          if (!strncmp_P(serialBuffer+2, PSTR("border "),7))  {
            if (!strcmp_P(serialBuffer+2+7, PSTR("none")))  {
              SmartView.setBorder(mon-1,0);
            } 
            if (!strcmp_P(serialBuffer+2+7, PSTR("red")))  {
              SmartView.setBorder(mon-1,1);
            } 
            if (!strcmp_P(serialBuffer+2+7, PSTR("green")))  {
              SmartView.setBorder(mon-1,2);
            } 
            if (!strcmp_P(serialBuffer+2+7, PSTR("blue")))  {
              SmartView.setBorder(mon-1,3);
            } 
          } 
        }
      }
    }
  }
}



