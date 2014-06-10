/*****************
 * Example: Hyperdeck Tester
 * This will connect to a Hyperdeck Studio and perform various tests, including recording and playback
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have a BlackMagic Design Hyperdeck Studio (Pro) connected to the same network as the Arduino
 * - You must make specific setups in the below lines where the comment "// SETUP" is found!
 */


/*
	IMPORTANT: If you want to use this library in your own projects and/or products,
	please play a fair game and heed the license rules! See our web page for a Q&A so
	you can keep a clear conscience: http://skaarhoj.com/about/licenses/
*/


#include <SPI.h>
#include <Ethernet.h>
#include <utility/w5100.h>


// MAC address and IP address for this *particular* Ethernet Shield!
// MAC address is printed on the shield
// IP address is an available address you choose on your subnet where the Hyperdeck Studio is also present:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xE8, 0xE9 };		// <= SETUP
IPAddress ip(192, 168, 10, 99);				// <= SETUP (Arduino IP)


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
#include <ClientBMDHyperdeckStudio.h>

// Connect to a Hyperdeck Studio recorder with this object
ClientBMDHyperdeckStudio hyperDeck; 


void setup() {

  // start the Ethernet connection:
  Ethernet.begin(mac, ip);

  // start the serial library:
  Serial.begin(115200);
  Serial << "\n-------------\n";

  // Setting short timeout on IP connections: (these settings worked well:)
  W5100.setRetransmissionTime(2000);  // Milli seconds
  W5100.setRetransmissionCount(2);

  // give the Ethernet shield a second to initialize:
  delay(1000);

  // Start Hyperdeck connection:
  hyperDeck.begin(IPAddress(192, 168, 10, 230));	 // <= SETUP (the IP address of the Hyperdeck Studio)
  hyperDeck.serialOutput(1);  // 1= normal, 2= medium verbose, 3=Super verbose
  hyperDeck.connect();  // For some reason the first connection attempt seems to fail, but in the runloop it will try to reconnect.

  while (!hyperDeck.hasInitialized())  {
    hyperDeck.runLoop();
  }

  // HyperDeck has initialized here:
  Serial << F("So, we are connected!\n");
  hyperDeck.runLoop(3000);

  Serial << F("[Type a letter and press Enter in the serial monitor to continue..]\n");
  while(!waitForSerialInput())  { 
    hyperDeck.runLoop(); 
  }





  // DISC SLOTS and file format:
  Serial << F("\nDISC SLOTS\n");
  for(uint8_t a=0; a<2; a++)  {
    Serial << F("Check if there is a disc in slot ") << (a+1) << F(":\n");
    if (hyperDeck.getSlotStatus(a+1)==ClientBMDHyperdeckStudio_SLOTSTATUS_MOUNTED)  {
      Serial << F("Yes, a disc is there and it has ") << hyperDeck.getSlotRecordingTime(a+1) << " seconds of recording time for us\n";
    } 
    else {
      Serial << F("No disc ready\n");
    }
  }
  Serial << F("Currently, slot ") << hyperDeck.getCurrentSlotId() << F(" will be used.\n");
  Serial << F("The file format used for recording is ");

  switch(hyperDeck.getFileFormat())  {
  case 1:
    Serial << F("QuickTime Uncompressed");
    break; 
  case 2:
    Serial << F("QuickTime ProRes HQ");
    break; 
  case 6:
    Serial << F("DNxHD220");
    break; 
  case 7:
    Serial << F("QuickTime DNxHD220");
    break; 
  default:
    Serial << F("unknown! (Probably ProRes, ProResLT or ProRes Proxy)");
    break;
  }
  Serial << F("\n");
  hyperDeck.runLoop(1000);  // 1000 seconds of delay, exercising the hyperdeck connection.

  // RECORDING
  Serial << F("\nRECORDING:\n");
  Serial << F("The video input source is currently set to ");
  switch(hyperDeck.getVideoInput())  {
  case ClientBMDHyperdeckStudio_VIDEOINPUT_SDI:
    Serial << F("SDI");
    break; 
  case ClientBMDHyperdeckStudio_VIDEOINPUT_HDMI:
    Serial << F("HDMI");
    break; 
  case ClientBMDHyperdeckStudio_VIDEOINPUT_COMPONENT:
    Serial << F("Component");
    break; 
  }
  Serial << F("\n");
  Serial << F("Will try to record something for 10 seconds...\n");

  Serial << F("[Type a letter to continue...]\n");
  while(!waitForSerialInput())  { 
    hyperDeck.runLoop(); 
  }

  hyperDeck.recordWithName("Testclip");

  unsigned long timesnap = millis();
  while(!hyperDeck.isRecording() && !hyperDeck.hasTimedOut(timesnap,1000))  {  // Waiting for hyperdeck to report back that it is recording. Give it 1 second.
    hyperDeck.runLoop();
  }
  if (!hyperDeck.isRecording())  {  // If hyperdeck is not recording yet, 
    Serial << F("The hyperdeck decided NOT to record anything, why?\n");
    Serial << F(" - No Input? ") << hyperDeck.getInputStatus() << F("\n");
  } 
  else {
    // Let it record for 10 seconds:
    while(!hyperDeck.hasTimedOut(timesnap,10000))  {
      hyperDeck.runLoop();
    }
    hyperDeck.stop();  // ... and stop

      // Notice, to check for a stopped recording, we are looking for the "preview" state (which is where we see the input source).
    timesnap = millis();  
    while(!hyperDeck.isInPreview() && !hyperDeck.hasTimedOut(timesnap,1000))  {  // Waiting for hyperdeck to report back that it has stopped. Give it 1 second.
      hyperDeck.runLoop();
    }
    if (!hyperDeck.isInPreview())  {
      Serial << F("The hyperdeck decided NOT to stop recording, why?\n");
      Serial << F(" - Transport Status: ") << hyperDeck.getTransportStatus() << F("\n");
    } 
    else {
      Serial << F("Hyperdeck stops in Preview Mode - you can see the input signal on the HyperDeck display.\n");
      Serial << F("We will now disable preview mode, which will load up the last recorded clip, ready to play\n");
      
      Serial << F("[Type a letter to continue...]\n");
      while(!waitForSerialInput())  { 
        hyperDeck.runLoop(); 
      }
      hyperDeck.previewEnable(false);
      timesnap = millis();  
      while(!hyperDeck.isStopped() && !hyperDeck.hasTimedOut(timesnap,1000))  {  // Waiting for hyperdeck to report back that it has stopped. Give it 1 second.
        hyperDeck.runLoop();
      }
      
      Serial << F("The clip just recorded has id ") << hyperDeck.getClipId() << F("\n");
      Serial << F("Now going to start of clip and playing it back...\n");

      Serial << F("[Type a letter to continue...]\n");
      while(!waitForSerialInput())  { 
        hyperDeck.runLoop(); 
      }

      hyperDeck.gotoClipStart();
      hyperDeck.play();

      timesnap = millis();  
      while(!hyperDeck.isPlaying() && !hyperDeck.hasTimedOut(timesnap,1000))  {  // Waiting for hyperdeck to report back that it it playing. Give it 1 second.
        hyperDeck.runLoop();
      }
      if (!hyperDeck.isPlaying())  {
        Serial << F("The hyperdeck decided NOT to play the clip, why?\n");
      } 
      else {
        while(!hyperDeck.hasTimedOut(timesnap,5000))  {
          hyperDeck.runLoop();
        }

        hyperDeck.stop();
        Serial << F("The HyperDeck should have stopped now\n");
        Serial << F("Going to end of clip and playing backwards in slow motion.\n");

        Serial << F("[Type a letter to continue...]\n");
        while(!waitForSerialInput())  { 
          hyperDeck.runLoop(); 
        }

        hyperDeck.gotoClipEnd();
        hyperDeck.playWithSpeed(-50);

        hyperDeck.runLoop(1000);  // Delay
        Serial << F("Playback speed: ") << hyperDeck.getPlaySpeed() << F("\n");
        hyperDeck.runLoop(4000);  // Delay
        Serial << F("Stop.\n");
        hyperDeck.stop();


        Serial << F("Going to 5 seconds before the end of clip and playing forward in slow motion.\n");

        Serial << F("[Type a letter to continue...]\n");
        while(!waitForSerialInput())  { 
          hyperDeck.runLoop(); 
        }

        hyperDeck.gotoClipEnd();
        hyperDeck.runLoop(1000); 
        hyperDeck.goBackwardsInTimecode(00,00,05,00);
        hyperDeck.runLoop(1000); 
        hyperDeck.playWithSpeed(50);
        
        while(hyperDeck.isPlaying())  {
          hyperDeck.runLoop(0); 
        }
        Serial << F("Done with playback test.\n");
        

      }
    }
  }





  // Wrapping up by disconnecting, connecting again and disconnecting finally:
  Serial << F("\nDISCONNECT TEST:\n");
  hyperDeck.disconnect();
  hyperDeck.runLoop(1000);

  Serial << F("\nReconnect test (via timeout in runloop()):\n");
  hyperDeck.runLoop(10000);  // 1000 seconds of delay, exercising the hyperdeck connection.

  Serial << F("\nDisconnnecting again.\n");
  hyperDeck.disconnect();


  Serial << F("\nDONE!\n");
}

void loop() {
}

bool waitForSerialInput()  {
  // Wait for keypress:
  if (Serial.available())  {

    // Flush:
    while (Serial.available() > 0) {
      Serial.read();
    }
    return true;
  }
  else{ 
    return false;
  }
}




