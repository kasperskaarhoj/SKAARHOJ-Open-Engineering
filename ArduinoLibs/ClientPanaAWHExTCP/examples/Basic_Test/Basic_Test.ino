#include <SPI.h>
#include <Ethernet.h>
#include <Streaming.h>
#include <utility/w5100.h>
EthernetClient client;


byte mac[] = {
  0x90, 0xA2, 0xDA, 0x00, 0xF2, 0x00
};
IPAddress ip(192, 168, 10, 99);

IPAddress camera(192, 168, 10, 14);


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
  cameraObj.serialOutput(2);
  cameraObj.connect();
}


void loop() {
  int a;

  // Zoom in:
  for (a = 50; a < 70; a = a + 3)  {
    cameraObj.doZoom(a);
    Serial << F("Zoom in: ") << a << F("\n");
    while (!cameraObj.isReady())  { // Takes about 70ms to get ready...
      cameraObj.runLoop();
      Serial << ".";
      delay(10);
    }
    delay(2000);
  }
  delay(3000);

  // Zoom out:
  for (a = 50; a > 30; a = a - 3)  {
    cameraObj.doZoom(a);
    Serial << F("Zoom out: ") << a << F("\n");
    while (!cameraObj.isReady())  {
      cameraObj.runLoop();
      Serial << ".";
      delay(10);
    }
    delay(2000);
  }
  delay(3000);
}



