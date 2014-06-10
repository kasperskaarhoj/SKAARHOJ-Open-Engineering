
#include <SPI.h>
#include <Ethernet.h>
EthernetClient client;


byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xF2, 0x00};
IPAddress ip(192,168,10,99);

IPAddress camera(192,168,10,25);


#include <ClientPanaAWHExTCP.h>
ClientPanaAWHExTCP cameraObj;


void setup() {

  // start serial port:
  Serial.begin(9600);

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
}


void loop() {
  int a;
  for(a=30;a<110;a=a+3)  {
    cameraObj.doZoom(a);
    while(!cameraObj.isReady())  {cameraObj.runLoop();}
 //   delay(2000);
  }
  for(a=70;a>30;a=a-3)  {
    cameraObj.doZoom(a);
    while(!cameraObj.isReady())  {cameraObj.runLoop();}
//    delay(2000);
  }
}



