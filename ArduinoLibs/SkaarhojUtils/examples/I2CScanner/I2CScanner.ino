/**
 * TCA9548 I2CScanner.pde -- I2C bus scanner for Arduino
 *
 * Based on code c. 2009, Tod E. Kurt, http://todbot.com/blog/
 * Copied from Adafruit website.
 */
 
#include "Wire.h"

#define TCAADDR 0x70
#include "Streaming.h"
 
void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}
 
 
// standard Arduino setup()
void setup()
{
    while (!Serial);
    delay(1000);
 
    Wire.begin();
    
    Serial.begin(115200);
    Serial.println("\nTCAScanner ready!");
    
    for (uint8_t t=0; t<8; t++) {
      tcaselect(t);
      Serial.print("TCA Port #"); Serial.println(t);
 
      for (uint8_t addr = 0; addr<=127; addr++) {
        if (addr == TCAADDR) continue;
      
        Wire.beginTransmission(addr);
        if(!Wire.endTransmission()) {
          Serial.print("Found I2C 0x");  
          Serial << _HEXPADL(addr,2,"0") << " (" << _DECPADL(addr,3," ") << ")" << " = " << _BINPADL(addr,7,"0") << "_ = " << (addr&0xF8) << "+" << (addr&0x7) << "\n";
        }
      }
    }
    Serial.println("\ndone");
}
 
void loop() 
{
}