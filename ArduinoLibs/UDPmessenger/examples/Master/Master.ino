// Listen in like this on a Mac:  nc -u -l 9910 | ./hexdumpone.phpsh
// Send on a Mac like this: echo -n '00 08 04 AA BB CC 02 CC' | xxd -r -p | nc -u -w 1 192.168.0.145 8765

// Including libraries:
#include <SPI.h>
#include <Ethernet.h>
#include <hex.h>
//#include <MemoryFree.h>


// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = { 
  0x90, 0xA2, 0xDF, 0x0D, 0x6B, 0xB9 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192, 168, 0, 146);              // <= SETUP!  IP address of the Arduino


#include <UDPmessenger.h>
UDPmessenger messenger;


// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}



void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  

  // Initialize a connection to the switcher:
  messenger.registerReceptionCallbackFunction(UDPmessengerReceivedCommand);
  messenger.begin(ip, 8766);  // Port number to listen on for UDP packets
  messenger.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!
}

uint16_t counter = 0;
unsigned long thisTime = 0;
uint16_t lastcounter = 0;

uint16_t receivecounter = 0;

void loop() {
  messenger.runLoop();  // Listening
  
  // Send something:
  messenger.clearDataArray();  
  messenger.addValueToDataBuffer(highByte(counter), 0);
  messenger.addValueToDataBuffer(lowByte(counter), 1);
  messenger.addCommand(123, 2);
  messenger.send(145,8765);
  counter++;
  
    
  if (hasTimedOut(thisTime, 1000))  {
    thisTime = millis();
    Serial.println(counter-lastcounter);
    lastcounter = counter;
  }
  
  delay(1);  // CRASH IF NO DELAY????!!!!
}

void UDPmessengerReceivedCommand(const uint8_t cmd, const uint8_t from, const uint8_t dataLength, const uint8_t *dataArray)  {
        // This will display the incoming data:
/*      Serial << ip[0] << "." << ip[1] << "." << ip[2] << "." << from << ": CMD=" << cmd << ", DATA=";
      for(uint8_t i=0; i<dataLength; i++)  {
        Serial.print(dataArray[i], DEC);
        Serial.print(",");
      }
    
      Serial.print((dataArray[0]<<8)|dataArray[1], DEC);
      Serial.println();
  */    
      
      if (receivecounter+1 != ((dataArray[0]<<8)|dataArray[1]) )  {
        Serial.println("Error in order");
      }
      receivecounter = (dataArray[0]<<8)|dataArray[1];
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


