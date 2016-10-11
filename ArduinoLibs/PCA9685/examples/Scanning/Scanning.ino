/*****************


 * - kasper
 */
 
#include <Wire.h>
#include <PCA9685.h>

#include <Streaming.h>

PCA9685 ledDriver; 



void setup()
{
  Serial.begin(115200); // set up serial
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  Wire.begin();           // Wire must be started!

}

void loop()
{
  Serial << "\n";

  for(uint8_t i=0;i<=63;i++)  {  
    // Set up each board:
    ledDriver.begin(i);  // Address pins A5-A0 set to B111000

    if (ledDriver.init())  {
      Serial << "\nBoard #" << i << " found\n";
    } else {
      Serial << ".";
    }
    delay(10);
  }
} 




