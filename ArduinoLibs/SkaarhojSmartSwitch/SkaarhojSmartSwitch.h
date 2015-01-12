/*  SkaarhojSmartSwitch library for Arduino
	Copyright (C) 2012 Kasper Skårhøj    <kasperskaarhoj@gmail.com> 
	Copyright (C) 2012 Filip Sandborg-Olsen   <filipsandborg@me.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SkaarhojSmartSwitch_H
#define SkaarhojSmartSwitch_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define PIXEL_ASPECT 1.333
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

// Button constants
#define BUTTON1	1 << 0
#define BUTTON2	1 << 1
#define BUTTON3	1 << 2
#define BUTTON4	1 << 3
#define BUTTON_ALL B1111

// Text options
#define TEXT_LEFT 		1 << 0
#define TEXT_CENTER 	1 << 1
#define TEXT_RIGHT 		1 << 2
#define TEXT_BACKGROUND	1 << 3
#define TEXT_REVERSE 	1 << 4
#define TEXT_NORMAL 	1 << 5

// Pixel consts
#define PIXEL_WHITE 0
#define PIXEL_BLACK 1

// Image constants
#define IMAGE_CENTER	1 << 0
#define IMAGE_RIGHT		1 << 1
#define IMAGE_LEFT		0

#include "Wire.h"
#include "MCP23017.h"

/**
  Version 1.0.0
	(Semantic Versioning)
**/

class SkaarhojSmartSwitch
{
  private:
	uint8_t _boardAddress;
	bool _reverseButtons;
	MCP23017 _buttonMux;
	uint8_t _buttonStatus;
	uint8_t _buttonStatusLastUp;
	uint8_t _buttonStatusLastDown;

	uint8_t _buttonMode;

	uint8_t _colorBalanceRed[10];	
	uint8_t _colorBalanceGreen[10];
	uint8_t _defaultColorNumber;
	uint8_t _buttonColorCache[8];
	bool _B1Alt;
	
	uint8_t *_pixMap[4];

	uint8_t _defaultColor;

	int _clockPin;
	int _dataPin;

	//extern const unsigned char _Font8x5[][6];
  public:
	SkaarhojSmartSwitch();
	void begin(int address);
	void begin(int address, const uint8_t clockPin, const uint8_t dataPin);
	void setColorBalance(int colorNumber, int redPart, int greenPart);
	void setDefaultColor(uint8_t color);
	void setButtonColorsToDefault();
	void setButtonColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t buttons);	// OK
	void setButtonBrightness(uint8_t brightness, uint8_t buttons);	// OK
	
	bool buttonUp(int buttonNumber);
	bool buttonDown(int buttonNumber);
	bool buttonIsPressed(int buttonNumber);
	uint8_t buttonUpAll();
	uint8_t buttonDownAll();
	uint8_t buttonIsPressedAll();
	bool isButtonIn(int buttonNumber, uint8_t allButtonsState);

	uint8_t getButtonModes();
	void setButtonModes(uint8_t modes);

	void clearPixmap(int buttons);
	void drawRectangle(int x, int y, int width, int height, int buttons, int color,  int border);
	void drawSquare(int x, int y, int len, int buttons);
	void drawCircle(int x, int y, int radius, int buttons);
	void drawLine(int start_x, int start_y, int end_x, int end_y, int buttons);
	void updateScreen(int buttons);
	void clearScreen(int buttons);
	void drawString(char* string, int buttons);
	void writeText(uint8_t buttons, char* textString, uint8_t lineNumber, int options);
	void writeTextXY(uint8_t buttons, char* textString, int x, int y, int options);
	inline void drawPixel(int x, int y, int val, int buttons);
	void drawImage(uint8_t buttons, int x, int y, int options, uint8_t* image);
	void drawChar(int x, int y, char c, int buttons, uint8_t* font, uint8_t reverse);
	void drawHorisontalLine(int y, int buttons);
	void drawVerticalLine(int x, int buttons);

	
  private:
	void _readButtonStatus();
	bool _validButtonNumber(int buttonNumber);
	bool _validColorNumber(int colorNumber);
	bool _validPercentage(int percentage);
	int _writeCommand(int address, int value, uint8_t buttons);
	void _writeEnableButtons(uint8_t buttons);
};
#endif 
