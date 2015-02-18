// Works up against the echo script of multiple devices listening on port 8765. IP addresses from 147 and forth
// Notice the trackAddress() feature along with thw W5100.setRetransmission*() features are used because the script must be tolerant to nodes/slaves/servers not answering
// The use of retransmission timeout is necessary because even though we send UDP packets, a timeout condition occurs if there is no device in the other end to receive it. That device doesn't have to respond back with a packet - just be able to receive. I guess over the network there is some type of acknowledgement of packet reception anyway, even though it's UDP
// However, it's not done with just re-transmission time - there is also a data corruption problem which I think is due to a Arduino EthernetUDP library bug: I have experienced that packets sent after trying to work with a non-responsive node will be double size - and obviously fail. 
// Therefore it's integrated in the library that if a node is not answering sufficiently fast, communication with it will cease until a timeout is reached and we will try again.

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

#include <utility/w5100.h>



void setup() { 

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  delay(1000);
  
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  

  W5100.setRetransmissionTime(0xD0);  // Milli seconds
  W5100.setRetransmissionCount(1);


  // Initialize a connection to the switcher:
  messenger.registerReceptionCallbackFunction(UDPmessengerReceivedCommand);
  messenger.begin(ip, 8766);  // Port number to listen on for UDP packets (and sending from!)
  messenger.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!

  // Set up tracking of addresses toward which we will play master and ask for values.
  messenger.trackAddress(147);
  messenger.trackAddress(148);
  messenger.trackAddress(149);
  messenger.trackAddress(150);
  messenger.trackAddress(151);
}


uint16_t lastCounter, counter = 0;
unsigned long thisTime = 0;
uint16_t interactions[5] = {0,0,0,0,0};

void loop() {
  messenger.runLoop();  // Listening, fetching answers etc.

  // Send something for test:
  messenger.resetTXBuffer(); 
  if (messenger.fitCommand(60))  {
    messenger.addValueToDataBuffer(highByte(counter), 0);  // Add values...
    messenger.addValueToDataBuffer(lowByte(counter), 1);  // Add values...
    messenger.addCommand(123, 60);  // Add command and increase TX pointer
  }
  messenger.send(147,8765);  // Sending the same packet to ip 192.168.0.147:8765
  messenger.send(148,8765);  // Sending the same packet to ip 192.168.0.148:8765
  messenger.send(149,8765);  // Sending the same packet to ip 192.168.0.148:8765
  messenger.send(150,8765);  // Sending the same packet to ip 192.168.0.150:8765
  messenger.send(151,8765);  // Sending the same packet to ip 192.168.0.151:8765


    // Prints how many interactions we have per second:
  if (hasTimedOut(thisTime, 1000))  {
    thisTime = millis();
    for(uint8_t i=0; i<5; i++)  {
      Serial << F("Slave 192.168.0.") << (147+i) << F(" responses: ") << interactions[i] << F("\n");
      interactions[i] = 0;
    }
    Serial << (counter-lastCounter) << F("\n\n");
    lastCounter = counter;
  }

    // Increase counter:
  counter++;

  delay(3);  // CRASH IF NO DELAY????!!!! Both Master and Slave seem to be able to crash at too high speeds? 
}

void UDPmessengerReceivedCommand(const uint8_t cmd, const uint8_t from, const uint8_t dataLength, const uint8_t *dataArray)  {
  // This will display the incoming data:
  /*
    Serial << ip[0] << "." << ip[1] << "." << ip[2] << "." << from << F(": CMD=") << _HEXPADL(cmd,2,"0") << F(", DATA=");
   for(uint8_t i=0; i<dataLength; i++)  {
   Serial << _HEXPADL(dataArray[i],2,"0") << (dataLength>i+1?F(","):F(""));
   }
   Serial.println();
   */

  if (from>=147 && from<=151)  {
    interactions[from-147]++;
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




