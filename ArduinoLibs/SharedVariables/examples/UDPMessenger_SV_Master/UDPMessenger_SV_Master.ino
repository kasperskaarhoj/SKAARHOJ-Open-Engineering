

// Including libraries: 
#include <SPI.h>
#include <Ethernet.h>
#include <Streaming.h>
#include <SkaarhojTools.h>
SkaarhojTools sTools(0);

// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
byte mac[] = { 
  0x90, 0xA1, 0xDA, 0xDF, 0x6E, 0x79 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192, 168, 10, 80);              // <= SETUP!  IP address of the Arduino

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
  Serial << F("\n\n******** SKAARDUINO TEST RIG START ********\n");
  Serial << F("Ethernet IP: ") << ip << F("\n");
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
  shareObj.shareLocalVariable(1, previewTally, 1, PSTR("Preview Tally Lamp"), PSTR(""));
  shareObj.shareLocalVariable(3, irisValue, 3, PSTR("Iris Value"), PSTR(""), 1, 2048);

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




