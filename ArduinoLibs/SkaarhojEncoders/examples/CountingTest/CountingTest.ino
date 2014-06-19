
#include "Wire.h"
#include "MCP23017.h"
#include "SkaarhojEncoders.h"
#include "Streaming.h"
SkaarhojEncoders encoders;



void setup() {

  // Start the Serial (debugging) and UDP:
  Serial.begin(115200);  
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  Wire.begin();
  encoders.begin(5);
  encoders.serialOutput(1);
}


void loop() {
  encoders.runLoop();
  for(uint8_t i=0; i<5; i++)  {
    int encValue = encoders.state(i,1000);
    if (encValue)  {
   //    Serial << F("Encoder ") << i << F(": ") << encValue << "\n"; 
    }
  }
}

