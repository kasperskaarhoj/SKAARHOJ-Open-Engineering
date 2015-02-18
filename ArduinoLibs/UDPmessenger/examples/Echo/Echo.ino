// Send on a Mac like this: 
//   echo -n '00 08 04 AA BB CC 02 CC' | xxd -r -p | nc -u -w 1 192.168.0.147 8765
//   echo -n '00 08 04 AA BB CC 02 CC' | xxd -r -p | nc -u -w 1 192.168.0.147 8765 | ./hexdumpone.phpsh
// If you use the last option, the return values will print nicer, but you'll need to press CTRL+C to abort the script right after.
// Listen in like this on a Mac:  nc -u -l 9910 | ./hexdumpone.phpsh (NOT RELEVANT for this script!)
// With "socat" you can also test that it reacts to broadcast message, for instance try this on a shell with socat installed:
// echo -n '00 08 04 AA BB CC 02 CC' | xxd -r -p | socat STDIO UDP4-DATAGRAM:192.168.0.255:8765,broadcast,range=192.168.0.0/24
// With this, all Arduinos on the network listening on 8765 will echo back to you.


// Including libraries:
#include <SPI.h>
#include <Ethernet.h>
#include <Streaming.h>

// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0E, 0x6C, 0xB7 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192, 168, 0, 147);              // <= SETUP!  IP address of the Arduino


#include <UDPmessenger.h>
UDPmessenger messenger;




void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  

  // Initialize a connection to the switcher:
  messenger.registerReceptionCallbackFunction(UDPmessengerReceivedCommand);
  messenger.begin(ip, 8765);  // Port number to listen on for UDP packets
  messenger.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!
}

void loop() {
  messenger.runLoop();  // Listening
}


void UDPmessengerReceivedCommand(const uint8_t cmd, const uint8_t from, const uint8_t dataLength, const uint8_t *dataArray)  {
        // This will display the incoming data:
      Serial << ip[0] << "." << ip[1] << "." << ip[2] << "." << from << F(": CMD=") << _HEXPADL(cmd,2,"0") << F(", DATA=");
      for(uint8_t i=0; i<dataLength; i++)  {
        Serial << _HEXPADL(dataArray[i],2,"0") << (dataLength>i+1?F(","):F(""));
      }
      Serial.println();
      
        // We will echo it back, but this would normally be data in response to the incoming data:
      for(uint8_t i=0; i<dataLength; i++)  {
        messenger.addValueToDataBuffer(dataArray[i], i);
      }
      messenger.addCommand(cmd, dataLength);
}

