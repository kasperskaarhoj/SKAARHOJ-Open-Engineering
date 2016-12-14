#include "ClientAJAKumoTCP.h"
#include <Streaming.h>

ClientAJAKumoTCP client;
#include <SPI.h>    
#include <Ethernet.h>
#include <utility/w5100.h>

byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xE8, 0xE9 };    // <= SETUP
IPAddress ip(10, 0, 10, 99); 


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Ethernet.begin(mac, ip);

  IPAddress ip(10,0,10,225);
  client.begin(ip);
  client.serialOutput(1);
  Serial.println("Start.. Waiting 2 seconds");
  delay(2000);
  Serial.println("GO!");
}

uint32_t lastRouteChange;
uint8_t i,j;
void loop() {
  client.runLoop();


/*  if(millis() - lastRouteChange > 10) {
    if(i == 15) j = ++j % 16;
    i = ++i % 16;
    Serial << "Routing input " << i+1 << " to output " << j+1 << "\n";
    client.routeInputToOutput(i+1,  j+1);  
    lastRouteChange = millis();
  }*/
}
