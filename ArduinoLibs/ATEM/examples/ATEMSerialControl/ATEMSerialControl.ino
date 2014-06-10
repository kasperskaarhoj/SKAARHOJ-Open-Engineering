/*****************
 * Example: Basic Serial server allowing you control of an ATEM switcher via the serial monitor
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an Atem Switcher connected to the same network as the Arduino - and you should have it working with the desktop software
 * - You must make specific set ups in the below lines where the comment "// SETUP" is found!
 */

/*
	IMPORTANT: If you want to use this library in your own projects and/or products,
 	please play a fair game and heed the license rules! See our web page for a Q&A so
 	you can keep a clear conscience: http://skaarhoj.com/about/licenses/
 */


#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>


// MAC address and IP address for this *particular* Ethernet Shield!
// MAC address is printed on the shield
// IP address is an available address you choose on your subnet where the switcher is also present:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xE8, 0xE9 };		// <= SETUP
IPAddress ip(192, 168, 10, 99);				// <= SETUP


// Include ATEM library and make an instance:
#include <ATEM.h>

// Connect to an ATEM switcher on this address and using this local port:
// The port number is chosen randomly among high numbers.
ATEM AtemSwitcher(IPAddress(192, 168, 10, 240), 56417);  // <= SETUP (the IP address of the ATEM switcher)


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
SkaarhojSerialServer SerialServer(Serial);  // Regular Serial object used.





/**
 * This function is a call back functions which is called when the server receives a text line over TCP.
 * This is where we convert instructions via Serial to ATEM commands
 */
void handleSerialIncoming()  {
  bool listing = false;  // If a "list" command is issued, this is set true and then all values will be printed.
  bool commandOK = false;  // This must be set when a command is accepted, otherwise we will output a NACK as well.

  if (SerialServer.isBufferEqualTo_P(PSTR("help")))  {  // if "help" is the command entered
    commandOK = true;
    SerialServer._HardSerial << F("List of commands:\n")
      << F("- help : This message\n")
        << F("- pgm : Get Program source\n")
          << F("- pgm=<num> : Set Program source (1-16 for inputs, 0 is black, \"color1\", \"color2\", \"media1\", \"media2\")\n")
            << F("- prv : Get Preview source\n")
              << F("- prv=<num> : Set Preview source (1-16 for inputs, 0 is black)\n")
                << F("- upstreamKeyer1 : Get status of upstream keyer 1\n")
                  << F("- upstreamKeyer1=<on/off> : Enable/Disable upstream keyer 1\n")
                    << F("- cut : Do a cut\n")
                      << F("- auto : Do an auto transition\n")
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
  else if (SerialServer.isBufferEqualTo_P(PSTR("cut")))  {  // if "cut" is entered as the command
    commandOK = true;
    AtemSwitcher.doCut();
    SerialServer._HardSerial << F("ACK\r\n");
  }
  else if (SerialServer.isBufferEqualTo_P(PSTR("auto")))  {  // if "auto" is entered as the command
    commandOK = true;
    AtemSwitcher.doAuto();
    SerialServer._HardSerial << F("ACK\r\n");
  }

  if (listing || SerialServer.isNextPartOfBuffer_P(PSTR("pgm")))  {  // if "pgm" is the first part of the command string entered...
    commandOK = true;
    if (SerialServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
      if (SerialServer.isNextPartOfBuffer_P(PSTR("color1")))  {
        AtemSwitcher.changeProgramInput(2001);
        AtemSwitcher.delay(50);
      }
      else if (SerialServer.isNextPartOfBuffer_P(PSTR("color2")))  {
        AtemSwitcher.changeProgramInput(2002);
        AtemSwitcher.delay(50);
      }
      else if (SerialServer.isNextPartOfBuffer_P(PSTR("media1")))  {
        AtemSwitcher.changeProgramInput(3010);
        AtemSwitcher.delay(50);
      }
      else if (SerialServer.isNextPartOfBuffer_P(PSTR("media2")))  {
        AtemSwitcher.changeProgramInput(3020);
        AtemSwitcher.delay(50);
      }
      else {
        uint8_t input = SerialServer.parseInt();  // This returns the integer value of the next part of the buffer and advances the buffer pointer. 
        if (input<=16)  {
          AtemSwitcher.changeProgramInput(input);
          AtemSwitcher.delay(50);
        }
      }
    }
    // Printing the value in any case:
    SerialServer._HardSerial << F("pgm=") 
      << AtemSwitcher.getProgramInput()
        << F("\r\n");
  } 

  if (listing || SerialServer.isNextPartOfBuffer_P(PSTR("prv")))  {  // if "prv" is the first part of the command string entered...
    commandOK = true;
    if (SerialServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
      uint8_t input = SerialServer.parseInt();  // This returns the integer value of the next part of the buffer and advances the buffer pointer. 
      if (input<=16)  {
        AtemSwitcher.changePreviewInput(input);
        AtemSwitcher.delay(50);
      }
    }
    // Printing the value in any case:
    SerialServer._HardSerial << F("prv=") 
      << AtemSwitcher.getPreviewInput()
        << F("\r\n");
  } 

  if (listing || SerialServer.isNextPartOfBuffer_P(PSTR("upstreamKeyer1")))  {  // if "upstreamKeyer1" is the first part of the command string entered...
    commandOK = true;
    if (SerialServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
      if (SerialServer.isNextPartOfBuffer_P(PSTR("on")))  {
        AtemSwitcher.changeUpstreamKeyOn(1, true);
        AtemSwitcher.delay(50);
      }
      else if (SerialServer.isNextPartOfBuffer_P(PSTR("off")))  {
        AtemSwitcher.changeUpstreamKeyOn(1, false);
        AtemSwitcher.delay(50);
      }
      else {
        // Printing the value in any case:
        SerialServer._HardSerial << F("Error: Couldn't read the value for \"upstreamKeyer1\", so didn't change it.") 
          << F("\r\n");
      }
    }
    // Printing the value in any case:
    SerialServer._HardSerial << F("upstreamKeyer1=") 
      << (AtemSwitcher.getUpstreamKeyerStatus(1) ? F("on") : F("off"))
        << F("\r\n");
  } 

  if (!commandOK) {  // NACK + hint is returned if we didn't recognize command
    SerialServer._HardSerial << F("NACK (type \"help\" for commands)\r\n");
  }
}





void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);



  // Initialize a connection to the switcher:
  AtemSwitcher.serialOutput(true);
  AtemSwitcher.connect();


  SerialServer.begin(115200);
  SerialServer.setHandleIncomingLine(handleSerialIncoming);  // Put only the name of the function
  SerialServer.serialOutput(0);
  SerialServer.enableEOLTimeout();  // Because we take data in from Arduino Serial monitor that doesn't terminate with a <cr>

  Serial << F("\n- - - - - - - -\nSerial Started (by SerialServer, in this example it's using Serial object)\n");  
  Serial << F("Type stuff in the serial monitor...\n");  
}

void loop() {
  AtemSwitcher.runLoop();
  SerialServer.runLoop();
}



