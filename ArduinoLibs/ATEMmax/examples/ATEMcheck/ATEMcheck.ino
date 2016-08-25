/*****************
 * Checking ATEM API
 * Connects to the Atem Switcher and goes through a series of tests of the switchers features.
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an Atem Switcher connected to the same network as the Arduino - and you should have it working with the desktop software
 * - You must make specific set ups in the below lines where the comment "// SETUP" is found!
 */




#define TEST_AUX 1
#define TEST_MESEC 1
#define TEST_COLORGEN 1
#define TEST_SUPERSOURCE 1
#define TEST_MP 1
#define TEST_DSK 1
#define TEST_FLY 1
#define TEST_AUDIO 1
#define TEST_AUDIO_LEVELS 1
#define TEST_CCU 1
#define TEST_TALLY 1
#define TEST_MACROS 1
#define TEST_SETTINGS 1



/*
 PREPARE:

 - Set 720p50 mode
 - Upload 50 frames or more to each clip bank + audio
 - Set up audio on external inputs?

 */



// Including libraries:
#include <SPI.h>
#include <Ethernet.h>
#include <Streaming.h>
#include <MemoryFree.h>
#include <SkaarhojPgmspace.h>


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
#include <ATEMmax.h>
ATEMmax AtemSwitcher;

uint16_t delayBetweenCmds = 500;    // Delay in ms between each command test
uint16_t delayBetweenValues = 100;    // Delay in ms between each submission of values in a command test. Down to 30 ms is OK
uint16_t timeOutDelay = 2000;      // Delay in which to time out, default around 1000.


void setup() {
  delay(2000);
  randomSeed(analogRead(5));  // For random port selection

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac, clientIp);
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  // Shows free memory:
  Serial << F("freeMemory()=") << freeMemory() << "\n";

  Serial << F("----------------\n");
  Serial << F("Serial Commands:\n");
  Serial << F("+: Fast Speed\n");
  Serial << F("-: Slow Speed\n");
  Serial << F("n: Normal Speed\n");
  Serial << F("p: Paused until new command\n");
  Serial << F("----------------\n");

  delay(2000);

  // Initialize a connection to the switcher:
  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(2);
  AtemSwitcher.connect();

}


/*
 * Utility function, printing a line
 */
void printLine()  {
  Serial << "-------------------------\n";
}

/*
 * Utility function, printing audio levels and peak
 */
void printAudioLevels(char * label, long L, long R, long LP, long RP)  {
  int l = AtemSwitcher.audioWord2Db(L);
  int r = AtemSwitcher.audioWord2Db(R);
  int lp = AtemSwitcher.audioWord2Db(LP);
  int rp = AtemSwitcher.audioWord2Db(RP);

  Serial << label;
  Serial << F(" (") << _DECPADL(l, 3, " ") << F("db) ");
  for (int a = 6; a >= -60; a -= 6)  {
    Serial << (lp > a && lp <= a + 6 ? F("P") : (l > a ? F("*") : F(" ")));
  }
  Serial << F("|");
  for (int a = -60; a <= 6; a += 6)  {
    Serial << (rp > a && rp <= a + 6 ? F("P") : (r > a ? F("*") : F(" ")));
  }
  Serial << F(" (") << r << F("db) \n");
}

/*
 * Utility function, returns true if keypress
 */
bool serialReadKeypress()  {
  bool wasPressed = false;
  while (Serial.available()) {
    Serial.read();
    wasPressed = true;
  }
  return wasPressed;
}

/*
 * Pause
 */
void processCommands()  {
  char readChar = ' ';

  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar > 32)  {
      readChar = inChar;
    }
  }
  switch (readChar)  {
    case '+':
      delayBetweenCmds = 100;
      delayBetweenValues = 30;
      timeOutDelay = 1000;
      Serial.println(F("**** FAST SPEED ****"));
      break;
    case '-':
      delayBetweenCmds = 3000;
      delayBetweenValues = 1000;
      timeOutDelay = 3000;
      Serial.println(F("**** SLOW SPEED ****"));
      break;
    case 'n':
      delayBetweenCmds = 500;
      delayBetweenValues = 100;
      timeOutDelay = 2000;
      Serial.println(F("**** NORMAL SPEED ****"));
      break;
    case 'p':
      Serial.println(F("**** PAUSED ****"));
      while (!serialReadKeypress())  {
        AtemSwitcher.runLoop();
      }
      Serial.println(F("**** STARTED AGAIN ****"));
      break;
  }
}

uint8_t getTallyBySourceResolveIndex(uint16_t videoSource)  {
  for (uint8_t a = 0; a < AtemSwitcher.getTallyBySourceSources(); a++)  {
    if (AtemSwitcher.getTallyBySourceVideoSource(a) == videoSource)  return a;
  }
  return 255;
}
















/*
 * Main loop
 */
void loop() {



  // Waiting for ATEM switcher to connect:
  while (!AtemSwitcher.hasInitialized())  {
    AtemSwitcher.runLoop();
  }
  AtemSwitcher.serialOutput(0);



  long timeout, origValue;   // Timing
  uint8_t a, b;     // General purpose

  Serial << F("\n\n*****************************************\n* ATEM Arduino API Test Report: \n*****************************************\n");

  Serial << F("Connected to: ") << AtemSwitcher.getProductIdName() << F("\n");
  Serial << F("Firmware Version: ") << AtemSwitcher.getProtocolVersionMajor() << F(".") << AtemSwitcher.getProtocolVersionMinor() << F("\n\n");
  Serial << F("Mix/Effect rows: ") << AtemSwitcher.getTopologyMEs() << F("\n");
  Serial << F("Total Sources: ") << AtemSwitcher.getTopologySources() << F("\n");
  Serial << F("AUX busses: ") << AtemSwitcher.getTopologyAUXbusses() << F("\n");
  Serial << F("Color Generators: ") << AtemSwitcher.getTopologyColorGenerators() << F("\n");
  Serial << F("DVEs: ") << AtemSwitcher.getTopologyDVEs() << F("\n");
  Serial << F("Stingers: ") << AtemSwitcher.getTopologyStingers() << F("\n");
  Serial << F("Super Sources: ") << AtemSwitcher.getTopologySuperSources() << F("\n");
  if (AtemSwitcher.getTopologySuperSources() > 0)  {
    Serial << F("Super Source Boxes: ") << AtemSwitcher.getSuperSourceConfigBoxes() << F("\n");
  }
  Serial << F("Downstream Keyers: ") << AtemSwitcher.getTopologyDownstreamKeyes() << F("\n");
  for (a = 0; a < AtemSwitcher.getTopologyMEs(); a++)  {
    Serial << F("Keyers on ME ") << (a + 1) << F(": ") << AtemSwitcher.getMixEffectConfigKeyersOnME(a) << F("\n");
  }
  Serial << F("Media Player Stills: ") << AtemSwitcher.getMediaPlayersStillBanks() << F("\n");
  Serial << F("Media Player Clips: ") << AtemSwitcher.getMediaPlayersClipBanks() << F("\n");
  Serial << F("Macro Pool Banks: ") << AtemSwitcher.getMacroPoolBanks() << F("\n");
  Serial << F("Multi Viewers: ") << AtemSwitcher.getMultiViewConfigMultiViewers() << F("\n");
  Serial << F("Tally Channels: ") << AtemSwitcher.getTallyChannelConfigTallyChannels() << F("\n");
  Serial << F("Audio Channels: ") << AtemSwitcher.getAudioMixerConfigAudioChannels() << F("\n");
  Serial << F("SD Output: ") << (AtemSwitcher.getTopologyHasSDOutput() ? F("Yes") : F("No")) << F("\n");

  Serial << F("Main Power: ") << (AtemSwitcher.getPowerStatus() & 1 ? 1 : 0) << F("\n");
  Serial << F("Backup Power: ") << (AtemSwitcher.getPowerStatus() & 2 ? 1 : 0) << F("\n");

  // Input properties:
  Serial << F("\nVideo Mixer Input (InPr) - Properties:\n");
  printLine();
  processCommands();
  Serial << F("Source - Short - Long Name - Avail.Ext.Port.Type - Ext.Port.Type - Port.Type - Avail:Keys/SS-b/SS-a/MV/Aux - Avail:MEs\n");
  for (a = 0; a < AtemSwitcher.maxAtemSeriesVideoInputs(); a++)  {
    Serial << _DECPADL(AtemSwitcher.getVideoIndexSrc(a), 5, " ") << F(" - ")
           << _STRPADR(AtemSwitcher.getInputShortName(AtemSwitcher.getVideoIndexSrc(a)), 4, " ") << F(" - ")
           << _STRPADR(AtemSwitcher.getInputLongName(AtemSwitcher.getVideoIndexSrc(a)), 20, " ") << F(" - ")
           << _BINPADL(AtemSwitcher.getInputAvailableExternalPortTypes(AtemSwitcher.getVideoIndexSrc(a)), 5, "0")  << F(" - ")
           << AtemSwitcher.getInputExternalPortType(AtemSwitcher.getVideoIndexSrc(a))  << F(" - ")
           << _DECPADL(AtemSwitcher.getInputPortType(AtemSwitcher.getVideoIndexSrc(a)), 3, " ")  << F(" - ")
           << _BINPADL(AtemSwitcher.getInputAvailability(AtemSwitcher.getVideoIndexSrc(a)), 5, "0")  << F(" - ")
           << _BINPADL(AtemSwitcher.getInputMEAvailability(AtemSwitcher.getVideoIndexSrc(a)), 2, "0")  << F("\n");
  }

  // Input properties:
  Serial << F("\nAudio Mixer Input (AMIP) - Properties:\n");
  printLine();
  processCommands();
  Serial << F("Source - Input Type - From MP? - Plug Type\n");
  for (a = 0; a <= 24; a++)  {
    Serial << _DECPADL(AtemSwitcher.getAudioIndexSrc(a), 4, " ") << F(" - ")
           << _DECPADL(AtemSwitcher.getAudioMixerInputType(AtemSwitcher.getAudioIndexSrc(a)), 3, " ") << F(" - ")
           << (AtemSwitcher.getAudioMixerInputFromMediaPlayer(AtemSwitcher.getAudioIndexSrc(a)) ? F("YES") : F("NO ")) << F(" - ")
           << _DECPADL(AtemSwitcher.getAudioMixerInputPlugtype(AtemSwitcher.getAudioIndexSrc(a)), 3, " ") << F("\n");
  }


  // Storing original video mode and setting to 720p50 for test:
  uint8_t format_R = AtemSwitcher.getVideoModeFormat();
  //  AtemSwitcher.setVideoModeFormat(4);  // 720p50 - but this will empty clip banks!
























#if TEST_AUX
  // Aux Source - Input
  if (AtemSwitcher.getTopologyAUXbusses() > 0)  {
    Serial << F("\n\n***************************\n* AUX Channels: \n***************************\n");

    for (uint8_t aUXChannel = 0; aUXChannel <= AtemSwitcher.getTopologyAUXbusses() - 1; aUXChannel++)	{
      Serial << F("\nAux Source - Input:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getAuxSourceInput(aUXChannel);
      for (uint16_t input = 0; input < AtemSwitcher.maxAtemSeriesVideoInputs(); input++)	{
        if (AtemSwitcher.getInputAvailability(AtemSwitcher.getVideoIndexSrc(input)) & 0x01)  {  // Checking if source can be used on AUX outputs
          AtemSwitcher.setAuxSourceInput(aUXChannel, AtemSwitcher.getVideoIndexSrc(input));

          timeout = millis();
          while (AtemSwitcher.getAuxSourceInput(aUXChannel) != AtemSwitcher.getVideoIndexSrc(input) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getAuxSourceInput(aUXChannel);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setAuxSourceInput(") << aUXChannel << F(", ") << AtemSwitcher.getVideoIndexSrc(input) << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay
        }
      }
      AtemSwitcher.setAuxSourceInput(aUXChannel, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay
    }
  }
  AtemSwitcher.runLoop(1000);
#endif


#if TEST_MESEC
  Serial << F("\n\n***************************\n* ME Sections: \n***************************\n");

  for (uint8_t mE = 0; mE < AtemSwitcher.getTopologyMEs(); mE++)	{

    // Program Input - Video Source
    Serial << F("\nProgram Input - Video Source:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getProgramInputVideoSource(mE);
    for (uint16_t videoSource = 0; videoSource < AtemSwitcher.maxAtemSeriesVideoInputs(); videoSource++)	{
      if (AtemSwitcher.getInputMEAvailability(AtemSwitcher.getVideoIndexSrc(videoSource)) & (B1 << mE))  {
        AtemSwitcher.setProgramInputVideoSource(mE, AtemSwitcher.getVideoIndexSrc(videoSource));
        timeout = millis();
        while (AtemSwitcher.getProgramInputVideoSource(mE) != AtemSwitcher.getVideoIndexSrc(videoSource) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getProgramInputVideoSource(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setProgramInputVideoSource(") << mE << F(", ") << AtemSwitcher.getVideoIndexSrc(videoSource) << F("): ") << (millis() - timeout) << F(" ms\n");

        // Check tally:
        if (mE == 0)  {
          AtemSwitcher.runLoop(100);
          if (!(AtemSwitcher.getTallyBySourceTallyFlags(getTallyBySourceResolveIndex(AtemSwitcher.getVideoIndexSrc(videoSource))) & B1))  {
            Serial << F(" - ERROR, Tally By Source did not reflect this state (On)!\n");
          }
          else {
            Serial << F(" - OK for Tally By Source\n");
          }
          if (AtemSwitcher.getVideoIndexSrc(videoSource) > 0 && AtemSwitcher.getVideoIndexSrc(videoSource) <= AtemSwitcher.getTallyByIndexSources())  {
            if (!(AtemSwitcher.getTallyByIndexTallyFlags(AtemSwitcher.getVideoIndexSrc(videoSource) - 1) & B1))  {
              Serial << F(" - ERROR, Tally By Index did not reflect this state (On)!\n");
            }
            else {
              Serial << F(" - OK for Tally By Index\n");
            }
          }
        }

        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay
      }
    }
    AtemSwitcher.setProgramInputVideoSource(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay


    // Preview Input - Video Source
    Serial << F("\nPreview Input - Video Source:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getPreviewInputVideoSource(mE);
    for (uint16_t videoSource = 0; videoSource < AtemSwitcher.maxAtemSeriesVideoInputs(); videoSource++)	{
      if (AtemSwitcher.getInputMEAvailability(AtemSwitcher.getVideoIndexSrc(videoSource)) & (B1 << mE))  {
        AtemSwitcher.setPreviewInputVideoSource(mE, AtemSwitcher.getVideoIndexSrc(videoSource));
        timeout = millis();
        while (AtemSwitcher.getPreviewInputVideoSource(mE) != AtemSwitcher.getVideoIndexSrc(videoSource) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getPreviewInputVideoSource(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setPreviewInputVideoSource(") << mE << F(", ") << AtemSwitcher.getVideoIndexSrc(videoSource) << F("): ") << (millis() - timeout) << F(" ms\n");

        // Check tally:
        if (mE == 0)  {
          AtemSwitcher.runLoop(100);
          if (!(AtemSwitcher.getTallyBySourceTallyFlags(getTallyBySourceResolveIndex(AtemSwitcher.getVideoIndexSrc(videoSource))) & B10))  {
            Serial << F(" - ERROR, Tally By Source did not reflect this state (On)!\n");
          }
          else {
            Serial << F(" - OK for Tally By Source\n");
          }
          if (AtemSwitcher.getVideoIndexSrc(videoSource) > 0 && AtemSwitcher.getVideoIndexSrc(videoSource) <= AtemSwitcher.getTallyByIndexSources())  {
            if (!(AtemSwitcher.getTallyByIndexTallyFlags(AtemSwitcher.getVideoIndexSrc(videoSource) - 1) & B10))  {
              Serial << F(" - ERROR, Tally By Index did not reflect this state (On)!\n");
            }
            else {
              Serial << F(" - OK for Tally By Index\n");
            }
          }
        }
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay
      }
    }
    AtemSwitcher.setPreviewInputVideoSource(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay


    // Transition - Style
    Serial << F("\nTransition - Style:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionStyle(mE);
    for (uint8_t style = 0; style <= 4; style += 1)	{
      if (style < 3 || (style == 3 && AtemSwitcher.getTopologyDVEs() > 0) || (style == 4 && AtemSwitcher.getTopologyStingers() > 0))  {
        AtemSwitcher.setTransitionStyle(mE, style);
        timeout = millis();
        while (AtemSwitcher.getTransitionStyle(mE) != style && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionStyle(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionStyle(") << mE << F(", ") << style << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay
      }
    }
    AtemSwitcher.setTransitionStyle(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay


    // Transition - Next Transition
    Serial << F("\nTransition - Next Transition:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionNextTransition(mE);
    for (uint8_t nextTransition = 1; nextTransition <= (1 << AtemSwitcher.getMixEffectConfigKeyersOnME(mE)); nextTransition = nextTransition << 1)	{
      AtemSwitcher.setTransitionNextTransition(mE, nextTransition);
      timeout = millis();
      while (AtemSwitcher.getTransitionNextTransition(mE) != nextTransition && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionNextTransition(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionNextTransition(") << mE << F(", ") << nextTransition << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay
    }
    AtemSwitcher.setTransitionNextTransition(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay


    // Transition Preview - Enabled
    Serial << F("\nTransition Preview - Enabled:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionPreviewEnabled(mE);
    for (uint8_t enabled = 0; enabled <= 1; enabled++)	{
      AtemSwitcher.setTransitionPreviewEnabled(mE, enabled);
      timeout = millis();
      while (AtemSwitcher.getTransitionPreviewEnabled(mE) != enabled && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionPreviewEnabled(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionPreviewEnabled(") << mE << F(", ") << enabled << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay
    }
    AtemSwitcher.setTransitionPreviewEnabled(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay


    // Transition Position - Position
    Serial << F("\nTransition Position - Position:\n");
    printLine();
    processCommands();
    for (uint16_t position = 0; position <= 9999; position += 500)	{
      AtemSwitcher.setTransitionPosition(mE, position);
      timeout = millis();
      while (AtemSwitcher.getTransitionPosition(mE) != position && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionPosition(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionPosition(") << mE << F(", ") << position << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay
    }
    AtemSwitcher.setTransitionPosition(mE, 0);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay


    // Cut + Auto:
    Serial << F("\nCut:\n");
    printLine();
    processCommands();

    a = AtemSwitcher.getProgramInputVideoSource(mE);
    b = AtemSwitcher.getPreviewInputVideoSource(mE);
    uint8_t rate = AtemSwitcher.getTransitionMixRate(mE);
    uint8_t style = AtemSwitcher.getTransitionStyle(mE);
    // Serial << F("Setup for Cut... ");
    AtemSwitcher.setProgramInputVideoSource(mE, 1);
    AtemSwitcher.setPreviewInputVideoSource(mE, 2);
    AtemSwitcher.runLoop(1000);
    // Serial << F("Cut!\n");
    AtemSwitcher.performCutME(mE);
    AtemSwitcher.runLoop(1000);

    timeout = millis();
    while (AtemSwitcher.getProgramInputVideoSource(mE) != 2 && AtemSwitcher.getPreviewInputVideoSource(mE) != 1 && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
      AtemSwitcher.runLoop();
    }
    if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
      Serial << F("ERROR");
    }
    else {
      Serial << F("OK");
    }
    Serial << F(" for performCutME(") << mE << F("): ") << (millis() - timeout) << F(" ms\n");

    Serial << F("Timecode for last command (Cut):") << _DECPADL(AtemSwitcher.getLastStateChangeTimeCodeHour(), 2, "0")
           << F(":") << _DECPADL(AtemSwitcher.getLastStateChangeTimeCodeMinute(), 2, "0")
           << F(":") << _DECPADL(AtemSwitcher.getLastStateChangeTimeCodeSecond(), 2, "0")
           << F(":") << _DECPADL(AtemSwitcher.getLastStateChangeTimeCodeFrame(), 2, "0")
           << F("\n");

    AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay

    Serial << F("\nAuto:\n");
    printLine();
    processCommands();
    AtemSwitcher.setTransitionMixRate(mE, 100);
    AtemSwitcher.setTransitionStyle(mE, 0);
    AtemSwitcher.runLoop(200);
    AtemSwitcher.performAutoME(mE);
    AtemSwitcher.runLoop(200);

    while (AtemSwitcher.getTransitionInTransition(mE))  {
      Serial << F("Frames remaining: ") << AtemSwitcher.getTransitionFramesRemaining(mE);

      Serial << F("  //  Timecode:") << _DECPADL(AtemSwitcher.getLastStateChangeTimeCodeHour(), 2, "0")
             << F(":") << _DECPADL(AtemSwitcher.getLastStateChangeTimeCodeMinute(), 2, "0")
             << F(":") << _DECPADL(AtemSwitcher.getLastStateChangeTimeCodeSecond(), 2, "0")
             << F(":") << _DECPADL(AtemSwitcher.getLastStateChangeTimeCodeFrame(), 2, "0")
             << F("\n");

      AtemSwitcher.runLoop(100);
    }

    // Reset:
    AtemSwitcher.setProgramInputVideoSource(mE, a);
    AtemSwitcher.setPreviewInputVideoSource(mE, b);
    AtemSwitcher.setTransitionMixRate(mE, rate);
    AtemSwitcher.setTransitionStyle(mE, style);
    AtemSwitcher.runLoop(200);

    AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay


    // Fade-to-Black
    Serial << F("\nFade to black:\n");
    printLine();
    processCommands();

    bool ftbEnabled = AtemSwitcher.getFadeToBlackStateFullyBlack(mE);
    rate = AtemSwitcher.getFadeToBlackRate(mE);
    AtemSwitcher.setFadeToBlackRate(mE, 100);
    AtemSwitcher.runLoop(200);
    AtemSwitcher.performFadeToBlackME(mE);
    AtemSwitcher.runLoop(200);
    while (AtemSwitcher.getFadeToBlackStateInTransition(mE))  {
      Serial << F("Frames remaining: ") << AtemSwitcher.getFadeToBlackStateFramesRemaining(mE);

      Serial << F("  //  Timecode:") << _DECPADL(AtemSwitcher.getLastStateChangeTimeCodeHour(), 2, "0")
             << F(":") << _DECPADL(AtemSwitcher.getLastStateChangeTimeCodeMinute(), 2, "0")
             << F(":") << _DECPADL(AtemSwitcher.getLastStateChangeTimeCodeSecond(), 2, "0")
             << F(":") << _DECPADL(AtemSwitcher.getLastStateChangeTimeCodeFrame(), 2, "0")
             << F("\n");

      AtemSwitcher.runLoop(100);
    }
    AtemSwitcher.setFadeToBlackRate(mE, 50);
    AtemSwitcher.runLoop(200);
    if (AtemSwitcher.getFadeToBlackStateFullyBlack(mE) != ftbEnabled)  {
      AtemSwitcher.performFadeToBlackME(mE);
      AtemSwitcher.runLoop(100);
      while (AtemSwitcher.getFadeToBlackStateInTransition(mE))  {
        Serial << F("Frames remaining: ") << AtemSwitcher.getFadeToBlackStateFramesRemaining(mE) << F("\n");
        AtemSwitcher.runLoop(100);
      }
    }
    else {
      Serial << F("ERROR: Fade-to-black didn't work right\n");
    }
    AtemSwitcher.setTransitionMixRate(mE, rate);

    // Transition Mix - Rate
    Serial << F("\nTransition Mix - Rate:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionMixRate(mE);
    for (uint8_t rate = 1; rate <= 250; rate += 50)	{
      AtemSwitcher.setTransitionMixRate(mE, rate);
      timeout = millis();
      while (AtemSwitcher.getTransitionMixRate(mE) != rate && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionMixRate(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionMixRate(") << mE << F(", ") << rate << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay
    }
    AtemSwitcher.setTransitionMixRate(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay


    // Transition Dip - Rate
    Serial << F("\nTransition Dip - Rate:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionDipRate(mE);
    for (uint8_t rate = 1; rate <= 250; rate += 50)	{
      AtemSwitcher.setTransitionDipRate(mE, rate);
      timeout = millis();
      while (AtemSwitcher.getTransitionDipRate(mE) != rate && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionDipRate(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionDipRate(") << mE << F(", ") << rate << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay
    }
    AtemSwitcher.setTransitionDipRate(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay


    // Transition Dip - Input
    Serial << F("\nTransition Dip - Input:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionDipInput(mE);
    for (uint16_t input = 0; input < AtemSwitcher.maxAtemSeriesVideoInputs(); input++)	{
      if (AtemSwitcher.getInputMEAvailability(AtemSwitcher.getVideoIndexSrc(input)) & (B1 << mE))  {
        AtemSwitcher.setTransitionDipInput(mE, AtemSwitcher.getVideoIndexSrc(input));
        timeout = millis();
        while (AtemSwitcher.getTransitionDipInput(mE) != AtemSwitcher.getVideoIndexSrc(input) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionDipInput(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionDipInput(") << mE << F(", ") << AtemSwitcher.getVideoIndexSrc(input)  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
    }
    AtemSwitcher.setTransitionDipInput(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Transition Wipe - Rate
    Serial << F("\nTransition Wipe - Rate:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionWipeRate(mE);
    for (uint8_t rate = 1; rate <= 250; rate += 50)	{
      AtemSwitcher.setTransitionWipeRate(mE, rate);
      timeout = millis();
      while (AtemSwitcher.getTransitionWipeRate(mE) != rate && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionWipeRate(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionWipeRate(") << mE << F(", ") << rate  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setTransitionWipeRate(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Transition Wipe - Pattern
    Serial << F("\nTransition Wipe - Pattern:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionWipePattern(mE);
    for (uint8_t pattern = 0; pattern <= 17; pattern++)	{
      AtemSwitcher.setTransitionWipePattern(mE, pattern);
      timeout = millis();
      while (AtemSwitcher.getTransitionWipePattern(mE) != pattern && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionWipePattern(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionWipePattern(") << mE << F(", ") << pattern  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setTransitionWipePattern(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Transition Wipe - Width
    Serial << F("\nTransition Wipe - Width:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionWipeWidth(mE);
    for (uint16_t width = 0; width <= 10000; width += 2000)	{
      AtemSwitcher.setTransitionWipeWidth(mE, width);
      timeout = millis();
      while (AtemSwitcher.getTransitionWipeWidth(mE) != width && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionWipeWidth(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionWipeWidth(") << mE << F(", ") << width  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setTransitionWipeWidth(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Transition Wipe - Fill Source
    Serial << F("\nTransition Wipe - Fill Source:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionWipeFillSource(mE);
    for (uint16_t fillSource = 0; fillSource < AtemSwitcher.maxAtemSeriesVideoInputs(); fillSource++)	{
      if (AtemSwitcher.getInputMEAvailability(AtemSwitcher.getVideoIndexSrc(fillSource)) & (B1 << mE))  {
        AtemSwitcher.setTransitionWipeFillSource(mE, AtemSwitcher.getVideoIndexSrc(fillSource));
        timeout = millis();
        while (AtemSwitcher.getTransitionWipeFillSource(mE) != AtemSwitcher.getVideoIndexSrc(fillSource) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionWipeFillSource(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionWipeFillSource(") << mE << F(", ") << AtemSwitcher.getVideoIndexSrc(fillSource)  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
    }
    AtemSwitcher.setTransitionWipeFillSource(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Transition Wipe - Symmetry
    Serial << F("\nTransition Wipe - Symmetry:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionWipeSymmetry(mE);
    for (uint16_t symmetry = 0; symmetry <= 10000; symmetry += 2000)	{
      AtemSwitcher.setTransitionWipeSymmetry(mE, symmetry);
      timeout = millis();
      while (AtemSwitcher.getTransitionWipeSymmetry(mE) != symmetry && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionWipeSymmetry(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionWipeSymmetry(") << mE << F(", ") << symmetry  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setTransitionWipeSymmetry(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Transition Wipe - Softness
    Serial << F("\nTransition Wipe - Softness:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionWipeSoftness(mE);
    for (uint16_t softness = 0; softness <= 10000; softness += 2000)	{
      AtemSwitcher.setTransitionWipeSoftness(mE, softness);
      timeout = millis();
      while (AtemSwitcher.getTransitionWipeSoftness(mE) != softness && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionWipeSoftness(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionWipeSoftness(") << mE << F(", ") << softness  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setTransitionWipeSoftness(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Transition Wipe - Position X
    Serial << F("\nTransition Wipe - Position X:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionWipePositionX(mE);
    for (uint16_t positionX = 0; positionX <= 10000; positionX += 2000)	{
      AtemSwitcher.setTransitionWipePositionX(mE, positionX);
      timeout = millis();
      while (AtemSwitcher.getTransitionWipePositionX(mE) != positionX && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionWipePositionX(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionWipePositionX(") << mE << F(", ") << positionX  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setTransitionWipePositionX(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Transition Wipe - Position Y
    Serial << F("\nTransition Wipe - Position Y:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionWipePositionY(mE);
    for (uint16_t positionY = 0; positionY <= 10000; positionY += 2000)	{
      AtemSwitcher.setTransitionWipePositionY(mE, positionY);
      timeout = millis();
      while (AtemSwitcher.getTransitionWipePositionY(mE) != positionY && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionWipePositionY(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionWipePositionY(") << mE << F(", ") << positionY  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setTransitionWipePositionY(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Transition Wipe - Reverse
    Serial << F("\nTransition Wipe - Reverse:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionWipeReverse(mE);
    for (uint8_t reverse = 0; reverse <= 1; reverse++)	{
      AtemSwitcher.setTransitionWipeReverse(mE, reverse);
      timeout = millis();
      while (AtemSwitcher.getTransitionWipeReverse(mE) != reverse && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionWipeReverse(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionWipeReverse(") << mE << F(", ") << reverse  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setTransitionWipeReverse(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Transition Wipe - FlipFlop
    Serial << F("\nTransition Wipe - FlipFlop:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getTransitionWipeFlipFlop(mE);
    for (uint8_t flipFlop = 0; flipFlop <= 1; flipFlop++)	{
      AtemSwitcher.setTransitionWipeFlipFlop(mE, flipFlop);
      timeout = millis();
      while (AtemSwitcher.getTransitionWipeFlipFlop(mE) != flipFlop && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionWipeFlipFlop(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setTransitionWipeFlipFlop(") << mE << F(", ") << flipFlop  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setTransitionWipeFlipFlop(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command

    if (AtemSwitcher.getTopologyDVEs() > 0)  {
      // Transition DVE - Rate
      Serial << F("\nTransition DVE - Rate:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionDVERate(mE);
      for (uint8_t rate = 1; rate <= 250; rate += 50)	{
        AtemSwitcher.setTransitionDVERate(mE, rate);
        timeout = millis();
        while (AtemSwitcher.getTransitionDVERate(mE) != rate && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionDVERate(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionDVERate(") << mE << F(", ") << rate  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionDVERate(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition DVE - Style
      Serial << F("\nTransition DVE - Style:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionDVEStyle(mE);
      for (uint8_t style = 0; style <= 34; style++)	{
        AtemSwitcher.setTransitionDVEStyle(mE, style);
        timeout = millis();
        while (AtemSwitcher.getTransitionDVEStyle(mE) != style && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionDVEStyle(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionDVEStyle(") << mE << F(", ") << style  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionDVEStyle(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition DVE - Fill Source
      Serial << F("\nTransition DVE - Fill Source:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionDVEFillSource(mE);
      for (uint16_t fillSource = 0; fillSource < AtemSwitcher.maxAtemSeriesVideoInputs(); fillSource++)	{
        if (AtemSwitcher.getInputMEAvailability(AtemSwitcher.getVideoIndexSrc(fillSource)) & (B1 << mE))  {
          AtemSwitcher.setTransitionDVEFillSource(mE, AtemSwitcher.getVideoIndexSrc(fillSource));
          timeout = millis();
          while (AtemSwitcher.getTransitionDVEFillSource(mE) != AtemSwitcher.getVideoIndexSrc(fillSource) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionDVEFillSource(mE);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setTransitionDVEFillSource(") << mE << F(", ") << AtemSwitcher.getVideoIndexSrc(fillSource)  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
      }
      AtemSwitcher.setTransitionDVEFillSource(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition DVE - Key Source
      Serial << F("\nTransition DVE - Key Source:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionDVEKeySource(mE);
      for (uint16_t keySource = 0; keySource < AtemSwitcher.maxAtemSeriesVideoInputs(); keySource++)	{
        if (AtemSwitcher.getInputAvailability(AtemSwitcher.getVideoIndexSrc(keySource)) & 0x10)  {  // Source on keys
          AtemSwitcher.setTransitionDVEKeySource(mE, AtemSwitcher.getVideoIndexSrc(keySource));
          timeout = millis();
          while (AtemSwitcher.getTransitionDVEKeySource(mE) != AtemSwitcher.getVideoIndexSrc(keySource) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionDVEKeySource(mE);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setTransitionDVEKeySource(") << mE << F(", ") << AtemSwitcher.getVideoIndexSrc(keySource)  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
      }
      AtemSwitcher.setTransitionDVEKeySource(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition DVE - Enable Key
      Serial << F("\nTransition DVE - Enable Key:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionDVEEnableKey(mE);
      for (uint8_t enableKey = 0; enableKey <= 1; enableKey++)	{
        AtemSwitcher.setTransitionDVEEnableKey(mE, enableKey);
        timeout = millis();
        while (AtemSwitcher.getTransitionDVEEnableKey(mE) != enableKey && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionDVEEnableKey(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionDVEEnableKey(") << mE << F(", ") << enableKey  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionDVEEnableKey(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition DVE - Pre Multiplied
      Serial << F("\nTransition DVE - Pre Multiplied:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionDVEPreMultiplied(mE);
      for (uint8_t preMultiplied = 0; preMultiplied <= 1; preMultiplied++)	{
        AtemSwitcher.setTransitionDVEPreMultiplied(mE, preMultiplied);
        timeout = millis();
        while (AtemSwitcher.getTransitionDVEPreMultiplied(mE) != preMultiplied && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionDVEPreMultiplied(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionDVEPreMultiplied(") << mE << F(", ") << preMultiplied  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionDVEPreMultiplied(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition DVE - Clip
      Serial << F("\nTransition DVE - Clip:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionDVEClip(mE);
      for (uint16_t clip = 0; clip <= 1000; clip += 200)	{
        AtemSwitcher.setTransitionDVEClip(mE, clip);
        timeout = millis();
        while (AtemSwitcher.getTransitionDVEClip(mE) != clip && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionDVEClip(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionDVEClip(") << mE << F(", ") << clip  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionDVEClip(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition DVE - Gain
      Serial << F("\nTransition DVE - Gain:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionDVEGain(mE);
      for (uint16_t gain = 0; gain <= 1000; gain += 200)	{
        AtemSwitcher.setTransitionDVEGain(mE, gain);
        timeout = millis();
        while (AtemSwitcher.getTransitionDVEGain(mE) != gain && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionDVEGain(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionDVEGain(") << mE << F(", ") << gain  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionDVEGain(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition DVE - Invert Key
      Serial << F("\nTransition DVE - Invert Key:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionDVEInvertKey(mE);
      for (uint8_t invertKey = 0; invertKey <= 1; invertKey++)	{
        AtemSwitcher.setTransitionDVEInvertKey(mE, invertKey);
        timeout = millis();
        while (AtemSwitcher.getTransitionDVEInvertKey(mE) != invertKey && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionDVEInvertKey(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionDVEInvertKey(") << mE << F(", ") << invertKey  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionDVEInvertKey(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition DVE - Reverse
      Serial << F("\nTransition DVE - Reverse:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionDVEReverse(mE);
      for (uint8_t reverse = 0; reverse <= 1; reverse++)	{
        AtemSwitcher.setTransitionDVEReverse(mE, reverse);
        timeout = millis();
        while (AtemSwitcher.getTransitionDVEReverse(mE) != reverse && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionDVEReverse(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionDVEReverse(") << mE << F(", ") << reverse  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionDVEReverse(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition DVE - FlipFlop
      Serial << F("\nTransition DVE - FlipFlop:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionDVEFlipFlop(mE);
      for (uint8_t flipFlop = 0; flipFlop <= 1; flipFlop++)	{
        AtemSwitcher.setTransitionDVEFlipFlop(mE, flipFlop);
        timeout = millis();
        while (AtemSwitcher.getTransitionDVEFlipFlop(mE) != flipFlop && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionDVEFlipFlop(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionDVEFlipFlop(") << mE << F(", ") << flipFlop  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionDVEFlipFlop(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command
    }


    if (AtemSwitcher.getTopologyStingers() > 0)  {

      // Transition Stinger - Source
      Serial << F("\nTransition Stinger - Source:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionStingerSource(mE);
      for (uint8_t source = 1; source <= 2; source += 1)	{
        AtemSwitcher.setTransitionStingerSource(mE, source);
        timeout = millis();
        while (AtemSwitcher.getTransitionStingerSource(mE) != source && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionStingerSource(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionStingerSource(") << mE << F(", ") << source  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionStingerSource(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition Stinger - Pre Multiplied
      Serial << F("\nTransition Stinger - Pre Multiplied:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionStingerPreMultiplied(mE);
      for (uint8_t preMultiplied = 0; preMultiplied <= 1; preMultiplied++)	{
        AtemSwitcher.setTransitionStingerPreMultiplied(mE, preMultiplied);
        timeout = millis();
        while (AtemSwitcher.getTransitionStingerPreMultiplied(mE) != preMultiplied && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionStingerPreMultiplied(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionStingerPreMultiplied(") << mE << F(", ") << preMultiplied  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionStingerPreMultiplied(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition Stinger - Clip
      Serial << F("\nTransition Stinger - Clip:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionStingerClip(mE);
      for (uint16_t clip = 0; clip <= 1000; clip += 200)	{
        AtemSwitcher.setTransitionStingerClip(mE, clip);
        timeout = millis();
        while (AtemSwitcher.getTransitionStingerClip(mE) != clip && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionStingerClip(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionStingerClip(") << mE << F(", ") << clip  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionStingerClip(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition Stinger - Gain
      Serial << F("\nTransition Stinger - Gain:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionStingerGain(mE);
      for (uint16_t gain = 0; gain <= 1000; gain += 200)	{
        AtemSwitcher.setTransitionStingerGain(mE, gain);
        timeout = millis();
        while (AtemSwitcher.getTransitionStingerGain(mE) != gain && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionStingerGain(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionStingerGain(") << mE << F(", ") << gain  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionStingerGain(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition Stinger - Invert Key
      Serial << F("\nTransition Stinger - Invert Key:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionStingerInvertKey(mE);
      for (uint8_t invertKey = 0; invertKey <= 1; invertKey++)	{
        AtemSwitcher.setTransitionStingerInvertKey(mE, invertKey);
        timeout = millis();
        while (AtemSwitcher.getTransitionStingerInvertKey(mE) != invertKey && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionStingerInvertKey(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionStingerInvertKey(") << mE << F(", ") << invertKey  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionStingerInvertKey(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition Stinger - Clip Duration
      Serial << F("\nTransition Stinger - Clip Duration:\n");
      printLine();
      processCommands();
      uint16_t clipDuration = AtemSwitcher.getTransitionStingerClipDuration(mE);
      for (uint16_t clipDuration = AtemSwitcher.getTransitionStingerTriggerPoint(mE) + AtemSwitcher.getTransitionStingerMixRate(mE); clipDuration <= 165; clipDuration += 15)	{
        AtemSwitcher.setTransitionStingerClipDuration(mE, clipDuration);
        timeout = millis();
        while (AtemSwitcher.getTransitionStingerClipDuration(mE) != clipDuration && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionStingerClipDuration(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionStingerClipDuration(") << mE << F(", ") << clipDuration  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command

      // Transition Stinger - Pre Roll
      Serial << F("\nTransition Stinger - Pre Roll:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionStingerPreRoll(mE);
      for (uint16_t preRoll = 0; preRoll <= 90; preRoll += 13)	{
        AtemSwitcher.setTransitionStingerPreRoll(mE, preRoll);
        timeout = millis();
        while (AtemSwitcher.getTransitionStingerPreRoll(mE) != preRoll && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionStingerPreRoll(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionStingerPreRoll(") << mE << F(", ") << preRoll  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionStingerPreRoll(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command

      // Transition Stinger - Trigger Point
      Serial << F("\nTransition Stinger - Trigger Point:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionStingerTriggerPoint(mE);
      for (uint16_t triggerPoint = 1; triggerPoint <= AtemSwitcher.getTransitionStingerClipDuration(mE) - AtemSwitcher.getTransitionStingerMixRate(mE); triggerPoint += 15)	{
        AtemSwitcher.setTransitionStingerTriggerPoint(mE, triggerPoint);
        timeout = millis();
        while (AtemSwitcher.getTransitionStingerTriggerPoint(mE) != triggerPoint && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionStingerTriggerPoint(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionStingerTriggerPoint(") << mE << F(", ") << triggerPoint  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionStingerTriggerPoint(mE, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Transition Stinger - Mix Rate
      Serial << F("\nTransition Stinger - Mix Rate:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getTransitionStingerMixRate(mE);
      for (uint16_t mixRate = 1; mixRate <= AtemSwitcher.getTransitionStingerClipDuration(mE) - AtemSwitcher.getTransitionStingerTriggerPoint(mE); mixRate += 15)	{
        AtemSwitcher.setTransitionStingerMixRate(mE, mixRate);
        timeout = millis();
        while (AtemSwitcher.getTransitionStingerMixRate(mE) != mixRate && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getTransitionStingerMixRate(mE);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setTransitionStingerMixRate(") << mE << F(", ") << mixRate  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setTransitionStingerMixRate(mE, origValue);
      AtemSwitcher.runLoop(100);
      AtemSwitcher.setTransitionStingerClipDuration(mE, clipDuration);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command
    }

    // Fade-To-Black - Rate
    Serial << F("\nFade-To-Black - Rate:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getFadeToBlackRate(mE);
    for (uint8_t rate = 1; rate <= 250; rate += 50)	{
      AtemSwitcher.setFadeToBlackRate(mE, rate);
      timeout = millis();
      while (AtemSwitcher.getFadeToBlackRate(mE) != rate && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getFadeToBlackRate(mE);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setFadeToBlackRate(") << mE << F(", ") << rate  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setFadeToBlackRate(mE, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command







    for (uint8_t keyer = 0; keyer < AtemSwitcher.getMixEffectConfigKeyersOnME(mE); keyer++)	{

      // Keyer On Air - Enabled
      Serial << F("\nKeyer On Air - Enabled:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyerOnAirEnabled(mE, keyer);
      for (uint8_t enabled = 0; enabled <= 1; enabled++)	{
        AtemSwitcher.setKeyerOnAirEnabled(mE, keyer, enabled);
        timeout = millis();
        while (AtemSwitcher.getKeyerOnAirEnabled(mE, keyer) != enabled && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyerOnAirEnabled(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyerOnAirEnabled(") << mE << F(", ") << keyer << F(", ") << enabled  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyerOnAirEnabled(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Keyer - Type
      Serial << F("\nKeyer - Type:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyerType(mE, keyer);
      for (uint8_t type = 0; type <= 3; type += 1)	{
        if (type != 3 || AtemSwitcher.getTopologyDVEs() > 0)  {
          AtemSwitcher.setKeyerType(mE, keyer, type);
          timeout = millis();
          while (AtemSwitcher.getKeyerType(mE, keyer) != type && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyerType(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyerType(") << mE << F(", ") << keyer << F(", ") << type  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
      }
      AtemSwitcher.setKeyerType(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Keyer - Masked
      Serial << F("\nKeyer - Masked:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyerMasked(mE, keyer);
      for (uint8_t masked = 0; masked <= 1; masked++)	{
        AtemSwitcher.setKeyerMasked(mE, keyer, masked);
        timeout = millis();
        while (AtemSwitcher.getKeyerMasked(mE, keyer) != masked && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyerMasked(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyerMasked(") << mE << F(", ") << keyer << F(", ") << masked  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyerMasked(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command

      if (AtemSwitcher.getTopologyDVEs() > 0)  {
        // Keyer - Fly Enabled
        Serial << F("\nKeyer - Fly Enabled:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyerFlyEnabled(mE, keyer);
        for (uint8_t enabled = 0; enabled <= 1; enabled++)	{
          AtemSwitcher.setKeyerFlyEnabled(mE, keyer, enabled);
          timeout = millis();
          while (AtemSwitcher.getKeyerFlyEnabled(mE, keyer) != enabled && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyerFlyEnabled(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyerFlyEnabled(") << mE << F(", ") << keyer << F(", ") << enabled  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyerFlyEnabled(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command
      }

      // Keyer - Top
      Serial << F("\nKeyer - Top:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyerTop(mE, keyer);
      for (int top = -9000; top <= 9000; top += 3600)	{
        AtemSwitcher.setKeyerTop(mE, keyer, top);
        timeout = millis();
        while (AtemSwitcher.getKeyerTop(mE, keyer) != top && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyerTop(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyerTop(") << mE << F(", ") << keyer << F(", ") << top  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyerTop(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Keyer - Bottom
      Serial << F("\nKeyer - Bottom:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyerBottom(mE, keyer);
      for (int bottom = -9000; bottom <= 9000; bottom += 3600)	{
        AtemSwitcher.setKeyerBottom(mE, keyer, bottom);
        timeout = millis();
        while (AtemSwitcher.getKeyerBottom(mE, keyer) != bottom && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyerBottom(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyerBottom(") << mE << F(", ") << keyer << F(", ") << bottom  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyerBottom(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Keyer - Left
      Serial << F("\nKeyer - Left:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyerLeft(mE, keyer);
      for (int left = -16000; left <= 16000; left += 6400)	{
        AtemSwitcher.setKeyerLeft(mE, keyer, left);
        timeout = millis();
        while (AtemSwitcher.getKeyerLeft(mE, keyer) != left && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyerLeft(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyerLeft(") << mE << F(", ") << keyer << F(", ") << left  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyerLeft(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Keyer - Right
      Serial << F("\nKeyer - Right:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyerRight(mE, keyer);
      for (int right = -16000; right <= 16000; right += 6400)	{
        AtemSwitcher.setKeyerRight(mE, keyer, right);
        timeout = millis();
        while (AtemSwitcher.getKeyerRight(mE, keyer) != right && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyerRight(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyerRight(") << mE << F(", ") << keyer << F(", ") << right  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyerRight(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Keyer - Fill Source
      Serial << F("\nKeyer - Fill Source:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyerFillSource(mE, keyer);
      for (uint16_t fillSource = 0; fillSource < AtemSwitcher.maxAtemSeriesVideoInputs(); fillSource++)	{
        if (AtemSwitcher.getInputMEAvailability(AtemSwitcher.getVideoIndexSrc(fillSource)) & (B1 << mE))  {
          AtemSwitcher.setKeyerFillSource(mE, keyer, AtemSwitcher.getVideoIndexSrc(fillSource));
          timeout = millis();
          while (AtemSwitcher.getKeyerFillSource(mE, keyer) != AtemSwitcher.getVideoIndexSrc(fillSource) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyerFillSource(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyerFillSource(") << mE << F(", ") << keyer << F(", ") << AtemSwitcher.getVideoIndexSrc(fillSource)  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
      }
      AtemSwitcher.setKeyerFillSource(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Keyer - Key Source
      Serial << F("\nKeyer - Key Source:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyerKeySource(mE, keyer);
      for (uint16_t keySource = 0; keySource < AtemSwitcher.maxAtemSeriesVideoInputs(); keySource++)	{
        if (AtemSwitcher.getInputAvailability(AtemSwitcher.getVideoIndexSrc(keySource)) & 0x10)  {
          AtemSwitcher.setKeyerKeySource(mE, keyer, AtemSwitcher.getVideoIndexSrc(keySource));
          timeout = millis();
          while (AtemSwitcher.getKeyerKeySource(mE, keyer) != AtemSwitcher.getVideoIndexSrc(keySource) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyerKeySource(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyerKeySource(") << mE << F(", ") << keyer << F(", ") << AtemSwitcher.getVideoIndexSrc(keySource)  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
      }
      AtemSwitcher.setKeyerKeySource(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Luma - Pre Multiplied
      Serial << F("\nKey Luma - Pre Multiplied:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyLumaPreMultiplied(mE, keyer);
      for (uint8_t preMultiplied = 0; preMultiplied <= 1; preMultiplied++)	{
        AtemSwitcher.setKeyLumaPreMultiplied(mE, keyer, preMultiplied);
        timeout = millis();
        while (AtemSwitcher.getKeyLumaPreMultiplied(mE, keyer) != preMultiplied && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyLumaPreMultiplied(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyLumaPreMultiplied(") << mE << F(", ") << keyer << F(", ") << preMultiplied  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyLumaPreMultiplied(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Luma - Clip
      Serial << F("\nKey Luma - Clip:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyLumaClip(mE, keyer);
      for (uint16_t clip = 0; clip <= 1000; clip += 200)	{
        AtemSwitcher.setKeyLumaClip(mE, keyer, clip);
        timeout = millis();
        while (AtemSwitcher.getKeyLumaClip(mE, keyer) != clip && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyLumaClip(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyLumaClip(") << mE << F(", ") << keyer << F(", ") << clip  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyLumaClip(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Luma - Gain
      Serial << F("\nKey Luma - Gain:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyLumaGain(mE, keyer);
      for (uint16_t gain = 0; gain <= 1000; gain += 200)	{
        AtemSwitcher.setKeyLumaGain(mE, keyer, gain);
        timeout = millis();
        while (AtemSwitcher.getKeyLumaGain(mE, keyer) != gain && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyLumaGain(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyLumaGain(") << mE << F(", ") << keyer << F(", ") << gain  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyLumaGain(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Luma - Invert Key
      Serial << F("\nKey Luma - Invert Key:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyLumaInvertKey(mE, keyer);
      for (uint8_t invertKey = 0; invertKey <= 1; invertKey++)	{
        AtemSwitcher.setKeyLumaInvertKey(mE, keyer, invertKey);
        timeout = millis();
        while (AtemSwitcher.getKeyLumaInvertKey(mE, keyer) != invertKey && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyLumaInvertKey(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyLumaInvertKey(") << mE << F(", ") << keyer << F(", ") << invertKey  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyLumaInvertKey(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Chroma - Hue
      Serial << F("\nKey Chroma - Hue:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyChromaHue(mE, keyer);
      for (uint16_t hue = 0; hue <= 3599; hue += 720)	{
        AtemSwitcher.setKeyChromaHue(mE, keyer, hue);
        timeout = millis();
        while (AtemSwitcher.getKeyChromaHue(mE, keyer) != hue && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyChromaHue(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyChromaHue(") << mE << F(", ") << keyer << F(", ") << hue  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyChromaHue(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Chroma - Gain
      Serial << F("\nKey Chroma - Gain:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyChromaGain(mE, keyer);
      for (uint16_t gain = 0; gain <= 1000; gain += 200)	{
        AtemSwitcher.setKeyChromaGain(mE, keyer, gain);
        timeout = millis();
        while (AtemSwitcher.getKeyChromaGain(mE, keyer) != gain && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyChromaGain(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyChromaGain(") << mE << F(", ") << keyer << F(", ") << gain  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyChromaGain(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Chroma - Y Suppress
      Serial << F("\nKey Chroma - Y Suppress:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyChromaYSuppress(mE, keyer);
      for (uint16_t ySuppress = 0; ySuppress <= 1000; ySuppress += 200)	{
        AtemSwitcher.setKeyChromaYSuppress(mE, keyer, ySuppress);
        timeout = millis();
        while (AtemSwitcher.getKeyChromaYSuppress(mE, keyer) != ySuppress && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyChromaYSuppress(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyChromaYSuppress(") << mE << F(", ") << keyer << F(", ") << ySuppress  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyChromaYSuppress(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Chroma - Lift
      Serial << F("\nKey Chroma - Lift:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyChromaLift(mE, keyer);
      for (uint16_t lift = 0; lift <= 1000; lift += 200)	{
        AtemSwitcher.setKeyChromaLift(mE, keyer, lift);
        timeout = millis();
        while (AtemSwitcher.getKeyChromaLift(mE, keyer) != lift && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyChromaLift(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyChromaLift(") << mE << F(", ") << keyer << F(", ") << lift  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyChromaLift(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Chroma - Narrow
      Serial << F("\nKey Chroma - Narrow:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyChromaNarrow(mE, keyer);
      for (uint8_t narrow = 0; narrow <= 1; narrow++)	{
        AtemSwitcher.setKeyChromaNarrow(mE, keyer, narrow);
        timeout = millis();
        while (AtemSwitcher.getKeyChromaNarrow(mE, keyer) != narrow && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyChromaNarrow(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyChromaNarrow(") << mE << F(", ") << keyer << F(", ") << narrow  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyChromaNarrow(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Pattern - Pattern
      Serial << F("\nKey Pattern - Pattern:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyPatternPattern(mE, keyer);
      for (uint8_t pattern = 0; pattern <= 17; pattern += 4)	{
        AtemSwitcher.setKeyPatternPattern(mE, keyer, pattern);
        timeout = millis();
        while (AtemSwitcher.getKeyPatternPattern(mE, keyer) != pattern && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyPatternPattern(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyPatternPattern(") << mE << F(", ") << keyer << F(", ") << pattern  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyPatternPattern(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Pattern - Size
      Serial << F("\nKey Pattern - Size:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyPatternSize(mE, keyer);
      for (uint16_t size = 0; size <= 10000; size += 2000)	{
        AtemSwitcher.setKeyPatternSize(mE, keyer, size);
        timeout = millis();
        while (AtemSwitcher.getKeyPatternSize(mE, keyer) != size && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyPatternSize(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyPatternSize(") << mE << F(", ") << keyer << F(", ") << size  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyPatternSize(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Pattern - Symmetry
      Serial << F("\nKey Pattern - Symmetry:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyPatternSymmetry(mE, keyer);
      for (uint16_t symmetry = 0; symmetry <= 10000; symmetry += 2000)	{
        AtemSwitcher.setKeyPatternSymmetry(mE, keyer, symmetry);
        timeout = millis();
        while (AtemSwitcher.getKeyPatternSymmetry(mE, keyer) != symmetry && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyPatternSymmetry(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyPatternSymmetry(") << mE << F(", ") << keyer << F(", ") << symmetry  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyPatternSymmetry(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Pattern - Softness
      Serial << F("\nKey Pattern - Softness:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyPatternSoftness(mE, keyer);
      for (uint16_t softness = 0; softness <= 10000; softness += 2000)	{
        AtemSwitcher.setKeyPatternSoftness(mE, keyer, softness);
        timeout = millis();
        while (AtemSwitcher.getKeyPatternSoftness(mE, keyer) != softness && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyPatternSoftness(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyPatternSoftness(") << mE << F(", ") << keyer << F(", ") << softness  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyPatternSoftness(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Pattern - Position X
      Serial << F("\nKey Pattern - Position X:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyPatternPositionX(mE, keyer);
      for (uint16_t positionX = 0; positionX <= 10000; positionX += 2000)	{
        AtemSwitcher.setKeyPatternPositionX(mE, keyer, positionX);
        timeout = millis();
        while (AtemSwitcher.getKeyPatternPositionX(mE, keyer) != positionX && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyPatternPositionX(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyPatternPositionX(") << mE << F(", ") << keyer << F(", ") << positionX  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyPatternPositionX(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Pattern - Position Y
      Serial << F("\nKey Pattern - Position Y:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyPatternPositionY(mE, keyer);
      for (uint16_t positionY = 0; positionY <= 10000; positionY += 2000)	{
        AtemSwitcher.setKeyPatternPositionY(mE, keyer, positionY);
        timeout = millis();
        while (AtemSwitcher.getKeyPatternPositionY(mE, keyer) != positionY && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyPatternPositionY(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyPatternPositionY(") << mE << F(", ") << keyer << F(", ") << positionY  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyPatternPositionY(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Key Pattern - Invert Pattern
      Serial << F("\nKey Pattern - Invert Pattern:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getKeyPatternInvertPattern(mE, keyer);
      for (uint8_t invertPattern = 0; invertPattern <= 1; invertPattern++)	{
        AtemSwitcher.setKeyPatternInvertPattern(mE, keyer, invertPattern);
        timeout = millis();
        while (AtemSwitcher.getKeyPatternInvertPattern(mE, keyer) != invertPattern && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyPatternInvertPattern(mE, keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setKeyPatternInvertPattern(") << mE << F(", ") << keyer << F(", ") << invertPattern  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setKeyPatternInvertPattern(mE, keyer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command

      if (AtemSwitcher.getTopologyDVEs() > 0)  {
        // Key DVE - Size X
        Serial << F("\nKey DVE - Size X:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVESizeX(mE, keyer);
        for (long sizeX = 0; sizeX <= 5000; sizeX += 990)	{
          AtemSwitcher.setKeyDVESizeX(mE, keyer, sizeX);
          timeout = millis();
          while (AtemSwitcher.getKeyDVESizeX(mE, keyer) != sizeX && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVESizeX(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVESizeX(") << mE << F(", ") << keyer << F(", ") << sizeX  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVESizeX(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Size Y
        Serial << F("\nKey DVE - Size Y:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVESizeY(mE, keyer);
        for (long sizeY = 0; sizeY <= 5000; sizeY += 990)	{
          AtemSwitcher.setKeyDVESizeY(mE, keyer, sizeY);
          timeout = millis();
          while (AtemSwitcher.getKeyDVESizeY(mE, keyer) != sizeY && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVESizeY(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVESizeY(") << mE << F(", ") << keyer << F(", ") << sizeY  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVESizeY(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Position X
        Serial << F("\nKey DVE - Position X:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEPositionX(mE, keyer);
        for (long positionX = -10000; positionX <= 10000; positionX += 990)	{
          AtemSwitcher.setKeyDVEPositionX(mE, keyer, positionX);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEPositionX(mE, keyer) != positionX && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEPositionX(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEPositionX(") << mE << F(", ") << keyer << F(", ") << positionX  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEPositionX(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Position Y
        Serial << F("\nKey DVE - Position Y:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEPositionY(mE, keyer);
        for (long positionY = -10000; positionY <= 10000; positionY += 990)	{
          AtemSwitcher.setKeyDVEPositionY(mE, keyer, positionY);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEPositionY(mE, keyer) != positionY && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEPositionY(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEPositionY(") << mE << F(", ") << keyer << F(", ") << positionY  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEPositionY(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Rotation
        Serial << F("\nKey DVE - Rotation:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVERotation(mE, keyer);
        for (long rotation = -10000; rotation <= 10000; rotation += 990)	{
          AtemSwitcher.setKeyDVERotation(mE, keyer, rotation);
          timeout = millis();
          while (AtemSwitcher.getKeyDVERotation(mE, keyer) != rotation && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVERotation(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVERotation(") << mE << F(", ") << keyer << F(", ") << rotation  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVERotation(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Border Enabled
        Serial << F("\nKey DVE - Border Enabled:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEBorderEnabled(mE, keyer);
        for (uint8_t borderEnabled = 0; borderEnabled <= 1; borderEnabled++)	{
          AtemSwitcher.setKeyDVEBorderEnabled(mE, keyer, borderEnabled);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEBorderEnabled(mE, keyer) != borderEnabled && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEBorderEnabled(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEBorderEnabled(") << mE << F(", ") << keyer << F(", ") << borderEnabled  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEBorderEnabled(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Shadow
        Serial << F("\nKey DVE - Shadow:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEShadow(mE, keyer);
        for (uint8_t shadow = 0; shadow <= 1; shadow++)	{
          AtemSwitcher.setKeyDVEShadow(mE, keyer, shadow);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEShadow(mE, keyer) != shadow && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEShadow(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEShadow(") << mE << F(", ") << keyer << F(", ") << shadow  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEShadow(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Border Bevel
        Serial << F("\nKey DVE - Border Bevel:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEBorderBevel(mE, keyer);
        for (uint8_t borderBevel = 0; borderBevel <= 3; borderBevel += 1)	{
          AtemSwitcher.setKeyDVEBorderBevel(mE, keyer, borderBevel);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEBorderBevel(mE, keyer) != borderBevel && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEBorderBevel(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEBorderBevel(") << mE << F(", ") << keyer << F(", ") << borderBevel  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEBorderBevel(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Border Outer Width
        Serial << F("\nKey DVE - Border Outer Width:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEBorderOuterWidth(mE, keyer);
        for (uint16_t borderOuterWidth = 0; borderOuterWidth <= 1600; borderOuterWidth += 320)	{
          AtemSwitcher.setKeyDVEBorderOuterWidth(mE, keyer, borderOuterWidth);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEBorderOuterWidth(mE, keyer) != borderOuterWidth && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEBorderOuterWidth(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEBorderOuterWidth(") << mE << F(", ") << keyer << F(", ") << borderOuterWidth  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEBorderOuterWidth(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Border Inner Width
        Serial << F("\nKey DVE - Border Inner Width:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEBorderInnerWidth(mE, keyer);
        for (uint16_t borderInnerWidth = 0; borderInnerWidth <= 1600; borderInnerWidth += 320)	{
          AtemSwitcher.setKeyDVEBorderInnerWidth(mE, keyer, borderInnerWidth);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEBorderInnerWidth(mE, keyer) != borderInnerWidth && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEBorderInnerWidth(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEBorderInnerWidth(") << mE << F(", ") << keyer << F(", ") << borderInnerWidth  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEBorderInnerWidth(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Border Outer Softness
        Serial << F("\nKey DVE - Border Outer Softness:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEBorderOuterSoftness(mE, keyer);
        for (uint8_t borderOuterSoftness = 0; borderOuterSoftness <= 100; borderOuterSoftness += 20)	{
          AtemSwitcher.setKeyDVEBorderOuterSoftness(mE, keyer, borderOuterSoftness);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEBorderOuterSoftness(mE, keyer) != borderOuterSoftness && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEBorderOuterSoftness(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEBorderOuterSoftness(") << mE << F(", ") << keyer << F(", ") << borderOuterSoftness  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEBorderOuterSoftness(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Border Inner Softness
        Serial << F("\nKey DVE - Border Inner Softness:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEBorderInnerSoftness(mE, keyer);
        for (uint8_t borderInnerSoftness = 0; borderInnerSoftness <= 100; borderInnerSoftness += 20)	{
          AtemSwitcher.setKeyDVEBorderInnerSoftness(mE, keyer, borderInnerSoftness);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEBorderInnerSoftness(mE, keyer) != borderInnerSoftness && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEBorderInnerSoftness(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEBorderInnerSoftness(") << mE << F(", ") << keyer << F(", ") << borderInnerSoftness  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEBorderInnerSoftness(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Border Bevel Softness
        Serial << F("\nKey DVE - Border Bevel Softness:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEBorderBevelSoftness(mE, keyer);
        for (uint8_t borderBevelSoftness = 0; borderBevelSoftness <= 100; borderBevelSoftness += 20)	{
          AtemSwitcher.setKeyDVEBorderBevelSoftness(mE, keyer, borderBevelSoftness);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEBorderBevelSoftness(mE, keyer) != borderBevelSoftness && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEBorderBevelSoftness(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEBorderBevelSoftness(") << mE << F(", ") << keyer << F(", ") << borderBevelSoftness  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEBorderBevelSoftness(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Border Bevel Position
        Serial << F("\nKey DVE - Border Bevel Position:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEBorderBevelPosition(mE, keyer);
        for (uint8_t borderBevelPosition = 0; borderBevelPosition <= 100; borderBevelPosition += 20)	{
          AtemSwitcher.setKeyDVEBorderBevelPosition(mE, keyer, borderBevelPosition);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEBorderBevelPosition(mE, keyer) != borderBevelPosition && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEBorderBevelPosition(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEBorderBevelPosition(") << mE << F(", ") << keyer << F(", ") << borderBevelPosition  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEBorderBevelPosition(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Border Opacity
        Serial << F("\nKey DVE - Border Opacity:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEBorderOpacity(mE, keyer);
        for (uint16_t borderOpacity = 0; borderOpacity <= 100; borderOpacity += 20)	{
          AtemSwitcher.setKeyDVEBorderOpacity(mE, keyer, borderOpacity);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEBorderOpacity(mE, keyer) != borderOpacity && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEBorderOpacity(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEBorderOpacity(") << mE << F(", ") << keyer << F(", ") << borderOpacity  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEBorderOpacity(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Border Hue
        Serial << F("\nKey DVE - Border Hue:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEBorderHue(mE, keyer);
        for (uint16_t borderHue = 0; borderHue <= 3599; borderHue += 720)	{
          AtemSwitcher.setKeyDVEBorderHue(mE, keyer, borderHue);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEBorderHue(mE, keyer) != borderHue && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEBorderHue(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEBorderHue(") << mE << F(", ") << keyer << F(", ") << borderHue  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEBorderHue(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Border Saturation
        Serial << F("\nKey DVE - Border Saturation:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEBorderSaturation(mE, keyer);
        for (uint16_t borderSaturation = 0; borderSaturation <= 1000; borderSaturation += 200)	{
          AtemSwitcher.setKeyDVEBorderSaturation(mE, keyer, borderSaturation);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEBorderSaturation(mE, keyer) != borderSaturation && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEBorderSaturation(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEBorderSaturation(") << mE << F(", ") << keyer << F(", ") << borderSaturation  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEBorderSaturation(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Border Luma
        Serial << F("\nKey DVE - Border Luma:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVEBorderLuma(mE, keyer);
        for (uint16_t borderLuma = 0; borderLuma <= 1000; borderLuma += 200)	{
          AtemSwitcher.setKeyDVEBorderLuma(mE, keyer, borderLuma);
          timeout = millis();
          while (AtemSwitcher.getKeyDVEBorderLuma(mE, keyer) != borderLuma && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVEBorderLuma(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVEBorderLuma(") << mE << F(", ") << keyer << F(", ") << borderLuma  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVEBorderLuma(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Light Source Direction
        Serial << F("\nKey DVE - Light Source Direction:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVELightSourceDirection(mE, keyer);
        for (uint16_t lightSourceDirection = 0; lightSourceDirection <= 3590; lightSourceDirection += 718)	{
          AtemSwitcher.setKeyDVELightSourceDirection(mE, keyer, lightSourceDirection);
          timeout = millis();
          while (AtemSwitcher.getKeyDVELightSourceDirection(mE, keyer) != lightSourceDirection && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVELightSourceDirection(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVELightSourceDirection(") << mE << F(", ") << keyer << F(", ") << lightSourceDirection  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVELightSourceDirection(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Light Source Altitude
        Serial << F("\nKey DVE - Light Source Altitude:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVELightSourceAltitude(mE, keyer);
        for (uint8_t lightSourceAltitude = 10; lightSourceAltitude <= 100; lightSourceAltitude += 18)	{
          AtemSwitcher.setKeyDVELightSourceAltitude(mE, keyer, lightSourceAltitude);
          timeout = millis();
          while (AtemSwitcher.getKeyDVELightSourceAltitude(mE, keyer) != lightSourceAltitude && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVELightSourceAltitude(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVELightSourceAltitude(") << mE << F(", ") << keyer << F(", ") << lightSourceAltitude  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVELightSourceAltitude(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


        // Key DVE - Rate
        Serial << F("\nKey DVE - Rate:\n");
        printLine();
        processCommands();
        origValue = AtemSwitcher.getKeyDVERate(mE, keyer);
        for (uint8_t rate = 1; rate <= 250; rate += 50)	{
          AtemSwitcher.setKeyDVERate(mE, keyer, rate);
          timeout = millis();
          while (AtemSwitcher.getKeyDVERate(mE, keyer) != rate && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getKeyDVERate(mE, keyer);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setKeyDVERate(") << mE << F(", ") << keyer << F(", ") << rate  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
        AtemSwitcher.setKeyDVERate(mE, keyer, origValue);
        AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command
      }
    }
  }
  AtemSwitcher.runLoop(1000);
#endif


#if TEST_COLORGEN

  Serial << F("\n\n***************************\n* Color Generators: \n***************************\n");

  for (uint8_t colorGenerator = 0; colorGenerator < AtemSwitcher.getTopologyColorGenerators(); colorGenerator++)	{

    // Color Generator - Hue
    Serial << F("\nColor Generator - Hue:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getColorGeneratorHue(colorGenerator);
    for (uint16_t hue = 0; hue <= 3599; hue += 720)	{
      AtemSwitcher.setColorGeneratorHue(colorGenerator, hue);
      timeout = millis();
      while (AtemSwitcher.getColorGeneratorHue(colorGenerator) != hue && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getColorGeneratorHue(colorGenerator);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setColorGeneratorHue(") << colorGenerator << F(", ") << hue  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setColorGeneratorHue(colorGenerator, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Color Generator - Saturation
    Serial << F("\nColor Generator - Saturation:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getColorGeneratorSaturation(colorGenerator);
    for (uint16_t saturation = 0; saturation <= 1000; saturation += 200)	{
      AtemSwitcher.setColorGeneratorSaturation(colorGenerator, saturation);
      timeout = millis();
      while (AtemSwitcher.getColorGeneratorSaturation(colorGenerator) != saturation && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getColorGeneratorSaturation(colorGenerator);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setColorGeneratorSaturation(") << colorGenerator << F(", ") << saturation  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setColorGeneratorSaturation(colorGenerator, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Color Generator - Luma
    Serial << F("\nColor Generator - Luma:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getColorGeneratorLuma(colorGenerator);
    for (uint16_t luma = 0; luma <= 1000; luma += 200)	{
      AtemSwitcher.setColorGeneratorLuma(colorGenerator, luma);
      timeout = millis();
      while (AtemSwitcher.getColorGeneratorLuma(colorGenerator) != luma && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getColorGeneratorLuma(colorGenerator);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setColorGeneratorLuma(") << colorGenerator << F(", ") << luma  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setColorGeneratorLuma(colorGenerator, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command

  }
  AtemSwitcher.runLoop(1000);
#endif


#if TEST_SUPERSOURCE

  // Super Source check:
  if (AtemSwitcher.getTopologySuperSources() > 0)  {
    Serial << F("\n\n***************************\n* Super Source: \n***************************\n");

    // Super Source - Fill Source
    Serial << F("\nSuper Source - Fill Source:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceFillSource();
    for (uint16_t fillSource = 0; fillSource < AtemSwitcher.maxAtemSeriesVideoInputs(); fillSource++)	{
      if (AtemSwitcher.getInputAvailability(AtemSwitcher.getVideoIndexSrc(fillSource)) & 0x04)  {
        AtemSwitcher.setSuperSourceFillSource(AtemSwitcher.getVideoIndexSrc(fillSource));
        timeout = millis();
        while (AtemSwitcher.getSuperSourceFillSource() != AtemSwitcher.getVideoIndexSrc(fillSource) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceFillSource();
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setSuperSourceFillSource(") << AtemSwitcher.getVideoIndexSrc(fillSource)  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
      }
    }
    AtemSwitcher.setSuperSourceFillSource(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Key Source
    Serial << F("\nSuper Source - Key Source:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceKeySource();
    for (uint16_t keySource = 0; keySource < AtemSwitcher.maxAtemSeriesVideoInputs(); keySource++)	{
      if ((AtemSwitcher.getInputAvailability(AtemSwitcher.getVideoIndexSrc(keySource)) & 0x10) && AtemSwitcher.getVideoIndexSrc(keySource) != 6000)  {
        AtemSwitcher.setSuperSourceKeySource(AtemSwitcher.getVideoIndexSrc(keySource));
        timeout = millis();
        while (AtemSwitcher.getSuperSourceKeySource() != AtemSwitcher.getVideoIndexSrc(keySource) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceKeySource();
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setSuperSourceKeySource(") << AtemSwitcher.getVideoIndexSrc(keySource)  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
      }
    }
    AtemSwitcher.setSuperSourceKeySource(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Foreground
    Serial << F("\nSuper Source - Foreground:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceForeground();
    for (uint8_t foreground = 0; foreground <= 1; foreground++)	{
      AtemSwitcher.setSuperSourceForeground(foreground);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceForeground() != foreground && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceForeground();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceForeground(") << foreground  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceForeground(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Pre Multiplied
    Serial << F("\nSuper Source - Pre Multiplied:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourcePreMultiplied();
    for (uint8_t preMultiplied = 0; preMultiplied <= 1; preMultiplied++)	{
      AtemSwitcher.setSuperSourcePreMultiplied(preMultiplied);
      timeout = millis();
      while (AtemSwitcher.getSuperSourcePreMultiplied() != preMultiplied && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourcePreMultiplied();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourcePreMultiplied(") << preMultiplied  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourcePreMultiplied(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Clip
    Serial << F("\nSuper Source - Clip:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceClip();
    for (uint16_t clip = 0; clip <= 1000; clip += 200)	{
      AtemSwitcher.setSuperSourceClip(clip);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceClip() != clip && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceClip();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceClip(") << clip  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceClip(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Gain
    Serial << F("\nSuper Source - Gain:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceGain();
    for (uint16_t gain = 0; gain <= 1000; gain += 200)	{
      AtemSwitcher.setSuperSourceGain(gain);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceGain() != gain && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceGain();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceGain(") << gain  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceGain(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Invert Key
    Serial << F("\nSuper Source - Invert Key:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceInvertKey();
    for (uint8_t invertKey = 0; invertKey <= 1; invertKey++)	{
      AtemSwitcher.setSuperSourceInvertKey(invertKey);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceInvertKey() != invertKey && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceInvertKey();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceInvertKey(") << invertKey  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceInvertKey(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Border Enabled
    Serial << F("\nSuper Source - Border Enabled:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceBorderEnabled();
    for (uint8_t borderEnabled = 0; borderEnabled <= 1; borderEnabled++)	{
      AtemSwitcher.setSuperSourceBorderEnabled(borderEnabled);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceBorderEnabled() != borderEnabled && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBorderEnabled();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceBorderEnabled(") << borderEnabled  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceBorderEnabled(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Border Bevel
    Serial << F("\nSuper Source - Border Bevel:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceBorderBevel();
    for (uint8_t borderBevel = 0; borderBevel <= 3; borderBevel += 1)	{
      AtemSwitcher.setSuperSourceBorderBevel(borderBevel);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceBorderBevel() != borderBevel && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBorderBevel();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceBorderBevel(") << borderBevel  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceBorderBevel(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Border Outer Width
    Serial << F("\nSuper Source - Border Outer Width:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceBorderOuterWidth();
    for (uint16_t borderOuterWidth = 0; borderOuterWidth <= 1600; borderOuterWidth += 320)	{
      AtemSwitcher.setSuperSourceBorderOuterWidth(borderOuterWidth);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceBorderOuterWidth() != borderOuterWidth && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBorderOuterWidth();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceBorderOuterWidth(") << borderOuterWidth  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceBorderOuterWidth(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Border Inner Width
    Serial << F("\nSuper Source - Border Inner Width:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceBorderInnerWidth();
    for (uint16_t borderInnerWidth = 0; borderInnerWidth <= 1600; borderInnerWidth += 320)	{
      AtemSwitcher.setSuperSourceBorderInnerWidth(borderInnerWidth);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceBorderInnerWidth() != borderInnerWidth && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBorderInnerWidth();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceBorderInnerWidth(") << borderInnerWidth  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceBorderInnerWidth(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Border Outer Softness
    Serial << F("\nSuper Source - Border Outer Softness:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceBorderOuterSoftness();
    for (uint8_t borderOuterSoftness = 0; borderOuterSoftness <= 100; borderOuterSoftness += 20)	{
      AtemSwitcher.setSuperSourceBorderOuterSoftness(borderOuterSoftness);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceBorderOuterSoftness() != borderOuterSoftness && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBorderOuterSoftness();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceBorderOuterSoftness(") << borderOuterSoftness  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceBorderOuterSoftness(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Border Inner Softness
    Serial << F("\nSuper Source - Border Inner Softness:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceBorderInnerSoftness();
    for (uint8_t borderInnerSoftness = 0; borderInnerSoftness <= 100; borderInnerSoftness += 20)	{
      AtemSwitcher.setSuperSourceBorderInnerSoftness(borderInnerSoftness);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceBorderInnerSoftness() != borderInnerSoftness && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBorderInnerSoftness();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceBorderInnerSoftness(") << borderInnerSoftness  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceBorderInnerSoftness(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Border Bevel Softness
    Serial << F("\nSuper Source - Border Bevel Softness:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceBorderBevelSoftness();
    for (uint8_t borderBevelSoftness = 0; borderBevelSoftness <= 100; borderBevelSoftness += 20)	{
      AtemSwitcher.setSuperSourceBorderBevelSoftness(borderBevelSoftness);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceBorderBevelSoftness() != borderBevelSoftness && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBorderBevelSoftness();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceBorderBevelSoftness(") << borderBevelSoftness  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceBorderBevelSoftness(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Border Bevel Position
    Serial << F("\nSuper Source - Border Bevel Position:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceBorderBevelPosition();
    for (uint8_t borderBevelPosition = 0; borderBevelPosition <= 100; borderBevelPosition += 20)	{
      AtemSwitcher.setSuperSourceBorderBevelPosition(borderBevelPosition);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceBorderBevelPosition() != borderBevelPosition && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBorderBevelPosition();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceBorderBevelPosition(") << borderBevelPosition  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceBorderBevelPosition(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Border Hue
    Serial << F("\nSuper Source - Border Hue:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceBorderHue();
    for (uint16_t borderHue = 0; borderHue <= 3599; borderHue += 720)	{
      AtemSwitcher.setSuperSourceBorderHue(borderHue);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceBorderHue() != borderHue && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBorderHue();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceBorderHue(") << borderHue  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceBorderHue(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Border Saturation
    Serial << F("\nSuper Source - Border Saturation:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceBorderSaturation();
    for (uint16_t borderSaturation = 0; borderSaturation <= 1000; borderSaturation += 200)	{
      AtemSwitcher.setSuperSourceBorderSaturation(borderSaturation);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceBorderSaturation() != borderSaturation && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBorderSaturation();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceBorderSaturation(") << borderSaturation  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceBorderSaturation(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Border Luma
    Serial << F("\nSuper Source - Border Luma:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceBorderLuma();
    for (uint16_t borderLuma = 0; borderLuma <= 1000; borderLuma += 200)	{
      AtemSwitcher.setSuperSourceBorderLuma(borderLuma);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceBorderLuma() != borderLuma && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBorderLuma();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceBorderLuma(") << borderLuma  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceBorderLuma(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Light Source Direction
    Serial << F("\nSuper Source - Light Source Direction:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceLightSourceDirection();
    for (uint16_t lightSourceDirection = 0; lightSourceDirection <= 3590; lightSourceDirection += 718)	{
      AtemSwitcher.setSuperSourceLightSourceDirection(lightSourceDirection);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceLightSourceDirection() != lightSourceDirection && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceLightSourceDirection();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceLightSourceDirection(") << lightSourceDirection  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceLightSourceDirection(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Super Source - Light Source Altitude
    Serial << F("\nSuper Source - Light Source Altitude:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getSuperSourceLightSourceAltitude();
    for (uint8_t lightSourceAltitude = 10; lightSourceAltitude <= 100; lightSourceAltitude += 18)	{
      AtemSwitcher.setSuperSourceLightSourceAltitude(lightSourceAltitude);
      timeout = millis();
      while (AtemSwitcher.getSuperSourceLightSourceAltitude() != lightSourceAltitude && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceLightSourceAltitude();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setSuperSourceLightSourceAltitude(") << lightSourceAltitude  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setSuperSourceLightSourceAltitude(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    for (uint8_t box = 0; box < AtemSwitcher.getSuperSourceConfigBoxes(); box++)	{

      // Super Source Box Parameters - Enabled
      Serial << F("\nSuper Source Box Parameters - Enabled:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getSuperSourceBoxParametersEnabled(box);
      for (uint8_t enabled = 0; enabled <= 1; enabled++)	{
        AtemSwitcher.setSuperSourceBoxParametersEnabled(box, enabled);
        timeout = millis();
        while (AtemSwitcher.getSuperSourceBoxParametersEnabled(box) != enabled && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBoxParametersEnabled(box);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setSuperSourceBoxParametersEnabled(") << box << F(", ") << enabled  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setSuperSourceBoxParametersEnabled(box, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Super Source Box Parameters - Input Source
      Serial << F("\nSuper Source Box Parameters - Input Source:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getSuperSourceBoxParametersInputSource(box);
      for (uint16_t inputSource = 0; inputSource < AtemSwitcher.maxAtemSeriesVideoInputs(); inputSource++)	{
        if (AtemSwitcher.getInputAvailability(AtemSwitcher.getVideoIndexSrc(inputSource)) & 0x08)  {
          AtemSwitcher.setSuperSourceBoxParametersInputSource(box, AtemSwitcher.getVideoIndexSrc(inputSource));
          timeout = millis();
          while (AtemSwitcher.getSuperSourceBoxParametersInputSource(box) != AtemSwitcher.getVideoIndexSrc(inputSource) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBoxParametersInputSource(box);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setSuperSourceBoxParametersInputSource(") << box << F(", ") << AtemSwitcher.getVideoIndexSrc(inputSource)  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
      }
      AtemSwitcher.setSuperSourceBoxParametersInputSource(box, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Super Source Box Parameters - Position X
      Serial << F("\nSuper Source Box Parameters - Position X:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getSuperSourceBoxParametersPositionX(box);
      for (int positionX = -4800; positionX <= 4800; positionX += 1920)	{
        AtemSwitcher.setSuperSourceBoxParametersPositionX(box, positionX);
        timeout = millis();
        while (AtemSwitcher.getSuperSourceBoxParametersPositionX(box) != positionX && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBoxParametersPositionX(box);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setSuperSourceBoxParametersPositionX(") << box << F(", ") << positionX  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setSuperSourceBoxParametersPositionX(box, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Super Source Box Parameters - Position Y
      Serial << F("\nSuper Source Box Parameters - Position Y:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getSuperSourceBoxParametersPositionY(box);
      for (int positionY = -2700; positionY <= 2700; positionY += 1080)	{
        AtemSwitcher.setSuperSourceBoxParametersPositionY(box, positionY);
        timeout = millis();
        while (AtemSwitcher.getSuperSourceBoxParametersPositionY(box) != positionY && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBoxParametersPositionY(box);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setSuperSourceBoxParametersPositionY(") << box << F(", ") << positionY  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setSuperSourceBoxParametersPositionY(box, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Super Source Box Parameters - Size
      Serial << F("\nSuper Source Box Parameters - Size:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getSuperSourceBoxParametersSize(box);
      for (uint16_t size = 70; size <= 1000; size += 186)	{
        AtemSwitcher.setSuperSourceBoxParametersSize(box, size);
        timeout = millis();
        while (AtemSwitcher.getSuperSourceBoxParametersSize(box) != size && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBoxParametersSize(box);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setSuperSourceBoxParametersSize(") << box << F(", ") << size  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setSuperSourceBoxParametersSize(box, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Super Source Box Parameters - Cropped
      Serial << F("\nSuper Source Box Parameters - Cropped:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getSuperSourceBoxParametersCropped(box);
      for (uint8_t cropped = 0; cropped <= 1; cropped++)	{
        AtemSwitcher.setSuperSourceBoxParametersCropped(box, cropped);
        timeout = millis();
        while (AtemSwitcher.getSuperSourceBoxParametersCropped(box) != cropped && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBoxParametersCropped(box);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setSuperSourceBoxParametersCropped(") << box << F(", ") << cropped  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setSuperSourceBoxParametersCropped(box, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Super Source Box Parameters - Crop Top
      Serial << F("\nSuper Source Box Parameters - Crop Top:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getSuperSourceBoxParametersCropTop(box);
      for (uint16_t cropTop = 0; cropTop <= 18000; cropTop += 3600)	{
        AtemSwitcher.setSuperSourceBoxParametersCropTop(box, cropTop);
        timeout = millis();
        while (AtemSwitcher.getSuperSourceBoxParametersCropTop(box) != cropTop && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBoxParametersCropTop(box);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setSuperSourceBoxParametersCropTop(") << box << F(", ") << cropTop  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setSuperSourceBoxParametersCropTop(box, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Super Source Box Parameters - Crop Bottom
      Serial << F("\nSuper Source Box Parameters - Crop Bottom:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getSuperSourceBoxParametersCropBottom(box);
      for (uint16_t cropBottom = 0; cropBottom <= 18000; cropBottom += 3600)	{
        AtemSwitcher.setSuperSourceBoxParametersCropBottom(box, cropBottom);
        timeout = millis();
        while (AtemSwitcher.getSuperSourceBoxParametersCropBottom(box) != cropBottom && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBoxParametersCropBottom(box);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setSuperSourceBoxParametersCropBottom(") << box << F(", ") << cropBottom  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setSuperSourceBoxParametersCropBottom(box, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Super Source Box Parameters - Crop Left
      Serial << F("\nSuper Source Box Parameters - Crop Left:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getSuperSourceBoxParametersCropLeft(box);
      for (uint16_t cropLeft = 0; cropLeft <= 32000; cropLeft += 6400)	{
        AtemSwitcher.setSuperSourceBoxParametersCropLeft(box, cropLeft);
        timeout = millis();
        while (AtemSwitcher.getSuperSourceBoxParametersCropLeft(box) != cropLeft && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBoxParametersCropLeft(box);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setSuperSourceBoxParametersCropLeft(") << box << F(", ") << cropLeft  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setSuperSourceBoxParametersCropLeft(box, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Super Source Box Parameters - Crop Right
      Serial << F("\nSuper Source Box Parameters - Crop Right:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getSuperSourceBoxParametersCropRight(box);
      for (uint16_t cropRight = 0; cropRight <= 32000; cropRight += 6400)	{
        AtemSwitcher.setSuperSourceBoxParametersCropRight(box, cropRight);
        timeout = millis();
        while (AtemSwitcher.getSuperSourceBoxParametersCropRight(box) != cropRight && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getSuperSourceBoxParametersCropRight(box);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setSuperSourceBoxParametersCropRight(") << box << F(", ") << cropRight  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setSuperSourceBoxParametersCropRight(box, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command

    }
  }
  AtemSwitcher.runLoop(1000);
#endif


#if TEST_MP
  for (uint8_t mediaPlayer = 0; mediaPlayer <= 1; mediaPlayer++)	{

    uint8_t mpType = AtemSwitcher.getMediaPlayerSourceType(mediaPlayer);
    uint8_t stillIdx = AtemSwitcher.getMediaPlayerSourceStillIndex(mediaPlayer);
    uint8_t clipIdx = AtemSwitcher.getMediaPlayerSourceClipIndex(mediaPlayer);
    bool loopFlag = AtemSwitcher.getClipPlayerLoop(mediaPlayer);

    // Media Player Source - Type
    Serial << F("\nMedia Player Source - Type:\n");
    printLine();
    processCommands();
    for (uint8_t type = 1; type <= 2; type += 1)	{
      if (type != 2 && AtemSwitcher.getMediaPlayersClipBanks() > 0)  {
        AtemSwitcher.setMediaPlayerSourceType(mediaPlayer, type);
        timeout = millis();
        while (AtemSwitcher.getMediaPlayerSourceType(mediaPlayer) != type && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getMediaPlayerSourceType(mediaPlayer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setMediaPlayerSourceType(") << mediaPlayer << F(", ") << type  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
    }
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Media Player Source - Still Index
    Serial << F("\nMedia Player Source - Still Index:\n");
    printLine();
    processCommands();
    AtemSwitcher.setMediaPlayerSourceType(mediaPlayer, 1);  // Setting MP to stills.
    AtemSwitcher.runLoop(100);
    for (uint8_t stillIndex = 0; stillIndex < AtemSwitcher.getMediaPlayersStillBanks(); stillIndex++)	{
      AtemSwitcher.setMediaPlayerSourceStillIndex(mediaPlayer, stillIndex);
      timeout = millis();
      while (AtemSwitcher.getMediaPlayerSourceStillIndex(mediaPlayer) != stillIndex && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getMediaPlayerSourceStillIndex(mediaPlayer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setMediaPlayerSourceStillIndex(") << mediaPlayer << F(", ") << stillIndex  << F("): ")
             << (AtemSwitcher.getMediaPlayerStillFilesIsUsed(stillIndex) ? F("") : F("[No file]"))
             << AtemSwitcher.getMediaPlayerStillFilesFileName(stillIndex) << F(": ")
             << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command

    if (AtemSwitcher.getMediaPlayersClipBanks() > 0)  {
      // Media Player Source - Clip Index
      Serial << F("\nMedia Player Source - Clip Index:\n");
      printLine();
      processCommands();
      AtemSwitcher.setMediaPlayerSourceType(mediaPlayer, 2);  // Setting MP to clips.
      AtemSwitcher.runLoop(100);
      for (uint8_t clipIndex = 0; clipIndex < AtemSwitcher.getMediaPlayersClipBanks(); clipIndex++)	{
        AtemSwitcher.setMediaPlayerSourceClipIndex(mediaPlayer, clipIndex);
        timeout = millis();
        while (AtemSwitcher.getMediaPlayerSourceClipIndex(mediaPlayer) != clipIndex && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getMediaPlayerSourceClipIndex(mediaPlayer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setMediaPlayerSourceClipIndex(") << mediaPlayer << F(", ") << clipIndex  << F("): ")
               << (AtemSwitcher.getMediaPlayerClipSourceIsUsed(clipIndex) ? F("") : F("[No file]"))
               << AtemSwitcher.getMediaPlayerClipSourceFileName(clipIndex) << F(", ") << AtemSwitcher.getMediaPlayerClipSourceFrames(clipIndex) << F(" frames: ")
               << (AtemSwitcher.getMediaPlayerAudioSourceIsUsed(clipIndex + 1) ? F("Audio: ") : F(":")) << AtemSwitcher.getMediaPlayerAudioSourceFileName(clipIndex + 1) << F(": ")
               << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command

      AtemSwitcher.setMediaPlayerSourceClipIndex(mediaPlayer, 0);
      AtemSwitcher.runLoop(100);

      // Clip Player Status - Loop
      Serial << F("\nClip Player Status - Loop:\n");
      printLine();
      processCommands();
      for (uint8_t loop = 0; loop <= 1; loop++)	{
        AtemSwitcher.setClipPlayerLoop(mediaPlayer, loop);
        timeout = millis();
        while (AtemSwitcher.getClipPlayerLoop(mediaPlayer) != loop && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getClipPlayerLoop(mediaPlayer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setClipPlayerLoop(") << mediaPlayer << F(", ") << loop  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      // Clip Player Status - Playing
      Serial << F("\nClip Player Status - Playing:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getClipPlayerPlaying(mediaPlayer);
      for (uint8_t playing = 0; playing <= 1; playing++)	{
        AtemSwitcher.setClipPlayerPlaying(mediaPlayer, playing);
        timeout = millis();
        while (AtemSwitcher.getClipPlayerPlaying(mediaPlayer) != playing && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getClipPlayerPlaying(mediaPlayer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setClipPlayerPlaying(") << mediaPlayer << F(", ") << playing  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(500);	// Short delay between values
      }
      AtemSwitcher.setClipPlayerPlaying(mediaPlayer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


      AtemSwitcher.setClipPlayerLoop(mediaPlayer, true);
      AtemSwitcher.runLoop(100);
      AtemSwitcher.setClipPlayerPlaying(mediaPlayer, true);
      AtemSwitcher.runLoop(100);
      Serial << F("Playing...") << F("\n");
      timeout = millis();
      while (!AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      AtemSwitcher.setClipPlayerPlaying(mediaPlayer, false);
      AtemSwitcher.runLoop(1000);


      // Clip Player Status - Clip Frame
      Serial << F("\nClip Player Status - Clip Frame:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getClipPlayerClipFrame(mediaPlayer);
      for (uint16_t clipFrame = 0; clipFrame <= 50; clipFrame += 10)	{
        AtemSwitcher.setClipPlayerClipFrame(mediaPlayer, clipFrame);
        timeout = millis();
        while (AtemSwitcher.getClipPlayerClipFrame(mediaPlayer) != clipFrame && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getClipPlayerClipFrame(mediaPlayer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setClipPlayerClipFrame(") << mediaPlayer << F(", ") << clipFrame  << F("): ")
               << F("getClipPlayerAtBeginning() = ") << AtemSwitcher.getClipPlayerAtBeginning(mediaPlayer) << F(": ")
               << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
      AtemSwitcher.setClipPlayerAtBeginning(mediaPlayer, true);
      timeout = millis();
      while (AtemSwitcher.getClipPlayerClipFrame(mediaPlayer) != 0 && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getClipPlayerClipFrame(mediaPlayer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setClipPlayerAtBeginning(") << mediaPlayer << F(", true): ")
             << (millis() - timeout) << F(" ms\n");

      AtemSwitcher.setClipPlayerClipFrame(mediaPlayer, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command
    }

    // Resetting:
    AtemSwitcher.setMediaPlayerSourceType(mediaPlayer, mpType);
    AtemSwitcher.setMediaPlayerSourceStillIndex(mediaPlayer, stillIdx);
    AtemSwitcher.setMediaPlayerSourceClipIndex(mediaPlayer, clipIdx);
    AtemSwitcher.setClipPlayerLoop(mediaPlayer, loopFlag);

    AtemSwitcher.runLoop(100);
  }
  AtemSwitcher.runLoop(1000);
#endif


#if TEST_DSK
  Serial << F("\n\n***************************\n* Downstream Keyers: \n***************************\n");

  for (uint8_t keyer = 0; keyer < AtemSwitcher.getTopologyDownstreamKeyes(); keyer++)	{

    // Downstream Keyer - Auto
    Serial << F("\nDownstream Keyer - Auto:\n");
    printLine();
    processCommands();

    bool dskEnabled = AtemSwitcher.getDownstreamKeyerOnAir(keyer);
    uint8_t rate = AtemSwitcher.getDownstreamKeyerRate(keyer);
    AtemSwitcher.setDownstreamKeyerRate(keyer, 100);
    AtemSwitcher.runLoop(200);
    AtemSwitcher.performDownstreamKeyerAutoKeyer(keyer);
    AtemSwitcher.runLoop(200);
    while (AtemSwitcher.getDownstreamKeyerInTransition(keyer))  {
      Serial << F("Frames remaining: ") << AtemSwitcher.getDownstreamKeyerFramesRemaining(keyer) <<
             (AtemSwitcher.getDownstreamKeyerIsAutoTransitioning(keyer) ? F(" - AUTO flag = true") : F("- ERROR: AUTO flag not set!")) << F("\n");
      AtemSwitcher.runLoop(100);
    }

    Serial << F("\n");
    AtemSwitcher.setDownstreamKeyerRate(keyer, 50);
    bool oldTie = AtemSwitcher.getDownstreamKeyerTie(keyer);
    AtemSwitcher.setDownstreamKeyerTie(keyer, true);
    AtemSwitcher.runLoop(200);
    if (AtemSwitcher.getDownstreamKeyerOnAir(keyer) != dskEnabled)  {
      AtemSwitcher.performAutoME(0);
      AtemSwitcher.runLoop(100);
      while (AtemSwitcher.getDownstreamKeyerInTransition(keyer))  {
        Serial << F("Frames remaining: ") << AtemSwitcher.getDownstreamKeyerFramesRemaining(keyer) <<
               (!AtemSwitcher.getDownstreamKeyerIsAutoTransitioning(keyer) ? F(" - AUTO flag = false") : F("- ERROR: AUTO flag set!")) << F("\n");
        AtemSwitcher.runLoop(100);
      }
    }
    else {
      Serial << F("ERROR: DSK auto didn't work right\n");
    }
    AtemSwitcher.setDownstreamKeyerRate(keyer, rate);
    AtemSwitcher.setDownstreamKeyerTie(keyer, oldTie);

    // Downstream Keyer - Auto
    Serial << F("\nDownstream Keyer - On Air:\n");
    printLine();
    processCommands();
    dskEnabled = AtemSwitcher.getDownstreamKeyerOnAir(keyer);
    AtemSwitcher.setDownstreamKeyerOnAir(keyer, !dskEnabled);
    timeout = millis();
    while (AtemSwitcher.getDownstreamKeyerOnAir(keyer) != !dskEnabled && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
      AtemSwitcher.runLoop();
    }
    if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
      Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerOnAir(keyer);
    }
    else {
      Serial << F("OK");
    }
    Serial << F(" for setDownstreamKeyerOnAir(") << keyer << F(", ") << !dskEnabled << F("): ") << (millis() - timeout) << F(" ms\n");
    AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    AtemSwitcher.setDownstreamKeyerOnAir(keyer, dskEnabled);



    // Downstream Keyer - Fill Source
    Serial << F("\nDownstream Keyer - Fill Source:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownstreamKeyerFillSource(keyer);
    for (uint16_t fillSource = 0; fillSource < AtemSwitcher.maxAtemSeriesVideoInputs(); fillSource++)	{
      if (AtemSwitcher.getInputMEAvailability(AtemSwitcher.getVideoIndexSrc(fillSource)) & B1)  {
        AtemSwitcher.setDownstreamKeyerFillSource(keyer, AtemSwitcher.getVideoIndexSrc(fillSource));
        timeout = millis();
        while (AtemSwitcher.getDownstreamKeyerFillSource(keyer) != AtemSwitcher.getVideoIndexSrc(fillSource) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerFillSource(keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setDownstreamKeyerFillSource(") << keyer << F(", ") << AtemSwitcher.getVideoIndexSrc(fillSource)  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
    }
    AtemSwitcher.setDownstreamKeyerFillSource(keyer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Downstream Keyer - Key Source
    Serial << F("\nDownstream Keyer - Key Source:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownstreamKeyerKeySource(keyer);
    for (uint16_t keySource = 0; keySource < AtemSwitcher.maxAtemSeriesVideoInputs(); keySource++)	{
      if (AtemSwitcher.getInputAvailability(AtemSwitcher.getVideoIndexSrc(keySource)) & 0x10)  {  // Source on keys
        AtemSwitcher.setDownstreamKeyerKeySource(keyer, AtemSwitcher.getVideoIndexSrc(keySource));
        timeout = millis();
        while (AtemSwitcher.getDownstreamKeyerKeySource(keyer) != AtemSwitcher.getVideoIndexSrc(keySource) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerKeySource(keyer);
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setDownstreamKeyerKeySource(") << keyer << F(", ") << AtemSwitcher.getVideoIndexSrc(keySource)  << F("): ") << (millis() - timeout) << F(" ms\n");
        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
    }
    AtemSwitcher.setDownstreamKeyerKeySource(keyer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Downstream Keyer - Tie
    Serial << F("\nDownstream Keyer - Tie:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownstreamKeyerTie(keyer);
    for (uint8_t tie = 0; tie <= 1; tie++)	{
      AtemSwitcher.setDownstreamKeyerTie(keyer, tie);
      timeout = millis();
      while (AtemSwitcher.getDownstreamKeyerTie(keyer) != tie && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerTie(keyer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setDownstreamKeyerTie(") << keyer << F(", ") << tie  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setDownstreamKeyerTie(keyer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Downstream Keyer - Rate
    Serial << F("\nDownstream Keyer - Rate:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownstreamKeyerRate(keyer);
    for (uint8_t rate = 1; rate <= 250; rate += 50)	{
      AtemSwitcher.setDownstreamKeyerRate(keyer, rate);
      timeout = millis();
      while (AtemSwitcher.getDownstreamKeyerRate(keyer) != rate && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerRate(keyer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setDownstreamKeyerRate(") << keyer << F(", ") << rate  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setDownstreamKeyerRate(keyer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Downstream Keyer - Pre Multiplied
    Serial << F("\nDownstream Keyer - Pre Multiplied:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownstreamKeyerPreMultiplied(keyer);
    for (uint8_t preMultiplied = 0; preMultiplied <= 1; preMultiplied++)	{
      AtemSwitcher.setDownstreamKeyerPreMultiplied(keyer, preMultiplied);
      timeout = millis();
      while (AtemSwitcher.getDownstreamKeyerPreMultiplied(keyer) != preMultiplied && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerPreMultiplied(keyer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setDownstreamKeyerPreMultiplied(") << keyer << F(", ") << preMultiplied  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setDownstreamKeyerPreMultiplied(keyer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Downstream Keyer - Clip
    Serial << F("\nDownstream Keyer - Clip:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownstreamKeyerClip(keyer);
    for (uint16_t clip = 0; clip <= 1000; clip += 200)	{
      AtemSwitcher.setDownstreamKeyerClip(keyer, clip);
      timeout = millis();
      while (AtemSwitcher.getDownstreamKeyerClip(keyer) != clip && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerClip(keyer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setDownstreamKeyerClip(") << keyer << F(", ") << clip  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setDownstreamKeyerClip(keyer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Downstream Keyer - Gain
    Serial << F("\nDownstream Keyer - Gain:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownstreamKeyerGain(keyer);
    for (uint16_t gain = 0; gain <= 1000; gain += 200)	{
      AtemSwitcher.setDownstreamKeyerGain(keyer, gain);
      timeout = millis();
      while (AtemSwitcher.getDownstreamKeyerGain(keyer) != gain && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerGain(keyer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setDownstreamKeyerGain(") << keyer << F(", ") << gain  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setDownstreamKeyerGain(keyer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Downstream Keyer - Invert Key(??)
    Serial << F("\nDownstream Keyer - Invert Key(??):\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownstreamKeyerInvertKey(keyer);
    for (uint8_t invertKey = 0; invertKey <= 1; invertKey++)	{
      AtemSwitcher.setDownstreamKeyerInvertKey(keyer, invertKey);
      timeout = millis();
      while (AtemSwitcher.getDownstreamKeyerInvertKey(keyer) != invertKey && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerInvertKey(keyer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setDownstreamKeyerInvertKey(") << keyer << F(", ") << invertKey  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setDownstreamKeyerInvertKey(keyer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Downstream Keyer - Masked
    Serial << F("\nDownstream Keyer - Masked:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownstreamKeyerMasked(keyer);
    for (uint8_t masked = 0; masked <= 1; masked++)	{
      AtemSwitcher.setDownstreamKeyerMasked(keyer, masked);
      timeout = millis();
      while (AtemSwitcher.getDownstreamKeyerMasked(keyer) != masked && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerMasked(keyer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setDownstreamKeyerMasked(") << keyer << F(", ") << masked  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setDownstreamKeyerMasked(keyer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Downstream Keyer - Top
    Serial << F("\nDownstream Keyer - Top:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownstreamKeyerTop(keyer);
    for (int top = -9000; top <= 9000; top += 3600)	{
      AtemSwitcher.setDownstreamKeyerTop(keyer, top);
      timeout = millis();
      while (AtemSwitcher.getDownstreamKeyerTop(keyer) != top && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerTop(keyer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setDownstreamKeyerTop(") << keyer << F(", ") << top  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setDownstreamKeyerTop(keyer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Downstream Keyer - Bottom
    Serial << F("\nDownstream Keyer - Bottom:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownstreamKeyerBottom(keyer);
    for (int bottom = -9000; bottom <= 9000; bottom += 3600)	{
      AtemSwitcher.setDownstreamKeyerBottom(keyer, bottom);
      timeout = millis();
      while (AtemSwitcher.getDownstreamKeyerBottom(keyer) != bottom && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerBottom(keyer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setDownstreamKeyerBottom(") << keyer << F(", ") << bottom  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setDownstreamKeyerBottom(keyer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Downstream Keyer - Left
    Serial << F("\nDownstream Keyer - Left:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownstreamKeyerLeft(keyer);
    for (int left = -16000; left <= 16000; left += 6400)	{
      AtemSwitcher.setDownstreamKeyerLeft(keyer, left);
      timeout = millis();
      while (AtemSwitcher.getDownstreamKeyerLeft(keyer) != left && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerLeft(keyer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setDownstreamKeyerLeft(") << keyer << F(", ") << left  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setDownstreamKeyerLeft(keyer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Downstream Keyer - Right
    Serial << F("\nDownstream Keyer - Right:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownstreamKeyerRight(keyer);
    for (int right = -16000; right <= 16000; right += 6400)	{
      AtemSwitcher.setDownstreamKeyerRight(keyer, right);
      timeout = millis();
      while (AtemSwitcher.getDownstreamKeyerRight(keyer) != right && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownstreamKeyerRight(keyer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setDownstreamKeyerRight(") << keyer << F(", ") << right  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setDownstreamKeyerRight(keyer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command

  }
  AtemSwitcher.runLoop(1000);
#endif



#if TEST_FLY
  Serial << F("\n\n***************************\n* Fly: \n***************************\n");

  if (AtemSwitcher.getTopologyDVEs() > 0)  {

    // Save settings:
    long temp_sizeX = AtemSwitcher.getKeyDVESizeX(0, 0);
    long temp_sizeY = AtemSwitcher.getKeyDVESizeY(0, 0);
    long temp_posX = AtemSwitcher.getKeyDVEPositionX(0, 0);
    long temp_posY = AtemSwitcher.getKeyDVEPositionY(0, 0);
    long temp_rot = AtemSwitcher.getKeyDVERotation(0, 0);
    uint16_t temp_oW = AtemSwitcher.getKeyDVEBorderOuterWidth(0, 0);
    uint16_t temp_iW = AtemSwitcher.getKeyDVEBorderInnerWidth(0, 0);
    uint16_t temp_oS = AtemSwitcher.getKeyDVEBorderOuterSoftness(0, 0);
    uint16_t temp_iS = AtemSwitcher.getKeyDVEBorderInnerSoftness(0, 0);
    uint16_t temp_bS = AtemSwitcher.getKeyDVEBorderBevelSoftness(0, 0);
    uint16_t temp_bP = AtemSwitcher.getKeyDVEBorderBevelPosition(0, 0);
    uint16_t temp_bO = AtemSwitcher.getKeyDVEBorderOpacity(0, 0);
    uint16_t temp_bH = AtemSwitcher.getKeyDVEBorderHue(0, 0);
    uint16_t temp_bSat = AtemSwitcher.getKeyDVEBorderSaturation(0, 0);
    uint16_t temp_bL = AtemSwitcher.getKeyDVEBorderLuma(0, 0);
    uint16_t temp_lSD = AtemSwitcher.getKeyDVELightSourceDirection(0, 0);
    uint16_t temp_lSA = AtemSwitcher.getKeyDVELightSourceAltitude(0, 0);
    AtemSwitcher.runLoop(1000);


    Serial << F("Keyframe A Set? ") << (AtemSwitcher.getKeyerFlyIsASet(0, 0) ? F("Yes") : F("No")) << F("\n");
    Serial << F("Keyframe B Set? ") << (AtemSwitcher.getKeyerFlyIsBSet(0, 0) ? F("Yes") : F("No")) << F("\n");
    Serial << F("Is At Keyframe? ") << _BINPADL(AtemSwitcher.getKeyerFlyIsAtKeyFrame(0, 0), 5, "0") << F("\n");

    // Set up keyframe A:
    Serial << F("\nSet up keyframe A:\n");
    printLine();

    AtemSwitcher.commandBundleStart();
    AtemSwitcher.setKeyDVESizeX(0, 0, random(100, 3000));
    AtemSwitcher.setKeyDVESizeY(0, 0, random(100, 3000));
    AtemSwitcher.setKeyDVEPositionX(0, 0, random(-10000, 10000));
    AtemSwitcher.setKeyDVEPositionY(0, 0, random(-10000, 10000));
    AtemSwitcher.setKeyDVERotation(0, 0, random(0, 10000));
    AtemSwitcher.setKeyDVEBorderOuterWidth(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderInnerWidth(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderOuterSoftness(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderInnerSoftness(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderBevelSoftness(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderBevelPosition(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderOpacity(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderHue(0, 0, random(0, 3550));
    AtemSwitcher.setKeyDVEBorderSaturation(0, 0, random(0, 1000));
    AtemSwitcher.setKeyDVEBorderLuma(0, 0, random(0, 1000));
    AtemSwitcher.setKeyDVELightSourceDirection(0, 0, random(0, 2000));
    AtemSwitcher.setKeyDVELightSourceAltitude(0, 0, random(10, 100));
    AtemSwitcher.commandBundleEnd();

    AtemSwitcher.setKeyerFlyKeyFrame(0, 0, 1);  // Stored in Keyframe A now
    AtemSwitcher.runLoop(1000);

    Serial << F("Keyframe A Set? ") << (AtemSwitcher.getKeyerFlyIsASet(0, 0) ? F("Yes") : F("No")) << F("\n");
    Serial << F("Keyframe B Set? ") << (AtemSwitcher.getKeyerFlyIsBSet(0, 0) ? F("Yes") : F("No")) << F("\n");
    Serial << F("Is At Keyframe? ") << _BINPADL(AtemSwitcher.getKeyerFlyIsAtKeyFrame(0, 0), 5, "0") << F("\n");

    // Set up keyframe B:
    Serial << F("\nSet up keyframe B:\n");
    printLine();

    AtemSwitcher.commandBundleStart();
    AtemSwitcher.setKeyDVESizeX(0, 0, random(100, 3000));
    AtemSwitcher.setKeyDVESizeY(0, 0, random(100, 3000));
    AtemSwitcher.setKeyDVEPositionX(0, 0, random(-10000, 10000));
    AtemSwitcher.setKeyDVEPositionY(0, 0, random(-10000, 10000));
    AtemSwitcher.setKeyDVERotation(0, 0, random(0, 10000));
    AtemSwitcher.setKeyDVEBorderOuterWidth(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderInnerWidth(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderOuterSoftness(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderInnerSoftness(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderBevelSoftness(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderBevelPosition(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderOpacity(0, 0, random(0, 100));
    AtemSwitcher.setKeyDVEBorderHue(0, 0, random(0, 3550));
    AtemSwitcher.setKeyDVEBorderSaturation(0, 0, random(0, 1000));
    AtemSwitcher.setKeyDVEBorderLuma(0, 0, random(0, 1000));
    AtemSwitcher.setKeyDVELightSourceDirection(0, 0, random(0, 2000));
    AtemSwitcher.setKeyDVELightSourceAltitude(0, 0, random(10, 100));
    AtemSwitcher.commandBundleEnd();

    AtemSwitcher.setKeyerFlyKeyFrame(0, 0, 2);  // Stored in Keyframe A now
    AtemSwitcher.runLoop(1000);


    Serial << F("Keyframe A Set? ") << (AtemSwitcher.getKeyerFlyIsASet(0, 0) ? F("Yes") : F("No")) << F("\n");
    Serial << F("Keyframe B Set? ") << (AtemSwitcher.getKeyerFlyIsBSet(0, 0) ? F("Yes") : F("No")) << F("\n");
    Serial << F("Is At Keyframe? ") << _BINPADL(AtemSwitcher.getKeyerFlyIsAtKeyFrame(0, 0), 5, "0") << F("\n");


    Serial << F("\nRun to A:\n");
    printLine();

    AtemSwitcher.setRunFlyingKeyKeyFrame(0, 0, 1);  // Run to A
    AtemSwitcher.runLoop(3000);

    Serial << F("Is At Keyframe? ") << _BINPADL(AtemSwitcher.getKeyerFlyIsAtKeyFrame(0, 0), 5, "0") << F("\n");

    if (AtemSwitcher.getKeyerFlyKeyFrameSizeX(0, 0, 1) != AtemSwitcher.getKeyDVESizeX(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameSizeX: ") << AtemSwitcher.getKeyerFlyKeyFrameSizeX(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVESizeX(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameSizeY(0, 0, 1) != AtemSwitcher.getKeyDVESizeY(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameSizeY: ") << AtemSwitcher.getKeyerFlyKeyFrameSizeY(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVESizeY(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFramePositionX(0, 0, 1) != AtemSwitcher.getKeyDVEPositionX(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFramePositionX: ") << AtemSwitcher.getKeyerFlyKeyFramePositionX(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVEPositionX(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFramePositionY(0, 0, 1) != AtemSwitcher.getKeyDVEPositionY(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFramePositionY: ") << AtemSwitcher.getKeyerFlyKeyFramePositionY(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVEPositionY(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameRotation(0, 0, 1) != AtemSwitcher.getKeyDVERotation(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameRotation: ") << AtemSwitcher.getKeyerFlyKeyFrameRotation(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVERotation(0, 0) << F("\n");
    }

    if (AtemSwitcher.getKeyerFlyKeyFrameBorderOuterWidth(0, 0, 1) != AtemSwitcher.getKeyDVEBorderOuterWidth(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderOuterWidth: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderOuterWidth(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVEBorderOuterWidth(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderInnerWidth(0, 0, 1) != AtemSwitcher.getKeyDVEBorderInnerWidth(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderInnerWidth: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderInnerWidth(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVEBorderInnerWidth(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderOuterSoftness(0, 0, 1) != AtemSwitcher.getKeyDVEBorderOuterSoftness(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderOuterSoftness: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderOuterSoftness(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVEBorderOuterSoftness(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderInnerSoftness(0, 0, 1) != AtemSwitcher.getKeyDVEBorderInnerSoftness(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderInnerSoftness: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderInnerSoftness(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVEBorderInnerSoftness(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderBevelSoftness(0, 0, 1) != AtemSwitcher.getKeyDVEBorderBevelSoftness(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderBevelSoftness: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderBevelSoftness(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVEBorderBevelSoftness(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderBevelPosition(0, 0, 1) != AtemSwitcher.getKeyDVEBorderBevelPosition(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderBevelPosition: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderBevelPosition(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVEBorderBevelPosition(0, 0) << F("\n");
    }

    if (AtemSwitcher.getKeyerFlyKeyFrameBorderOpacity(0, 0, 1) != AtemSwitcher.getKeyDVEBorderOpacity(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderOpacity: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderOpacity(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVEBorderOpacity(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderHue(0, 0, 1) != AtemSwitcher.getKeyDVEBorderHue(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderHue: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderHue(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVEBorderHue(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderSaturation(0, 0, 1) != AtemSwitcher.getKeyDVEBorderSaturation(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderSaturation: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderSaturation(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVEBorderSaturation(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderLuma(0, 0, 1) != AtemSwitcher.getKeyDVEBorderLuma(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderLuma: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderLuma(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVEBorderLuma(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameLightSourceDirection(0, 0, 1) != AtemSwitcher.getKeyDVELightSourceDirection(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameLightSourceDirection: ") << AtemSwitcher.getKeyerFlyKeyFrameLightSourceDirection(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVELightSourceDirection(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameLightSourceAltitude(0, 0, 1) != AtemSwitcher.getKeyDVELightSourceAltitude(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameLightSourceAltitude: ") << AtemSwitcher.getKeyerFlyKeyFrameLightSourceAltitude(0, 0, 1) << F(" != ") << AtemSwitcher.getKeyDVELightSourceAltitude(0, 0) << F("\n");
    }








    Serial << F("\nRun to B:\n");
    printLine();

    AtemSwitcher.setRunFlyingKeyKeyFrame(0, 0, 2);  // Run to B
    AtemSwitcher.runLoop(3000);

    Serial << F("Is At Keyframe? ") << _BINPADL(AtemSwitcher.getKeyerFlyIsAtKeyFrame(0, 0), 5, "0") << F("\n");


    if (AtemSwitcher.getKeyerFlyKeyFrameSizeX(0, 0, 2) != AtemSwitcher.getKeyDVESizeX(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameSizeX: ") << AtemSwitcher.getKeyerFlyKeyFrameSizeX(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVESizeX(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameSizeY(0, 0, 2) != AtemSwitcher.getKeyDVESizeY(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameSizeY: ") << AtemSwitcher.getKeyerFlyKeyFrameSizeY(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVESizeY(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFramePositionX(0, 0, 2) != AtemSwitcher.getKeyDVEPositionX(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFramePositionX: ") << AtemSwitcher.getKeyerFlyKeyFramePositionX(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVEPositionX(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFramePositionY(0, 0, 2) != AtemSwitcher.getKeyDVEPositionY(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFramePositionY: ") << AtemSwitcher.getKeyerFlyKeyFramePositionY(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVEPositionY(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameRotation(0, 0, 2) != AtemSwitcher.getKeyDVERotation(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameRotation: ") << AtemSwitcher.getKeyerFlyKeyFrameRotation(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVERotation(0, 0) << F("\n");
    }

    if (AtemSwitcher.getKeyerFlyKeyFrameBorderOuterWidth(0, 0, 2) != AtemSwitcher.getKeyDVEBorderOuterWidth(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderOuterWidth: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderOuterWidth(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVEBorderOuterWidth(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderInnerWidth(0, 0, 2) != AtemSwitcher.getKeyDVEBorderInnerWidth(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderInnerWidth: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderInnerWidth(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVEBorderInnerWidth(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderOuterSoftness(0, 0, 2) != AtemSwitcher.getKeyDVEBorderOuterSoftness(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderOuterSoftness: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderOuterSoftness(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVEBorderOuterSoftness(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderInnerSoftness(0, 0, 2) != AtemSwitcher.getKeyDVEBorderInnerSoftness(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderInnerSoftness: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderInnerSoftness(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVEBorderInnerSoftness(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderBevelSoftness(0, 0, 2) != AtemSwitcher.getKeyDVEBorderBevelSoftness(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderBevelSoftness: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderBevelSoftness(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVEBorderBevelSoftness(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderBevelPosition(0, 0, 2) != AtemSwitcher.getKeyDVEBorderBevelPosition(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderBevelPosition: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderBevelPosition(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVEBorderBevelPosition(0, 0) << F("\n");
    }

    if (AtemSwitcher.getKeyerFlyKeyFrameBorderOpacity(0, 0, 2) != AtemSwitcher.getKeyDVEBorderOpacity(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderOpacity: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderOpacity(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVEBorderOpacity(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderHue(0, 0, 2) != AtemSwitcher.getKeyDVEBorderHue(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderHue: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderHue(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVEBorderHue(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderSaturation(0, 0, 2) != AtemSwitcher.getKeyDVEBorderSaturation(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderSaturation: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderSaturation(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVEBorderSaturation(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameBorderLuma(0, 0, 2) != AtemSwitcher.getKeyDVEBorderLuma(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameBorderLuma: ") << AtemSwitcher.getKeyerFlyKeyFrameBorderLuma(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVEBorderLuma(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameLightSourceDirection(0, 0, 2) != AtemSwitcher.getKeyDVELightSourceDirection(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameLightSourceDirection: ") << AtemSwitcher.getKeyerFlyKeyFrameLightSourceDirection(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVELightSourceDirection(0, 0) << F("\n");
    }
    if (AtemSwitcher.getKeyerFlyKeyFrameLightSourceAltitude(0, 0, 2) != AtemSwitcher.getKeyDVELightSourceAltitude(0, 0))  {
      Serial << F("ERROR: getKeyerFlyKeyFrameLightSourceAltitude: ") << AtemSwitcher.getKeyerFlyKeyFrameLightSourceAltitude(0, 0, 2) << F(" != ") << AtemSwitcher.getKeyDVELightSourceAltitude(0, 0) << F("\n");
    }




    Serial << F("\nRun to Full:\n");
    printLine();

    AtemSwitcher.setRunFlyingKeyKeyFrame(0, 0, 3);  // Run to Full
    AtemSwitcher.runLoop(3000);

    Serial << F("Is At Keyframe? ") << _BINPADL(AtemSwitcher.getKeyerFlyIsAtKeyFrame(0, 0), 5, "0") << F("\n");


    Serial << F("\nRun to Infinite Index 8:\n");
    printLine();

    AtemSwitcher.commandBundleStart();
    AtemSwitcher.setRunFlyingKeyKeyFrame(0, 0, 4);  // Run to Infinite
    AtemSwitcher.setRunFlyingKeyRuntoInfiniteindex(0, 0, 8);
    AtemSwitcher.commandBundleEnd();
    AtemSwitcher.runLoop(3000);

    Serial << F("Is At Keyframe? ") << _BINPADL(AtemSwitcher.getKeyerFlyIsAtKeyFrame(0, 0), 5, "0") << F("\n");
    Serial << F("Infinite Index: ") << AtemSwitcher.getKeyerFlyRuntoInfiniteindex(0, 0) << F("\n");

    AtemSwitcher.commandBundleStart();
    AtemSwitcher.setRunFlyingKeyKeyFrame(0, 0, 4);  // Run to Infinite
    AtemSwitcher.setRunFlyingKeyRuntoInfiniteindex(0, 0, 3);
    AtemSwitcher.commandBundleEnd();
    AtemSwitcher.runLoop(3000);

    Serial << F("Is At Keyframe? ") << _BINPADL(AtemSwitcher.getKeyerFlyIsAtKeyFrame(0, 0), 5, "0") << F("\n");
    Serial << F("Infinite Index: ") << AtemSwitcher.getKeyerFlyRuntoInfiniteindex(0, 0) << F("\n");


    AtemSwitcher.setRunFlyingKeyKeyFrame(0, 0, 3);  // Run to Full
    AtemSwitcher.runLoop(3000);



    // Restore settings:
    AtemSwitcher.commandBundleStart();
    AtemSwitcher.setKeyDVESizeX(0, 0, temp_sizeX);
    AtemSwitcher.setKeyDVESizeY(0, 0, temp_sizeY);
    AtemSwitcher.setKeyDVEPositionX(0, 0, temp_posX);
    AtemSwitcher.setKeyDVEPositionY(0, 0, temp_posY);
    AtemSwitcher.setKeyDVERotation(0, 0, temp_rot);
    AtemSwitcher.setKeyDVEBorderOuterWidth(0, 0, temp_oW);
    AtemSwitcher.setKeyDVEBorderInnerWidth(0, 0, temp_iW);
    AtemSwitcher.setKeyDVEBorderOuterSoftness(0, 0, temp_oS);
    AtemSwitcher.setKeyDVEBorderInnerSoftness(0, 0, temp_iS);
    AtemSwitcher.setKeyDVEBorderBevelSoftness(0, 0, temp_bS);
    AtemSwitcher.setKeyDVEBorderBevelPosition(0, 0, temp_bP);
    AtemSwitcher.setKeyDVEBorderOpacity(0, 0, temp_bO);
    AtemSwitcher.setKeyDVEBorderHue(0, 0, temp_bH);
    AtemSwitcher.setKeyDVEBorderSaturation(0, 0, temp_bSat);
    AtemSwitcher.setKeyDVEBorderLuma(0, 0, temp_bL);
    AtemSwitcher.setKeyDVELightSourceDirection(0, 0, temp_lSD);
    AtemSwitcher.setKeyDVELightSourceAltitude(0, 0, temp_lSA);
    AtemSwitcher.commandBundleEnd();
  }







  AtemSwitcher.runLoop(1000);
#endif



#if TEST_AUDIO
  Serial << F("\n\n***************************\n* Audio Sources: \n***************************\n");

  // Audio Mixer Master - Volume
  Serial << F("\nAudio Mixer Master - Volume:\n");
  printLine();
  processCommands();
  origValue = AtemSwitcher.getAudioMixerMasterVolume();
  for (int volume = -60; volume <= 6; volume += 6)	{
    AtemSwitcher.setAudioMixerMasterVolume(AtemSwitcher.audioDb2Word(volume));
    timeout = millis();
    while (AtemSwitcher.getAudioMixerMasterVolume() != AtemSwitcher.audioDb2Word(volume) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
      AtemSwitcher.runLoop();
    }
    if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
      Serial << F("ERROR. Value is: ") << AtemSwitcher.getAudioMixerMasterVolume();
    }
    else {
      Serial << F("OK");
    }
    Serial << F(" for setAudioMixerMasterVolume(audioDb2Word(") << volume  << F(")): ") << (millis() - timeout) << F(" ms\n");
    AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
  }
  AtemSwitcher.setAudioMixerMasterVolume(origValue);
  AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


  if (AtemSwitcher.getAudioMixerConfigHasMonitor())  {
    // Audio Mixer Monitor - Monitor Audio
    Serial << F("\nAudio Mixer Monitor - Monitor Audio:\n");
    printLine();
    processCommands();
    bool monitorState = AtemSwitcher.getAudioMixerMonitorMonitorAudio();
    for (uint8_t monitorAudio = 0; monitorAudio <= 1; monitorAudio++)	{
      AtemSwitcher.setAudioMixerMonitorMonitorAudio(monitorAudio);
      timeout = millis();
      while (AtemSwitcher.getAudioMixerMonitorMonitorAudio() != monitorAudio && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getAudioMixerMonitorMonitorAudio();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setAudioMixerMonitorMonitorAudio(") << monitorAudio  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setAudioMixerMonitorMonitorAudio(true);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Audio Mixer Monitor - Volume
    Serial << F("\nAudio Mixer Monitor - Volume:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getAudioMixerMonitorVolume();
    for (int volume = -60; volume <= 6; volume += 6)	{
      AtemSwitcher.setAudioMixerMonitorVolume(AtemSwitcher.audioDb2Word(volume));
      timeout = millis();
      while (AtemSwitcher.getAudioMixerMonitorVolume() != AtemSwitcher.audioDb2Word(volume) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getAudioMixerMonitorVolume();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setAudioMixerMonitorVolume(audioDb2Word(") << volume  << F(")): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setAudioMixerMonitorVolume(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Audio Mixer Monitor - Mute
    Serial << F("\nAudio Mixer Monitor - Mute:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getAudioMixerMonitorMute();
    for (uint8_t mute = 0; mute <= 1; mute++)	{
      AtemSwitcher.setAudioMixerMonitorMute(mute);
      timeout = millis();
      while (AtemSwitcher.getAudioMixerMonitorMute() != mute && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getAudioMixerMonitorMute();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setAudioMixerMonitorMute(") << mute  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setAudioMixerMonitorMute(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Audio Mixer Monitor - Solo
    Serial << F("\nAudio Mixer Monitor - Solo:\n");
    printLine();
    processCommands();
    bool isSolo = AtemSwitcher.getAudioMixerMonitorSolo();
    for (uint8_t solo = 0; solo <= 1; solo++)	{
      AtemSwitcher.setAudioMixerMonitorSolo(solo);
      timeout = millis();
      while (AtemSwitcher.getAudioMixerMonitorSolo() != solo && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getAudioMixerMonitorSolo();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setAudioMixerMonitorSolo(") << solo  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setAudioMixerMonitorSolo(true);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Audio Mixer Monitor - Solo Input
    Serial << F("\nAudio Mixer Monitor - Solo Input:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getAudioMixerMonitorSoloInput();
    for (uint16_t soloInput = 0; soloInput < AtemSwitcher.getAudioMixerTallySources(); soloInput++)	{
      AtemSwitcher.setAudioMixerMonitorSoloInput(AtemSwitcher.getAudioMixerTallyAudioSource(soloInput));
      timeout = millis();
      while (AtemSwitcher.getAudioMixerMonitorSoloInput() != AtemSwitcher.getAudioMixerTallyAudioSource(soloInput) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getAudioMixerMonitorSoloInput();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setAudioMixerMonitorSoloInput(") << AtemSwitcher.getAudioMixerTallyAudioSource(soloInput)  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setAudioMixerMonitorSoloInput(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Audio Mixer Monitor - Dim
    Serial << F("\nAudio Mixer Monitor - Dim:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getAudioMixerMonitorDim();
    for (uint8_t dim = 0; dim <= 1; dim++)	{
      AtemSwitcher.setAudioMixerMonitorDim(dim);
      timeout = millis();
      while (AtemSwitcher.getAudioMixerMonitorDim() != dim && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getAudioMixerMonitorDim();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setAudioMixerMonitorDim(") << dim  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setAudioMixerMonitorDim(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    AtemSwitcher.setAudioMixerMonitorMonitorAudio(monitorState);
    AtemSwitcher.setAudioMixerMonitorSolo(isSolo);
  }

  for (uint16_t audioSource = 0; audioSource < AtemSwitcher.getAudioMixerTallySources(); audioSource++)	{

    // Audio Mixer Input - Mix Option
    Serial << F("\nAudio Mixer Input - Mix Option:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getAudioMixerInputMixOption(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource));
    for (uint8_t mixOption = 0; mixOption <= 2; mixOption += 1)	{
      if (!(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource) > 1000 && AtemSwitcher.getAudioMixerTallyAudioSource(audioSource) < 1999 && mixOption == 2) && !(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource) > 2000 && AtemSwitcher.getAudioMixerTallyAudioSource(audioSource) < 2999 && mixOption == 1))  {
        AtemSwitcher.setAudioMixerInputMixOption(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource), mixOption);
        timeout = millis();
        while (AtemSwitcher.getAudioMixerInputMixOption(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource)) != mixOption && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          AtemSwitcher.runLoop();
        }
        if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
          Serial << F("ERROR. Value is: ") << AtemSwitcher.getAudioMixerInputMixOption(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource));
        }
        else {
          Serial << F("OK");
        }
        Serial << F(" for setAudioMixerInputMixOption(") << AtemSwitcher.getAudioMixerTallyAudioSource(audioSource) << F(", ") << mixOption  << F("): ") << (millis() - timeout) << F(" ms\n");

        // Check tally:
        AtemSwitcher.runLoop(100);
        if (mixOption == 0 && AtemSwitcher.getAudioMixerTallyIsMixedIn(audioSource))  {
          Serial << F(" - ERROR, Audio Tally did not reflect this state (Off)!\n");
        }
        else if (mixOption == 1 && !AtemSwitcher.getAudioMixerTallyIsMixedIn(audioSource))  {
          Serial << F(" - ERROR, Audio Tally did not reflect this state (On)!\n");
        }

        AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
      }
    }
    AtemSwitcher.setAudioMixerInputMixOption(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource), origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Audio Mixer Input - Volume
    Serial << F("\nAudio Mixer Input - Volume:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getAudioMixerInputVolume(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource));
    for (int volume = -60; volume <= 6; volume += 6)	{
      AtemSwitcher.setAudioMixerInputVolume(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource), AtemSwitcher.audioDb2Word(volume));
      timeout = millis();
      while (AtemSwitcher.getAudioMixerInputVolume(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource)) != AtemSwitcher.audioDb2Word(volume) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getAudioMixerInputVolume(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource));
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setAudioMixerInputVolume(") << AtemSwitcher.getAudioMixerTallyAudioSource(audioSource) << F(", audioDb2Word(") << volume  << F(")): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setAudioMixerInputVolume(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource), origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command


    // Audio Mixer Input - Balance
    Serial << F("\nAudio Mixer Input - Balance:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getAudioMixerInputBalance(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource));
    for (int balance = -10000; balance <= 10000; balance += 4000)	{
      AtemSwitcher.setAudioMixerInputBalance(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource), balance);
      timeout = millis();
      while (AtemSwitcher.getAudioMixerInputBalance(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource)) != balance && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getAudioMixerInputBalance(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource));
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setAudioMixerInputBalance(") << AtemSwitcher.getAudioMixerTallyAudioSource(audioSource) << F(", ") << balance  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setAudioMixerInputBalance(AtemSwitcher.getAudioMixerTallyAudioSource(audioSource), origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command

  }
  AtemSwitcher.runLoop(1000);
#endif


#if TEST_AUDIO_LEVELS
  Serial << F("\n\n***************************\n* Audio Levels: \n***************************\n");
  /*
    // Setting up:
   AtemSwitcher.runLoop(1000);
   uint8_t mpType = AtemSwitcher.getMediaPlayerSourceType(0);
   uint8_t clipIdx = AtemSwitcher.getMediaPlayerSourceClipIndex(0);
   bool loopFlag = AtemSwitcher.getClipPlayerLoop(0);
   uint16_t prgSource = AtemSwitcher.getProgramInputVideoSource(0);

   AtemSwitcher.setMediaPlayerSourceType(0,2);
   AtemSwitcher.setMediaPlayerSourceClipIndex(0,0);
   AtemSwitcher.setClipPlayerLoop(0,true);
   AtemSwitcher.setProgramInputVideoSource(0,3010);
   AtemSwitcher.runLoop(200);
   AtemSwitcher.setClipPlayerPlaying(0, true);
   AtemSwitcher.runLoop(200);
   */

  AtemSwitcher.setAudioLevelsEnable(true);
  AtemSwitcher.runLoop(200);

  // Master levels tested:
  Serial << F("\nAudio Levels - Master:\n");
  printLine();
  processCommands();
  for (a = 0; a < 5; a++)  {
    timeout = millis();
    while (!AtemSwitcher.hasTimedOut(timeout, 3000))	{
      AtemSwitcher.runLoop(100);
      printAudioLevels("Master", AtemSwitcher.getAudioMixerLevelsMasterLeft(), AtemSwitcher.getAudioMixerLevelsMasterRight(), AtemSwitcher.getAudioMixerLevelsMasterPeakLeft(), AtemSwitcher.getAudioMixerLevelsMasterPeakRight());
    }
    AtemSwitcher.setResetAudioMixerPeaksMaster(true);
    Serial << "Reset peaking\n";
  }
  AtemSwitcher.runLoop(1000);

  Serial << F("\nAudio Levels - Monitor:\n");
  printLine();
  processCommands();
  timeout = millis();
  while (!AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
    AtemSwitcher.runLoop(100);
    printAudioLevels("Monitor", AtemSwitcher.getAudioMixerLevelsMonitor(), AtemSwitcher.getAudioMixerLevelsMonitor(), 0, 0);
  }
  AtemSwitcher.runLoop(1000);

  // Sources tested:
  for (b = 0; b < AtemSwitcher.getAudioMixerLevelsSources(); b++)  {
    Serial << F("\nAudio Levels - Source ") << AtemSwitcher.getAudioMixerLevelsSourceOrder(b) << F(":\n");
    printLine();
    processCommands();
    for (a = 0; a < 3; a++)  {
      timeout = millis();
      while (!AtemSwitcher.hasTimedOut(timeout, 2000))	{
        AtemSwitcher.runLoop(100);
        printAudioLevels("Source", AtemSwitcher.getAudioMixerLevelsSourceLeft(b), AtemSwitcher.getAudioMixerLevelsSourceRight(b), AtemSwitcher.getAudioMixerLevelsSourcePeakLeft(b), AtemSwitcher.getAudioMixerLevelsSourcePeakRight(b));
      }
      AtemSwitcher.setResetAudioMixerPeaksInputSource(AtemSwitcher.getAudioMixerLevelsSourceOrder(b));
      Serial << "Reset peaking\n";
    }
  }
  AtemSwitcher.runLoop(1000);

  AtemSwitcher.setAudioLevelsEnable(false);
  AtemSwitcher.runLoop(1000);

  /*
    // Resetting:
   AtemSwitcher.setClipPlayerPlaying(0, false);
   AtemSwitcher.runLoop(100);
   AtemSwitcher.setClipPlayerLoop(0, loopFlag);
   AtemSwitcher.runLoop(100);
   AtemSwitcher.setMediaPlayerSourceClipIndex(0, clipIdx);
   AtemSwitcher.runLoop(100);
   AtemSwitcher.setMediaPlayerSourceType(0, 1);
   AtemSwitcher.runLoop(100);
   AtemSwitcher.setProgramInputVideoSource(0, prgSource);
   AtemSwitcher.runLoop(100);
   */
  AtemSwitcher.runLoop(1000);
#endif


#if TEST_CCU
  Serial << F("\n\n***************************\n* Camera Control: \n***************************\n");
  for (uint8_t input = 1; input <= 5; input += 2)	{
    // Camera Control - Iris
    Serial << F("\nCamera Control - Iris:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlIris(input);
    for (int iris = 0; iris <= 2048; iris += 410)	{
      AtemSwitcher.setCameraControlIris(input, iris);
      timeout = millis();
      while (AtemSwitcher.getCameraControlIris(input) != iris && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlIris(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlIris(") << input << F(", ") << iris  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlIris(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Focus
    Serial << F("\nCamera Control - Focus:\n");
    printLine();
    processCommands();
    int focus_T = AtemSwitcher.getCameraControlFocus(input);
    for (int focus = -200; focus <= 200; focus += 20)	{
      AtemSwitcher.setCameraControlFocus(input, focus);
      timeout = millis();
      while (AtemSwitcher.getCameraControlFocus(input) != focus_T + focus && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << (focus_T + focus);
      }
      else {
        Serial << F("OK");
      }
      focus_T = AtemSwitcher.getCameraControlFocus(input);
      Serial << F(" for setCameraControlFocus(") << input << F(", ") << focus  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Gain
    Serial << F("\nCamera Control - Gain:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlGain(input);
    uint16_t gainValues[] = {
      512, 1024, 2048 , 4096
    };
    for (int gain = 0; gain < 4; gain++)	{
      AtemSwitcher.setCameraControlGain(input, gainValues[gain]);
      timeout = millis();
      while ((AtemSwitcher.getCameraControlGain(input) >> 8) != (gainValues[gain] >> 8) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlGain(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlGain(") << input << F(", ") << gainValues[gain]  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlGain(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - White Balance
    Serial << F("\nCamera Control - White Balance:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlWhiteBalance(input);
    uint16_t whiteBalanceValues[] = {
      3200, 4500, 5000, 5600, 6500, 7500
    };
    for (int wb = 0; wb < 6; wb++)	{
      AtemSwitcher.setCameraControlWhiteBalance(input, whiteBalanceValues[wb]);
      timeout = millis();
      while ((AtemSwitcher.getCameraControlWhiteBalance(input) >> 8) != (whiteBalanceValues[wb] >> 8) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlWhiteBalance(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlWhiteBalance(") << input << F(", ") << whiteBalanceValues[wb]  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlWhiteBalance(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Zoom Speed
    Serial << F("\nCamera Control - Zoom Speed:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlZoomSpeed(input);
    for (int zoomSpeed = -2048; zoomSpeed <= 2048; zoomSpeed += 820)	{
      AtemSwitcher.setCameraControlZoomSpeed(input, zoomSpeed);
      timeout = millis();
      while (AtemSwitcher.getCameraControlZoomSpeed(input) != zoomSpeed && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlZoomSpeed(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlZoomSpeed(") << input << F(", ") << zoomSpeed  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlZoomSpeed(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Lift R
    Serial << F("\nCamera Control - Lift R:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlLiftR(input);
    for (int liftR = -4096; liftR <= 4096; liftR += 1639)	{
      AtemSwitcher.setCameraControlLiftR(input, liftR);
      timeout = millis();
      while (AtemSwitcher.getCameraControlLiftR(input) != liftR && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlLiftR(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlLiftR(") << input << F(", ") << liftR  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlLiftR(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Gamma R
    Serial << F("\nCamera Control - Gamma R:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlGammaR(input);
    for (int gammaR = -4096; gammaR <= 4096; gammaR += 1639)	{
      AtemSwitcher.setCameraControlGammaR(input, gammaR);
      timeout = millis();
      while (AtemSwitcher.getCameraControlGammaR(input) != gammaR && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlGammaR(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlGammaR(") << input << F(", ") << gammaR  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlGammaR(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Gain R
    Serial << F("\nCamera Control - Gain R:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlGainR(input);
    for (int gainR = -4096; gainR <= 4096; gainR += 1639)	{
      AtemSwitcher.setCameraControlGainR(input, gainR);
      timeout = millis();
      while (AtemSwitcher.getCameraControlGainR(input) != gainR && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlGainR(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlGainR(") << input << F(", ") << gainR  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlGainR(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Lum Mix
    Serial << F("\nCamera Control - Lum Mix:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlLumMix(input);
    for (int lumMix = 0; lumMix <= 2048; lumMix += 410)	{
      AtemSwitcher.setCameraControlLumMix(input, lumMix);
      timeout = millis();
      while (AtemSwitcher.getCameraControlLumMix(input) != lumMix && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlLumMix(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlLumMix(") << input << F(", ") << lumMix  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlLumMix(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Hue
    Serial << F("\nCamera Control - Hue:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlHue(input);
    for (int hue = -2048; hue <= 2048; hue += 820)	{
      AtemSwitcher.setCameraControlHue(input, hue);
      timeout = millis();
      while (AtemSwitcher.getCameraControlHue(input) != hue && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlHue(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlHue(") << input << F(", ") << hue  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlHue(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Shutter
    Serial << F("\nCamera Control - Shutter:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlShutter(input);
    uint16_t shutterValues[] = {
      20000, 16667, 13333, 11111, 10000, 8333, 6667, 5556, 4000, 2778, 2000, 1379, 1000, 690, 500
    };  // Microseconds exposure!
    for (int shutter = 0; shutter <= 14; shutter++)	{
      AtemSwitcher.setCameraControlShutter(input, shutterValues[shutter]);
      timeout = millis();
      while (AtemSwitcher.getCameraControlShutter(input) != shutterValues[shutter] && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlShutter(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlShutter(") << input << F(", ") << shutterValues[shutter]  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlShutter(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Lift G
    Serial << F("\nCamera Control - Lift G:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlLiftG(input);
    for (int liftG = -4096; liftG <= 4096; liftG += 1639)	{
      AtemSwitcher.setCameraControlLiftG(input, liftG);
      timeout = millis();
      while (AtemSwitcher.getCameraControlLiftG(input) != liftG && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlLiftG(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlLiftG(") << input << F(", ") << liftG  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlLiftG(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Gamma G
    Serial << F("\nCamera Control - Gamma G:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlGammaG(input);
    for (int gammaG = -4096; gammaG <= 4096; gammaG += 1639)	{
      AtemSwitcher.setCameraControlGammaG(input, gammaG);
      timeout = millis();
      while (AtemSwitcher.getCameraControlGammaG(input) != gammaG && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlGammaG(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlGammaG(") << input << F(", ") << gammaG  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlGammaG(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Gain G
    Serial << F("\nCamera Control - Gain G:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlGainG(input);
    for (int gainG = -4096; gainG <= 4096; gainG += 1639)	{
      AtemSwitcher.setCameraControlGainG(input, gainG);
      timeout = millis();
      while (AtemSwitcher.getCameraControlGainG(input) != gainG && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlGainG(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlGainG(") << input << F(", ") << gainG  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlGainG(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Contrast
    Serial << F("\nCamera Control - Contrast:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlContrast(input);
    for (int contrast = 0; contrast <= 4096; contrast += 820)	{
      AtemSwitcher.setCameraControlContrast(input, contrast);
      timeout = millis();
      while (AtemSwitcher.getCameraControlContrast(input) != contrast && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlContrast(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlContrast(") << input << F(", ") << contrast  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlContrast(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Saturation
    Serial << F("\nCamera Control - Saturation:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlSaturation(input);
    for (int saturation = 0; saturation <= 4096; saturation += 820)	{
      AtemSwitcher.setCameraControlSaturation(input, saturation);
      timeout = millis();
      while (AtemSwitcher.getCameraControlSaturation(input) != saturation && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlSaturation(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlSaturation(") << input << F(", ") << saturation  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlSaturation(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Lift B
    Serial << F("\nCamera Control - Lift B:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlLiftB(input);
    for (int liftB = -4096; liftB <= 4096; liftB += 1639)	{
      AtemSwitcher.setCameraControlLiftB(input, liftB);
      timeout = millis();
      while (AtemSwitcher.getCameraControlLiftB(input) != liftB && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlLiftB(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlLiftB(") << input << F(", ") << liftB  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlLiftB(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Gamma B
    Serial << F("\nCamera Control - Gamma B:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlGammaB(input);
    for (int gammaB = -4096; gammaB <= 4096; gammaB += 1639)	{
      AtemSwitcher.setCameraControlGammaB(input, gammaB);
      timeout = millis();
      while (AtemSwitcher.getCameraControlGammaB(input) != gammaB && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlGammaB(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlGammaB(") << input << F(", ") << gammaB  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlGammaB(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Gain B
    Serial << F("\nCamera Control - Gain B:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlGainB(input);
    for (int gainB = -4096; gainB <= 4096; gainB += 1639)	{
      AtemSwitcher.setCameraControlGainB(input, gainB);
      timeout = millis();
      while (AtemSwitcher.getCameraControlGainB(input) != gainB && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlGainB(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlGainB(") << input << F(", ") << gainB  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlGainB(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Lift Y
    Serial << F("\nCamera Control - Lift Y:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlLiftY(input);
    for (int liftY = -4096; liftY <= 4096; liftY += 1639)	{
      AtemSwitcher.setCameraControlLiftY(input, liftY);
      timeout = millis();
      while (AtemSwitcher.getCameraControlLiftY(input) != liftY && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlLiftY(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlLiftY(") << input << F(", ") << liftY  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlLiftY(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Gamma Y
    Serial << F("\nCamera Control - Gamma Y:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlGammaY(input);
    for (int gammaY = -4096; gammaY <= 4096; gammaY += 1639)	{
      AtemSwitcher.setCameraControlGammaY(input, gammaY);
      timeout = millis();
      while (AtemSwitcher.getCameraControlGammaY(input) != gammaY && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlGammaY(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlGammaY(") << input << F(", ") << gammaY  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlGammaY(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested


    // Camera Control - Gain Y
    Serial << F("\nCamera Control - Gain Y:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getCameraControlGainY(input);
    for (int gainY = -4096; gainY <= 4096; gainY += 1639)	{
      AtemSwitcher.setCameraControlGainY(input, gainY);
      timeout = millis();
      while (AtemSwitcher.getCameraControlGainY(input) != gainY && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getCameraControlGainY(input);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setCameraControlGainY(") << input << F(", ") << gainY  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between commands
    }
    AtemSwitcher.setCameraControlGainY(input, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next feature is tested
  }
  AtemSwitcher.runLoop(1000);
#endif


#if TEST_TALLY
  Serial << F("\n\n***************************\n* Tallies: \n***************************\n");

  Serial << F("\nAudio Tally:\n(Send key press to stop)\n");
  printLine();
  processCommands();
  a = 0;
  timeout = millis();
  while (!AtemSwitcher.hasTimedOut(timeout, 120000) && !serialReadKeypress())	{
    if (!(a % 10))  {
      Serial << F("\n");
      for (b = 0; b < AtemSwitcher.getAudioMixerTallySources(); b++)  {
        Serial << _DECPADL(AtemSwitcher.getAudioMixerTallyAudioSource(b), 5, " ");
      }
      Serial << F("\n");
    }
    for (b = 0; b < AtemSwitcher.getAudioMixerTallySources(); b++)  {
      Serial << _DECPADL(AtemSwitcher.getAudioMixerTallyIsMixedIn(b), 5, " ");
    }
    Serial << F("\n");
    AtemSwitcher.runLoop(1000);
    a++;
  }

  Serial << F("\nVideo Tally (source):\n(Send key press to stop)\n");
  printLine();
  processCommands();
  a = 0;
  timeout = millis();
  while (!AtemSwitcher.hasTimedOut(timeout, 120000) && !serialReadKeypress())	{
    if (!(a % 10))  {
      Serial << F("\n");
      for (b = 0; b < AtemSwitcher.getTallyBySourceSources(); b++)  {
        Serial << _DECPADL(AtemSwitcher.getTallyBySourceVideoSource(b), 6, " ");
      }
      Serial << F("\n");
    }
    for (b = 0; b < AtemSwitcher.getTallyBySourceSources(); b++)  {
      Serial << F("    ") << _BINPADL(AtemSwitcher.getTallyBySourceTallyFlags(b), 2, "0");
    }
    Serial << F("\n      ");

    for (b = 0; b < AtemSwitcher.getTallyByIndexSources(); b++)  {
      Serial << F("    ") << _BINPADL(AtemSwitcher.getTallyByIndexTallyFlags(b), 2, "0");
    }
    Serial << F("\n");

    AtemSwitcher.runLoop(1000);
    a++;
  }
  AtemSwitcher.runLoop(1000);
#endif


#if TEST_MACROS
  Serial << F("\n\n***************************\n* Macroes: \n***************************\n");

  Serial << F("\nExisting Macros (1-10):\n");
  printLine();
  processCommands();

  for (uint8_t i = 0; i < 10; i++)  {
    if (AtemSwitcher.getMacroPropertiesIsUsed(i))  {
      Serial << i << F(": ") << AtemSwitcher.getMacroPropertiesName(i) << F("\n");
    }
  }
  AtemSwitcher.runLoop(1000);

  Serial << F("\nRecord Macro (10):\n");
  printLine();
  processCommands();

  if (AtemSwitcher.getMacroRecordingStatusIsRecording())  {
    Serial << F("Is recording! (ERROR): Index: ") << AtemSwitcher.getMacroRecordingStatusIndex() << F("\n");
  } else {
    Serial << F("Is not recording yet (good thing).\n");
  }

  AtemSwitcher.setMacroStartRecordingIndex(9);
  AtemSwitcher.runLoop(1000);

  if (AtemSwitcher.getMacroRecordingStatusIsRecording())  {
    Serial << F("Is recording, Index: ") << (AtemSwitcher.getMacroRecordingStatusIndex() + 1) << F("\n");
  } else {
    Serial << F("Is not recording yet (ERROR).\n");
  }

  AtemSwitcher.setProgramInputVideoSource(0, 1);
  AtemSwitcher.setPreviewInputVideoSource(0, 2);
  AtemSwitcher.setMacroAddPauseFrames(25);
  AtemSwitcher.runLoop(1000);

  AtemSwitcher.setProgramInputVideoSource(0, 2);
  AtemSwitcher.setPreviewInputVideoSource(0, 3);
  AtemSwitcher.setMacroAddPauseFrames(25);
  AtemSwitcher.runLoop(1000);

  AtemSwitcher.setProgramInputVideoSource(0, 3);
  AtemSwitcher.setPreviewInputVideoSource(0, 4);
  AtemSwitcher.setMacroAction(0xffff, 3);  // Insert "Wait for user"
  AtemSwitcher.runLoop(1000);

  AtemSwitcher.setProgramInputVideoSource(0, 4);
  AtemSwitcher.setPreviewInputVideoSource(0, 5);
  AtemSwitcher.runLoop(1000);

  AtemSwitcher.setMacroAction(0xffff, 2);  // Stop recording
  AtemSwitcher.runLoop(1000);

  if (AtemSwitcher.getMacroRecordingStatusIsRecording())  {
    Serial << F("Is recording still! (ERROR)\n");
  } else {
    Serial << F("Has stopped recording. Great. Now, ready for playback:\n");
  }
  AtemSwitcher.runLoop(1000);

  if (AtemSwitcher.getMacroRunStatusState() != 0)  {
    Serial << F("ERROR: Run Status is not zero: ") <<  AtemSwitcher.getMacroRunStatusState() << F("\n");
  }
  if (!AtemSwitcher.getMacroPropertiesIsUsed(9))  {
    Serial << F("ERROR: No macro seems to be recorded!\n");
  }


  AtemSwitcher.setMacroRunChangePropertiesLooping(false);
  AtemSwitcher.setMacroAction(9, 0);  // Run Macro 10 (index 9)
  AtemSwitcher.runLoop(100);
  if (AtemSwitcher.getMacroRunStatusState() == 1 && AtemSwitcher.getMacroRunStatusIndex() == 9 && !AtemSwitcher.getMacroRunStatusIsLooping())  {
    Serial << F("Macro is running correctly, waiting for the user input point...\n");
    while ((AtemSwitcher.getMacroRunStatusState()&B10) != 2)  {
      AtemSwitcher.runLoop();
    }

    Serial << F("Waiting point reached, now continue:\n");
    AtemSwitcher.runLoop(2000);
    AtemSwitcher.setMacroAction(0xffff, 4);  // Continue command

    Serial << F("Waiting for it to end:\n");
    while (AtemSwitcher.getMacroRunStatusState() != 0)  {
      AtemSwitcher.runLoop();
    }

    Serial << F("Done...\n");
  } else {
    Serial << F("ERROR: Run Status is not zero: ") <<  AtemSwitcher.getMacroRunStatusState() << F("\n");
    AtemSwitcher.setMacroAction(0xffff, 1);  // Stop macro.
  }

  Serial << F("Now, run with looping: \n");
  AtemSwitcher.setMacroRunChangePropertiesLooping(true);
  AtemSwitcher.setMacroAction(9, 0);  // Run Macro 10 (index 9)
  AtemSwitcher.runLoop(1000);
  if (AtemSwitcher.getMacroRunStatusState() == 1 && AtemSwitcher.getMacroRunStatusIndex() == 9 && AtemSwitcher.getMacroRunStatusIsLooping())  {
    Serial << F("Macro is running correctly....stopping it\n");
    AtemSwitcher.setMacroAction(0xffff, 1);  // Stop macro.
  }
  AtemSwitcher.runLoop(1000);

  if (AtemSwitcher.getMacroRunStatusState() != 0)  {
    Serial << F("ERROR: Run Status is not zero: ") <<  AtemSwitcher.getMacroRunStatusState() << F("\n");
  } else {
    Serial << F("Done, the macro played correctly, now deleting it...\n");
    AtemSwitcher.setMacroAction(9, 5);  // Delete
  }
  AtemSwitcher.runLoop(1000);

  if (AtemSwitcher.getMacroPropertiesIsUsed(9))  {
    Serial << F("ERROR: No macro seems to be recorded!\n");
  }
  AtemSwitcher.setMacroRunChangePropertiesLooping(false);



#endif


#if TEST_SETTINGS
  Serial << F("\n\n***************************\n* Settings: \n***************************\n");


  // Video Mode - Format
  Serial << F("\nVideo Mode - Format:\n");
  printLine();
  processCommands();
  long vModes = AtemSwitcher.getVideoMixerConfigModes();
  Serial << F("Video modes: ") << _BINPADL(vModes, 18, "0") << F("\n");
  for (uint8_t format = 0; format <= 17; format++)	{
    if ((vModes >> format) & B1)  {
      AtemSwitcher.setVideoModeFormat(format);
      timeout = millis();
      while (AtemSwitcher.getVideoModeFormat() != format && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getVideoModeFormat();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setVideoModeFormat(") << format << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay
    }
  }
  AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay


  // Down Converter - Mode
  if (AtemSwitcher.getTopologyHasSDOutput())  {
    Serial << F("\nDown Converter - Mode:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getDownConverterMode();
    for (uint8_t mode = 0; mode <= 2; mode += 1)	{
      AtemSwitcher.setDownConverterMode(mode);
      timeout = millis();
      while (AtemSwitcher.getDownConverterMode() != mode && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getDownConverterMode();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setDownConverterMode(") << mode << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay
    }
    AtemSwitcher.setDownConverterMode(origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay
  }


  for (uint8_t multiViewer = 0; multiViewer < AtemSwitcher.getMultiViewConfigMultiViewers(); multiViewer++)	{

    // Multi Viewer Properties - Layout
    Serial << F("\nMulti Viewer Properties - Layout:\n");
    printLine();
    processCommands();
    origValue = AtemSwitcher.getMultiViewerPropertiesLayout(multiViewer);
    for (uint8_t layout = 0; layout <= 3; layout += 1)	{
      AtemSwitcher.setMultiViewerPropertiesLayout(multiViewer, layout);
      timeout = millis();
      while (AtemSwitcher.getMultiViewerPropertiesLayout(multiViewer) != layout && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getMultiViewerPropertiesLayout(multiViewer);
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setMultiViewerPropertiesLayout(") << multiViewer << F(", ") << layout  << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
    }
    AtemSwitcher.setMultiViewerPropertiesLayout(multiViewer, origValue);
    AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command

    for (uint8_t windowIndex = 2; windowIndex <= 9; windowIndex++)	{

      // Multi Viewer Input - Video Source
      Serial << F("\nMulti Viewer Input - Video Source:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getMultiViewerInputVideoSource(multiViewer, windowIndex);
      for (uint16_t videoSource = 0; videoSource < AtemSwitcher.maxAtemSeriesVideoInputs(); videoSource++)	{
        if (AtemSwitcher.getInputAvailability(AtemSwitcher.getVideoIndexSrc(videoSource)) & 0x02)  {  // Checking if source can be used on MV
          AtemSwitcher.setMultiViewerInputVideoSource(multiViewer, windowIndex, AtemSwitcher.getVideoIndexSrc(videoSource));
          timeout = millis();
          while (AtemSwitcher.getMultiViewerInputVideoSource(multiViewer, windowIndex) != AtemSwitcher.getVideoIndexSrc(videoSource) && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getMultiViewerInputVideoSource(multiViewer, windowIndex);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setMultiViewerInputVideoSource(") << multiViewer << F(", ") << windowIndex << F(", ") << AtemSwitcher.getVideoIndexSrc(videoSource)  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
      }
      AtemSwitcher.setMultiViewerInputVideoSource(multiViewer, windowIndex, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command

    }

    // Setting MV to default values:
    AtemSwitcher.setMultiViewerInputVideoSource(0, 0, 10011);  // ME1, Preview
    AtemSwitcher.setMultiViewerInputVideoSource(0, 1, 10010);  // ME1, Program
    AtemSwitcher.setMultiViewerInputVideoSource(1, 0, 10021);  // ME2, Preview
    AtemSwitcher.setMultiViewerInputVideoSource(1, 1, 10020);  // ME2, Program
  }


  for (uint16_t videoSource = 0; videoSource < AtemSwitcher.maxAtemSeriesVideoInputs(); videoSource++)	{
    uint8_t portTypesForThisSource = AtemSwitcher.getInputAvailableExternalPortTypes(AtemSwitcher.getVideoIndexSrc(videoSource));

    if (portTypesForThisSource > 0)  {
      // Input - External Port Type
      Serial << F("\nInput - External Port Type:\n");
      printLine();
      processCommands();
      origValue = AtemSwitcher.getInputExternalPortType(AtemSwitcher.getVideoIndexSrc(videoSource));
      for (uint8_t externalPortType = 1; externalPortType <= 5; externalPortType++)	{
        uint8_t vMap[] = {0, 1, 2, 4, 3, 5};
        if (portTypesForThisSource & (B1 << (vMap[externalPortType] - 1)))  {
          AtemSwitcher.setInputExternalPortType(AtemSwitcher.getVideoIndexSrc(videoSource), externalPortType);
          timeout = millis();
          while (AtemSwitcher.getInputExternalPortType(AtemSwitcher.getVideoIndexSrc(videoSource)) != externalPortType && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            AtemSwitcher.runLoop();
          }
          if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
            Serial << F("ERROR. Value is: ") << AtemSwitcher.getInputExternalPortType(videoSource);
          }
          else {
            Serial << F("OK");
          }
          Serial << F(" for setInputExternalPortType(") << AtemSwitcher.getVideoIndexSrc(videoSource) << F(", ") << externalPortType  << F("): ") << (millis() - timeout) << F(" ms\n");
          AtemSwitcher.runLoop(delayBetweenValues);	// Short delay between values
        }
      }
      AtemSwitcher.setInputExternalPortType(videoSource, origValue);
      AtemSwitcher.runLoop(delayBetweenCmds);	// Delay before next command
    }
  }

  Serial << F("\nLong and short names:\n");
  printLine();
  processCommands();
  for (uint16_t videoSource = 0; videoSource < AtemSwitcher.maxAtemSeriesVideoInputs(); videoSource++)	{
    if ((AtemSwitcher.getInputMEAvailability(AtemSwitcher.getVideoIndexSrc(videoSource)) & 0x01) && AtemSwitcher.getVideoIndexSrc(videoSource) < 10000)  { // It seems, that all sources that can go on ME can be renamed!
      char origStrValue[21];

      // Long names:
      strncpy (origStrValue, AtemSwitcher.getInputLongName(AtemSwitcher.getVideoIndexSrc(videoSource)), sizeof(origStrValue));
      AtemSwitcher.setInputLongName(AtemSwitcher.getVideoIndexSrc(videoSource), "TEST STRING LONG 12345");

      timeout = millis();
      while (strcmp ("TEST STRING LONG 123", AtemSwitcher.getInputLongName(AtemSwitcher.getVideoIndexSrc(videoSource))) != 0 && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getInputLongName(AtemSwitcher.getVideoIndexSrc(videoSource));
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setInputLongName() ") << AtemSwitcher.getVideoIndexSrc(videoSource) << F(", \"") << "TEST STRING LONG 12345" << F("\"): ") << (millis() - timeout) << F(" ms\n");

      AtemSwitcher.setInputLongName(AtemSwitcher.getVideoIndexSrc(videoSource), origStrValue);
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay


      // Short names:
      strncpy (origStrValue, AtemSwitcher.getInputShortName(AtemSwitcher.getVideoIndexSrc(videoSource)), sizeof(origStrValue));
      AtemSwitcher.setInputShortName(AtemSwitcher.getVideoIndexSrc(videoSource), "T12345");

      timeout = millis();
      while (strcmp ("T123", AtemSwitcher.getInputShortName(AtemSwitcher.getVideoIndexSrc(videoSource))) != 0 && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getInputShortName(AtemSwitcher.getVideoIndexSrc(videoSource));
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setInputShortName() ") << AtemSwitcher.getVideoIndexSrc(videoSource) << F(", \"") << "T12345" << F("\"): ") << (millis() - timeout) << F(" ms\n");

      AtemSwitcher.setInputShortName(AtemSwitcher.getVideoIndexSrc(videoSource), origStrValue);
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay
    }
  }
  // Reset names for those which can't be set from ATEM Software Control:
  AtemSwitcher.setInputLongName(0, "Black");
  AtemSwitcher.setInputLongName(1000, "Color Bars");
  AtemSwitcher.setInputLongName(2001, "Color 1");
  AtemSwitcher.setInputLongName(2002, "Color 2");
  AtemSwitcher.setInputLongName(3010, "Media Player 1");
  AtemSwitcher.setInputLongName(3011, "Media Player 1 Key");
  AtemSwitcher.setInputLongName(3020, "Media Player 2");
  AtemSwitcher.setInputLongName(3021, "Media Player 2 Key");
  AtemSwitcher.setInputLongName(6000, "Super Source");
  AtemSwitcher.runLoop(1000);


  // Media Pool Storage - Clip 1 Max Length
  Serial << F("\nMedia Pool Storage - Clip 1 Max Length:\n");
  printLine();
  processCommands();
  uint16_t maxFrames = AtemSwitcher.getMediaPoolStorageClip1MaxLength() + AtemSwitcher.getMediaPoolStorageClip2MaxLength();
  origValue = AtemSwitcher.getMediaPoolStorageClip1MaxLength();
  if (maxFrames > 0 && AtemSwitcher.getMediaPlayersClipBanks() > 0)  {
    for (uint16_t clip1MaxLength = 0; clip1MaxLength <= maxFrames; clip1MaxLength += maxFrames / 5)	{
      AtemSwitcher.setMediaPoolStorageClip1MaxLength(clip1MaxLength);
      timeout = millis();
      while (AtemSwitcher.getMediaPoolStorageClip1MaxLength() != clip1MaxLength && !AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        AtemSwitcher.runLoop();
      }
      if (AtemSwitcher.hasTimedOut(timeout, timeOutDelay))	{
        Serial << F("ERROR. Value is: ") << AtemSwitcher.getMediaPoolStorageClip1MaxLength();
      }
      else {
        Serial << F("OK");
      }
      Serial << F(" for setMediaPoolStorageClip1MaxLength(") << F("): ") << (millis() - timeout) << F(" ms\n");
      AtemSwitcher.runLoop(delayBetweenValues);	// Short delay
    }
  }
  AtemSwitcher.setMediaPoolStorageClip1MaxLength(origValue);
  AtemSwitcher.runLoop(delayBetweenCmds);	// Short delay
#endif




  if (AtemSwitcher.getVideoModeFormat() != format_R)  {
    Serial << F("\nResetting video mode format\n");
    AtemSwitcher.setVideoModeFormat(format_R);
  }


  Serial << F("\n\n**************** DONE ! *******************\n");
  Serial << F("freeMemory()=") << freeMemory() << "\n";
  Serial << F("millis()/1000=") << millis() / 1000 << "\n";
  while (true) {
  };
}
