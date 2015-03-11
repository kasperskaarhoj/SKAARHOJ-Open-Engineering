
#include "Wire.h"
#include "MCP23017.h"
#include "SkaarhojEncoders.h"
#include "Streaming.h"
//  #include "SkaarhojPgmspace.h"  - 23/2 2014
SkaarhojEncoders encoders;



void setup() {

  // Start the Serial (debugging) and UDP:
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  Wire.begin();
  encoders.begin(1);
  encoders.serialOutput(0);
  //  encoders.setStateCheckDelay(0);
}


void loop() {
  encoders.testProgramme(B11111);
}

