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
  0x90, 0xA2, 0xDA, 0xDF, 0x6E, 0x79 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192, 168, 0, 140);              // <= SETUP!  IP address of the Arduino

uint8_t slaveAddr = 147;   // The slave we talk to in this sketch

#include <SkaarhojBufferTools.h>
#include <SkaarhojTCPServer.h>
SkaarhojTCPServer TCPServer(8899);  // Port 8899 used for telnet server

#include <UDPmessenger.h>
UDPmessenger messenger;

#include <utility/w5100.h>

#include <SkaarhojPgmspace.h>
#include <SharedVariables.h>

SharedVariables shareObj(12);  // Number of shared variables we allocate memory to handle  (every registered variable consumes 12 bytes of memory)



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
  /*        // This will display the incoming data:
   Serial << ip[0] << "." << ip[1] << "." << ip[2] << "." << from << F(": CMD=") << _HEXPADL(cmd,2,"0") << F(", DATA=");
   for(uint8_t i=0; i<dataLength; i++)  {
   Serial << _HEXPADL(dataArray[i],2,"0") << (dataLength>i+1?F(","):F(""));
   }
   Serial.println();
   */
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
void handleExternalUDPReadResponse(const uint8_t slaveIdx, const uint8_t slaveAddress, const uint8_t dataLength, const uint8_t *dataArray)  {
  // This will display the incoming data:
  Serial << F("Read Response from: ") << ip[0] << "." << ip[1] << "." << ip[2] << "." << slaveAddress << F(": IDX=") << _HEXPADL(slaveIdx,2,"0") << F(", DATA=");
  for(uint8_t i=0; i<dataLength; i++)  {
    Serial << _HEXPADL(dataArray[i],2,"0") << (dataLength>i+1?F(","):F(""));
  }
  Serial.println();


  long temp;
  char tempChar;
  float tempFloat;
  char tempStr[5];
  uint8_t tempUint8Array[3];
  int tempIntArray[3];

  //  switch(slaveAddress)  {
  //  case 147:
  switch(slaveIdx)  {
  case 0:
    temp = shareObj.read_int(dataArray);
    Serial << F("Value, int: ") << temp << F("\n");
    break; 
  case 1:
    temp = shareObj.read_bool(dataArray);
    Serial << F("Value, bool: ") << temp << F("\n");
    break; 
  case 2:
    temp = shareObj.read_uint8_t(dataArray);
    Serial << F("Value, uint8_t: ") << temp << F("\n");
    break; 
  case 3:
    temp = shareObj.read_uint16_t(dataArray);
    Serial << F("Value, uint16_t: ") << temp << F("\n");
    break; 
  case 4:
    temp = shareObj.read_long(dataArray);
    Serial << F("Value, long: ") << temp << F("\n");
    break; 
  case 5:
    temp = shareObj.read_unsigned_long(dataArray);
    Serial << F("Value, unsigned long: ") << (unsigned long)temp << F("\n");
    break; 
  case 6:
    tempFloat = shareObj.read_float(dataArray);
    Serial << F("Value, float: ") << tempFloat << F("\n");
    break; 
  case 7:
    tempChar = shareObj.read_char(dataArray);
    Serial << F("Value, char: ") << tempChar << F("\n");
    break; 
  case 8:
    shareObj.read_string(dataArray, tempStr, sizeof(tempStr));
    Serial << F("Value, string: ") << tempStr << F("\n");
    break; 
  case 9:
    shareObj.read_uint8_array(dataArray, tempUint8Array, sizeof(tempUint8Array));
    Serial << F("Value, uint8_t array: {");
    for(uint8_t i=0; i<sizeof(tempUint8Array); i++)  {
      Serial << tempUint8Array[i] << F(",");
    }
    Serial << F("}\n");
    break; 
  case 10:
    shareObj.read_int_array(dataArray, tempIntArray, sizeof(tempIntArray));
    Serial << F("Value, int array: {");
    for(uint8_t i=0; i<sizeof(tempIntArray)/2; i++)  {
      Serial << tempIntArray[i] << F(",");
    }
    Serial << F("}\n");
    break; 
  }
  //  break; 
  // }

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

  Serial << F("Overview of variables:\n");
  shareObj.printOverview(Serial);
  Serial << F("Values:\n");
  shareObj.printValues(Serial);



  // UDP SV Master configuration:
  shareObj.messengerObject(messenger);

  // Set up tracking of addresses toward which we will play master and ask for values.
  messenger.trackAddress(slaveAddr);


  Serial << F("freeMemory()=") << freeMemory() << "\n";
}

void loop() {

  if (!messenger.isPresent(slaveAddr))  {
    Serial << F("\nNot connected - Send ping to slave: \n");
    shareObj.sendUDPPing(slaveAddr);
    lDelay(1000);
  } 
  else {

    Serial << F("\nSend single values to slave: \n");
    // /Should/Must ALWAYS cast the outgoing value to make sure SharedVariables (shareObj) pick up the right data format for sending it.
    // BTW, this data format has to be the exact format used on the slave receiving the value! So a bool to a bool, an int to an int, a long to a long etc...
    shareObj.setRemoteVariableOverUDP((int)-2000, slaveAddr, 0);  // Sending the value "-2000" to slave x.x.x.slaveAddr, slaveIdx 0
    shareObj.setRemoteVariableOverUDP((bool)false, slaveAddr, 1);  // Sending the value "true" to slave x.x.x.slaveAddr, slaveIdx 1
    shareObj.setRemoteVariableOverUDP((uint8_t)123, slaveAddr, 2);    // etc...
    shareObj.setRemoteVariableOverUDP((uint16_t)45231, slaveAddr, 3);
    shareObj.setRemoteVariableOverUDP((long)-1000000, slaveAddr, 4);
    shareObj.setRemoteVariableOverUDP((unsigned long)1000000, slaveAddr, 5);
    shareObj.setRemoteVariableOverUDP((float)123.45, slaveAddr, 6);

    // No need to cast char, char arrays (strings), and other supported array type:
    shareObj.setRemoteVariableOverUDP('Y', slaveAddr, 7); // Sending the char "Y" to slave x.x.x.slaveAddr, slaveIdx 7

      char test_string_direct[] = "OPQRST";  
    uint8_t test_array_direct[] = {
      10, 20, 30, 40                };
    int test_array_int_direct[] = { 
      -500, -400, -300, -200                 };  

    shareObj.setRemoteVariableOverUDP(test_string_direct, sizeof(test_string_direct), slaveAddr, 8);   // Sending the string "OPQRSTU" to slave x.x.x.slaveAddr, slaveIdx 8
    shareObj.setRemoteVariableOverUDP(test_array_direct, sizeof(test_array_direct), slaveAddr, 9);   // Sending the byte array "10,20,30,40,50,70" to slave x.x.x.slaveAddr, slaveIdx 9
    shareObj.setRemoteVariableOverUDP(test_array_int_direct, sizeof(test_array_int_direct), slaveAddr, 10);   // Sending the int array "-500,-400,-300" to slave x.x.x.slaveAddr, slaveIdx 10
    lDelay(5000);



    Serial << F("\nSend get requests to slave: \n");

    shareObj.getRemoteVariableOverUDP(slaveAddr, 0);  // Asking the slave x.x.x.slaveAddr for the value of slaveIdx 0. Return value is handled by callback function "handleExternalUDPReadResponse"
    shareObj.getRemoteVariableOverUDP(slaveAddr, 1);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 2);  // etc...

    shareObj.getRemoteVariableOverUDP(slaveAddr, 3);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 4);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 5);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 6);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 7);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 8);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 9);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 10);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 11);  // etc...
    lDelay(5000);


    Serial << F("\nSend varying length arrays (shorter): \n");

    char test_string_direct2[] = "OPQR";  
    uint8_t test_array_direct2[] = {
      11, 22, 33                };
    int test_array_int_direct2[] = { 
      -555, -666, -777                 };  

    shareObj.setRemoteVariableOverUDP(test_string_direct2, sizeof(test_string_direct2), slaveAddr, 8);
    shareObj.setRemoteVariableOverUDP(test_array_direct2, sizeof(test_array_direct2), slaveAddr, 9);
    shareObj.setRemoteVariableOverUDP(test_array_int_direct2, sizeof(test_array_int_direct2), slaveAddr, 10);
    lDelay(5000);


    Serial << F("\nSend set bundle to slave: \n");
    // /Should/Must ALWAYS cast the outgoing value to make sure SharedVariables (shareObj) pick up the right data format for sending it.
    // BTW, this data format has to be the exact format used on the slave receiving the value! So a bool to a bool, an int to an int, a long to a long etc...
    shareObj.startBundle(slaveAddr);

    shareObj.setRemoteVariableOverUDP((int)-1000, slaveAddr, 0);
    shareObj.setRemoteVariableOverUDP((bool)true, slaveAddr, 1);
    shareObj.setRemoteVariableOverUDP((uint8_t)78, slaveAddr, 2);
    shareObj.setRemoteVariableOverUDP((uint16_t)13131, slaveAddr, 3);
    shareObj.setRemoteVariableOverUDP((long)-12121212, slaveAddr, 4);
    shareObj.setRemoteVariableOverUDP((unsigned long)666444222, slaveAddr, 5);
    shareObj.setRemoteVariableOverUDP((float)456.78, slaveAddr, 6);

    // No need to cast char, char arrays (strings), and other supported array type:
    shareObj.setRemoteVariableOverUDP('N', slaveAddr, 7); // Sending the char "Y" to slave x.x.x.slaveAddr, slaveIdx 7

      char test_string_direct4[] = "abcdef";  
    uint8_t test_array_direct4[] = {
      77, 78, 79, 80                };
    int test_array_int_direct4[] = { 
      -501, -401, -301, -201                 };  

    shareObj.setRemoteVariableOverUDP(test_string_direct4, sizeof(test_string_direct4), slaveAddr, 8);
    shareObj.setRemoteVariableOverUDP(test_array_direct4, sizeof(test_array_direct4), slaveAddr, 9);
    shareObj.setRemoteVariableOverUDP(test_array_int_direct4, sizeof(test_array_int_direct4), slaveAddr, 10);

    shareObj.endBundle();

    lDelay(5000);


    Serial << F("\nSend varying length arrays (longer): \n");

    char test_string_direct3[] = "LAZYDOGJUMPS";  
    uint8_t test_array_direct3[] = { 
      50, 51, 52, 53, 54, 55, 56                };
    int test_array_int_direct3[] = { 
      -10000,  -10001,  -10002,  -10003,  -10004                 };  

    shareObj.setRemoteVariableOverUDP(test_string_direct3, sizeof(test_string_direct3), slaveAddr, 8);
    shareObj.setRemoteVariableOverUDP(test_array_direct3, sizeof(test_array_direct3), slaveAddr, 9);
    shareObj.setRemoteVariableOverUDP(test_array_int_direct3, sizeof(test_array_int_direct3), slaveAddr, 10);
    lDelay(5000);



    Serial << F("\nSend mixed bundle to slave: \n");
    // /Should/Must ALWAYS cast the outgoing value to make sure SharedVariables (shareObj) pick up the right data format for sending it.
    // BTW, this data format has to be the exact format used on the slave receiving the value! So a bool to a bool, an int to an int, a long to a long etc...
    shareObj.startBundle(slaveAddr);

    shareObj.getRemoteVariableOverUDP(slaveAddr, 0);  // Asking the slave x.x.x.slaveAddr for the value of slaveIdx 0. Return value is handled by callback function "handleExternalUDPReadResponse"
    shareObj.getRemoteVariableOverUDP(slaveAddr, 1);  // etc...

    shareObj.getRemoteVariableOverUDP(slaveAddr, 6);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 3);  // etc...
    shareObj.setRemoteVariableOverUDP((uint16_t)141414, slaveAddr, 3);
    shareObj.setRemoteVariableOverUDP((long)-1999, slaveAddr, 4);
    shareObj.getRemoteVariableOverUDP(slaveAddr, 3);  // etc...
    shareObj.setRemoteVariableOverUDP((unsigned long)7777777, slaveAddr, 5);
    shareObj.getRemoteVariableOverUDP(slaveAddr, 10);  // etc...
    shareObj.setRemoteVariableOverUDP((float)89898.77, slaveAddr, 6);
    shareObj.getRemoteVariableOverUDP(slaveAddr, 10);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 11);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 6);  // etc...
    shareObj.setRemoteVariableOverUDP(test_string_direct3, sizeof(test_string_direct2), slaveAddr, 8);
    shareObj.setRemoteVariableOverUDP(test_array_direct3, sizeof(test_array_direct2), slaveAddr, 9);
    shareObj.getRemoteVariableOverUDP(slaveAddr, 9);  // etc...
    shareObj.setRemoteVariableOverUDP(test_array_int_direct3, sizeof(test_array_int_direct2), slaveAddr, 10);
    shareObj.getRemoteVariableOverUDP(slaveAddr, 10);  // etc...

    shareObj.endBundle();

    lDelay(5000);


    Serial << F("\nSend get bundle to slave: \n");
    // /Should/Must ALWAYS cast the outgoing value to make sure SharedVariables (shareObj) pick up the right data format for sending it.
    // BTW, this data format has to be the exact format used on the slave receiving the value! So a bool to a bool, an int to an int, a long to a long etc...
    shareObj.startBundle(slaveAddr);

    shareObj.getRemoteVariableOverUDP(slaveAddr, 0);  // Asking the slave x.x.x.slaveAddr for the value of slaveIdx 0. Return value is handled by callback function "handleExternalUDPReadResponse"
    shareObj.getRemoteVariableOverUDP(slaveAddr, 1);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 2);  // etc...

    shareObj.getRemoteVariableOverUDP(slaveAddr, 3);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 4);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 5);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 6);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 7);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 8);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 9);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 10);  // etc...
    shareObj.getRemoteVariableOverUDP(slaveAddr, 11);  // etc...

    shareObj.endBundle();

    lDelay(5000);


    Serial << F("\n\n\n\nEnd of loop, starting over... \n\n\n");
    lDelay(5000);
  }
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




