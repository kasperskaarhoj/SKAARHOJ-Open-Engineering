/*****************
 * Setting MAC Addresses for a SKAARHOJ device
 * 
 * - kasper
 */





// Including libraries: 
#include <EEPROM.h>      // For storing IP numbers

// MAC address for this *particular* Ethernet Shield!
// MAC address is printed on the shield
static uint8_t mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x3F, 0xB6 };    // MAC address of your SKAARHOJ device


char buffer[18];

void setup() { 
  Serial.begin(115200);
  Serial.println("\n- - - - - - - -\nSerial Started (ConfigMACAddress)\n");

  // 10-16: Arduino MAC address (6+1 byte)
  // Set MAC address + checksum:
  EEPROM.write(10,mac[0]);
  EEPROM.write(11,mac[1]);
  EEPROM.write(12,mac[2]);
  EEPROM.write(13,mac[3]);
  EEPROM.write(14,mac[4]);
  EEPROM.write(15,mac[5]);
  EEPROM.write(16, (mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5]) & 0xFF);

  sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print("Storing SKAARHOJ Device (Arduino) MAC address: \n    ");
  Serial.print(buffer);
  Serial.print(" - Checksum: ");
  Serial.println(EEPROM.read(16));


  EEPROM.write(17,0);

  Serial.println("DONE! Everything worked! Now, time to upload the sketch for this model...\n");
}

void loop() {
}

