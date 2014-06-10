/*****************
 * Example: Basic ASCII TCP Telnet client, relaying input from the serial monitor
 * This sketch makes the Arduino into a telnet client (connects to a server somewhere) which forwards input from the serial monitor to the connected telnet server.
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
#include <utility/w5100.h>


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
#include <SkaarhojASCIIClient.h>
#include <SkaarhojTCPClient.h>
SkaarhojTCPClient telnetClient;



#include <SkaarhojSerialServer.h>
SkaarhojSerialServer SerialServer(Serial);



/**
 * This function is a call back functions which is called when the server receives a text line over TCP.
 * This is where your local implementation details has to be processed!
 */

void handleTelnetIncoming()  {
  telnetClient.setInitialized();
  Serial << telnetClient.getRemainingBuffer() << "\n";
}
void handleSerialIncoming()  {
  telnetClient._client << SerialServer.getRemainingBuffer() << "\n";
}



void setup() { 

  delay(2000);
   
  // start the Ethernet connection:
  Ethernet.begin(mac,ip);

  // start the serial library:
  SerialServer.begin(115200);
  SerialServer.setHandleIncomingLine(handleSerialIncoming);  // Put only the name of the function
  SerialServer.serialOutput(3);
  SerialServer.enableEOLTimeout();  // Calling this without parameters sets EOL Timeout to 2ms which is enough for 9600 baud and up. Use only if the serial client (like Arduinos serial monitor) doesn't by itself send a <cr> token.
  Serial << F("Type stuff in the serial monitor to send over telnet...\n");  

  
  // Setting short timeout on IP connections:
  // May result in "connection failed" messages, in particular on the first connection attempt
  W5100.setRetransmissionTime(3000);  // Milli seconds. NOTICE: Experiments show that this value cannot be set much lower, otherwise a connection will be impossible.
  W5100.setRetransmissionCount(2);
  
  delay(1000);
  
  Serial.println("connecting...");
  telnetClient.begin(IPAddress(192, 168, 10, 123), 9992);  // Telnet server expected on 192.168.10.123 port 9992 (do so with "nc -l 9992" in terminal on a Mac/Linux)
  telnetClient.serialOutput(3);
  telnetClient.setHandleIncomingLine(handleTelnetIncoming);
  telnetClient.disablePing();
  telnetClient.disableStatus();
  telnetClient.connect();
}

void loop() {
  SerialServer.runLoop();
  telnetClient.runLoop();
}

