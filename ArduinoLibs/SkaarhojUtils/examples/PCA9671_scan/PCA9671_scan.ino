#include "Wire.h"

int chipAddress = B1011101;  // 88 is the base for SCA/SCL based address settings!

void setup()
{
  Serial.begin(115200); // set up serial
  Serial.println("Start");

  Wire.begin();
  delay(100);

  for (uint8_t chipAddress = 16; chipAddress < 120; chipAddress++)   {
    Wire.requestFrom(chipAddress, 2);
    Serial.print(chipAddress);
    Serial.print("/");
    Serial.println(chipAddress, BIN);
    if (Wire.available()) {
      while (Wire.available()) {
        Serial.print(" - ");
        Serial.println(Wire.read(), BIN);
      }
    }
    delay(20);
  }
}

void loop() {
}

