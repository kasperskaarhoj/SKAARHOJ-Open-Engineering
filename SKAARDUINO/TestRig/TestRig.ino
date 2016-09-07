/**
 * Test application for SKAARDUINO mounted in Test Rig
 *
 * Press and hold the config button to enable extensive test of IO, otherwise it will quickly cycle status LED and test ethernet only.
 */



// Including libraries:
#include <SPI.h>
#include <Wire.h>
#include <Ethernet.h>
#include <Streaming.h>
#ifdef __arm__  /* Arduino DUE */
#include "SkaarhojDueEEPROM.h"
SkaarhojDueEEPROM EEPROM;
#else
#include "EEPROM.h"
#endif

#include <SkaarhojTools.h>
SkaarhojTools sTools(0);

// MAC address and IP address for this *particular* SKAARDUINO
byte mac[] = {0x90, 0xA1, 0xDA, 0, 0, 0};     // If last three bytes are zero it will set a random Mac address and store in EEPROM (only if not already set, or if config button is pressed)
IPAddress ip(192, 168, 10, 0);  // If last byte is zero it will set a random IP address and store in EEPROM (only if not already set, or if config button is pressed)

uint8_t slaveAddr = 2;   // The Test Rig slave we talk to in this sketch

#include <SkaarhojBufferTools.h>
#include <SkaarhojTCPServer.h>
SkaarhojTCPServer TCPServer(8899);  // Port 8899 used for telnet server

#include <UDPmessenger.h>
UDPmessenger messenger;

#include <utility/w5100.h>

#include <SkaarhojPgmspace.h>
#include <SharedVariables.h>

SharedVariables shareObj(4);  // Number of shared variables we allocate memory to handle (every registered variable consumes 12 bytes of memory)


// Test shared variables (slave function of this device):
bool someBoolean = false;
uint16_t someWord = 65535;
long someLong = 65535;
char someString[60];

#include <Adafruit_GFX.h>
#include <SkaarhojDisplayArray.h>

SkaarhojDisplayArray display;





// width x height = 128,13
static const uint8_t SKAARHOJ_Logo[] PROGMEM = {
  B00011111, B11111111, B00111000, B00011111, B00000011, B11000000, B00000011, B11000000, B01111111, B11111100, B00111100, B00000111, B10000001, B11111110, B00000000, B00001111,
  B01111111, B11111111, B00111000, B00111110, B00000011, B11100000, B00000011, B11100000, B01111111, B11111111, B00111100, B00000111, B10000111, B11111111, B11000000, B00001111,
  B11111111, B11111111, B00111000, B01111100, B00000111, B11110000, B00000111, B11100000, B01111111, B11111111, B10111100, B00000111, B10011111, B11001111, B11100000, B00001111,
  B11111000, B00000000, B00111000, B11111000, B00000111, B11110000, B00001111, B11110000, B01111000, B00001111, B10111100, B00000111, B10011110, B00000011, B11100000, B00001111,
  B11111000, B00000000, B00111001, B11110000, B00001111, B01111000, B00001111, B01110000, B01111000, B00000111, B10111100, B00000111, B10111110, B00000001, B11100000, B00001111,
  B11111111, B11111100, B00111011, B11100000, B00001111, B01111000, B00011110, B01111000, B01111000, B00011111, B10111111, B11111111, B10111100, B00000001, B11110000, B00001111,
  B01111111, B11111111, B00111111, B11000000, B00011110, B00111100, B00011110, B00111100, B01111111, B11111111, B00111111, B11111111, B10111100, B00000001, B11110000, B00001111,
  B00011111, B11111111, B10111011, B11100000, B00011110, B00111100, B00111110, B00111100, B01111111, B11111110, B00111111, B11111111, B10111100, B00000001, B11110000, B00001111,
  B00000000, B00001111, B10111001, B11110000, B00111111, B11111110, B00111111, B11111110, B01111000, B01111100, B00111100, B00000111, B10111110, B00000001, B11100000, B00001111,
  B00000000, B00001111, B10111000, B11111000, B00111111, B11111110, B01111111, B11111110, B01111000, B00111110, B00111100, B00000111, B10011110, B00000001, B11100000, B00011111,
  B01111111, B11111111, B10111000, B01111100, B01111000, B00001111, B01111000, B00001111, B01111000, B00011111, B00111100, B00000111, B10011111, B10000111, B11000000, B00111110,
  B01111111, B11111111, B00111000, B00111110, B01111000, B00001111, B11110000, B00001111, B01111000, B00001111, B10111100, B00000111, B10001111, B11111111, B10011111, B11111110,
  B01111111, B11111100, B00111000, B00011111, B11110000, B00000111, B11110000, B00000111, B11111000, B00000111, B10111100, B00000111, B10000001, B11111110, B00011111, B11110000,
};







/**
 * Callback function for telnet incoming lines
 * Passes the TCPserver object on towards the shareObj so it can process the input.
 */
void handleTelnetIncoming()  {
  shareObj.incomingASCIILine(TCPServer, TCPServer._server);

  Serial << F("Values:\n");
  shareObj.printValues(Serial);
}

/**
 * Callback function for UDP incoming data
 * Passes the messenger object on towards the shareObj so it can process the input.
 */
void UDPmessengerReceivedCommand(const uint8_t cmd, const uint8_t from, const uint8_t dataLength, const uint8_t *dataArray)  {
  shareObj.incomingBinBuffer(messenger, cmd, from, dataLength, dataArray);
}

/**
 * Callback function for shareObj when an external event from UDP or Telnet has changed a local shared variable
 */
void handleExternalChangeOfValue(uint8_t idx)  {
  Serial << F("Value idx=") << idx << F(" changed: ");
  shareObj.printSingleValue(Serial, idx);
  Serial << F("\n");
}

/**
 * Callback function for when slaves return read response data. This data has no place to end up (unless associated with a local shared variable) so it must be handled accordingly.
 */
int testRigSlaveVariable = 0;
uint8_t testRigSlaveTestStatus = 0;
void handleExternalUDPReadResponse(const uint8_t slaveIdx, const uint8_t slaveAddress, const uint8_t dataLength, const uint8_t *dataArray)  {
  // This will display the incoming data:
  /*  Serial << F("Read Response from: ") << ip[0] << "." << ip[1] << "." << ip[2] << "." << slaveAddress << F(": IDX=") << _HEXPADL(slaveIdx, 2, "0") << F(", DATA=");
    for (uint8_t i = 0; i < dataLength; i++)  {
      Serial << _HEXPADL(dataArray[i], 2, "0") << (dataLength > i + 1 ? F(",") : F(""));
    }
    Serial.println();
  */
  long temp;

  if (slaveAddress == 2)    {
    switch (slaveIdx)  {
      case 0:
        temp = shareObj.read_int(dataArray);
        //      Serial << F("Value, int: ") << temp << F("\n");
        if (temp == testRigSlaveVariable + 1) {
          testRigSlaveTestStatus = 1; // OK
        } else {
          testRigSlaveTestStatus = 2; // ERROR
        }
        break;
    }
  }
}






#include <MemoryFree.h>

void setup() {

  randomSeed(analogRead(0) + analogRead(1) + analogRead(5) + analogRead(6) + analogRead(7));

  // Initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  Serial << F("\n\n******** SKAARDUINO TEST RIG START ********\n");

  // Setup:
  pinMode(18, INPUT);    // CFG input (active low)  - if you set it to INPUT_PULLUP, the resistor on the Bottom part will not be strong enough to pull it down!!

  digitalWrite(13, 1);   // To prevent brief light in LEDs upon boot
  digitalWrite(15, 1);   // To prevent brief light in LEDs upon boot
  digitalWrite(14, 1);   // To prevent brief light in LEDs upon boot
  pinMode(13, OUTPUT);    // Red Status LED, active low
  pinMode(14, OUTPUT);    // Blue Status LED, active low
  pinMode(15, OUTPUT);    // Green Status LED, active low

  pinMode(10, INPUT);    // "D2"
  pinMode(11, INPUT);    // "D3"
  pinMode(12, OUTPUT);    // "D4"
  pinMode(2, INPUT);    // "D8"
  pinMode(3, INPUT);    // "D9"
  pinMode(19, INPUT);    // "D10"

  delay(100);




  // I2C / Display setup:
  Serial << F("Calling Wire.begin()\n");
  Wire.begin();

  Serial << F("Writing to display\n");
  display.begin(4, 1);
  display.clearDisplay();   // clears the screen and buffer
  display.display(B1);  // Write to all
  display.setRotation(0);

  bool configMode = false;


  // ***************************
  // Config/Reset button:
  // ***************************

  // Test display:
  Serial << F("Test display\n");
  display.clearDisplay();
  display.drawBitmap(0, 0, SKAARHOJ_Logo, 128, 13, WHITE);
  display.setCursor(0, 18);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print("CFG Button");
  display.display(B1);


  // Check CFG input:
  Serial << F("\nTEST: Check if config button is held down after power-up/reset...\n");
  if (!digitalRead(18)) {   // Config mode, light up status LED in red and stop.
    digitalWrite(13, 0);   // Red
    digitalWrite(15, 1);   // Green
    digitalWrite(14, 1);   // Blue
    Serial << F(" => Config Mode ON: Status LED is red.\n");

    display.println(" = ON!");
    display.display(B1);

    configMode = true;
  } else {
    Serial << F(" => Config Mode OFF\n");
    display.println(" = Off");
    display.display(B1);
  }
  delay(500);


  // Setting MAC address from(/to) EEPROM:
  bool createMACaddress = false;
  char buffer[18];
  if (mac[3] == 0 && mac[4] == 0 && mac[5] == 0)  {
    if (configMode || (uint8_t)EEPROM.read(16) != ((EEPROM.read(10) + EEPROM.read(11) + EEPROM.read(12) + EEPROM.read(13) + EEPROM.read(14) + EEPROM.read(15)) & 0xFF))  {
      Serial << F("MAC address not found in EEPROM memory (or config mode)! Creating random one...\n");

      mac[3] = random(0, 256);
      mac[4] = random(0, 256);
      mac[5] = random(0, 256);

      // Set MAC address + checksum:
      EEPROM.write(10, mac[0]);
      EEPROM.write(11, mac[1]);
      EEPROM.write(12, mac[2]);
      EEPROM.write(13, mac[3]);
      EEPROM.write(14, mac[4]);
      EEPROM.write(15, mac[5]);
      EEPROM.write(16, (mac[0] + mac[1] + mac[2] + mac[3] + mac[4] + mac[5]) & 0xFF);

      sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      Serial.print("Storing SKAARDUINO MAC address: \n    ");
      Serial.print(buffer);
      Serial.print(" - Checksum: ");
      Serial.println(EEPROM.read(16));

      createMACaddress = true;
    }

    // Getting from EEPROM:
    mac[0] = EEPROM.read(10);
    mac[1] = EEPROM.read(11);
    mac[2] = EEPROM.read(12);
    mac[3] = EEPROM.read(13);
    mac[4] = EEPROM.read(14);
    mac[5] = EEPROM.read(15);
  }
  sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial << F("SKAARDUINO MAC address: ") << buffer << F(" - Checksum: ")
         << ((mac[0] + mac[1] + mac[2] + mac[3] + mac[4] + mac[5]) & 0xFF) << "\n";

  // Setting IP:
  if (ip[3] == 0)  {

    if (createMACaddress)  {
      Serial << F("Creating random IP address ...\n");

      ip[3] = random(3, 250);

      // Set IP address:
      EEPROM.write(2, ip[0]);
      EEPROM.write(3, ip[1]);
      EEPROM.write(4, ip[2]);
      EEPROM.write(5, ip[3]);

      sprintf(buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
      Serial.print("Storing SKAARHOJ Device (Arduino) IP address: ");
      Serial.println(buffer);
    }

    ip[0] = EEPROM.read(2);
    ip[1] = EEPROM.read(3);
    ip[2] = EEPROM.read(4);
    ip[3] = EEPROM.read(5);
  }
  sprintf(buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  Serial << F("SKAARDUINO IP address: ") << buffer << "\n";


  if (configMode) {
    delay(2000);
    digitalWrite(13, 1);   // Red
    digitalWrite(15, 1);   // Green
    digitalWrite(14, 1);   // Blue
    delay(200);
  }


  // ***************************
  // Cycle LEDs
  // ***************************
  display.clearDisplay();
  display.drawBitmap(0, 0, SKAARHOJ_Logo, 128, 13, WHITE);
  display.setCursor(0, 18);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println("Cycling Status LED");
  display.display(B1);


  // Test status LED: (cycled colors, ends with off):
  Serial << F("\nTEST: Cycles Status LED colors\n");
  for (uint8_t a = 1; a <= 7; a++)   {
    statusLed(a);
    delay(300);
  }
  statusLed(0);
  delay(500);



  if (configMode) {


    // ***************************
    // Digital IO check
    // ***************************
    display.clearDisplay();
    display.drawBitmap(0, 0, SKAARHOJ_Logo, 128, 13, WHITE);
    display.setCursor(0, 18);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.println("#1 Digital IO Check");
    display.display(B1);

    bool errorFlag = false;
    Serial << F("\nTEST #1: Digital pin 2 and 3...\n");

    pinMode(2, OUTPUT);    // "D8"
    pinMode(3, INPUT);    // "D9"
    digitalWrite(2, 1);
    delay(20);
    if (digitalRead(3) != 1)   {
      Serial << F(" => ERROR: D2 was set high, but D3 didn't follow!\n");
      errorFlag = true;
    }
    digitalWrite(2, 0);
    delay(20);
    if (digitalRead(3) != 0)   {
      Serial << F(" => ERROR: D2 was set low, but D3 didn't follow!\n");
      errorFlag = true;
    }

    pinMode(2, INPUT);    // "D8"
    pinMode(3, OUTPUT);    // "D9"
    digitalWrite(3, 1);
    delay(20);
    if (digitalRead(2) != 1)   {
      Serial << F(" => ERROR: D3 was set high, but D2 didn't follow!\n");
      errorFlag = true;
    }
    digitalWrite(3, 0);
    delay(20);
    if (digitalRead(2) != 0)   {
      Serial << F(" => ERROR: D3 was set low, but D2 didn't follow!\n");
      errorFlag = true;
    }

    if (!errorFlag)   Serial << F(" => OK: D2 and D3 worked\n");
    flashStatusLed(errorFlag ? 1 : 2);
    delay(500);





    display.clearDisplay();
    display.drawBitmap(0, 0, SKAARHOJ_Logo, 128, 13, WHITE);
    display.setCursor(0, 18);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.println("#2 Digital IO Check");
    display.display(B1);

    errorFlag = false;
    Serial << F("\nTEST #2: Digital pin 10 and 11...\n");

    pinMode(10, OUTPUT);    // "D2"
    pinMode(11, INPUT);    // "D3"
    digitalWrite(10, 1);
    delay(20);
    if (digitalRead(11) != 1)   {
      Serial << F(" => ERROR: D10 was set high, but D11 didn't follow!\n");
      errorFlag = true;
    }
    digitalWrite(10, 0);
    delay(20);
    if (digitalRead(11) != 0)   {
      Serial << F(" => ERROR: D10 was set low, but D11 didn't follow!\n");
      errorFlag = true;
    }

    pinMode(10, INPUT);    // "D2"
    pinMode(11, OUTPUT);    // "D3"
    digitalWrite(11, 1);
    delay(20);
    if (digitalRead(10) != 1)   {
      Serial << F(" => ERROR: D11 was set high, but D10 didn't follow!\n");
      errorFlag = true;
    }
    digitalWrite(11, 0);
    delay(20);
    if (digitalRead(10) != 0)   {
      Serial << F(" => ERROR: D11 was set low, but D10 didn't follow!\n");
      errorFlag = true;
    }

    if (!errorFlag)   Serial << F(" => OK: D10 and D11 worked\n");
    flashStatusLed(errorFlag ? 1 : 2);
    delay(500);



    // ***************************
    // Serial 1:
    // ***************************
    display.clearDisplay();
    display.drawBitmap(0, 0, SKAARHOJ_Logo, 128, 13, WHITE);
    display.setCursor(0, 18);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.println("#3: Serial1 check");
    display.display(B1);

    // Initialize serial communication at 115200 bits per second:
    Serial << F("\nTEST #3: Check Serial1\n");
    Serial1.begin(115200);
    Serial1 << F("A");
    delay(50);
    if (Serial1.available()) {
      int inByte = Serial1.read();
      if (inByte == 65) {
        Serial << F(" => OK: Serial input was correct\n");
        flashStatusLed(2);
      } else {
        Serial << F(" => ERROR: Serial1 input was not 'A' but '") << (char)inByte << F("'\n");
        flashStatusLed(1);
      }
    } else {
      Serial << F(" => ERROR: No Serial input data\n");
      flashStatusLed(1);
    }
    delay(500);






    // ***************************
    // Analog
    // ***************************
    display.clearDisplay();
    display.drawBitmap(0, 0, SKAARHOJ_Logo, 128, 13, WHITE);
    display.setCursor(0, 18);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.println("#4 Analog check");
    display.display(B1);

    // Initialize serial communication at 115200 bits per second:
    Serial << F("\nTEST #4: Analog inputs\n");
    errorFlag = false;

    digitalWrite(12, HIGH);
    delay(2000);
    if (!isAround(analogRead(2), 878))  {
      Serial << F(" => ERROR: analogRead(2) was not around 878 but ") << analogRead(2) << F(" instead\n");
      errorFlag = true;
    }
    if (!isAround(analogRead(3), 658))  {
      Serial << F(" => ERROR: analogRead(3) was not around 658 but ") << analogRead(3) << F(" instead\n");
      errorFlag = true;
    }
    if (!isAround(analogRead(4), 439))  {
      Serial << F(" => ERROR: analogRead(4) was not around 439 but ") << analogRead(4) << F(" instead\n");
      errorFlag = true;
    }
    if (!isAround(analogRead(0), 219))  {
      Serial << F(" => ERROR: analogRead(0) was not around 219 but ") << analogRead(0) << F(" instead\n");
      errorFlag = true;
    }

    digitalWrite(12, LOW);
    delay(2000);
    if (!isAround(analogRead(2), 0))  {
      Serial << F(" => ERROR: analogRead(2) was not around 0 but ") << analogRead(2) << F(" instead\n");
      errorFlag = true;
    }
    if (!isAround(analogRead(3), 0))  {
      Serial << F(" => ERROR: analogRead(3) was not around 0 but ") << analogRead(3) << F(" instead\n");
      errorFlag = true;
    }
    if (!isAround(analogRead(4), 0))  {
      Serial << F(" => ERROR: analogRead(4) was not around 0 but ") << analogRead(4) << F(" instead\n");
      errorFlag = true;
    }
    if (!isAround(analogRead(0), 0))  {
      Serial << F(" => ERROR: analogRead(0) was not around 0 but ") << analogRead(0) << F(" instead\n");
      errorFlag = true;
    }

    analogWrite(12, 128);
    delay(2000);
    if (!isAround(analogRead(2), 439))  {
      Serial << F(" => ERROR: analogRead(2) was not around 439 but ") << analogRead(2) << F(" instead\n");
      errorFlag = true;
    }
    if (!isAround(analogRead(3), 329))  {
      Serial << F(" => ERROR: analogRead(3) was not around 329 but ") << analogRead(3) << F(" instead\n");
      errorFlag = true;
    }
    if (!isAround(analogRead(4), 219))  {
      Serial << F(" => ERROR: analogRead(4) was not around 219 but ") << analogRead(4) << F(" instead\n");
      errorFlag = true;
    }
    if (!isAround(analogRead(0), 109))  {
      Serial << F(" => ERROR: analogRead(0) was not around 109 but ") << analogRead(0) << F(" instead\n");
      errorFlag = true;
    }

    // TODO: Add check for D10

    if (!errorFlag)   Serial << F(" => OK: Analog inputs worked\n");
    flashStatusLed(errorFlag ? 1 : 2);
    delay(500);

  }

  // ***************************
  // Start the Ethernet:
  // ***************************
  // Test display:
  display.clearDisplay();
  display.drawBitmap(0, 0, SKAARHOJ_Logo, 128, 13, WHITE);
  display.setCursor(0, 17);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println("#5 Ethernet Test");
  display << F("IP: ") << ip;
  display.display(B1);


  Ethernet.begin(mac, ip);
  delay(1000);

  Serial << F("\nEthernet IP: ") << ip << F("\n");
  Serial << F("UDP Shared Variables server/slave on port 8765\n");
  Serial << F("Telnet server on port 8899\n");

  W5100.setRetransmissionTime(0xD0);  // Milli seconds
  W5100.setRetransmissionCount(1);

  // Initialize UDP messenger and connect to shareObj through callback:
  messenger.registerReceptionCallbackFunction(UDPmessengerReceivedCommand);
  messenger.begin(ip, 8765);  // Port number to listen on for UDP packets
  messenger.serialOutput(true);  // Remove or comment

  shareObj.setExternalChangeOfVariableCallback(handleExternalChangeOfValue);
  shareObj.setExternalReadResponseCallback(handleExternalUDPReadResponse);

  // NOTE: Names (the first "PSTR()" in the argument list of shareLocalVariable()) for sharing variables is the string by which the variable is referenced via the buffer interface - make sure not to name two variables the same or substrings of each other, for instance dont use names like "My Var" and "My Var2" because "My Var" is a substring of "My Var2". This may lead to unexpected behaviours.
  shareObj.shareLocalVariable(0, someBoolean, 3, PSTR("Test Bool"), PSTR(""));
  shareObj.shareLocalVariable(1, someWord, 3, PSTR("Test Word"), PSTR(""));
  shareObj.shareLocalVariable(2, someLong, 3, PSTR("Test Long"), PSTR(""));
  shareObj.shareLocalVariable(3, someString, 3, sizeof(someString), PSTR("Test String"), PSTR(""));

  // UDP SV Master configuration:
  shareObj.messengerObject(messenger);

  // Set up tracking of addresses toward which we will play master and ask for values.
  messenger.trackAddress(slaveAddr);


  // Test connection over UDP to test rig slave:
  Serial << F("\nTEST #5: Sending UDP data (SharedVariables) to test rig slave on 192.168.10.2...\n");
  testRigSlaveVariable = random(-10000, 10000);
  testRigSlaveTestStatus = 0;
  shareObj.startBundle(slaveAddr);
  shareObj.setRemoteVariableOverUDP((int)testRigSlaveVariable, slaveAddr, 0);
  shareObj.getRemoteVariableOverUDP(slaveAddr, 0);
  shareObj.endBundle();
  lDelay(500);
  if (testRigSlaveTestStatus == 1)    {
    Serial << F(" => OK: Ethernet UDP data exchange with test rig slave on 192.168.10.2\n");
    flashStatusLed(2);
  } else if (testRigSlaveTestStatus == 2) {
    Serial << F(" => ERROR: Response OK, but wrong value!\n");
    flashStatusLed(1);
  } else {
    Serial << F(" => ERROR: No response to UDP message sent to test rig slave\n");
    flashStatusLed(1);
  }




  // Done with initial tests:
  delay(1000);

  // Test display:
  display.clearDisplay();
  display.drawBitmap(0, 0, SKAARHOJ_Logo, 128, 13, WHITE);
  display.setCursor(0, 18);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println("DONE");
  display.display(B1);

  Serial << F("\nInitial Linear Test done, freeMemory()=") << freeMemory() << "\n";
  flashStatusLed(4);
  flashStatusLed(4);
  flashStatusLed(4);
  delay(2000);
}

unsigned long runLoopCounter, prev_runLoopCounter, prevTime;
unsigned long errorInstances = 0;
bool ethernetError = false;

void loop() {

  lDelay(0);  // Runloops

  // Rotate colors on Status LED, so we can see the run loop is alive!
  if (errorInstances > 0) {
    digitalWrite(13, (((millis() >> 8) & 0x1) != 0) || !((millis() >> 9) % (errorInstances + 1))); // Red
    digitalWrite(15, (((millis() >> 8) & 0x1) != 0) || !((millis() >> 9) % (errorInstances + 1))); // Green
    digitalWrite(14, 1); // Blue
  } else {
    //    digitalWrite(13, ((millis() >> 8) & 0x3) != 0); // Red
    //  digitalWrite(15, ((millis() >> 8) & 0x3) != 1); // Green
    // digitalWrite(14, ((millis() >> 8) & 0x3) != 2); // Blue
    digitalWrite(13, 1); // Red
    digitalWrite(15, 1); // Green
    digitalWrite(14, ((millis() >> 8) & 0x1)); // Blue
  }

  // Talk to test rig slave:
  testRigSlaveVariable = random(-10000, 10000);
  testRigSlaveTestStatus = 0;
  shareObj.startBundle(slaveAddr);
  shareObj.setRemoteVariableOverUDP((int)testRigSlaveVariable, slaveAddr, 0);
  shareObj.getRemoteVariableOverUDP(slaveAddr, 0);
  shareObj.endBundle();

  unsigned long timer = millis();
  do {
    TCPServer.runLoop();  // Keep TCPServer running
    messenger.runLoop();  // Listening for UDP incoming
  }
  while (testRigSlaveTestStatus == 0 && !sTools.hasTimedOut(timer, 200));

  if (testRigSlaveTestStatus != 1)    {
    flashStatusLed(1);
    flashStatusLed(1);
    flashStatusLed(1);
    Serial << F(" => ERROR: UDP message response testRigSlaveTestStatus != 1. testRigSlaveTestStatus=") << testRigSlaveTestStatus << F("\n");
    if (!ethernetError) {
      errorInstances++;
      ethernetError = true;
    }
  } else {
    ethernetError = false;
  }

  if (millis() / 1000 != prevTime)    {
    prevTime = millis() / 1000;
    prev_runLoopCounter = runLoopCounter;
    runLoopCounter = 0;

    Serial << F("RunLoopCount: ") << prev_runLoopCounter << F(" - ") << F("Error Instances:") << errorInstances << F("\n");
  }

  // Test display:
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display << F("RunLoopCount: ") << prev_runLoopCounter << F("\n");
  display << F("Error Instances:") << errorInstances << F("\n");
  display.setTextSize(2);
  display << millis();
  display.setTextSize(1);
  display << " A:" << analogRead(0);
  display.display(B1);


  runLoopCounter++;
}


/**
 * Use this to make delays (because it will continously read digital inputs and update those counters!)
 */
void lDelay(uint16_t delayVal)  {
  unsigned long timer = millis();
  do {
    TCPServer.runLoop();  // Keep TCPServer running
    messenger.runLoop();  // Listening for UDP incoming
  }
  while (delayVal > 0 && !sTools.hasTimedOut(timer, delayVal));
}

void statusLed(uint8_t color)    {
  switch (color)   {
    case 1: //  red
      digitalWrite(13, 0);   // Red
      digitalWrite(15, 1);   // Green
      digitalWrite(14, 1);   // Blue
      break;
    case 2: //  green
      digitalWrite(13, 1);   // Red
      digitalWrite(15, 0);   // Green
      digitalWrite(14, 1);   // Blue
      break;
    case 3: //  blue
      digitalWrite(13, 1);   // Red
      digitalWrite(15, 1);   // Green
      digitalWrite(14, 0);   // Blue
      break;
    case 4: //  white
      digitalWrite(13, 0);   // Red
      digitalWrite(15, 0);   // Green
      digitalWrite(14, 0);   // Blue
      break;
    case 5: //  yellow
      digitalWrite(13, 0);   // Red
      digitalWrite(15, 0);   // Green
      digitalWrite(14, 1);   // Blue
      break;
    case 6: //  cyan
      digitalWrite(13, 1);   // Red
      digitalWrite(15, 0);   // Green
      digitalWrite(14, 0);   // Blue
      break;
    case 7: //  magenta
      digitalWrite(13, 0);   // Red
      digitalWrite(15, 1);   // Green
      digitalWrite(14, 0);   // Blue
      break;
    default: //  off
      digitalWrite(13, 1);   // Red
      digitalWrite(15, 1);   // Green
      digitalWrite(14, 1);   // Blue
      break;
  }
}


void flashStatusLed(uint8_t color)    {
  statusLed(color);
  delay(500);
  statusLed(0);
  delay(500);
}

bool isAround(int input, int around)    {
  return (input + 10 > around && input - 10 < around);
}

