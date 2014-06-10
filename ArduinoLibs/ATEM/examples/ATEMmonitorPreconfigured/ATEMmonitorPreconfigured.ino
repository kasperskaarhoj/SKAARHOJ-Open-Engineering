/*****************
 * Example: ATEM Monitor Preconfigured
 * Connects to the Atem Switcher and outputs changes to Preview and Program on the Serial monitor (at 9600 baud)
 * Arduino must be configured with ethernet addresses using the sketch "ConfigEthernetAddresses" first.
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an Atem Switcher connected to the same network as the Arduino - and you should have it working with the desktop software
 * - You must make specific set ups in the below lines where the comment "// SETUP" is found!
 */



// Including libraries: 
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EEPROM.h>      // For storing IP numbers

// Include ATEM library and make an instance:
#include <ATEM.h>
ATEM AtemSwitcher;

//#include <MemoryFree.h>

// Configure the IP addresses and MAC address with the sketch "ConfigEthernetAddresses":
uint8_t ip[4];        // Will hold the Arduino IP address
uint8_t atem_ip[4];  // Will hold the ATEM IP address
uint8_t mac[6];    // Will hold the Arduino Ethernet shield/board MAC address (loaded from EEPROM memory, set with ConfigEthernetAddresses example sketch)



// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}



void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Serial.begin(9600);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  



  // Setting the Arduino IP address:
  ip[0] = EEPROM.read(0+2);
  ip[1] = EEPROM.read(1+2);
  ip[2] = EEPROM.read(2+2);
  ip[3] = EEPROM.read(3+2);

  // Setting the ATEM IP address:
  atem_ip[0] = EEPROM.read(0+2+4);
  atem_ip[1] = EEPROM.read(1+2+4);
  atem_ip[2] = EEPROM.read(2+2+4);
  atem_ip[3] = EEPROM.read(3+2+4);
  
  Serial << F("SKAARHOJ Device IP Address: ") << ip[0] << "." << ip[1] << "." << ip[2] << "." << ip[3] << "\n";
  Serial << F("ATEM Switcher IP Address: ") << atem_ip[0] << "." << atem_ip[1] << "." << atem_ip[2] << "." << atem_ip[3] << "\n";
  
  // Setting MAC address:
  mac[0] = EEPROM.read(10);
  mac[1] = EEPROM.read(11);
  mac[2] = EEPROM.read(12);
  mac[3] = EEPROM.read(13);
  mac[4] = EEPROM.read(14);
  mac[5] = EEPROM.read(15);
  char buffer[18];
  sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial << F("SKAARHOJ Device MAC address: ") << buffer << F(" - Checksum: ")
        << ((mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5]) & 0xFF) << "\n";
  if ((uint8_t)EEPROM.read(16)!=((mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5]) & 0xFF))  {
    Serial << F("MAC address not found in EEPROM memory!\n") <<
      F("Please load example sketch ConfigEthernetAddresses to set it.\n") <<
      F("The MAC address is found on the backside of your Ethernet Shield/Board\n (STOP)");
    while(true);
  }

  Ethernet.begin(mac, ip);




  // Initialize a connection to the switcher:
  AtemSwitcher.begin(IPAddress(atem_ip[0],atem_ip[1],atem_ip[2],atem_ip[3]), 56417);
  AtemSwitcher.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher.connect();

  // Shows free memory:  
//  Serial << F("freeMemory()=") << freeMemory() << "\n";
}

void loop() {
  // Check for packets, respond to them etc. Keeping the connection alive!
  // VERY important that this function is called all the time - otherwise connection might be lost because packets from the switcher is
  // overlooked and not responded to.
    AtemSwitcher.runLoop();

  // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher.isConnectionTimedOut())  {
    Serial << F("Connection to ATEM Switcher has timed out - reconnecting!\n");
    AtemSwitcher.connect();
  }  

  // If you fancy to make delays in your sketches, ALWAYS do it using the AtemSwitcher delay function - this will wait while calling ru
  // Loop() checking for packets and thus keeping the connection up.
    AtemSwitcher.delay(5000);


  // If you monitor the serial output, you should see a lot of "ACK, rpID: xxxx" and then every 5 seconds this message:
  Serial << F("End of normal loop() - still kicking?\n");

  // Now, try also to disconnect the network cable - and see if it reconnects properly.
}

