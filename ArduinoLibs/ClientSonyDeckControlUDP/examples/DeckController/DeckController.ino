/*****************
 * Example: Sony Deck Control Monitor
 * This will connect to and allow keyboard based control (through the serial monitor) of a Sony compatible deck
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have a proper broadcast deck connected to your Arduinos Serial1 with a RS-422 to TTL converter.
 * - You must make specific setups in the below lines where the comment "// SETUP" is found!
 */


/*
	IMPORTANT: If you want to use this library in your own projects and/or products,
 	please play a fair game and heed the license rules! See our web page for a Q&A so
 	you can keep a clear conscience: http://skaarhoj.com/about/licenses/
 */

#include <SPI.h>
#include <Ethernet.h>
#include <SkaarhojBufferTools.h>
#include <SkaarhojASCIIClient.h>
#include <SkaarhojUDPClient.h>
#include <ClientSonyDeckControlUDP.h>
ClientSonyDeckControlUDP Deck;


// no-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
#include <Streaming.h>



// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 2, 177);

// Enter the IP address of the server you're connecting to:
IPAddress server(192, 168, 2, 1);




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


  // Initialize the serial port:
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  
  // start the Ethernet connection:
  Ethernet.begin(mac, ip);

  
  
  // Initialize HyperDeck object:
  Deck.begin(server);
  Deck.serialOutput(1);  // Normally, this should be commented out!
  Deck.connect();



  while(!Deck.hasInitialized())  {
    Deck.runLoop();
  }

  if (Deck.isInLocalModeOnly())  {
    Serial << F("Enable \"Remote Control\" on your deck!...(waiting)\n");
    while(Deck.isInLocalModeOnly())  {
      Deck.runLoop();
    }
  }


  Serial << F("Ready to control Broadcast Deck. Type any of these keys + Enter in the serial monitor to control the Deck:\n");

  Serial << F("[space] : play @ 100% speed\n");
  Serial << F("l : fast forward\n");
  Serial << F("k : stop\n");
  Serial << F("j : rewind\n");
  Serial << F("r : record\n");
}

bool isOnline = false;

void loop() {
  // Runloop:
  Deck.runLoop();



  // Detect commands:
  if (Deck.hasInitialized())  {
    if (loadSerialCommand())  {
      switch((char)serialBuffer[0])  {
      case 'j':
        Deck.doRewind();
        break;      
      case 'k':
        Deck.doStop();
        break;      
      case 'l':
        Deck.doFastForward();
        break;      
      case ' ':
        if (!Deck.isPlaying())  {
          Deck.doPlay();
        } 
        else {
          Deck.doStop();
        }
        break;      
      case 'r':
        Deck.doRecord();
        break;      
      }
    }
  }  
}


