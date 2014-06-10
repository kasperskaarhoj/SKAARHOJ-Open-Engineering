/*****************
 * Example: ATEM Monitor Quad!
 * Connects to four Atem Switcher and outputs changes to Preview and Program on the Serial monitor (at 115200 baud)
 * It has been confirmed to work with four switchers simultaneously; an ATEM 2M/E, ATEM 1M/E, ATEM Prod. Studio 4K and ATEM TeleVision Studio
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an Atem Switcher connected to the same network as the Arduino - and you should have it working with the desktop software
 * - You must make specific set ups in the below lines where the comment "// SETUP" is found!
 */



// Including libraries: 
#include <SPI.h>
#include <Ethernet.h>
#include <utility/w5100.h>

#include <MemoryFree.h>


// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192, 168, 10, 101);              // <= SETUP!  IP address of the Arduino


// Include ATEM library and make an instance:
// Connect to an ATEM switcher on this address and using this local port:
// The port number is chosen randomly among high numbers.
#include <ATEM.h>
ATEM AtemSwitcher1(IPAddress(192, 168, 10, 240), 56419);  // <= SETUP (the IP address of the ATEM switcher)
ATEM AtemSwitcher2(IPAddress(192, 168, 10, 241), 56420);  // <= SETUP (the IP address of the ATEM switcher)
ATEM AtemSwitcher3(IPAddress(192, 168, 10, 242), 56421);  // <= SETUP (the IP address of the ATEM switcher)
ATEM AtemSwitcher4(IPAddress(192, 168, 10, 243), 56422);  // <= SETUP (the IP address of the ATEM switcher)



// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}



void setup() { 

  delay(1000);

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  

  // It's absolutely necessary to define a low retransmission time when working with atem connections. 
  // Otherwise the arduino may hang up in various network related issues which can break the connection to working atem switchers.
  // In this sketch for example, if one switcher is not present and this short retransmission time was not set, the connection to the other two would quickly be lost.
  W5100.setRetransmissionTime(200);  // Milli seconds
  W5100.setRetransmissionCount(1);


  // Initialize a connection to the switcher:
  AtemSwitcher1.serialOutput(0);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher1.connect();

  AtemSwitcher2.serialOutput(0);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher2.connect();

  AtemSwitcher3.serialOutput(0);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher3.connect();

  AtemSwitcher4.serialOutput(0);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher4.connect();

  // Shows free memory:  
  //  Serial << F("freeMemory()=") << freeMemory() << "\n";
}

bool AtemOnline1 = false;
bool AtemOnline2 = false;
bool AtemOnline3 = false;
bool AtemOnline4 = false;

void loop() {
  runLoopCount();

  // Check for packets, respond to them etc. Keeping the connection alive!
  // VERY important that this function is called all the time - otherwise connection might be lost because packets from the switcher is
  // overlooked and not responded to.
  AtemSwitcher1.runLoop();
  AtemSwitcher2.runLoop();
  AtemSwitcher3.runLoop();
  AtemSwitcher4.runLoop();

  // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher1.isConnectionTimedOut())  {
    Serial << F("Connection to FIRST ATEM Switcher has timed out - reconnecting!\n");
    AtemSwitcher1.connect();
    AtemOnline1 = false;
  }
  if (AtemSwitcher1.hasInitialized())  {
    if (!AtemOnline1)  {
      AtemOnline1 = true;
    //  AtemSwitcher1.sendAudioLevelNumbers(true);  // To push it even more, send audio levels!
    }
  }

  // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher2.isConnectionTimedOut())  {
    Serial << F("Connection to SECOND ATEM Switcher has timed out - reconnecting!\n");
    AtemSwitcher2.connect();
  }  
  if (AtemSwitcher2.hasInitialized())  {
    if (!AtemOnline2)  {
      AtemOnline2 = true;
   //   AtemSwitcher2.sendAudioLevelNumbers(true);  // To push it even more, send audio levels!
    }
  }

  // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher3.isConnectionTimedOut())  {
    Serial << F("Connection to THIRD ATEM Switcher has timed out - reconnecting!\n");
    AtemSwitcher3.connect();
  }  
  if (AtemSwitcher3.hasInitialized())  {
    if (!AtemOnline3)  {
      AtemOnline3 = true;
 //     AtemSwitcher3.sendAudioLevelNumbers(true);  // To push it even more, send audio levels!
    }
  }

  // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher4.isConnectionTimedOut())  {
    Serial << F("Connection to FORTH ATEM Switcher has timed out - reconnecting!\n");
    AtemSwitcher4.connect();
  }  
  if (AtemSwitcher4.hasInitialized())  {
    if (!AtemOnline4)  {
      AtemOnline4 = true;
 //     AtemSwitcher4.sendAudioLevelNumbers(true);  // To push it even more, send audio levels!
    }
  }
}






/*****************
 *
 * Utility functions
 *
 *****************/

/**
 * A way we measure how many times the run-loop has been passed over a second: (debugging)
 * [Called from loop()]
 */
void runLoopCount()  {
  static unsigned long runLoopCountTime = 0;
  static unsigned int runLoopCounter = 0;

  runLoopCounter++; 
  if (hasTimedOut(runLoopCountTime, 1000)) {
    unsigned long timePassed = millis()-runLoopCountTime;
    if (timePassed!=0)  {
      Serial << F("RunLoopCount: ") << (((float)runLoopCounter)/timePassed*1000) << " (" << runLoopCounter << " in " << timePassed << "ms)" << F(" freeMemory()=") << freeMemory() << "\n";
      runLoopCountTime = millis();
      runLoopCounter = 0; 
    }
  } 
}

/**
 * Timer-funtion. Input a timestamp (ms) and a timeout time (ms) and if the current time is "timeout time" longer than the input timestamp it returns true.
 */
bool hasTimedOut(unsigned long time, unsigned long timeout)  {
  if ((unsigned long)(time + timeout) < (unsigned long)millis())  {  // This should "wrap around" if time+timout is larger than the size of unsigned-longs, right?
    return true; 
  } 
  else {
    return false;
  }
}

