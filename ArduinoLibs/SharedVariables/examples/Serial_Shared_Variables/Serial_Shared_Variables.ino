/*****************
 * Example: Basic Shared Variables with Serial server
 * You can input to the script with the Serial Monitor
 * CURRENTLY: There is a bug with Arduino 1.5.8, works on 1.0.5!
 *
 * - kasper
 */

/*
	IMPORTANT: If you want to use this library in your own projects and/or products,
 	please play a fair game and heed the license rules! See our web page for a Q&A so
 	you can keep a clear conscience: http://skaarhoj.com/about/licenses/
 */


#include <SPI.h>
#include <Ethernet.h>
#include <UDPmessenger.h>


#include <SkaarhojBufferTools.h>
#include <SkaarhojSerialServer.h>
SkaarhojSerialServer SerialServer(Serial);

#include <SkaarhojPgmspace.h>
#include <SharedVariables.h>
#include <Streaming.h>

SharedVariables shareObj(12);  // Number of shared variables we allocate memory to handle  (every registered variable consumes 12 bytes of memory)

// Test variables:
int test_int = 10;
bool test_bool = true;
uint8_t test_uint8_t = 12;
uint16_t test_uint16_t = 12345;
long test_long = -1171510507;
unsigned long test_unsigned_long = 3123456789;
float test_float = 3.14159265358979;
char test_char = 'A';
char test_string[] = "ABCDEF";
uint8_t test_array[] = {1, 2, 3, 4};
int test_array_int[] = { -1, -2000, 30, 30000};

uint16_t test_uint16_t_array[] = {1000, 2000, 3000, 4000};  // Used to just pass a single position...





void handleSerialIncoming()  {
  shareObj.incomingASCIILine(SerialServer, SerialServer._HardSerial);
  Serial << F("Line!!\n");
}

void handleExternalChangeOfValue(uint8_t idx)  {
  Serial << F("Value idx=") << idx << F(" changed!\n");
}

#include <MemoryFree.h>

void setup() {

    // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Serial.println("\n\n******* START ********");

  SerialServer.begin(115200);
  SerialServer.setHandleIncomingLine(handleSerialIncoming);  // Put only the name of the function
  SerialServer.serialOutput(3);
  SerialServer.enableEOLTimeout();  // Calling this without parameters sets EOL Timeout to 2ms which is enough for 9600 baud and up. Use only if the serial client (like Arduinos serial monitor) doesn't by itself send a <cr> token.
  
  shareObj.setExternalChangeOfVariableCallback(handleExternalChangeOfValue);


  Serial << F("freeMemory()=") << freeMemory() << "\n";

  // NOTE: Names (the first "PSTR()" in the argument list of shareLocalVariable()) for sharing variables is the string by which the variable is referenced via the buffer interface - make sure not to name two variables the same or substrings of each other, for instance dont use names like "My Var" and "My Var2" because "My Var" is a substring of "My Var2". This may lead to unexpected behaviours.
  shareObj.shareLocalVariable(0, test_int, 3, PSTR("Test int"), PSTR("A test integer"), -1000, 1000);
  shareObj.shareLocalVariable(1, test_bool, 3, PSTR("Test bool"), PSTR("A test boolean"));
  shareObj.shareLocalVariable(2, test_uint8_t, 3, PSTR("Test uint8_t"), PSTR("A test uint8_t"), 1, 100);
  shareObj.shareLocalVariable(3, test_uint16_t, 3, PSTR("Test uint16_t"), PSTR("A test uint16_t"), 1, 40000);
  shareObj.shareLocalVariable(4, test_long, 3, PSTR("Test long"), PSTR("A test long"));
  shareObj.shareLocalVariable(5, test_unsigned_long, 3, PSTR("Test unsigned long"), PSTR("A test unsigned long"));
  shareObj.shareLocalVariable(6, test_float, 3, PSTR("Test float"), PSTR("A test float"), -10, 10);
  shareObj.shareLocalVariable(7, test_char, 3, PSTR("Test char"), PSTR("A test char"));
  shareObj.shareLocalVariable(8, test_string, sizeof(test_string), 3, PSTR("Test string"), PSTR("A test string"));
  shareObj.shareLocalVariable(9, test_array, sizeof(test_array), 3, PSTR("Test array, uint8_t"), PSTR("A test array with single bytes"));
  shareObj.shareLocalVariable(10, test_array_int, sizeof(test_array_int), 3, PSTR("Test array, integer"), PSTR("A test array with integers"));
  shareObj.shareLocalVariable(11, test_uint16_t_array[1], 3, PSTR("Test element uint16_t_array[1]"), PSTR("Testing an individual position of an array"), 1, 10000);

  Serial << F("Overview of variables:\n");
  shareObj.printOverview(Serial);
  Serial << F("Values:\n");
  shareObj.printValues(Serial);
}

void loop() {
  SerialServer.runLoop();
}
