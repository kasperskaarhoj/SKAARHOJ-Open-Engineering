

#include "SkaarhojAnalog.h"
#include "ADS7828.h"
#include "Wire.h"
SkaarhojAnalog potmeter1;
SkaarhojAnalog potmeter2;
SkaarhojAnalog potmeter3;
SkaarhojAnalog potmeter4;

//#include <MemoryFree.h>

#include <Streaming.h>


void setup() {

  // Start the Serial (debugging) and UDP:
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  potmeter1.uniDirectionalSlider_init(10, 35, 35, 3, 0);
  potmeter1.uniDirectionalSlider_disableUnidirectionality(true);

  potmeter2.uniDirectionalSlider_init(10, 35, 35, 3, 1);
  potmeter2.uniDirectionalSlider_disableUnidirectionality(true);
  /*
  potmeter3.uniDirectionalSlider_init(10, 35, 35, 0, 2);
  potmeter3.uniDirectionalSlider_disableUnidirectionality(true);
  
  potmeter4.uniDirectionalSlider_init(10, 35, 35, 0, 3);
  potmeter4.uniDirectionalSlider_disableUnidirectionality(true);
  */
}

void loop() {
  if (potmeter1.uniDirectionalSlider_hasMoved()) {
    Serial << "1:" << potmeter1.uniDirectionalSlider_position() << "\n";
  }
  /*if (potmeter2.uniDirectionalSlider_hasMoved()) {
    Serial << "2:" << potmeter2.uniDirectionalSlider_position() << "\n";
  }*/
  /*if (potmeter3.uniDirectionalSlider_hasMoved()) {
    Serial << "3:" << potmeter3.uniDirectionalSlider_position() << "\n";
  }
  if (potmeter4.uniDirectionalSlider_hasMoved()) {
    Serial << "4:" << potmeter4.uniDirectionalSlider_position() << "\n";
  }*/
}

