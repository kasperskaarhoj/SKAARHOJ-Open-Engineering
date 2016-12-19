/*****************
 * Setting Ethernet Addresses for a SKAARHOJ device
 * 
 * - kasper
 */





// Including libraries: 
#include <EEPROM.h>      // For storing IP numbers


// Configure the IP addresses and MAC address with the sketch "ConfigEthernetAddresses":
uint8_t ip[4];        // Will hold the Arduino IP address
uint8_t atem_ip[4];  // Will hold the ATEM IP address
uint8_t mac[6];    // Will hold the Arduino Ethernet shield/board MAC address (loaded from EEPROM memory, set with ConfigEthernetAddresses example sketch)
uint8_t atem2_ip[4];  // Will hold the ATEM IP address
uint8_t videohub_ip[4];  // Will hold the ATEM IP address


int calibrationCoordinatesX[5];
int calibrationCoordinatesY[5];


// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
//template<class T>
//inline Print &operator <<(Print &obj, T arg)
//{  
//  obj.print(arg); 
//  return obj; 
//}

#include<Streaming.h>

char buffer[18];
uint8_t checksumByte;

void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  


  if (EEPROM.read(0) != 12 ||  EEPROM.read(1) != 232)  {  // Just randomly selected values which should be unlikely to be in EEPROM by default.
    Serial << F("WARNING: Configuration of IP addresses may never have happened! Please check them below:\n");
  }

  // Setting the Arduino IP address:
  ip[0] = EEPROM.read(0+2);
  ip[1] = EEPROM.read(1+2);
  ip[2] = EEPROM.read(2+2);
  ip[3] = EEPROM.read(3+2);
  Serial << F("SKAARHOJ Device IP Address: \n    ") << ip[0] << "." << ip[1] << "." << ip[2] << "." << ip[3] << "\n";

  // Setting the ATEM IP address:
  atem_ip[0] = EEPROM.read(0+2+4);
  atem_ip[1] = EEPROM.read(1+2+4);
  atem_ip[2] = EEPROM.read(2+2+4);
  atem_ip[3] = EEPROM.read(3+2+4);
  Serial << F("ATEM Switcher IP Address: \n    ") << atem_ip[0] << "." << atem_ip[1] << "." << atem_ip[2] << "." << atem_ip[3] << "\n";

  // Setting MAC address:
  mac[0] = EEPROM.read(10);
  mac[1] = EEPROM.read(11);
  mac[2] = EEPROM.read(12);
  mac[3] = EEPROM.read(13);
  mac[4] = EEPROM.read(14);
  mac[5] = EEPROM.read(15);
  char buffer[18];
  sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  checksumByte = ((mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5]) & 0xFF);
  Serial << F("SKAARHOJ Device MAC address: \n     ") << buffer << F(" - Checksum: ")
    << checksumByte << "\n";
  if ((uint8_t)EEPROM.read(16)!=checksumByte)  {
    Serial << F("MAC address not found in EEPROM memory!\n");
  }

  // Setting the 2. ATEM IP address:
  atem2_ip[0] = EEPROM.read(40);
  atem2_ip[1] = EEPROM.read(41);
  atem2_ip[2] = EEPROM.read(42);
  atem2_ip[3] = EEPROM.read(43);
  checksumByte = ((atem2_ip[0]+atem2_ip[1]+atem2_ip[2]+atem2_ip[3]) & 0xFF);
  if (checksumByte == EEPROM.read(44))  {
    Serial << F("Second ATEM Switcher IP Address (if any): \n    ") << atem2_ip[0] << "." << atem2_ip[1] << "." << atem2_ip[2] << "." << atem2_ip[3] 
      << " - Checksum: " << checksumByte << "\n";
  } 
  else{
    Serial << F("Second ATEM Switcher IP Address (if any): \n    N/A\n");
  }

  // Setting the videohub IP address:
  videohub_ip[0] = EEPROM.read(45);
  videohub_ip[1] = EEPROM.read(46);
  videohub_ip[2] = EEPROM.read(47);
  videohub_ip[3] = EEPROM.read(48);
  checksumByte = ((videohub_ip[0]+videohub_ip[1]+videohub_ip[2]+videohub_ip[3]) & 0xFF);
  if (checksumByte == EEPROM.read(49))  {
    Serial << F("Videohub IP Address (if any): \n    ") << videohub_ip[0] << "." << videohub_ip[1] << "." << videohub_ip[2] << "." << videohub_ip[3] 
      << " - Checksum: " << checksumByte << "\n";
  } 
  else {
    Serial << F("Videohub IP Address (if any): \n    N/A\n");
  }


  // Touch Screen calibration:
  calibrationCoordinatesX[1] = (EEPROM.read(50) << 8) | EEPROM.read(51);
  calibrationCoordinatesY[1] = (EEPROM.read(52) << 8) | EEPROM.read(53);
  calibrationCoordinatesX[2] = (EEPROM.read(54) << 8) | EEPROM.read(55);
  calibrationCoordinatesY[2] = (EEPROM.read(56) << 8) | EEPROM.read(57);
  calibrationCoordinatesX[3] = (EEPROM.read(58) << 8) | EEPROM.read(59);
  calibrationCoordinatesY[3] = (EEPROM.read(60) << 8) | EEPROM.read(61);
  calibrationCoordinatesX[4] = (EEPROM.read(62) << 8) | EEPROM.read(63);
  calibrationCoordinatesY[4] = (EEPROM.read(64) << 8) | EEPROM.read(65);
  checksumByte = ((int)(calibrationCoordinatesX[1]
    + calibrationCoordinatesY[1]
    + calibrationCoordinatesX[2]
    + calibrationCoordinatesY[2]
    + calibrationCoordinatesX[3]
    + calibrationCoordinatesY[3]
    + calibrationCoordinatesX[4]
    + calibrationCoordinatesY[4]))  & 0xFF;  // checksum
  if (checksumByte == EEPROM.read(66))  {
    Serial << F("TouchScreen Calibration Data: \n    (") 
      << calibrationCoordinatesX[1] << "," 
      << calibrationCoordinatesY[1] << "," 
      << calibrationCoordinatesX[2] << "," 
      << calibrationCoordinatesY[2] << "," 
      << calibrationCoordinatesX[3] << "," 
      << calibrationCoordinatesY[3] << "," 
      << calibrationCoordinatesX[4] << "," 
      << calibrationCoordinatesY[4] << ")" 
      << " - Checksum: " << checksumByte << "\n";
  } 
  else {
    Serial << F("TouchScreen Calibration Data: \n    N/A\n");
  }
    

}

void loop() {
}


