/*
 * Retrieve the MAC address from a Microchip 24AA125E48 I2C ROM, and report it
 * to the serial console at 57600bps. The I2C address of the ROM is set to 0x50,
 * which assumes both the address pins are tied to 0V.
 */

#define I2C_ADDRESS 0x50
// Including libraries: 
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EEPROM.h>      // For storing IP numbers
#include <Wire.h>
#include <Streaming.h>


static uint8_t mac[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; 
static uint8_t ip[] = {
  192, 168, 10, 99 };    // IP address of your SKAARHOJ device

static uint8_t atemip[] = {
  192, 168, 10, 240 };    // IP address of your ATEM switcher

static uint8_t atem2ip[] = {
  192, 168, 10, 241 };    // IP address of your second ATEM switcher

static uint8_t videohubip[] = {
  192, 168, 10, 243 };    // IP address of your Videohub

char buffer[18];

int greenLEDPin1 = 2;
int redLEDPin1 = 3;
int greenLEDPin2 = 22;
int redLEDPin2 = 23;

bool isConfigMode;

void setup()
{
  Wire.begin();         // Join i2c bus (I2C address is optional for the master)
  Serial.begin(115200);
  for(int i = 0; i < 30; i++)
  {
    Serial.println(" ");
  }
  Serial.println("Starting test for MAC address ROM");
  Serial.print("Getting MAC: ");
  
  mac[0] = readRegister(0xFA);
  mac[1] = readRegister(0xFB);
  mac[2] = readRegister(0xFC);
  mac[3] = readRegister(0xFD);
  mac[4] = readRegister(0xFE);
  mac[5] = readRegister(0xFF);
  
  char tmpBuf[17];
  sprintf(tmpBuf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(tmpBuf);
  Serial.println(" TEST OK");

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

  pinMode(A1,INPUT_PULLUP);
  delay(100);
  isConfigMode = (analogRead(A1) > 500) ? false : true;

  delay(500);
  Serial << F("Config Mode: ") << isConfigMode << "\n";  
  delay(500);
    
  // Sets the Bi-color LED to off = "no connection"
  digitalWrite(redLEDPin1,false);
  digitalWrite(greenLEDPin1,false);
  digitalWrite(redLEDPin2,false);
  digitalWrite(greenLEDPin2,false);
  pinMode(redLEDPin1, OUTPUT);
  pinMode(greenLEDPin1, OUTPUT);
  pinMode(redLEDPin2, OUTPUT);
  pinMode(greenLEDPin2, OUTPUT);  

//  EEPROM.write(90, 80);
//  Serial << F("\nPort = 80\n\n");
}

void loop()
{
  Serial << F("RED\n");
  isConfigMode = (analogRead(A1) > 500) ? false : true;
  Serial << F("Config Mode: ") << isConfigMode << "\n";  
  digitalWrite(greenLEDPin1,false);
  digitalWrite(greenLEDPin2,false);
  digitalWrite(redLEDPin1,true);
  digitalWrite(redLEDPin2,true);
  delay(1000);

  Serial << F("GREEN\n");
  isConfigMode = (analogRead(A1) > 500) ? false : true;
  Serial << F("Config Mode: ") << isConfigMode << "\n";  
  digitalWrite(greenLEDPin1,true);
  digitalWrite(greenLEDPin2,true);
  digitalWrite(redLEDPin1,false);
  digitalWrite(redLEDPin2,false);
  delay(1000);

  Serial << F("ORANGE\n");
  isConfigMode = (analogRead(A1) > 500) ? false : true;
  Serial << F("Config Mode: ") << isConfigMode << "\n";  
  digitalWrite(greenLEDPin1,true);
  digitalWrite(greenLEDPin2,true);
  digitalWrite(redLEDPin1,true);
  digitalWrite(redLEDPin2,true);
  delay(1000);

  Serial << F("Off\n");
  isConfigMode = (analogRead(A1) > 500) ? false : true;
  Serial << F("Config Mode: ") << isConfigMode << "\n";  
  digitalWrite(greenLEDPin1,false);
  digitalWrite(greenLEDPin2,false);
  digitalWrite(redLEDPin1,false);
  digitalWrite(redLEDPin2,false);
  delay(1000);
}

byte readRegister(byte r)
{
  unsigned char v;
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(r);  // Register to read
  Wire.endTransmission();

  Wire.requestFrom(I2C_ADDRESS, 1); // Read a byte
  while(!Wire.available())
  {
    // Wait
  }
  v = Wire.read();
  return v;
} 
