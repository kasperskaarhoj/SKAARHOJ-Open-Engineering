/*****************
 * This example of the MCP23017 library assumes that all pins on the chip is set up as inputs.
 * It will read the inputs and print to Serial out. So if you open the Serial-monitor you should see 
 * binary numbers written 10 times a second. As you short one of the inputs to ground it should change from "1" to "0"
 * 
 * Notice that the chips 3 address pins are assumed to be grounded (address = 0). You need to change the address
 * if this is any different.
 *
 * If you can confirm that the chip responds to this example you should be ready to do more complex stuff with it.
 * - kasper
 */
 
#include <Wire.h>
#include <MCP23017.h>
#include <Streaming.h>

MCP23017 GPIOchip; 

void setup()
{
  Serial.begin(115200); // set up serial
  Serial.println("Serial Started");

  Wire.begin();           // Wire must be started!
  GPIOchip.begin(B101);  // Address set to zero.
  GPIOchip.init();
  GPIOchip.internalPullupMask((B11111111 <<8) | B11111111);  //GPA7..GPA0, GPB7..GPB0

  GPIOchip.pinMode(0,  INPUT);
  GPIOchip.pinMode(1,  INPUT);
  GPIOchip.pinMode(2,  INPUT);
  GPIOchip.pinMode(3,  INPUT);
  GPIOchip.pinMode(4,  INPUT);
  GPIOchip.pinMode(5,  INPUT);
  GPIOchip.pinMode(6,  INPUT);
  GPIOchip.pinMode(7,  INPUT);
  GPIOchip.pinMode(8,  INPUT);
  GPIOchip.pinMode(9,  INPUT);
  GPIOchip.pinMode(10,  INPUT);
  GPIOchip.pinMode(11,  INPUT);
  GPIOchip.pinMode(12,  INPUT);
  GPIOchip.pinMode(13,  INPUT);
  GPIOchip.pinMode(14,  INPUT);
  GPIOchip.pinMode(15,  INPUT);
}

void loop()
{
  word buttonStatus = GPIOchip.digitalWordRead();
  Serial << _BINPADL(buttonStatus, 16, "0") << F("\n");
  delay(5);
} 




