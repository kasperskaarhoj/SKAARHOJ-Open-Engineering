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

#include <SkaarhojSmartSwitch.h>
SkaarhojSmartSwitch SmartSwitch;

#include <Streaming.h>



// Animated logo
static uint8_t movielogo[] PROGMEM = {
  32, // width
  20, // height

  /* page 0 (lines 0-7) */
  0xfc,0xfe,0xf3,0xf3,0xff,0xff,0xf3,0xf3,0xff,0xff,0xf3,0xf3,0xff,0xff,0xf7,0xf3,
  0xff,0xff,0xf7,0xf3,0xf7,0xff,0xff,0xf3,0xf3,0xfe,0xfc,0x80,0x80,0xc0,0xc0,0xc0,
  
  /* page 1 (lines 8-15) */
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1f,0x1f,0x3f,0x3f,0x7f,
  
  /* page 2 (lines 16-23) */
  0x3,0x7,0xc,0xc,0xf,0xf,0xc,0xc,0xf,0xf,0xc,0xc,0xf,0xf,0xe,0xc,
  0xf,0xf,0xe,0xc,0xe,0xf,0xf,0xc,0xe,0x7,0x3,0x0,0x0,0x0,0x0,0x0,
  
};




void setup() {

  // Start the Serial (debugging) and UDP:
  Serial.begin(9600);  
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  // Always initialize Wire before starting the SmartSwitch library
  Wire.begin(); 

  // Switches on address 0:  
  //SmartSwitch.begin(0);	// Address 0, pins 5+6 default
  SmartSwitch.begin(4,48,49);	// Address 4, pins 48+49

  // Setting full brightness (range 0-7) of all buttons:
  SmartSwitch.setButtonBrightness(7, BUTTON_ALL);
  SmartSwitch.setButtonBrightness(7, BUTTON_ALL);

  // Setting white color for all buttons:
  SmartSwitch.setButtonColor(3,3,3,BUTTON_ALL);

  // Clear the display (if anything is on it:)
  SmartSwitch.clearScreen(BUTTON_ALL);
  delay(500);

  // Draw an image on the pixmap for button 1, centered, but offset -2 pixels in y direction (upwards). X-Y has origin in upper left corner and positive axes towards right/down
  SmartSwitch.drawImage(BUTTON1, 0, -2, IMAGE_CENTER, movielogo);
  SmartSwitch.updateScreen(BUTTON1);  // Writes the buffered pixmap to the button.
  delay(500);

  // Writing a bit of text on top of the pixmap - and write to button:
  SmartSwitch.writeText(BUTTON1, "TESTING", 3, TEXT_CENTER | TEXT_BACKGROUND | TEXT_REVERSE);
  SmartSwitch.updateScreen(BUTTON1);  // Writes the buffered pixmap to the button.

}
int i = 0;
int state, modes;
void loop() {
  word buttons = SmartSwitch.buttonUpAll();
  for(int j=0; j<4; j++) {
    if(buttons & (1 << j)) {
      modes = SmartSwitch.getButtonModes();
      Serial.print("Button ");
      Serial.print(j, DEC);
      Serial.println(" was pressed!");
      state = modes >> j*2 & B11;
      Serial.print("State: ");
      Serial.println(state, BIN);
      if(state == 0) {
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.drawLine(0,0,63,31, 1<<j);
        SmartSwitch.drawLine(0,31,63,0, 1<<j);
        SmartSwitch.drawHorisontalLine(0, 1<<j);
        SmartSwitch.drawHorisontalLine(31, 1<<j);
        SmartSwitch.drawVerticalLine(0, 1<<j);
        SmartSwitch.drawVerticalLine(63, 1<<j);
        SmartSwitch.updateScreen(1<<j);
        SmartSwitch.setButtonModes(modes ^ 1 << 2*j);
        SmartSwitch.setButtonColor(3,0,0, 1<<j);
      } else if (state == 1) {
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.drawCircle(31, 16, 21, 1<<j);
        SmartSwitch.updateScreen(1<<j);
        SmartSwitch.setButtonModes(modes ^ (state | 2) << 2*j);
        SmartSwitch.setButtonColor(0,3,0, 1<<j);
      } else {
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.updateScreen(1<<j);
        SmartSwitch.setButtonModes(modes ^ state << 2*j);
        SmartSwitch.setButtonColor(0,0,3, 1<<j);
      }
      Serial.print("ButtonState: ");
      Serial.println(SmartSwitch.getButtonModes(), BIN);
      
    }
  }
  /*clearPixmap(15);
  //drawCircle(10,10,9,1<<i);
  //drawRectangle(20,1,18,18,1<<i);
  drawLine(0,0,63,31, 1<<i);
  drawLine(0,31,63,0, 1<<i);
  drawHorisontalLine(0, 1<<i);
  drawHorisontalLine(31, 1<<i);
  drawVerticalLine(0, 1<<i);
  drawVerticalLine(63, 1<<i);
  updateScreen(15);
  if(i==3) {
   i = 0;
  } else {
    i++;
  }*/
}

void animatedBox() {
    int i = 0;
    int inc = 3;
    while(1){
    i = i + inc;
    if(i+12 > 256) {
      inc = -5;
      
    } else if(i < 0) {
      inc = 5;
      continue;
    }
    SmartSwitch.clearPixmap(15);
    //if(i % 64 > 0 && i > 64 && i < 256) {
 
    //SmartSwitch.drawRectangle(i%64,0,12,12, 1 << i/64, 0);
    //}
    SmartSwitch.updateScreen(15);
    }
}


