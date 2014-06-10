/*****************


 * - kasper
 */
 
#include <Wire.h>
#include <PCA9685.h>


// no-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}

PCA9685 ledDriver; 



void setup()
{
  Serial.begin(9600); // set up serial
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




