/*****************
 * Example: Master (and slave) for Basic Shared Variables with UDP messenger (and TCP Telnet server)
 * With this example, the Arduino becomes a UDP messenger server/slave (and Telnet Server as an extra feature, not mandatory) on 192.168.10.99 through which you can get and set values of the shared variables
 * The sketch also implements Master/Client features which actively set up relations to slaves and set/get values to/from them. This is different from the pure slave which will only respond to incoming requests.
 *
 * The sketch assumes there are slaves to talk to on the same network. (Use example sketch "UDPMessenger_SV_Slave" for create such a slave on x.x.x.slaveAddr)
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must make specific set ups in the below lines where the comment "// SETUP" is found!
 */
/*
	IMPORTANT: If you want to use this library in your own projects and/or products,
 	please play a fair game and heed the license rules! See our web page for a Q&A so
 	you can keep a clear conscience: http://skaarhoj.com/about/licenses/
 */

// Including libraries: 
#include <SPI.h>
#include <Ethernet.h>
#include <Streaming.h>
#include <SkaarhojTools.h>
SkaarhojTools sTools(0);

// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0xBF, 0x3E, 0x79 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192, 168, 0, 153);              // <= SETUP!  IP address of the Arduino (should be found in masterSlavesInNetwork[])

uint8_t masterSlavesInNetwork[] = {
  150,151,152,153,154,155,156,157};   // The network nodes to check

#include <SkaarhojBufferTools.h>
#include <SkaarhojTCPServer.h>
SkaarhojTCPServer TCPServer(8899);  // Port 8899 used for telnet server

#include <UDPmessenger.h>
UDPmessenger messenger;

#include <utility/w5100.h>

#include <SkaarhojPgmspace.h>
#include <SharedVariables.h>

SharedVariables shareObj(25);  // Number of shared variables we allocate memory to handle  (every registered variable consumes 12 bytes of memory)



// Test shared variables (slave function of this device):
int test_int = 10;
bool test_bool = true;
uint8_t test_uint8_t = 12;
uint16_t test_uint16_t = 12345;
long test_long = -1171510507;
unsigned long test_unsigned_long = 3123456789;
float test_float = 3.14159265;
char test_char = 'A';
char test_string[] = "ABCDEF";
uint8_t test_array[] = {
  1, 2, 3, 4};
int test_array_int[] = { 
  -1, -2000, 30, 30000};

uint16_t test_uint16_t_array[] = {
  1000, 2000, 3000, 4000};  // Used to just pass a single position...


int valueSendCount[8];
int valueResponseCount[8];
int incomingMsgCount[8];
long totalValueIncomingWriteCount;
long totalValueSendCount;
long totalValueResponseCount;
int totalValueIncomingWriteCountPerSec;
int totalValueSendCountPerSec;
int totalValueResponseCountPerSec;
uint8_t isDevicePreset[8];


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

  incomingMsgCount[sTools.shapeInt(from-150,0,7)]++;
}

/**
 * Callback function for shareObj when an external event from UDP or Telnet has changed a local shared variable
 */
void handleExternalChangeOfValue(uint8_t idx)  {
  totalValueIncomingWriteCount++;
}

/**
 * Callback function for when slaves return read response data. This data has no place to end up (unless associated with a local shared variable) so it must be handled accordingly.
 */
void handleExternalUDPReadResponse(const uint8_t slaveIdx, const uint8_t slaveAddress, const uint8_t dataLength, const uint8_t *dataArray)  {
  valueResponseCount[sTools.shapeInt(slaveAddress-150,0,7)]++;
  totalValueResponseCount++;
}






#include <MemoryFree.h>

void setup() {
  // Start the Ethernet:
  Ethernet.begin(mac,ip);
  delay(1000);

  // Initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Serial.println("\n\n******* START ********");
  Serial.println("UDP MASTER on port 8765\n**********************");



  W5100.setRetransmissionTime(0xD0);  // Milli seconds
  W5100.setRetransmissionCount(1);

  // Initialize Telnet Server and connect to shareObj through callback:
  TCPServer.begin();
  TCPServer.setHandleIncomingLine(handleTelnetIncoming);  // Put only the name of the function
  TCPServer.serialOutput(3);

  // Initialize UDP messenger and connect to shareObj through callback:
  messenger.registerReceptionCallbackFunction(UDPmessengerReceivedCommand);
  messenger.begin(ip, 8765);  // Port number to listen on for UDP packets (8765 for slaves (multiple), 8766 used for this master)
  messenger.serialOutput(true);  // Remove or comment

  shareObj.setExternalChangeOfVariableCallback(handleExternalChangeOfValue);
  shareObj.setExternalReadResponseCallback(handleExternalUDPReadResponse);

  // NOTE: Names (the first "PSTR()" in the argument list of shareLocalVariable()) for sharing variables is the string by which the variable is referenced via the buffer interface - make sure not to name two variables the same or substrings of each other, for instance dont use names like "My Var" and "My Var2" because "My Var" is a substring of "My Var2". This may lead to unexpected behaviours.
  shareObj.shareLocalVariable(0, test_int, 2, PSTR("Test int"), PSTR("A test integer"), -1000, 1000);
  shareObj.shareLocalVariable(1, test_bool, 1, PSTR("Test bool"), PSTR("A test boolean"));
  shareObj.shareLocalVariable(2, test_uint8_t, 3, PSTR("Test uint8_t"), PSTR("A test uint8_t"), 1, 100);
  shareObj.shareLocalVariable(3, test_uint16_t, 3, PSTR("Test uint16_t"), PSTR("A test uint16_t"), 1, 40000);
  shareObj.shareLocalVariable(4, test_long, 3, PSTR("Test long"), PSTR("A test long"));
  shareObj.shareLocalVariable(5, test_unsigned_long, 3, PSTR("Test unsigned long"), PSTR("A test unsigned long"));
  shareObj.shareLocalVariable(6, test_float, 3, PSTR("Test float"), PSTR("A test float"), -10, 10);
  shareObj.shareLocalVariable(7, test_char, 3, PSTR("Test char"), PSTR("A test char"));
  shareObj.shareLocalVariable(8, test_string, sizeof(test_string), 3, PSTR("Test string"), PSTR("A test string"));
  shareObj.shareLocalVariable(9, test_array, sizeof(test_array), 3, PSTR("Test array, uint8_t"), PSTR("A test array with single bytes"));
  shareObj.shareLocalVariable(10, test_array_int, sizeof(test_array_int), 3, PSTR("Test array, integer"), PSTR("A test array with integers"));
  shareObj.shareLocalVariable(11, test_uint16_t_array[1], 3, PSTR("Test element uint16_t_array[1]"), PSTR("Testing an individual position of an arrayTesting an individual position of an arrayTesting an individual position of an array"), 1, 10000);

  shareObj.shareLocalVariable(14, valueSendCount, sizeof(valueSendCount), 3, PSTR("valueSendCount"), PSTR(""));
  shareObj.shareLocalVariable(15, valueResponseCount, sizeof(valueResponseCount), 3, PSTR("valueResponseCount"), PSTR(""));
  shareObj.shareLocalVariable(16, incomingMsgCount, sizeof(incomingMsgCount), 3, PSTR("incomingMsgCount"), PSTR(""));
  shareObj.shareLocalVariable(17, isDevicePreset, sizeof(isDevicePreset), 3, PSTR("isDevicePreset"), PSTR(""));
  shareObj.shareLocalVariable(18, totalValueIncomingWriteCount, 3, PSTR("totalValueIncomingWriteCount"), PSTR(""));
  shareObj.shareLocalVariable(19, totalValueSendCount, 3, PSTR("totalValueSendCount"), PSTR(""));
  shareObj.shareLocalVariable(20, totalValueResponseCount, 3, PSTR("totalValueResponseCount"), PSTR(""));
  shareObj.shareLocalVariable(21, totalValueIncomingWriteCountPerSec, 3, PSTR("totalValueIncomingWriteCountPerSec"), PSTR(""));
  shareObj.shareLocalVariable(22, totalValueSendCountPerSec, 3, PSTR("totalValueSendCountPerSec"), PSTR(""));
  shareObj.shareLocalVariable(23, totalValueResponseCountPerSec, 3, PSTR("totalValueResponseCountPerSec"), PSTR(""));



  Serial << F("Overview of variables:\n");
  shareObj.printOverview(Serial);
  Serial << F("Values:\n");
  shareObj.printValues(Serial);



  // UDP SV Master configuration:
  shareObj.messengerObject(messenger);

  // Set up tracking of addresses toward which we will play master and ask for values.
  for(uint8_t i=0; i<sizeof(masterSlavesInNetwork); i++)  {
    messenger.trackAddress(masterSlavesInNetwork[i]);
  }


  Serial << F("freeMemory()=") << freeMemory() << "\n";
}

void loop() {
  uint8_t slaveAddr = masterSlavesInNetwork[random(0, sizeof(masterSlavesInNetwork))];
  if (slaveAddr!=ip[3])  {

    isDevicePreset[sTools.shapeInt(slaveAddr-150,0,7)] = messenger.isPresent(slaveAddr);

    if (messenger.isPresent(slaveAddr))  {
      uint8_t numberOfValues = random(1, 10);

      valueSendCount[sTools.shapeInt(slaveAddr-150,0,7)]+= numberOfValues;
      totalValueSendCount+= numberOfValues;

      Serial << F("Target: ") << slaveAddr << F(" with ") << numberOfValues << F(" values\n");

      shareObj.startBundle(slaveAddr);

      for(uint8_t i=0; i<numberOfValues; i++)  {
        uint8_t type = random(2, 5);
        switch(type)  {
        case 2:
          shareObj.setRemoteVariableOverUDP((uint8_t)random(0,256), slaveAddr, 2);
          break;
        case 3:
          shareObj.setRemoteVariableOverUDP((uint16_t)random(0,65536), slaveAddr, 3);
          break;
        case 4:
          shareObj.setRemoteVariableOverUDP((long)random(-1000000,1000000), slaveAddr, 4);
          break;
        }
      }
      shareObj.endBundle();
    } else {
      shareObj.sendUDPPing(slaveAddr);
    }
  }


  static unsigned long timer = millis();
  static long totalValueIncomingWriteCount_old = 0;
  static long totalValueSendCount_old = 0;
  static long totalValueResponseCount_old = 0;
  if (sTools.hasTimedOut(timer, 1000))  {
    totalValueIncomingWriteCountPerSec = totalValueIncomingWriteCount - totalValueIncomingWriteCount_old;
    totalValueSendCountPerSec = totalValueSendCount - totalValueSendCount_old;
    totalValueResponseCountPerSec = totalValueResponseCount - totalValueResponseCount_old;

    totalValueIncomingWriteCount_old = totalValueIncomingWriteCount;
    totalValueSendCount_old = totalValueSendCount;
    totalValueResponseCount_old = totalValueResponseCount;
  }

  lDelay(100);
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
  while (delayVal>0 && !sTools.hasTimedOut(timer, delayVal));
}







