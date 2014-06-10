/*****************
 * Example: Video Hub Monitor, using RS422 serial communication
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


void setup() {

  delay(2000);
  
  // start the serial library:
  Serial.begin(115200);

  Videohub.begin();
  Videohub.serialOutput(3);
  Videohub.connect();
}

void loop() {
  Videohub.runLoop(1000);
}





