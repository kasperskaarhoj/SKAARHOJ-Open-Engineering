
#include "Wire.h"
#include "MCP23017.h"
#include "SkaarhojEncoders.h"
SkaarhojEncoders encoders;



// no-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}



void setup() {

  // Start the Serial (debugging) and UDP:
  Serial.begin(115200);  
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  Wire.begin();
  encoders.begin(7);
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

