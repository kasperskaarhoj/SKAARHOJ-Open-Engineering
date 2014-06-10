// Initial test, sending commands from Arduino to GrassValley Kayak switcher


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

  GVKayak.begin(GVip);
  GVKayak.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!
}


void loop() {

  Serial << F("Sending cut\n");
  GVKayak.doCut();

  delay(2000);


  // AUTO:
  Serial << F("Sending Auto\n");
  GVKayak.doAuto();

  delay(2000);


  // Slow Transition position over 10 steps:
  Serial << F("Slow Transisions:\n");
  for(int i=0;i<=10; i++)  {
    GVKayak.changeTransitionPosition(i*100);

    delay(1000);
  }

  delay(2000);


  // Fast Transition position over a second, 100 commands sent! 3 Times:
  Serial << F("3 Fast Transisions:\n");
  for (int ii=0; ii<3; ii++)  {
    for(int i=0;i<=100; i++)  {
      GVKayak.changeTransitionPosition(i*10);

      delay(20);
    }
    delay(1000);
  }

  delay(2000);
  
  // Select Program bus:
  Serial << F("Program bus select:\n");
  for(int i=1;i<=4; i++)  {
    GVKayak.changeProgramInput(i);
    delay(1000);
  }
  GVKayak.changeProgramInput(1);

  delay(2000);

  // Select Preset bus:
  Serial << F("Preset bus select:\n");
  for(int i=1;i<=4; i++)  {
    GVKayak.changePresetInput(i);
    delay(1000);
  }
  GVKayak.changePresetInput(2);

  delay(2000);
  
}

