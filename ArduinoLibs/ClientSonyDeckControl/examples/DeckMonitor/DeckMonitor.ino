/*****************
 * Example: Sony Deck Control Monitor
 * This will connect to a broadcast deck (such as AJA Ki Pro, Black Magic Design HyperDeck Studio etc) via RS-422 and show status.
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

#include <SkaarhojBufferTools.h>
#include <SkaarhojASCIIClient.h>
#include <SkaarhojSerialClient.h>
#include <ClientSonyDeckControl.h>
ClientSonyDeckControl Deck(Serial1);


// no-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}


void setup() {

  delay(1000);
  
  // Initialize the serial port:
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  // Initialize HyperDeck object:
  Deck.begin();
  Deck.pingTimeout(2000);  // Normally set to 200
  Deck.serialOutput(3);  // Normally, this should be commented out!
  Deck.connect();
}

bool isOnline = false;

void loop() {
  // Runloop:
  Deck.runLoop();
}

