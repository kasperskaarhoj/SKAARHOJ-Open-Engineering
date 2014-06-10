/*****************
 * This example returns state of a joystick from a SKAARHOJ Joystick board connected to 
 * an Arduino Mega Extended Breakout Shield
 * 
 * - kasper
 */



#include <SPI.h>
#include <Ethernet.h>

#include "SkaarhojUtils.h"
SkaarhojUtils utils;
SkaarhojUtils utils2;



byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xF2, 0x00};
IPAddress ip(192,168,10,99);

IPAddress camera(192,168,10,25);


#include <ClientPanaAWHExTCP.h>
ClientPanaAWHExTCP cameraObj;


//#include <MemoryFree.h>

// no-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}



void setup() {

  // Start the Serial (debugging) and UDP:
  Serial.begin(9600);  
  Serial << F("\n- - - - - - - -\nSerial Started\n");

    // Shows free memory:  
//  Serial << F("freeMemory()=") << freeMemory() << "\n";

  // give the ethernet module time to boot up:
  delay(1000);

  // start the Ethernet connection using a fixed IP address and DNS server:
  Ethernet.begin(mac, ip);

  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
//  W5100.setRetransmissionTime(0xD0);  // Milli seconds
//  W5100.setRetransmissionCount(1);

  cameraObj.begin(camera);
  cameraObj.connect();

  // Initializing the joystick at A12(H) and A13(V) + 35 (Joystick 1). A14 and A15, 36 is also possible for Joystick 2
  utils.joystick_init(10, A12, A13, 35);
  utils2.joystick_init(10, A14, A15, 36);
}


void loop() {
  cameraObj.runLoop();
  if (cameraObj.isReady())  {
    if (utils.joystick_hasMoved(0))  {
      cameraObj.doPan((utils.joystick_position(0)+100)/2);
    }
  }
  if (cameraObj.isReady())  {
    if (utils.joystick_hasMoved(1))  {
      cameraObj.doTilt((utils.joystick_position(1)+100)/2);
    }
  }
  if (cameraObj.isReady())  {
    if (utils2.joystick_hasMoved(1))  {
      cameraObj.doZoom((utils2.joystick_position(1)+100)/2);
    }
  }
}



