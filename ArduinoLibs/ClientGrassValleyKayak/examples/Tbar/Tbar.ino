/*****************
 * 
 * - kasper
 */


#include "SkaarhojUtils.h"
SkaarhojUtils utils;


#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008


// Set up MAC, IP local and Switcher IP:
byte mac[] = {  
  0x90, 0xA2, 0xDA, 0x0D, 0x23, 0x2D };
IPAddress ip(192, 168, 55, 46);  // Local IP of the Arduino
IPAddress GVip(192, 168, 55, 40);  // The Kayak Switcher IP

#include <ClientGrassValleyKayak.h>
ClientGrassValleyKayak GVKayak;


// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}



void setup() {
  // start the Ethernet and UDP:
  Ethernet.begin(mac,ip);

  // Start Serial:
  Serial.begin(9600);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  


  // Initializing the slider:
  utils.uniDirectionalSlider_init(10, 35, 35, A0);  // These 4 arguments are the default arguments given, if you call the function without any. See the library SkaarhojUtils for more info on what they are for.
  utils.uniDirectionalSlider_hasMoved();


  GVKayak.begin(GVip);
  GVKayak.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!
}


void loop() {

  // Look for T-bar movement:
  if (utils.uniDirectionalSlider_hasMoved())  {
    Serial << F("New position ") << utils.uniDirectionalSlider_position() << "\n";
    GVKayak.changeTransitionPosition(utils.uniDirectionalSlider_position());  
  }
}

