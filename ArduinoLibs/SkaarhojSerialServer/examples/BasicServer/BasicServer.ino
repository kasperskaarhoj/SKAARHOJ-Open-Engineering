/*****************
 * Example: Basic Serial Server
 * With this example, the Arduino handles various commands sent over the Serial bus. Use the Arduino serial monitor to test it (at 115200 baud)
 *
 * - kasper
 */

/*
	IMPORTANT: If you want to use this library in your own projects and/or products,
 	please play a fair game and heed the license rules! See our web page for a Q&A so
 	you can keep a clear conscience: http://skaarhoj.com/about/licenses/
 */




// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}


#include <SkaarhojBufferTools.h>
#include <SkaarhojSerialServer.h>
SkaarhojSerialServer SerialServer(Serial);





/**
 * This function is a call back functions which is called when the server receives a text line over TCP.
 * This is where your local implementation details has to be processed!
 */

// Used to demonstrate how to get values in and out via the server:
uint8_t exampleIPAddr[4];
int exampleInteger;
bool exampleBoolean;
char exampleString[10+1];

void handleSerialIncoming()  {
  exampleString[10] = 0;  // Make sure it's terminated

  bool listing = false;  // If a "list" command is issued, this is set true and then all values will be printed.
  bool commandOK = false;  // This must be set when a command is accepted, otherwise we will output a NACK as well.
  

  if (SerialServer.isBufferEqualTo_P(PSTR("help")))  {  // if "help" is the command entered
    commandOK = true;
    SerialServer._HardSerial << F("List of commands:\n")
      << F("- help : This message\n")
        << F("- ip : Get IP address (example)\n")
          << F("- ip=<num>.<num>.<num>.<num> : Set IP address (example)\n")
            << F("- balance : Get balance value (example)\n")
              << F("- balance=<integer> : Set balance value (example)\n")
                << F("- light : Get status of light (example)\n")
                  << F("- light=<on/off> : Set value of light (example)\n")
                    << F("- name : Get status of name (example)\n")
                      << F("- name=<namestring, 10 chars> : Set value of name (example)\n")
                        << F("- ping : Checking connection\n")
                          << F("- list : Show all values\n")
                          << F("\r\n");

  } 
  else if (SerialServer.isBufferEqualTo_P(PSTR("ping")))  {  // if "ping" is entered as the command
    commandOK = true;
    SerialServer._HardSerial << F("ACK\r\n");
  } 
  else if (SerialServer.isBufferEqualTo_P(PSTR("list")))  {  // if "list" is entered as the command
    commandOK = true;
    listing = true;
  } 

    // Now to the actual commands:
  if (listing || SerialServer.isNextPartOfBuffer_P(PSTR("ip")))  {  // if "ip" is the first part of the command string entered...
    commandOK = true;
    if (SerialServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
      exampleIPAddr[0] = SerialServer.parseInt();  // This returns the integer value of the next part of the buffer and advances the buffer pointer. 
      SerialServer.isNextPartOfBuffer_P(PSTR("."));  // This assumes the next is "." and advances the pointer.
      exampleIPAddr[1] = SerialServer.parseInt();
      SerialServer.isNextPartOfBuffer_P(PSTR("."));
      exampleIPAddr[2] = SerialServer.parseInt();
      SerialServer.isNextPartOfBuffer_P(PSTR("."));
      exampleIPAddr[3] = SerialServer.parseInt();
      // If the IP address was entered perfectly after the form "X.X.X.X" we should now have it correctly. There is no check for it otherwise except that we return the new value for review:
    }
    // Printing the value in any case:
    SerialServer._HardSerial << F("ip=") 
      << exampleIPAddr[0] << F(".")
        << exampleIPAddr[1] << F(".")
          << exampleIPAddr[2] << F(".")
            << exampleIPAddr[3]
              << F("\r\n");
  } 
  
  if (listing || SerialServer.isNextPartOfBuffer_P(PSTR("balance")))  {  // if "ip" is the first part of the command string entered...
    commandOK = true;
    if (SerialServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
      exampleInteger = SerialServer.parseInt();  // This returns the integer value of the next part of the buffer and advances the buffer pointer. 
    }
    // Printing the value in any case:
    SerialServer._HardSerial << F("balance=") 
      << exampleInteger
        << F("\r\n");
  } 
  
  if (listing || SerialServer.isNextPartOfBuffer_P(PSTR("light")))  {  // if "ip" is the first part of the command string entered...
    commandOK = true;
    if (SerialServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
      if (SerialServer.isNextPartOfBuffer_P(PSTR("on")))  {
        exampleBoolean = true;
      }
      else if (SerialServer.isNextPartOfBuffer_P(PSTR("off")))  {
        exampleBoolean = false;
      }
      else {
        // Printing the value in any case:
        SerialServer._HardSerial << F("Error: Couldn't read the value for \"light\", so didn't change it.") 
          << F("\r\n");
      }
    }
    // Printing the value in any case:
    SerialServer._HardSerial << F("light=") 
      << (exampleBoolean ? F("on") : F("off"))
        << F("\r\n");
  } 
  
  if (listing || SerialServer.isNextPartOfBuffer_P(PSTR("name")))  {  // if "ip" is the first part of the command string entered...
    commandOK = true;
    if (SerialServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
      strncpy(exampleString, SerialServer.getRemainingBuffer(),10);
    }
    // Printing the value in any case:
    SerialServer._HardSerial << F("name=") 
      << (exampleString)
        << F("\r\n");
  } 
  
  if (!commandOK) {  // NACK + hint is returned if we didn't recognize command
    SerialServer._HardSerial << F("NACK (type \"help\" for commands)\r\n");
  }
}



void setup() { 

  delay(1000);
    
  SerialServer.begin(115200);
  SerialServer.setHandleIncomingLine(handleSerialIncoming);  // Put only the name of the function
  SerialServer.serialOutput(3);

  SerialServer.enableEOLTimeout();  // Calling this without parameters sets EOL Timeout to 2ms which is enough for 9600 baud and up. Use only if the serial client (like Arduinos serial monitor) doesn't by itself send a <cr> token.
  //SerialServer.enableEOLTimeout(500);  // This means the server will respond after 500 ms of idle time on incoming bytes. 500 is far too much but useful to illustrate the way it works
  //SerialServer.setEOLChar('@');  // This can set a line termination character. Default is <cr> but Arduinos Serial Monitor doesn't send that character, so we could use some other char instead if we wanted.

  Serial << F("\n- - - - - - - -\nSerial Started (by SerialServer, in this example it's using Serial object)\n");  
  Serial << F("Type stuff in the serial monitor...\n");  
  Serial << F("Notice: You should set SerialServer.serialOutput(0) if you want to see what the server REALLY outputs on it's own. Right now you also get all sorts of debug info out of it...\n");  
}

unsigned long lastValue=0;

void loop() {
  SerialServer.runLoop();
  
    // This is a way to broadcast to all attached clients:
  if (lastValue != millis()/10000)  {
    lastValue = millis()/10000;
    SerialServer._HardSerial << "SecSinceStart: " << (millis()/1000) << "\n";
  }
}



