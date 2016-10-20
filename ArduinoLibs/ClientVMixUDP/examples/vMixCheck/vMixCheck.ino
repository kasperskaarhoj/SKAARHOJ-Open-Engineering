/*****************
   Checking vMix API
   Connects to the vMix UDP API and goes through a series of tests of the switchers features.

   - kasper
*/




#define TEST_ME 0
#define TEST_RECORD_STREAM 1

#define LOOP 1







// Including libraries:
#include <SPI.h>
#include <Ethernet.h>
#include <Streaming.h>
#include <MemoryFree.h>
#include <SkaarhojPgmspace.h>

// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9}; // <= SETUP!  MAC address of the Arduino
IPAddress clientIp(10, 0, 10, 227);              // <= SETUP!  IP address of the Arduino
IPAddress switcherIp(10, 0, 11, 227);           // <= SETUP!  IP address of the ATEM Switcher

IPAddress dnsserver(0, 0, 0, 0);
IPAddress gateway(0, 0, 0, 0);
IPAddress subnet(255, 255, 0, 0);

// Include ClientVMix library and make an instance:
// The port number is chosen randomly among high numbers.
#include <ClientVMixUDP.h>
ClientVMixUDP VmixSwitcher;


uint16_t delayBetweenCmds = 500;    // Delay in ms between each command test
uint16_t delayBetweenValues = 100;    // Delay in ms between each submission of values in a command test. Down to 30 ms is OK
uint16_t timeOutDelay = 2000;      // Delay in which to time out, default around 1000.


void setup() {

  delay(2000);
  randomSeed(analogRead(5));  // For random port selection

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac, clientIp, dnsserver, gateway, subnet);
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
  VmixSwitcher.begin(switcherIp);
  VmixSwitcher.serialOutput(0x80);
  VmixSwitcher.connect();
}



/*
   Utility function, printing a line
*/
void printLine()  {
  Serial << "-------------------------\n";
}

/*
   Utility function, printing audio levels and peak
*/
void printAudioLevels(char * label, long L, long R)  {
  int l = L;
  int r = R;

  Serial << label;
  Serial << F(" (") << _DECPADL(l, 3, " ") << F("db) ");
  for (int a = 6; a >= -60; a -= 6)  {
    Serial << (l > a ? F("*") : F(" "));
  }
  Serial << F("|");
  for (int a = -60; a <= 6; a += 6)  {
    Serial << (r > a ? F("*") : F(" "));
  }
  Serial << F(" (") << r << F("db) \n");
}

/*
   Utility function, returns true if keypress
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
   Pause
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
        VmixSwitcher.runLoop();
      }
      Serial.println(F("**** STARTED AGAIN ****"));
      break;
  }
}




void loop() {



  // Waiting for ATEM switcher to connect:
  while (!VmixSwitcher.hasInitialized())  {
    VmixSwitcher.runLoop();
  }
  //   VmixSwitcher.serialOutput(0);



  long timeout, origValue;   // Timing
  uint8_t a, b;     // General purpose

  Serial << F("\n\n*****************************************\n* VMIX Arduino API Test Report: \n*****************************************\n");

  Serial << F("Firmware Version: ") << VmixSwitcher.getvMixVersionA() << F(".") << VmixSwitcher.getvMixVersionB() << F(".") << VmixSwitcher.getvMixVersionC() << F(".") << VmixSwitcher.getvMixVersionD() << F("\n\n");

  // Input properties:
  Serial << F("\nVideo Mixer Input (InPr) - Properties:\n");
  printLine();
  processCommands();
  Serial << F("Source - Short - Long Name - Loop - Port.Type - Paused - Duration\n");
  for (a = 0; a < 100; a++)  {
    if (VmixSwitcher.getInputPropertiesType(a) > 0)   {
      Serial << _DECPADL(a + 1, 2, " ") << F(" - ")
             << _STRPADR(VmixSwitcher.getInputPropertiesShortName(a), 4, " ") << F(" - ")
             << _STRPADR(VmixSwitcher.getInputPropertiesLongName(a), 20, " ") << F(" - ")
             << (VmixSwitcher.getInputPropertiesLoop(a) ? "Y" : "N")  << F(" - ")
             << _DECPADL(VmixSwitcher.getInputPropertiesType(a), 2, " ")  << F(" - ")
             << (VmixSwitcher.getInputPropertiesState(a) ? "Y" : "N")  << F(" - ")
             << _DECPADL(VmixSwitcher.getInputPropertiesDuration(a), 5, " ")  << F("\n");
    }
  }

  // Input properties:
  Serial << F("\nAudio Mixer Input (AuPr) - Properties:\n");
  printLine();
  processCommands();
  Serial << F("Source  - Solo - Bal - Vol - Muted - Busses(BAM)\n");
  for (a = 0; a <= 100; a++)  {
    if (VmixSwitcher.getInputPropertiesType(a) > 0)   {
      Serial << _DECPADL(a + 1, 2, " ") << F(" - ")
             << (VmixSwitcher.getAudioPropertiesSolo(a) ? "Y" : "N")  << F(" - ")
             << _DECPADL(VmixSwitcher.getAudioPropertiesBalance(a), 4, " ")  << F(" - ")
             << _DECPADL(VmixSwitcher.getAudioPropertiesVolume(a), 4, " ")  << F(" - ")
             << (VmixSwitcher.getAudioPropertiesMuted(a) ? "Y" : "N")  << F(" - ")
             << _BINPADL(VmixSwitcher.getAudioPropertiesBusses(a), 3, "0")  << F("\n");
    }
  }

  Serial << F("Master audio properties:\n");
  Serial << F("Master volume: ") << VmixSwitcher.getMasterAudioPropertiesVolume() << "\n";
  Serial << F("Headph volume: ") << VmixSwitcher.getMasterAudioPropertiesHeadphoneVolume() << "\n";
  Serial << F("Master muted : ") << (VmixSwitcher.getMasterAudioPropertiesMuted()?"yes":"no") << "\n";

  VmixSwitcher.runLoop();
  delay(1000);


  #if TEST_STREAM_RECORD

  timeout = millis();
  Serial << F("\nStarting recording:\n");
  VmixSwitcher.performRecordAction(true);
  while (VmixSwitcher.getRecordActive() != true && !VmixSwitcher.hasTimedOut(timeout, timeOutDelay)) {
    VmixSwitcher.runLoop();
  }
  if (VmixSwitcher.hasTimedOut(timeout, timeOutDelay))  {
    Serial << F("ERROR");
  } else {
    Serial << F("OK");
  }

  delay(5000);

  timeout = millis();
  Serial << F("\nStopping recording:\n");
  VmixSwitcher.performRecordAction(false);
  while (VmixSwitcher.getRecordActive() != false && !VmixSwitcher.hasTimedOut(timeout, timeOutDelay)) {
    VmixSwitcher.runLoop();
  }
  if (VmixSwitcher.hasTimedOut(timeout, timeOutDelay))  {
    Serial << F("ERROR");
  } else {
    Serial << F("OK");
  }

  delay(1000);

    timeout = millis();
  Serial << F("\nStarting streaming:");
  VmixSwitcher.performStreamAction(true);
  while (VmixSwitcher.getStreamActive() != true && !VmixSwitcher.hasTimedOut(timeout, timeOutDelay)) {
    VmixSwitcher.runLoop();
  }
  if (VmixSwitcher.hasTimedOut(timeout, timeOutDelay))  {
    Serial << F("ERROR");
  } else {
    Serial << F("OK");
  }

  delay(5000);

  timeout = millis();
  Serial << F("\nStopping streaming: ");
  VmixSwitcher.performStreamAction(false);
  while (VmixSwitcher.getStreamActive() != false && !VmixSwitcher.hasTimedOut(timeout, timeOutDelay)) {
    VmixSwitcher.runLoop();
  }
  if (VmixSwitcher.hasTimedOut(timeout, timeOutDelay))  {
    Serial << F("ERROR");
  } else {
    Serial << F("OK");
  }

  delay(1000);

#endif

#if TEST_ME
  // Transition Position - Position
  Serial << F("\nTransition Position:\n");
  printLine();
  processCommands();
  for (uint16_t position = 0; position <= 255; position += 100)	{
    VmixSwitcher.setTransitionPosition(position);
    Serial << F("setTransitionPosition(") << position << F("): ") << (millis() - timeout) << F(" ms\n");
    VmixSwitcher.runLoop(delayBetweenValues);	// Short delay
  }
  VmixSwitcher.setTransitionPosition(255);
  VmixSwitcher.runLoop(delayBetweenCmds);	// Short delay


  // Cut + Auto:
  Serial << F("\nCut:\n");
  printLine();
  processCommands();

  a = VmixSwitcher.getActiveInput();
  b = VmixSwitcher.getPreviewInput();
  // Serial << F("Setup for Cut... ");
  VmixSwitcher.setActiveInput(1);
  VmixSwitcher.setPreviewInput(2);
  VmixSwitcher.runLoop(1000);
  // Serial << F("Cut!\n");
  VmixSwitcher.performCutAction(true);
  VmixSwitcher.runLoop(1000);

  timeout = millis();
  while (VmixSwitcher.getActiveInput() != 2 && VmixSwitcher.getPreviewInput() != 1 && !VmixSwitcher.hasTimedOut(timeout, timeOutDelay))	{
    VmixSwitcher.runLoop();
  }
  if (VmixSwitcher.hasTimedOut(timeout, timeOutDelay))	{
    Serial << F("ERROR");
  }
  else {
    Serial << F("OK");
  }
  Serial << F(" for performCutAction(true): ") << (millis() - timeout) << F(" ms\n");

  VmixSwitcher.runLoop(delayBetweenCmds);	// Short delay

  Serial << F("\nFade:\n");
  printLine();
  processCommands();
  VmixSwitcher.runLoop(200);
  VmixSwitcher.performFadeFader(true);
  VmixSwitcher.runLoop(2000);

  // Reset:
  VmixSwitcher.setActiveInput(a);
  VmixSwitcher.setPreviewInput(b);
  VmixSwitcher.runLoop(200);

  VmixSwitcher.runLoop(delayBetweenCmds);	// Short delay


  // Fade-to-Black
  Serial << F("\nFade to black:\n");
  printLine();
  processCommands();

  bool ftbEnabled = VmixSwitcher.getFadeToBlackActive();
  VmixSwitcher.runLoop(200);
  VmixSwitcher.performFadeToBlackAction(true);
  VmixSwitcher.runLoop(2000);
  VmixSwitcher.performFadeToBlackAction(true);
  VmixSwitcher.runLoop(2000);
#endif




#if (!LOOP)
  Serial << F("\n\n**************** DONE ! *******************\n");
  Serial << F("freeMemory()=") << freeMemory() << "\n";
  Serial << F("millis()/1000=") << millis() / 1000 << "\n";
  while (true) {
  };
#endif
}
