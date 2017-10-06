/*********************************************************************
This library is inspired by the Adafruit_SSD1306 library from Adafruit
written by Limor Fried/Ladyada  for Adafruit Industries. The original
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing
open source code, so SKAARHOJ encourage you to support Adafruit and
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/

#include "SkaarhojOLED32x64.h"

#ifdef __arm__ // Arduino Due:
#define _BV(bit) (1 << (bit))
#endif

// The memory buffer for the LCD
static uint8_t buffer12832[SKAARHOJDISPARRAY_LCDHEIGHT * SKAARHOJDISPARRAY_LCDWIDTH / 8];

// The most basic function, set a single pixel
void SkaarhojOLED32x64::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((bbox_width > 0 && x >= bbox_width) || (bbox_height > 0 && y >= bbox_height))
    return;

  x += bbox_x;
  y += bbox_y;

  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }

  // x is which column
  if ((color == WHITE)^_invPixels)
    buffer12832[x + (y / 8) * SKAARHOJDISPARRAY_LCDWIDTH] |= _BV((y % 8));
  else
    buffer12832[x + (y / 8) * SKAARHOJDISPARRAY_LCDWIDTH] &= ~_BV((y % 8));
}

// Empty constructor.
SkaarhojOLED32x64::SkaarhojOLED32x64() : Adafruit_GFX(SKAARHOJDISPARRAY_LCDWIDTH, SKAARHOJDISPARRAY_LCDHEIGHT) {}

void SkaarhojOLED32x64::begin(bool initReset, bool vccDefault) {
  // NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())

  _boardAddress = 0x3C;

  Wire.beginTransmission(_boardAddress);
  // Display Off:
  sendCommand(0xAE);

  // Set Display Clock Divide Ratio / Oscillator Frequency:
  sendCommand(0xD5);
  sendCommand(0x80);

  // Set Multiplex Ratio:
  sendCommand(0xA8);
  sendCommand(0x1F);

  // Set Display Offset:
  sendCommand(0xD3);
  sendCommand(0x0);

  // Set Display Start Line:
  sendCommand(0x00);

  // Set Charge Pump:
  sendCommand(0x8D);
  sendCommand(0x14); // Internal VSS

  // Memory Mode (Not part of default application suggestion!)
  sendCommand(0x20);
  sendCommand(0x00); // Horizontal Addressing Mode

  // Set Segment Re-Map:
  sendCommand(0xA1);

  // Set COM Output Scan Direction:
  sendCommand(0xC8);

  // Set COM Pins Hardware Configuration:
  sendCommand(0xDA);
  sendCommand(0x02);

  // Set Contrast Control:
  sendCommand(0x81);
  sendCommand(0x8F);

  // Set Pre-Charge Period:
  sendCommand(0xD9);
  sendCommand(0xF1);

  // Set VCOMH Deselect Level:
  sendCommand(0xDB);
  sendCommand(0x40);

  // Set Entire Display On/Off:
  sendCommand(0xA4);

  // Set Normal/Inverse Display:
  sendCommand(0xA6);



  // Turn Display On:
  sendCommand(0xAF);


  delay(100);
}

void SkaarhojOLED32x64::sendCommand(uint8_t c) {
  Wire.beginTransmission(_boardAddress);
  Wire.write(0x00);
  Wire.write(c);
  Wire.endTransmission();
}

// clear everything
void SkaarhojOLED32x64::clearDisplay(void) { memset(buffer12832, 0, (SKAARHOJDISPARRAY_LCDWIDTH * SKAARHOJDISPARRAY_LCDHEIGHT / 8)); }

void SkaarhojOLED32x64::invertDisplay(bool i) {
  if (i) {
    sendCommand(0xA7); // Inverted
  } else {
    sendCommand(0xA6); // Normal,
  }
}

void SkaarhojOLED32x64::display() {
    sendCommand(0x21);
    sendCommand(0x0);
    sendCommand(0x63);

    sendCommand(0x20);
    sendCommand(0);
    sendCommand(3);

    sendCommand(0x40);
    for(uint16_t i=0; i < 256; i++) {
      sendCommand(0xFF);
    }
}

// void SkaarhojOLED32x64::testProgramme(uint8_t buttonMask) {
//   static uint16_t lastTime;

//   for (int i = 0; i < 8; i++) {
//     if (buttonMask & (B1 << i)) {

//       clearDisplay(); // clears the screen and buffer
//       setTextColor(WHITE);
//       setCursor(0, 0);
//       setTextSize(1);
//       print(millis(), HEX);
//       if (i == 0) {
//         print(F(" [dt="));
//         print((uint16_t)millis() - lastTime);
//         lastTime = millis();
//         print(F("]"));
//       }
//       print(F(" "));
//       setTextSize(2);
//       print((uint8_t)millis(), BIN);
//       setTextSize(1);
//       print(millis(), DEC);
//       print(F(" - "));
//       print(millis(), BIN);
//       display(B1 << i);
//     }
//   }
// }
