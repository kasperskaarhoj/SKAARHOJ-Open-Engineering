/*****************
 * Example: Basic Shared Variables
 */
 
 
#include <SPI.h>
#include <Ethernet.h>
#include <UDPmessenger.h>

#include <SkaarhojPgmspace.h>
#include <SharedVariables.h>
#include <Streaming.h>
#include <SkaarhojBufferTools.h>

SharedVariables shareObj(12);  // Number of shared variables we allocate memory to handle


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




void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Serial.println("\n\n******* START ********");


  Serial.println("Initial values:");
  Serial << F("test_int = ") << test_int << F("\n");
  Serial << F("test_bool = ") << test_bool << F("\n");
  Serial << F("test_uint8_t = ") << test_uint8_t << F("\n");
  Serial << F("test_uint16_t = ") << test_uint16_t << F("\n");
  Serial << F("test_long = ") << test_long << F("\n");
  Serial << F("test_unsigned_long = ") << test_unsigned_long << F("\n");
  Serial << F("test_float = ") << test_float << F("\n");
  Serial << F("test_char = ") << test_char << F("\n");
  Serial << F("test_string = ") << test_string << F("\n");
  Serial << F("test_array = ") << test_array[0] << F(",") << test_array[1] << F(",") << test_array[2] << F(",") << test_array[3] << F("\n");
  Serial << F("test_array_int = ") << test_array_int[0] << F(",") << test_array_int[1] << F(",") << test_array_int[2] << F(",") << test_array_int[3] << F("\n");
  Serial << F("test_uint16_t_array[1] = ") << test_uint16_t_array[1] << F("\n");

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


  Serial << F("... (test modification of values in global space, increment by 100) ...\n");
  test_int += 100;
  test_bool = false;
  test_uint8_t += 100;
  test_uint16_t += 100;
  test_long += 100;
  test_unsigned_long += 100;
  test_float += 100;
  test_char = 'B';
  strncpy(test_string, "HIJKLMN", sizeof(test_string) - 1);

  uint8_t tmp[] = {100, 101, 102, 103, 104, 105};
  memcpy(test_array, tmp, sizeof(test_array));

  for (uint8_t i = 0; i < sizeof(test_array_int)/2; i++)	{
    test_array_int[i] = -test_array_int[i];
  }

  test_uint16_t_array[1] += 100;



  Serial.println("New values:");
  Serial << F("test_int = ") << test_int << F("\n");
  Serial << F("test_bool = ") << test_bool << F("\n");
  Serial << F("test_uint8_t = ") << test_uint8_t << F("\n");
  Serial << F("test_uint16_t = ") << test_uint16_t << F("\n");
  Serial << F("test_long = ") << test_long << F("\n");
  Serial << F("test_unsigned_long = ") << test_unsigned_long << F("\n");
  Serial << F("test_float = ") << test_float << F("\n");
  Serial << F("test_char = ") << test_char << F("\n");
  Serial << F("test_string = ") << test_string << F("\n");
  Serial << F("test_string = ") << test_string << F("\n");
  Serial << F("test_array = ") << test_array[0] << F(",") << test_array[1] << F(",") << test_array[2] << F(",") << test_array[3] << F("\n");
  Serial << F("test_array_int = ") << test_array_int[0] << F(",") << test_array_int[1] << F(",") << test_array_int[2] << F(",") << test_array_int[3] << F("\n");
  Serial << F("test_uint16_t_array[1] = ") << test_uint16_t_array[1] << F("\n");


  Serial << F("... (test modification of values in share object, by 10) ...\n");
  shareObj.test(0);
  shareObj.test(1);
  shareObj.test(2);
  shareObj.test(3);
  shareObj.test(4);
  shareObj.test(5);
  shareObj.test(6);
  shareObj.test(7);
  shareObj.test(8);
  shareObj.test(9);
  shareObj.test(10);
  shareObj.test(11);


  Serial.println("New values:");
  Serial << F("test_int = ") << test_int << F("\n");
  Serial << F("test_bool = ") << test_bool << F("\n");
  Serial << F("test_uint8_t = ") << test_uint8_t << F("\n");
  Serial << F("test_uint16_t = ") << test_uint16_t << F("\n");
  Serial << F("test_long = ") << test_long << F("\n");
  Serial << F("test_unsigned_long = ") << test_unsigned_long << F("\n");
  Serial << F("test_float = ") << test_float << F("\n");
  Serial << F("test_char = ") << test_char << F("\n");
  Serial << F("test_string = ") << test_string << F("\n");
  Serial << F("test_array = ") << test_array[0] << F(",") << test_array[1] << F(",") << test_array[2] << F(",") << test_array[3] << F("\n");
  Serial << F("test_array_int = ") << test_array_int[0] << F(",") << test_array_int[1] << F(",") << test_array_int[2] << F(",") << test_array_int[3] << F("\n");
  Serial << F("test_uint16_t_array[1] = ") << test_uint16_t_array[1] << F("\n");


  shareObj.printOverview(Serial);
  shareObj.printValues(Serial);
}
void loop() {
}




