/*****************
 * Example: Kramer VP-728 Presentation Scaler
 * This will connect to and allow keyboard based control (through the serial monitor) of a Kramer VP-728
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have a TTL Serial to RS-232 adaptor hooked up to your Arduino's Serial1 RX/TX
 * - You must have a Kramer VP-728 connected to this RS-232 connection (Crossed Cable)
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
#include <SkaarhojASCIIClient.h>
#include <SkaarhojSerialClient.h>
#include <ClientKramerVP728.h>
ClientKramerVP728 VP728scaler(Serial1); 







/****
 * Code for a simple serial monitor command reader:
 ****/
#define SER_BUFFER_SIZE 30
char serialBuffer[SER_BUFFER_SIZE];
uint8_t serialBufferPointer = 255;

bool loadSerialCommand()  {  // Call in loop() to check for commands
  if (serialBufferPointer==255)  {  // Initialization
    serialBuffer[0]=0;  // Null-terminate empty buffer
    serialBufferPointer = 0;
  }
  if (Serial.available())  {  // A line of characters has been sent from the serial monitor - no <lf> termination! Just waiting...
    serialBufferPointer=0;  // so, we can start over again filling the buffer
    delay(10);  // Wait for all characters to arrive.
    while(Serial.available())  {
      char c = Serial.read();
      if (serialBufferPointer < SER_BUFFER_SIZE-1)	{	// one byte for null termination reserved
        serialBuffer[serialBufferPointer] = c;
        serialBuffer[serialBufferPointer+1] = 0;
        serialBufferPointer++;
      } 
      else {
        Serial.println(F("ERROR: Buffer overflow."));
      }    
    }
    //Serial << F("SERIAL COMMAND: ") << serialBuffer << F("\n");
    return true;
  }
  return false;
}











void setup() {
  delay(1000);

  // start the serial library:
  Serial.begin(115200);
  Serial << "\n-------------\n";

  VP728scaler.begin();
  VP728scaler.serialOutput(2);
  VP728scaler.connect();

  while(!VP728scaler.hasInitialized())  {
    VP728scaler.runLoop();
  }

  Serial << F("Ready to control VP-728. Type any of these commands + Enter in the serial monitor:\n");

  Serial << F("input=<0-8> : Select input 1-4, UXGA1 (4), UXGA1 (5), HDMI1 (6), HDMI2 (7), USB (8)\n");
  Serial << F("brightness=<0-100> : Brightness of picture\n");
  Serial << F("contrast=<0-100> : Contrast of picture\n");
  Serial << F("color=<0-100> : Saturation of picture\n");
  Serial << F("hue=<0-360> : Hue of picture\n");
  Serial << F("sharpness=<0-100> : Sharpness of picture\n");
  Serial << F("gamma=<0-4> : Gamma of picture\n");
  Serial << F("outputRes=<0-38> : Output resolution (See variables elsewhere)\n");
  Serial << F("aspectRatio=<0-5> : Aspect Ratio\n");
  Serial << F("zoom=<0-11> : Zoom levels\n");
  Serial << F("audioInputVolume=<-22 - 22> : Input audio volume\n");
  Serial << F("audioOutputVolume=<-100 - 24> : Output audio volume\n");
  Serial << F("audioBass=<-36 - 36> : Audio Bass\n");
  Serial << F("audioTreble=<-36 - 36> : Audio Treble\n");
  Serial << F("audioBalance=<-10 - 10> : Audio balance\n");
  Serial << F("audioDelay=<0-340> : Audio delay in ms\n");
  Serial << F("freeze=<0/1> : Freeze picture\n");
  Serial << F("blank=<0/1> : Blank picture\n");
  Serial << F("power=<0/1> : Power of unit\n");
  Serial << F("mute=<0/1> : Mute audio\n");
  Serial << F("lock=<0/1> : Lock panel\n");
  Serial << F("HDCPSetting=<0-4> : HDCP Setting\n");
  Serial << F("list : List current settings\n");
  Serial << F("\n");
}

void loop() {
  VP728scaler.runLoop();



  // Detect commands:
  if (VP728scaler.hasInitialized())  {
    if (loadSerialCommand())  {
      Serial.println(serialBuffer);
      // "list"
      if (!strcmp_P(serialBuffer,PSTR("list")))  {
        Serial << F("Listing:\n");
        Serial << F("\n");

        Serial << F("Input Source: ") << VP728scaler.getInputSource() << F("\n");
        Serial << F("Brightness: ") << VP728scaler.getBrightness() << F("\n");
        Serial << F("Contrast: ") << VP728scaler.getContrast() << F("\n");
        Serial << F("Color: ") << VP728scaler.getColor() << F("\n");
        Serial << F("Hue: ") << VP728scaler.getHue() << F("\n");
        Serial << F("Sharpness: ") << VP728scaler.getSharpness() << F("\n");
        Serial << F("Gamma: ") << VP728scaler.getGamma() << F("\n");
        Serial << F("Output Resolution: ") << VP728scaler.getOutputResolution() << F("\n");
        Serial << F("Aspect Ratio: ") << VP728scaler.getAspectRatioType() << F("\n");
        Serial << F("Zoom Type: ") << VP728scaler.getZoomType() << F("\n");
        Serial << F("Audio Input Volume: ") << VP728scaler.getAudioInputVolume() << F("\n");
        Serial << F("Audio Output Volume: ") << VP728scaler.getAudioOutputVolume() << F("\n");
        Serial << F("Audio Bass: ") << VP728scaler.getAudioBass() << F("\n");
        Serial << F("Audio Treble: ") << VP728scaler.getAudioTreble() << F("\n");
        Serial << F("Audio Balance: ") << VP728scaler.getAudioBalance() << F("\n");
        Serial << F("Audio Delay: ") << VP728scaler.getAudioDelay() << F("\n");
        Serial << F("Freeze: ") << VP728scaler.getFreeze() << F("\n");
        Serial << F("Blank: ") << VP728scaler.getBlank() << F("\n");
        Serial << F("Power: ") << VP728scaler.getPower() << F("\n");
        Serial << F("Mute: ") << VP728scaler.getMute() << F("\n");
        Serial << F("Lock: ") << VP728scaler.getLock() << F("\n");
        Serial << F("Input Status: ") << VP728scaler.getInputStatus() << F("\n");
        Serial << F("HDCP Setting: ") << VP728scaler.getHDCPSetting() << F("\n");
        Serial << F("HDCP support on HDMI 1?: ") << VP728scaler.getHDCPonInputState(1) << F("\n");
        Serial << F("HDCP support on HDMI 2?: ") << VP728scaler.getHDCPonInputState(2) << F("\n");
      } 
      else if (!strncmp_P(serialBuffer,PSTR("input="),6))  {
        Serial << atoi(serialBuffer+6) << "///\n";
        VP728scaler.setInputSource(atoi(serialBuffer+6));
      }
      else if (!strncmp_P(serialBuffer,PSTR("brightness="),11))  {
        VP728scaler.setBrightness(atoi(serialBuffer+11));
      }
      else if (!strncmp_P(serialBuffer,PSTR("contrast="),9))  {
        VP728scaler.setContrast(atoi(serialBuffer+9));
      }
      else if (!strncmp_P(serialBuffer,PSTR("color="),6))  {
        VP728scaler.setColor(atoi(serialBuffer+6));
      }
      else if (!strncmp_P(serialBuffer,PSTR("hue="),4))  {
        VP728scaler.setHue(atoi(serialBuffer+4));
      }
      else if (!strncmp_P(serialBuffer,PSTR("sharpness="),10))  {
        VP728scaler.setSharpness(atoi(serialBuffer+10));
      }
      else if (!strncmp_P(serialBuffer,PSTR("gamma="),6))  {
        VP728scaler.setGamma(atoi(serialBuffer+6));
      }
      else if (!strncmp_P(serialBuffer,PSTR("outputRes="),10))  {
        VP728scaler.setOutputResolution(atoi(serialBuffer+10));
      }
      else if (!strncmp_P(serialBuffer,PSTR("aspectRatio="),12))  {
        VP728scaler.setAspectRatioType(atoi(serialBuffer+12));
//        VP728scaler.setAspectRatioFreely(int hpan,int vpan,int hzoom,int vzoom);
      }
      else if (!strncmp_P(serialBuffer,PSTR("zoom="),5))  {
        VP728scaler.setZoomType(atoi(serialBuffer+5));
//        VP728scaler.setZoomFreely(uint8_t zoomLevel, uint8_t hpan, uint8_t vpan);
      }
      else if (!strncmp_P(serialBuffer,PSTR("audioInputVolume="),17))  {
        VP728scaler.setAudioInputVolume(atoi(serialBuffer+17));
      }
      else if (!strncmp_P(serialBuffer,PSTR("audioOutputVolume="),18))  {
        VP728scaler.setAudioOutputVolume(atoi(serialBuffer+18));
      }
      else if (!strncmp_P(serialBuffer,PSTR("audioBass="),10))  {
        VP728scaler.setAudioBass(atoi(serialBuffer+10));
      }
      else if (!strncmp_P(serialBuffer,PSTR("audioTreble="),12))  {
        VP728scaler.setAudioTreble(atoi(serialBuffer+12));
      }
      else if (!strncmp_P(serialBuffer,PSTR("audioBalance="),13))  {
        VP728scaler.setAudioBalance(atoi(serialBuffer+13));
      }
      else if (!strncmp_P(serialBuffer,PSTR("audioDelay="),11))  {
        VP728scaler.setAudioDelay(atoi(serialBuffer+11));
      }
      else if (!strncmp_P(serialBuffer,PSTR("freeze="),7))  {
        VP728scaler.setFreeze(atoi(serialBuffer+7));
      }
      else if (!strncmp_P(serialBuffer,PSTR("blank="),6))  {
        VP728scaler.setBlank(atoi(serialBuffer+6));
      }
      else if (!strncmp_P(serialBuffer,PSTR("power="),6))  {
        VP728scaler.setPower(atoi(serialBuffer+6));
      }
      else if (!strncmp_P(serialBuffer,PSTR("mute="),5))  {
        VP728scaler.setMute(atoi(serialBuffer+5));
      }
      else if (!strncmp_P(serialBuffer,PSTR("lock="),5))  {
        VP728scaler.setLock(atoi(serialBuffer+5));
      }
      else if (!strncmp_P(serialBuffer,PSTR("HDCPSetting="),12))  {
        VP728scaler.setHDCPSetting(atoi(serialBuffer+12));
      }
    }
  }  
}






