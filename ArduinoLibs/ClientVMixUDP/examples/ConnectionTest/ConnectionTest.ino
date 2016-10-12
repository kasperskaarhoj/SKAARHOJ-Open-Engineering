// Including libraries:
#include <SPI.h>
#include <Ethernet.h>
#include <Streaming.h>
#include <MemoryFree.h>
#include <SkaarhojPgmspace.h>

// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9}; // <= SETUP!  MAC address of the Arduino
IPAddress clientIp(192, 168, 1, 99);              // <= SETUP!  IP address of the Arduino
IPAddress switcherIp(192, 168, 1, 34);           // <= SETUP!  IP address of the ATEM Switcher

// Include ClientVMix library and make an instance:
// The port number is chosen randomly among high numbers.
#include <ClientVMixUDP.h>
ClientVMixUDP VmixSwitcher;

void setup() {

  randomSeed(analogRead(5)); // For random port selection

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac, clientIp);
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  // Initialize a connection to the switcher:
  VmixSwitcher.begin(switcherIp);
  VmixSwitcher.serialOutput(0x80);
  VmixSwitcher.connect();
}

void loop() { VmixSwitcher.runLoop(); }
