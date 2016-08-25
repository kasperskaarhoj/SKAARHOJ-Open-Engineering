#include "Wire.h"
#include <BMDSDIControl.h>
#include "Streaming.h"


BMD_SDICameraControl_I2C  sdiCameraControl;

void setup() {
  sdiCameraControl.begin(0x6E);
  Serial.begin(115200);

  // Turn on control overrides
  sdiCameraControl.setOverride(true);

}

float value = 0.0;
float increment = 0.01;
void loop() {
  if(value + increment > 1.0 || value + increment < 0.0) increment = -increment;
  value += increment;
  sdiCameraControl.writeCommandFixed16(1, 9, 0, 0, value);
  delay(50);
  Serial << "Setting iris to " << value << "\n";
}
