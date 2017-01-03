/*****************
 * Setting Ethernet Addresses for a SKAARHOJ device
 * 
 * - kasper
 */





// Including libraries: 
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EEPROM.h>      // For storing IP numbers

// MAC address for this *particular* Ethernet Shield!
// MAC address is printed on the shield
static uint8_t mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x82, 0x2D };    // MAC address of your SKAARHOJ device

static uint8_t ip[] = {
  192, 168, 10, 99 };    // IP address of your SKAARHOJ device

static uint8_t atemip[] = {
  192, 168, 10, 240 };    // IP address of your ATEM switcher

static uint8_t atem2ip[] = {
  192, 168, 10, 241 };    // IP address of your second ATEM switcher

static uint8_t videohubip[] = {
  192, 168, 10, 243 };    // IP address of your Videohub



char buffer[18];

void setup() { 
  Serial.begin(115200);
  Serial.println("\n- - - - - - - -\nSerial Started (ConfigEthernetAddresses - EXTENDED)\n");

  // Set these random values so this initialization is only run once!
  EEPROM.write(0,12);
  EEPROM.write(1,232);

  // Set default IP address for Arduino/C100 panel (192.168.10.99)
  EEPROM.write(2,ip[0]);
  EEPROM.write(3,ip[1]);
  EEPROM.write(4,ip[2]);
  EEPROM.write(5,ip[3]);
  sprintf(buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  Serial.print("Storing SKAARHOJ Device (Arduino) IP address: ");
  Serial.println(buffer);

  // Set default IP address for ATEM Switcher (192.168.10.240):
  EEPROM.write(2+4,atemip[0]);
  EEPROM.write(3+4,atemip[1]);
  EEPROM.write(4+4,atemip[2]);
  EEPROM.write(5+4,atemip[3]);
  sprintf(buffer, "%d.%d.%d.%d", atemip[0], atemip[1], atemip[2], atemip[3]);
  Serial.print("Storing ATEM IP address: ");
  Serial.println(buffer);

  // Set default IP address for ATEM Switcher (192.168.10.241):
  EEPROM.write(40,atem2ip[0]);
  EEPROM.write(41,atem2ip[1]);
  EEPROM.write(42,atem2ip[2]);
  EEPROM.write(43,atem2ip[3]);
  EEPROM.write(44,(atem2ip[0]+atem2ip[1]+atem2ip[2]+atem2ip[3]) & 0xFF);
  sprintf(buffer, "%d.%d.%d.%d", atem2ip[0], atem2ip[1], atem2ip[2], atem2ip[3]);
  Serial.print("Storing Second ATEM IP address: ");
  Serial.println(buffer);

  // Set default IP address for Videohub router Switcher (192.168.10.241):
  EEPROM.write(45,videohubip[0]);
  EEPROM.write(46,videohubip[1]);
  EEPROM.write(47,videohubip[2]);
  EEPROM.write(48,videohubip[3]);
  EEPROM.write(49,(videohubip[0]+videohubip[1]+videohubip[2]+videohubip[3]) & 0xFF);
  sprintf(buffer, "%d.%d.%d.%d", videohubip[0], videohubip[1], videohubip[2], videohubip[3]);
  Serial.print("Storing Videohub IP address: ");
  Serial.println(buffer);

  // Set MAC address:
  EEPROM.write(10,mac[0]);
  EEPROM.write(11,mac[1]);
  EEPROM.write(12,mac[2]);
  EEPROM.write(13,mac[3]);
  EEPROM.write(14,mac[4]);
  EEPROM.write(15,mac[5]);
  EEPROM.write(16, (mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5]) & 0xFF);

  sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print("Storing SKAARHOJ Device (Arduino) MAC address: ");
  Serial.print(buffer);
  Serial.print(" - Checksum: ");
  Serial.println(EEPROM.read(16));


  Serial.println("DONE! Everything worked! Now, time to upload the sketch for this model...\n");


}

void loop() {
}

