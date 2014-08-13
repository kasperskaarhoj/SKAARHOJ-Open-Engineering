
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
  encoders.serialOutput(0);
//  encoders.setStateCheckDelay(0);
}


void loop() {
  encoders.runLoop();
  for(uint8_t i=0; i<5; i++)  {
    int encValue = encoders.state(i,1000);
    if (encValue)  {
       Serial << F("Enc ") << i << F(": ") << encValue << ((encValue==1 || encValue==-1)? String(" Count: ")+String(encoders.lastCount(i)) : String("") )<< "\n"; 
    }
  }
}

