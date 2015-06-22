/*****************
 * Example: Basic TCP Telnet server allowing you control of an ATEM switcher via a console on port 8899
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
#include <Streaming.h>

// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9
};      // <= SETUP!  MAC address of the Arduino
IPAddress clientIp(192, 168, 10, 99);        // <= SETUP!  IP address of the Arduino
IPAddress switcherIp(192, 168, 10, 240);     // <= SETUP!  IP address of the ATEM Switcher

// Include ATEMbase library and make an instance:
// The port number is chosen randomly among high numbers.
#include <ATEMbase.h>
#include <ATEMext.h>
ATEMext AtemSwitcher;




#include <SkaarhojBufferTools.h>
#include <SkaarhojTCPServer.h>
SkaarhojTCPServer TCPServer(8899);  // Port 8899 used




/**
 * This function is a call back functions which is called when the server receives a text line over TCP.
 * This is where we convert instructions via Telnet to ATEM commands
 */
void handleTelnetIncoming()  {
  bool listing = false;  // If a "list" command is issued, this is set true and then all values will be printed.
  bool commandOK = false;  // This must be set when a command is accepted, otherwise we will output a NACK as well.

  if (TCPServer.isBufferEqualTo_P(PSTR("help")))  {  // if "help" is the command entered
    commandOK = true;
    TCPServer._currentClient << F("List of commands:\n")
                             << F("- help : This message\n")
                             << F("- pgm : Get ME1 Program source\n")
                             << F("- pgm=<num> : Set ME1 Program source\n")
                             << F("- prv : Get ME1 Preview source\n")
                             << F("- prv=<num> : Set ME1 Preview source\n")
                             << F("- aux<ch> : Get AUX source for AUX <ch> (AUX1 = 0, AUX2= 1, etc)\n")
                             << F("- aux<ch>=<num> : Set AUX source for AUX <ch> (AUX1 = 0, AUX2= 1, etc)\n")
                             << F("- cut : Do a cut\n")
                             << F("- auto : Do an auto transition\n")
                             << F("- ping : Checking connection\n")
                             << F("- list : Show all values\n")
                             << F("For all video source numbers <num>, see bottom of page http://skaarhoj.com/fileadmin/BMDPROTOCOL.html\n")
                             << F("\n");

  }
  else if (TCPServer.isBufferEqualTo_P(PSTR("ping")))  {  // if "ping" is entered as the command
    commandOK = true;
    TCPServer._currentClient << F("ACK\r\n");
  }
  else if (TCPServer.isBufferEqualTo_P(PSTR("list")))  {  // if "list" is entered as the command
    commandOK = true;
    listing = true;
  }
  else if (TCPServer.isBufferEqualTo_P(PSTR("cut")))  {  // if "cut" is entered as the command
    commandOK = true;
    AtemSwitcher.performCutME(0);
    TCPServer._currentClient << F("ACK\r\n");
  }
  else if (TCPServer.isBufferEqualTo_P(PSTR("auto")))  {  // if "auto" is entered as the command
    commandOK = true;
    AtemSwitcher.performAutoME(0);
    TCPServer._currentClient << F("ACK\r\n");
  }

  if (listing || TCPServer.isNextPartOfBuffer_P(PSTR("pgm")))  {  // if "pgm" is the first part of the command string entered...
    commandOK = true;
    if (TCPServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
        uint16_t input = TCPServer.parseInt();  // This returns the integer value of the next part of the buffer and advances the buffer pointer.
        AtemSwitcher.setProgramInputVideoSource(0,input);
        AtemSwitcher.runLoop(50);
    }
    // Printing the value in any case:
    TCPServer._currentClient << F("pgm=")
                             << AtemSwitcher.getProgramInputVideoSource(0)
                             << F("\r\n");
  }

  if (listing || TCPServer.isNextPartOfBuffer_P(PSTR("prv")))  {  // if "prv" is the first part of the command string entered...
    commandOK = true;
    if (TCPServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
      uint16_t input = TCPServer.parseInt();  // This returns the integer value of the next part of the buffer and advances the buffer pointer.
        AtemSwitcher.setPreviewInputVideoSource(0,input);
        AtemSwitcher.runLoop(50);
    }
    // Printing the value in any case:
    TCPServer._currentClient << F("prv=")
                             << AtemSwitcher.getPreviewInputVideoSource(0)
                             << F("\r\n");
  }

  if (TCPServer.isNextPartOfBuffer_P(PSTR("aux")))  {  // if "aux" is the first part of the command string entered...
    uint8_t auxChannel = TCPServer.parseInt();
    if (auxChannel>0 && auxChannel<=AtemSwitcher.getTopologyAUXbusses())  {
      commandOK = true;
      if (TCPServer.isNextPartOfBuffer_P(PSTR("=")))  {  // if "=" comes right after the command, we will be setting the value...
          uint16_t input = TCPServer.parseInt();  // This returns the integer value of the next part of the buffer and advances the buffer pointer.
          AtemSwitcher.setAuxSourceInput(auxChannel-1,input);
          AtemSwitcher.runLoop(50);
      }
      // Printing the value in any case:
      TCPServer._currentClient << F("aux") << auxChannel << F("=")
                               << AtemSwitcher.getAuxSourceInput(auxChannel-1)
                               << F("\r\n");
    }
  }
  if (listing)  {
     for(uint8_t auxChannel=1; auxChannel<=AtemSwitcher.getTopologyAUXbusses();auxChannel++)  {
        // Printing the value in any case:
        TCPServer._currentClient << F("aux") << (auxChannel) << F("=")
                               << AtemSwitcher.getAuxSourceInput(auxChannel-1)
                               << F("\r\n");
     }
  }

  if (!commandOK) {  // NACK + hint is returned if we didn't recognize command
    TCPServer._server << F("NACK (type \"help\" for commands)\r\n");
  }
}





void setup() {

  randomSeed(analogRead(5));  // For random port selection

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac, clientIp);
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  // Initialize a connection to the switcher:
  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(2);
  AtemSwitcher.connect();


  TCPServer.begin();
  TCPServer.setHandleIncomingLine(handleTelnetIncoming);  // Put only the name of the function
  TCPServer.serialOutput(3);


  Serial << F("Open a Telnet connection to 192.168.10.99 on port 8899...\n");
}

void loop() {
  AtemSwitcher.runLoop();
  TCPServer.runLoop();
}


