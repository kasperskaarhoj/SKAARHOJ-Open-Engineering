// Works up against the echo script

// Including libraries:
#include <SPI.h>
#include <Ethernet.h>
#include <Streaming.h>


// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = { 
  0x91, 0xA2, 0xDF, 0x0D, 0x6B, 0xB9 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192, 168, 0, 146);              // <= SETUP!  IP address of the Arduino


#include <UDPmessenger.h>
UDPmessenger messenger;




void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  

  // Initialize a connection to the switcher:
  messenger.registerReceptionCallbackFunction(UDPmessengerReceivedCommand);
  messenger.begin(ip, 8766);  // Port number to listen on for UDP packets (and sending from!)
  messenger.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!
}

uint16_t counter = 0;
unsigned long thisTime = 0;
uint16_t lastcounter = 0;

uint16_t receivecounter = 0;

void loop() {
  messenger.runLoop();  // Listening

  // Send something:
  messenger.resetTXBuffer();  
  if (messenger.fitCommand(2))  {
    messenger.addValueToDataBuffer(highByte(counter), 0);  // Add values...
    messenger.addValueToDataBuffer(lowByte(counter), 1);  // Add values...
    messenger.addCommand(123, 2);  // Add command and increase TX pointer
  }
  counter++;
  messenger.send(147,8765);


  // Prints how many interactions we have per second:
  if (hasTimedOut(thisTime, 1000))  {
    thisTime = millis();
    Serial.println(counter-lastcounter);
    lastcounter = counter;
  }

    // With delay(0) I have seen 2400+ messages sent a second, delay(1) is 710 messages/sec, delay(2) is 410 messages/sec... (Arduino Mega 2580)
  delay(1);  // CRASH IF NO DELAY????!!!! Both Master and Slave seem to be able to crash at too high speeds? 
}

void UDPmessengerReceivedCommand(const uint8_t cmd, const uint8_t from, const uint8_t dataLength, const uint8_t *dataArray)  {
  // This will display the incoming data:
/*  Serial << ip[0] << "." << ip[1] << "." << ip[2] << "." << from << F(": CMD=") << _HEXPADL(cmd,2,"0") << F(", DATA=");
  for(uint8_t i=0; i<dataLength; i++)  {
    Serial << _HEXPADL(dataArray[i],2,"0") << (dataLength>i+1?F(","):F(""));
  }
  Serial.println();
*/

  if (receivecounter+1 != ((dataArray[0]<<8)|dataArray[1]) )  {
  //  Serial.println("Error in order (try increase delay()! Depends on slaves ability to respond back.)");
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



