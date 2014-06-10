/*********************************************************************
This library is inspired by the Adafruit_SSD1306 library from Adafruit
written by Limor Fried/Ladyada  for Adafruit Industries. The original 
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing 
open source code, so SKAARHOJ encourage you to support Adafruit and 
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/

#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>

#include <Wire.h>

#include "Adafruit_GFX.h"
#include "SkaarhojOLEDRocker.h"


// The memory buffer for the LCD
static uint8_t buffer[SKAARHOJOLEDRocker_LCDWIDTH * SKAARHOJOLEDRocker_LCDHEIGHT / 8];

// The most basic function, set a single pixel
void SkaarhojOLEDRocker::drawPixel(int16_t x, int16_t y, uint16_t color) {
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
  if (color == WHITE) 
    buffer[x/8+ y*SKAARHOJOLEDRocker_LCDWIDTH/8] |= B10000000 >> (x%8);  
  else
    buffer[x/8+ y*SKAARHOJOLEDRocker_LCDHEIGHT/8] &= ~(B10000000 >> (x%8)); 
}

// Empty constructor.
SkaarhojOLEDRocker::SkaarhojOLEDRocker() : Adafruit_GFX(SKAARHOJOLEDRocker_LCDWIDTH, SKAARHOJOLEDRocker_LCDHEIGHT) {}		


void SkaarhojOLEDRocker::begin(uint8_t address) {
	// NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())

	_boardAddress = 88 | (address & B111);	// 0-7

  	// Set SPI pins up:
	_clockPin = 48;
	_dataPin = 49;

	pinMode(_clockPin, OUTPUT);
	pinMode(_dataPin, OUTPUT);

    clkport     = portOutputRegister(digitalPinToPort(_clockPin));
    clkpinmask  = digitalPinToBitMask(_clockPin);
    mosiport    = portOutputRegister(digitalPinToPort(_dataPin));
    mosipinmask = digitalPinToBitMask(_dataPin);

	// Control pins:
	_cs = 0;
	_dc = false;
	_vcc = false;
	_rst = false;
	writeControlPins();
  	delay(1000);

  	// bring VAH high:
	_vcc = true;
	writeControlPins();

  	// wait 10ms
  	delay(2);

  	// bring out of reset
	_rst = true;
	writeControlPins();
  	delay(2);


    // Init sequence for OLED module
	uint8_t cs = 1;


		// Software Reset:
	chipSelect(cs);
    sendCommand(0x01);
	chipSelect(0);

		// Reserved 1-3:
	chipSelect(cs);
    sendCommand(0x10);
    sendData(0x03);
	chipSelect(0);

	chipSelect(cs);
    sendCommand(0x12);
    sendData(0x63);
	chipSelect(0);

	chipSelect(cs);
    sendCommand(0x13);
    sendData(0x00);
	chipSelect(0);

		// Dot Matrix Display StandBy ON/OFF:
	chipSelect(cs);
    sendCommand(0x14);
    sendData(0x00);
	chipSelect(0);

		// Reserved 4-8
	chipSelect(cs);
    sendCommand(0x16);
    sendData(0x00);
	chipSelect(0);

	chipSelect(cs);
    sendCommand(0x18);
    sendData(0x09);
	chipSelect(0);

		// Reserved 9
	chipSelect(cs);
    sendCommand(0x48);
    sendData(0x03);
	chipSelect(0);

		// System Clock Division Ratio Setting
	chipSelect(cs);
    sendCommand(0xD0);
    sendData(0x80);
	chipSelect(0);

		// Reserved 11
	chipSelect(cs);
    sendCommand(0xDD);
    sendData(0x88);
	chipSelect(0);



		// Dot Matrix Display ON/OFF:
	chipSelect(cs);
    sendCommand(0x02);
    sendData(0x01);
	chipSelect(0);

  	delay(10);
}


void SkaarhojOLEDRocker::sendCommand(uint8_t c) { 

	setDC(false);
    fastSPIwrite(c);
}
void SkaarhojOLEDRocker::sendData(uint8_t c) {

	setDC(true);
    fastSPIwrite(c);
}

// clear everything
void SkaarhojOLEDRocker::clearDisplay(void) {
  memset(buffer, 0, (SKAARHOJOLEDRocker_LCDWIDTH*SKAARHOJOLEDRocker_LCDHEIGHT/8));
}

void SkaarhojOLEDRocker::display(uint8_t cs) {

	chipSelect(cs);
    sendCommand(0x34);
    sendData(0x00);
	chipSelect(0);

	chipSelect(cs);
    sendCommand(0x35);
    sendData(0x0B);
	chipSelect(0);

	chipSelect(cs);
    sendCommand(0x36);
    sendData(0x00);
	chipSelect(0);

	chipSelect(cs);
    sendCommand(0x37);
    sendData(0x3F);
	chipSelect(0);
	

	chipSelect(cs);
 	sendCommand(0x08);  // Enable write to data ram

	setDC(true);

    for (uint16_t i=0; i<768; i++) {
		// Writing 8 pixels in one go:
      fastSPIwrite(buffer[i]);
    }

	chipSelect(0);
}

inline void SkaarhojOLEDRocker::fastSPIwrite(uint8_t d) {
  for(uint8_t bit = 0x80; bit; bit >>= 1) {
    *clkport &= ~clkpinmask;
    if(d & bit) *mosiport |=  mosipinmask;
    else        *mosiport &= ~mosipinmask;
    *clkport |=  clkpinmask;
  }
}

void SkaarhojOLEDRocker::chipSelect(uint8_t cs) {
	_cs = cs;
	writeControlPins();
}

void SkaarhojOLEDRocker::setDC(bool dc) {
	_dc = dc;
	writeControlPins();
}

void SkaarhojOLEDRocker::setVCC(bool vcc) {
	_vcc = vcc;
	writeControlPins();
}

void SkaarhojOLEDRocker::writeControlPins() {
  Wire.beginTransmission(_boardAddress);
  Wire.write(((uint8_t)~_cs));
  Wire.write((_rst << 7) | (_dc << 6) | (_vcc << 5));
  Wire.endTransmission();
}



