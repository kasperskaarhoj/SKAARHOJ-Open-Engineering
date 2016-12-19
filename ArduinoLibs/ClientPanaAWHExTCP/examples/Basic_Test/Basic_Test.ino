#include <SPI.h>
#include <Ethernet.h>
#include <Streaming.h>
#include <utility/w5100.h>
EthernetClient client;


byte mac[] = {
  0x90, 0xA2, 0xDA, 0x00, 0xF2, 0x05
};
IPAddress ip(192, 168, 0, 99);

IPAddress camera(192, 168, 0, 14);


#include <ClientPanaAWHExTCP.h>
ClientPanaAWHExTCP cameraObj;


void setup() {

  // start serial port:
  Serial.begin(115200);

  // give the ethernet module time to boot up:
  delay(1000);

  // start the Ethernet connection using a fixed IP address and DNS server:
  Ethernet.begin(mac, ip);

  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
    W5100.setRetransmissionTime(0xD0);  // Milli seconds
    W5100.setRetransmissionCount(1);

  cameraObj.begin(camera);
  cameraObj.serialOutput(1);
  cameraObj.connect();
}


void loop() {
  int a;
  // Zoom out:
  for (a = -100; a <= 100; a++)  {
    //cameraObj.doZoom(a);
    //cameraObj.setGainR(a);
    //while (!cameraObj.isReady())  {
      cameraObj.runLoop();

  }
}



