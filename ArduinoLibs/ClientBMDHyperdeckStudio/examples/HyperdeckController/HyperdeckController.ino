/*****************
 * Example: Hyperdeck Controller
 * This will connect to and allow keyboard based control (through the serial monitor) of a HyperDeck studio
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
#include <ClientBMDHyperdeckStudio.h>
ClientBMDHyperdeckStudio hyperDeck; 












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
  W5100.setRetransmissionTime(2000);  // Milli seconds
  W5100.setRetransmissionCount(2);

  // give the Ethernet shield a second to initialize:
  delay(1000);

  // Start Hyperdeck connection:
  hyperDeck.begin(IPAddress(192, 168, 10, 230));	 // <= SETUP (the IP address of the Hyperdeck Studio)
  hyperDeck.serialOutput(1);  // 1= normal, 2= medium verbose, 3=Super verbose
  hyperDeck.connect();  // For some reason the first connection attempt seems to fail, but in the runloop it will try to reconnect.

  while(!hyperDeck.hasInitialized())  {
    hyperDeck.runLoop();
  }

  Serial << F("Ready to control HyperDeck. Type any of these keys + Enter in the serial monitor to control the HyperDeck:\n");

  Serial << F("j : increase backwards speed\n");
  Serial << F("k : stop\n");
  Serial << F("l : increase forward speed\n");
  Serial << F("u : previous clip\n");
  Serial << F("i : preview(live) or current clip toggle\n");
  Serial << F("o : next clip\n");
  Serial << F("[space] : play @ 100% speed\n");
  Serial << F("r : record\n");
  Serial << F("list : List files on hyperdeck (last 10)\n");
  Serial << F("load <clipID> : Cue up clip with a given id (eg. from list)\n");
  Serial << F("q : disconnect and quit\n");
}

void loop() {
  hyperDeck.runLoop();

  // Tracking state changes:
  static uint8_t HDtransportStatus;
  if (hyperDeck.getTransportStatus()!=HDtransportStatus)  {
    HDtransportStatus = hyperDeck.getTransportStatus();
    Serial << "Transport status: ";
    switch(HDtransportStatus)  {
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

  static int HDplaySpeed;
  if (hyperDeck.getPlaySpeed()!=HDplaySpeed)  {
    HDplaySpeed = hyperDeck.getPlaySpeed();
    Serial << "Play speed: " << HDplaySpeed << "\n";
  }

  static uint8_t HDclipId;
  if (hyperDeck.getClipId()!=HDclipId)  {
    HDclipId = hyperDeck.getClipId();
    Serial << "Clip ID: " << HDclipId << "\n";
  }


  // Detect commands:
  if (hyperDeck.hasInitialized())  {
    if (loadSerialCommand())  {
      if (!strcmp_P(serialBuffer,PSTR("list")))  {
        Serial << F("Total files on Hyperdeck: ") << hyperDeck.getTotalClipCount() << F("\n");
        Serial << F("Last 10 filenames and clip ids:\n");
        for(uint8_t i=0; i<ClientBMDHyperdeckStudio_CLIPS; i++)  {
          Serial << F("\"") << hyperDeck.getFileName(i) << F("\" - Clip ID: ") << hyperDeck.getFileClipId(i) << F("\n");
        }
      } 
      else 
        if (!strncmp_P(serialBuffer,PSTR("load "),5))  {
        uint8_t clipId = atoi(serialBuffer+5);
        for(uint8_t i=0; i<ClientBMDHyperdeckStudio_CLIPS; i++)  {
          if (clipId==hyperDeck.getFileClipId(i))  {
            Serial << F("Loading \"") << hyperDeck.getFileName(i) << F("\"\n");
            hyperDeck.gotoClipID(clipId);
          }
        }
      } 
      else {
        switch((char)serialBuffer[0])  {
        case 'j':
          if (HDplaySpeed<=0)  {
            hyperDeck.playWithSpeed(HDplaySpeed<0 ? HDplaySpeed*2 : -25);
          } 
          else {
            hyperDeck.playWithSpeed(HDplaySpeed>25 ? HDplaySpeed/2 : 0);
          }
          break;      
        case 'k':
          hyperDeck.stop();
          break;      
        case 'l':
          if (HDplaySpeed>=0)  {
            hyperDeck.playWithSpeed(HDplaySpeed>0 ? HDplaySpeed*2 : 25);
          } 
          else {
            hyperDeck.playWithSpeed(HDplaySpeed<-25 ? HDplaySpeed/2 : 0);
          }
          break;      
        case ' ':
          if (HDtransportStatus!=ClientBMDHyperdeckStudio_TRANSPORT_PLAY)  {
            hyperDeck.play();
          } 
          else {
            hyperDeck.stop();
          }
          break;      
        case 'r':
          hyperDeck.record();
          break;      

        case 'u':
          hyperDeck.gotoClipID(HDclipId-1);
          break;
        case 'o':
          hyperDeck.gotoClipID(HDclipId+1);
          break;
        case 'i':
          if (HDtransportStatus==ClientBMDHyperdeckStudio_TRANSPORT_PREVIEW)  {
            hyperDeck.previewEnable(false);
          } 
          else {
            hyperDeck.previewEnable(true);
          }
          break;

        case 'q':  // Disconnect and quit:
          hyperDeck.disconnect();
          hyperDeck.runLoop(500);
          while(true);
          break;      
        }
      }
    }
  }
}





