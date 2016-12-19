/*****************
 * Setting IP Addresses for a SKAARHOJ device
 * 
 * - kasper
 */





// Including libraries: 
#include <EEPROM.h>      // For storing IP numbers

static uint8_t mac[6];
  
static uint8_t ip[] = {
  192, 168, 10, 99 };    // IP address of your SKAARHOJ device, typical default is 192.168.10.99

static uint8_t atemip[] = {
  192, 168, 10, 240 };    // IP address of your ATEM switcher, factory default is 192.168.10.240



//// No-cost stream operator as described at 
//// http://arduiniana.org/libraries/streaming/
//template<class T>
//inline Print &operator <<(Print &obj, T arg)
//{  
//  obj.print(arg); 
//  return obj; 
//}

#include<Streaming.h>

char buffer[18];

void setup() { 
  Serial.begin(115200);
  Serial.println("\n- - - - - - - -\nSerial Started (ConfigIPAddresses)\n");

  // Set these random values so this initialization is only run once! 
  // These values confirm that Device IP and ATEM IP has been set at some point (they don't have checksums)
  EEPROM.write(0,12);
  EEPROM.write(1,232);

  // Set default IP address for Arduino (192.168.10.99)
  EEPROM.write(2,ip[0]);
  EEPROM.write(3,ip[1]);
  EEPROM.write(4,ip[2]);
  EEPROM.write(5,ip[3]);
  sprintf(buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  Serial.print("Storing SKAARHOJ Device (Arduino) IP address: \n    ");
  Serial.println(buffer);

  // Set default IP address for ATEM Switcher (192.168.10.240):
  EEPROM.write(6,atemip[0]);
  EEPROM.write(7,atemip[1]);
  EEPROM.write(8,atemip[2]);
  EEPROM.write(9,atemip[3]);
  sprintf(buffer, "%d.%d.%d.%d", atemip[0], atemip[1], atemip[2], atemip[3]);
  Serial.print("Storing ATEM IP address: \n    ");
  Serial.println(buffer);
  
  
  // Reading MAC address:
  mac[0] = EEPROM.read(10);
  mac[1] = EEPROM.read(11);
  mac[2] = EEPROM.read(12);
  mac[3] = EEPROM.read(13);
  mac[4] = EEPROM.read(14);
  mac[5] = EEPROM.read(15);
  char buffer[18];
  sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial << F("SKAARHOJ Device MAC address: \n    ") << buffer << F(" - Checksum: ")
        << ((mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5]) & 0xFF);
  if ((uint8_t)EEPROM.read(16)!=((mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5]) & 0xFF))  {
    Serial << F("\nStored checksum mismatched: ") << (uint8_t)EEPROM.read(16) << F(" => MAC address not found in EEPROM memory!\n") <<
      F("Please load example sketch ConfigEthernetAddresses to set it.\n") <<
      F("The MAC address is found on the backside of your Ethernet Shield/Board/Device\n (STOP)");
    while(true);
  } else {
    Serial << " => OK";
  }
  Serial << "\n";

  Serial.println("\nDONE! Everything worked! Now, time to upload the sketch for this model...\n");
}

void loop() {
}

