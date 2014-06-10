/*****************
 * Example: Basic TCP Telnet server
 * With this example, the Arduino becomes a Telnet Server on 192.168.10.99 to which you can connect on port 8899 with a telnet client application (terminal program)
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


// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192, 168, 10, 99);              // <= SETUP!  IP address of the Arduino


// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}


#include <SkaarhojBufferTools.h>
#include <SkaarhojTCPServer.h>
SkaarhojTCPServer TCPServer(8899);  // Port 8899 used







/**
 * This function is a call back functions which is called when the server receives a text line over TCP.
 * This is where your local implementation details has to be processed!
 */

// Used to demonstrate how to get values in and out via the server:
uint8_t exampleIPAddr[4];
int exampleInteger;
bool exampleBoolean;
char exampleString[10+1];

void handleTelnetIncoming()  {
  exampleString[10] = 0;  // Make sure it's terminated

  bool listing = false;  // If a "list" command is issued, this is set true and then all values will be printed.
  bool commandOK = false;  // This must be set when a command is accepted, otherwise we will output a NACK as well.
  

  if (TCPServer.isBufferEqualTo_P(PSTR("help")))  {  // if "help" is the command entered
    commandOK = true;
    TCPServer._currentClient << F("List of commands:\n")
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
  else if (TCPServer.isBufferEqualTo_P(PSTR("ping")))  {  // if "ping" is entered as the command
    commandOK = true;
    TCPServer._currentClient << F("ACK\r\n");
  } 
  else if (TCPServer.isBufferEqualTo_P(PSTR("list")))  {  // if "list" is entered as the command
    commandOK = true;
    listing = true;
  } 

    // Now to the actual commands:
  if (listing || TCPServer.isNextPartOfBuffer_P(PSTR("ip")))  {  // if "ip" is the first part of the command string entered...
    commandOK = true;
    if (TCPServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
      exampleIPAddr[0] = TCPServer.parseInt();  // This returns the integer value of the next part of the buffer and advances the buffer pointer. 
      TCPServer.isNextPartOfBuffer_P(PSTR("."));  // This assumes the next is "." and advances the pointer.
      exampleIPAddr[1] = TCPServer.parseInt();
      TCPServer.isNextPartOfBuffer_P(PSTR("."));
      exampleIPAddr[2] = TCPServer.parseInt();
      TCPServer.isNextPartOfBuffer_P(PSTR("."));
      exampleIPAddr[3] = TCPServer.parseInt();
      // If the IP address was entered perfectly after the form "X.X.X.X" we should now have it correctly. There is no check for it otherwise except that we return the new value for review:
    }
    // Printing the value in any case:
    TCPServer._currentClient << F("ip=") 
      << exampleIPAddr[0] << F(".")
        << exampleIPAddr[1] << F(".")
          << exampleIPAddr[2] << F(".")
            << exampleIPAddr[3]
              << F("\r\n");
  } 
  
  if (listing || TCPServer.isNextPartOfBuffer_P(PSTR("balance")))  {  // if "ip" is the first part of the command string entered...
    commandOK = true;
    if (TCPServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
      exampleInteger = TCPServer.parseInt();  // This returns the integer value of the next part of the buffer and advances the buffer pointer. 
    }
    // Printing the value in any case:
    TCPServer._currentClient << F("balance=") 
      << exampleInteger
        << F("\r\n");
  } 
  
  if (listing || TCPServer.isNextPartOfBuffer_P(PSTR("light")))  {  // if "ip" is the first part of the command string entered...
    commandOK = true;
    if (TCPServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
      if (TCPServer.isNextPartOfBuffer_P(PSTR("on")))  {
        exampleBoolean = true;
      }
      else if (TCPServer.isNextPartOfBuffer_P(PSTR("off")))  {
        exampleBoolean = false;
      }
      else {
        // Printing the value in any case:
        TCPServer._currentClient << F("Error: Couldn't read the value for \"light\", so didn't change it.") 
          << F("\r\n");
      }
    }
    // Printing the value in any case:
    TCPServer._currentClient << F("light=") 
      << (exampleBoolean ? F("on") : F("off"))
        << F("\r\n");
  } 
  
  if (listing || TCPServer.isNextPartOfBuffer_P(PSTR("name")))  {  // if "ip" is the first part of the command string entered...
    commandOK = true;
    if (TCPServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
      strncpy(exampleString, TCPServer.getRemainingBuffer(),10);
    }
    // Printing the value in any case:
    TCPServer._currentClient << F("name=") 
      << (exampleString)
        << F("\r\n");
  } 
  
  if (!commandOK) {  // NACK + hint is returned if we didn't recognize command
    TCPServer._server << F("NACK (type \"help\" for commands)\r\n");
  }
}



void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);

  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  


  TCPServer.begin();
  TCPServer.setHandleIncomingLine(handleTelnetIncoming);  // Put only the name of the function
  TCPServer.serialOutput(3);


  Serial << F("Open a Telnet connection to 192.168.10.99 on port 8899...\n");  
}

unsigned long lastValue=0;

void loop() {
  TCPServer.runLoop();
  
    // This is a way to broadcast to all attached clients:
  if (lastValue != millis()/10000)  {
    lastValue = millis()/10000;
    TCPServer._server << "SecSinceStart: " << (millis()/1000) << "\n";
  }
}



