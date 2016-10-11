/*****************
 * This example demonstrates the library assuming that LEDs are connected to the outputs
 * It will run through all 16 LEDs turning them up and down in 10 steps.
 * The PCA9685 chip is assumed to have PINS A0,A1 and A2 grounded (LOW) and A3,A4 and A5 set to VCC (5V, HIGH)
 * 
 * - kasper
 */

#include <Wire.h>
#include <PCA9685.h>


PCA9685 ledDriver; 

void setup()
{
  Serial.begin(115200); // set up serial
  Serial.println("Serial Started");

  Wire.begin();           // Wire must be started!
  ledDriver.begin(B111000);  // Address pins A5-A0 set to B111000
  ledDriver.init();


}

void loop()
{
  // Notice: setLEDDimmed cannot be used in a loop to give you a pleasant 
  // "turning-up" of the LED. This is because each time you set a level for a LED
  // it will calculate random timing intervals for the PWM function in the chip
  // This is done in order to distribute current consumptions of the full time period.
  
  for(int level=5;level>=0;level--)  {
    for(int i=0; i<16; i++)  {
      ledDriver.setLEDDimmed(i,level*20);
      delay(100);     
    }
  }
  for(int level=0;level<=5;level++)  {
    for(int i=0; i<16; i++)  {
      ledDriver.setLEDDimmed(i,level*20);
      delay(100);     
    }
  }
} 


