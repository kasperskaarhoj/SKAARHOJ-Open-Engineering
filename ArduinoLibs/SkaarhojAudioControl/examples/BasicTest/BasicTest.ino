/*****************
 * 
 * 
 * 
 * 
 * 
 * - kasper
 */


#include <Wire.h>
#include <MCP23017.h>
#include <PCA9685.h>

#include <SkaarhojAudioControl.h>
SkaarhojAudioControl AudioControl;

#include <Streaming.h>




void setup() {

  // Start the Serial (debugging) and UDP:
  Serial.begin(115200);  
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  // Always initialize Wire before starting the AudioControl library
  Wire.begin(); 

  AudioControl.begin(5);	// Address 4
  AudioControl.setIsMasterBoard();
  delay(1000);



  for(int j=0; j<5; j++) {
    AudioControl.setButtonLight(j+1, true);
    delay(500);
  }
  for(int j=0; j<5; j++) {
    AudioControl.setButtonLight(j+1, false);
    delay(500);
  }

  for(int i=1; i<=2; i++) {
    for(int j=0; j<4; j++) {
      AudioControl.setChannelIndicatorLight(i, j);
      delay(500);
    }
  }
  AudioControl.setChannelIndicatorLight(1, 0);
  AudioControl.setChannelIndicatorLight(2, 0);
  
  AudioControl.VUmeter(16445,16445);
  delay(500);
  AudioControl.VUmeter(0,0);
  delay(500);
  AudioControl.VUmeter(16445,16445);
  delay(500);
  AudioControl.VUmeter(0,0);
  delay(500);

}

void loop() {
  word buttons = AudioControl.buttonDownAll();
  for(int j=0; j<5; j++) {
    if(buttons & (1 << j)) {
      Serial.print("Button ");
      Serial.print(j, DEC);
      Serial.println(" was pressed!");
    }
  }
}


