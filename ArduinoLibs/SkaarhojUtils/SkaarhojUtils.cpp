/*  SkaarhojUtils Arduino library with various utilities for products from SKAARHOJ.com
    Copyright (C) 2012 Kasper Skårhøj    <kasperskaarhoj@gmail.com> 

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
#include "SkaarhojUtils.h"


SkaarhojUtils::SkaarhojUtils(){
	_debugMode = false;
}	// Empty constructor.
void SkaarhojUtils::debugMode()	{
	_debugMode=true;	
}


/**
 * Slider functions:
 */
void SkaarhojUtils::uniDirectionalSlider_init(){
	uniDirectionalSlider_init(10, 35, 35, A0);
}
void SkaarhojUtils::uniDirectionalSlider_init(int sliderTolerance, int sliderLowEndOffset, int sliderHighEndOffset, uint8_t analogInputPin)	{

		// Configuration constants, should have setter-methods:
	_uniDirectionalSlider_analogInputPin = analogInputPin;
	_uniDirectionalSlider_sliderTolerance = sliderTolerance;  // >0. How much it should be moved before it counts as a change.
    _uniDirectionalSlider_sliderLowEndOffset = sliderLowEndOffset;  // >0. How far the slider is moved in the low end before we start registering the value range: The starting position.
    _uniDirectionalSlider_sliderHighEndOffset = sliderHighEndOffset;  // >0. How far the slider is moved in the high end before we start registering the value range: The ending position.

	pinMode(analogInputPin, INPUT);
	
		// Internal variables during operation:
	_uniDirectionalSlider_previousSliderValue=-1;
	_uniDirectionalSlider_previousTransitionPosition=-1;
	_uniDirectionalSlider_sliderDirectionUp = false;
	_uniDirectionalSlider_disableUnidirectionality = false;
	
	uniDirectionalSlider_hasMoved();	// Make sure the _uniDirectionalSlider_previousSliderValue is set correctly.
}

void SkaarhojUtils::uniDirectionalSlider_disableUnidirectionality(bool disable) {
  _uniDirectionalSlider_disableUnidirectionality = disable;
  _uniDirectionalSlider_previousSliderValue = -1;
  _uniDirectionalSlider_previousTransitionPosition = -1;
  _uniDirectionalSlider_sliderDirectionUp = false;

  uniDirectionalSlider_hasMoved(); // Make sure the _uniDirectionalSlider_previousSliderValue is set correctly.
}

#include <Streaming.h>
bool SkaarhojUtils::uniDirectionalSlider_hasMoved()	{
	int sliderValue = analogRead(_uniDirectionalSlider_analogInputPin);
	bool closeToEnd = false;

	if (sliderValue - _uniDirectionalSlider_sliderLowEndOffset < _uniDirectionalSlider_sliderTolerance && sliderValue < _uniDirectionalSlider_previousSliderValue)
	closeToEnd = true;
	if (sliderValue + _uniDirectionalSlider_sliderHighEndOffset > 1023 - _uniDirectionalSlider_sliderTolerance && sliderValue > _uniDirectionalSlider_previousSliderValue)
	closeToEnd = true;

	if (abs(sliderValue - _uniDirectionalSlider_previousSliderValue) >= _uniDirectionalSlider_sliderTolerance || closeToEnd) {

	// Find direction:
	if (!_uniDirectionalSlider_disableUnidirectionality && sliderValue >= _uniDirectionalSlider_previousSliderValue + _uniDirectionalSlider_sliderTolerance && (_uniDirectionalSlider_previousSliderValue == -1 || _uniDirectionalSlider_previousSliderValue < _uniDirectionalSlider_sliderLowEndOffset)) {
	  _uniDirectionalSlider_sliderDirectionUp = true;
	}
	if (!_uniDirectionalSlider_disableUnidirectionality && sliderValue <= _uniDirectionalSlider_previousSliderValue - _uniDirectionalSlider_sliderTolerance && (_uniDirectionalSlider_previousSliderValue == -1 || _uniDirectionalSlider_previousSliderValue > 1023 - _uniDirectionalSlider_sliderHighEndOffset)) {
	  _uniDirectionalSlider_sliderDirectionUp = false;
	}

	_uniDirectionalSlider_previousSliderValue = sliderValue;

	int16_t transitionPosition = 1000L * (int32_t)(sliderValue - _uniDirectionalSlider_sliderLowEndOffset) / (int32_t)(1023 - _uniDirectionalSlider_sliderLowEndOffset - _uniDirectionalSlider_sliderHighEndOffset);
	transitionPosition = constrain(transitionPosition, 0, 1000);

	if (!_uniDirectionalSlider_sliderDirectionUp)
	  transitionPosition = 1000 - transitionPosition;
	if (_uniDirectionalSlider_previousTransitionPosition != transitionPosition) {
	  bool returnValue = true;
	  if ((_uniDirectionalSlider_previousTransitionPosition == 0 || _uniDirectionalSlider_previousTransitionPosition == 1000) && (transitionPosition == 0 || transitionPosition == 1000)) {
	    returnValue = false;
	  }
	  _uniDirectionalSlider_previousTransitionPosition = transitionPosition;
	  return returnValue;
	}
	}
	return false;
}

int SkaarhojUtils::uniDirectionalSlider_position()	{
	return _uniDirectionalSlider_previousTransitionPosition;
}

bool SkaarhojUtils::uniDirectionalSlider_isAtEnd()	{
	return (_uniDirectionalSlider_previousTransitionPosition==1000 || _uniDirectionalSlider_previousTransitionPosition==0);
}






/**
 * Joystick
 */
void SkaarhojUtils::joystick_init(int tolerance, uint8_t analogInputPinHorizontal, uint8_t analogInputPinVertical, uint8_t digitalInputPin)	{

		// Configuration constants, should have setter-methods:
	_joystick_analogInputPin[0] = analogInputPinHorizontal;
	_joystick_analogInputPin[1] = analogInputPinVertical;
	_joystick_digitalInputPin = digitalInputPin;
	_joystick_tolerance = tolerance; 
	
	pinMode(analogInputPinHorizontal, INPUT);
	pinMode(analogInputPinVertical, INPUT);
	pinMode(digitalInputPin, INPUT_PULLUP);

		// Internal variables during operation:
	_joystick_previousPosition[0] = -1000;
	_joystick_previousPosition[1] = -1000;
	_joystick_previousValue[0] = -1000;
	_joystick_previousValue[1] = -1000;
	
		// Reset:
	_joystick_centerValue[0] = analogRead(_joystick_analogInputPin[0]);
	_joystick_centerValue[1] = analogRead(_joystick_analogInputPin[1]);
	joystick_hasMoved(0);
	joystick_hasMoved(1);
	
	Serial.print("Center values: ");
	Serial.print(_joystick_centerValue[0]);
	Serial.print(",");
	Serial.print(_joystick_centerValue[1]);
	Serial.println();
}

bool SkaarhojUtils::joystick_hasMoved(uint8_t index)	{	// Index is 0 or 1
	if (index <=1)	{
		int potValue = analogRead(_joystick_analogInputPin[index])-_joystick_centerValue[index];

		if (potValue >= _joystick_previousValue[index]+_joystick_tolerance || potValue <= _joystick_previousValue[index]-_joystick_tolerance)  {

			_joystick_previousValue[index] = potValue;

			int joystickPosition = potValue;
			if (potValue<0)	{
				joystickPosition = (long)100*(long)joystickPosition/(long)_joystick_centerValue[index];
			} else {
				joystickPosition = (long)100*(long)joystickPosition/(long)(1023-_joystick_centerValue[index]);
			}
			if (joystickPosition>97) joystickPosition=100;
			if (joystickPosition<-97) joystickPosition=-100;
			if (joystickPosition < 3 && joystickPosition > -3)	joystickPosition=0;
		
			if (_joystick_previousPosition[index]!=joystickPosition)  {
				_joystick_previousPosition[index]=-joystickPosition;
				return true;
			}
		}
		return false;
	}
}

int SkaarhojUtils::joystick_position(uint8_t index)	{
	return _joystick_previousPosition[index];
}
bool SkaarhojUtils::joystick_buttonUp() {
	bool buttonChange = (_joystick_buttonStatusLastUp ^ joystick_buttonIsPressed());
	_joystick_buttonStatusLastUp ^= buttonChange;

	return (buttonChange & ~_joystick_buttonStatus) ? true : false;
}
bool SkaarhojUtils::joystick_buttonDown() {
	bool buttonChange = (_joystick_buttonStatusLastDown ^ joystick_buttonIsPressed());
	_joystick_buttonStatusLastDown ^= buttonChange;

	return (buttonChange & _joystick_buttonStatus) ? true : false;
}
bool SkaarhojUtils::joystick_buttonIsPressed() {
	_joystick_buttonStatus = !digitalRead(_joystick_digitalInputPin);
	return _joystick_buttonStatus;
}


/**
 * Switches functions
 */
void SkaarhojUtils::switch_init(uint8_t digitalInputPin_sw0,uint8_t digitalInputPin_sw1)	{	// For now, only two pins, but this can be extended...
	_switch_digitalInputPins[0] = digitalInputPin_sw0;
	_switch_digitalInputPins[1] = digitalInputPin_sw1;
	
	pinMode(_switch_digitalInputPins[0], INPUT_PULLUP);
	pinMode(_switch_digitalInputPins[1], INPUT_PULLUP);
	delay(2);
	
	_switch_invertFilter = 0;
	switch_readPinsStatus();
	_switch_invertFilter = _switch_buttonStatus;	// Automatically detect default state and invert it...
}
bool SkaarhojUtils::switch_buttonUp(uint8_t switchNumber) {	// Returns true if a switch 0-7 is has just been released
	if (switchNumber<8)	{
		switch_readPinsStatus();

		uint8_t mask = (B1 << switchNumber);
		uint8_t buttonChange = (_switch_buttonStatusLastUp ^ _switch_buttonStatus) & mask;
		_switch_buttonStatusLastUp ^= buttonChange;

		return (buttonChange & ~_switch_buttonStatus) ? true : false;
	} else return false;
}
bool SkaarhojUtils::switch_buttonDown(uint8_t switchNumber) {	// Returns true if a switch 0-7 is has just been pushed down
	if (switchNumber<8)	{
		switch_readPinsStatus();

		uint8_t mask = (B1 << switchNumber);
		uint8_t buttonChange = (_switch_buttonStatusLastDown ^ _switch_buttonStatus) & mask;
		_switch_buttonStatusLastDown ^= buttonChange;

		return (buttonChange & _switch_buttonStatus) ? true : false;
	} else return false;
}
bool SkaarhojUtils::switch_buttonIsPressed(uint8_t switchNumber) {	// Returns true if a switch 0-7 is currently pressed
	if (switchNumber<8)	{
		switch_readPinsStatus();
		return (_switch_buttonStatus & (B1 << switchNumber)) ? true : false;
	} else return false;
}
void SkaarhojUtils::switch_readPinsStatus()	{
	_switch_buttonStatus = 
		(digitalRead(_switch_digitalInputPins[0]) +
		(digitalRead(_switch_digitalInputPins[1]) << 1)) ^ _switch_invertFilter;
}


/**
 * Encoders functions
 */
void SkaarhojUtils::encoders_init() {
	_encoders_countOn[0] = false;
	_encoders_countOn[1] = false;
	
	_encoders_pushOn[0] = false;
	_encoders_pushOn[1] = false;

	_encoders_pushOnTriggerTimeFired[0] = false;
	_encoders_pushOnTriggerTimeFired[1] = false;
	
	_encoders_interruptStateNum[0] = 0;
	_encoders_interruptStateNum[1] = 0;

	_encoders_interruptStateNumMem[0] = 0;
	_encoders_interruptStateNumMem[1] = 0;
	
	pinMode(3, INPUT);
	pinMode(5, INPUT);
	pinMode(6, INPUT);
	pinMode(2, INPUT);
	pinMode(8, INPUT);
	pinMode(9, INPUT);
}
void SkaarhojUtils::encoders_interrupt(uint8_t encNum)	{
	if (encNum <2)	{
		if (digitalRead(encNum==1?8:5))	{
			_encoders_interruptStateNum[encNum]++;
		} else {
			_encoders_interruptStateNum[encNum]--;
		}
		_encoders_interruptStateTime[encNum] = millis();
	}
}

int SkaarhojUtils::encoders_state(uint8_t encNum) {
	return encoders_state(encNum, 0);
}
int SkaarhojUtils::encoders_lastCount(uint8_t encNum) {
	return _encoders_interruptStateLastCount[encNum];
}
int SkaarhojUtils::encoders_state(uint8_t encNum, unsigned int buttonPushTriggerDelay) {
	uint8_t trigger_pin;
	uint8_t direction_pin;
	uint8_t push_pin;

		// Check:
	if (encNum <2)	{
		if (encNum==1)	{
			trigger_pin = 2;
			direction_pin = 8;
			push_pin = 9;
		} else {	// encNum == 0
			trigger_pin = 3;
			direction_pin = 5;
			push_pin = 6;
		}

		bool isPushed = digitalRead(push_pin);

			// If rotations has been detected in either direction:
		if (_encoders_interruptStateNumMem[encNum] != _encoders_interruptStateNum[encNum])	{
			
				// Check if the last interrupt generated signal is younger than 1000 ms, only then will we accept it, otherwise we ignore.
			if (_encoders_interruptStateTime[encNum]+1000 > (unsigned long)millis())	{	
				_encoders_interruptStateLastCount[encNum] = _encoders_interruptStateNum[encNum]-_encoders_interruptStateNumMem[encNum];
			} else {
				_encoders_interruptStateLastCount[encNum] = 0;
			}
			_encoders_interruptStateNumMem[encNum] = _encoders_interruptStateNum[encNum];	// Set ..NumMem to ..Mem. This is to avoid setting the value of a variable that might also be written to from the interrupt - which will crash the Arduino!

			if (_debugMode)	{
				Serial.print(F("Number of trigger signals: "));
				Serial.println(_encoders_interruptStateLastCount[encNum], DEC);	// TEMP
			}
			return _encoders_interruptStateLastCount[encNum]!=0 ? (_encoders_interruptStateLastCount[encNum] < 0 ? -1 : 1) : 0;
		}

			// Push:
		delay(1);
		isPushed = isPushed & digitalRead(push_pin);
		if (isPushed)  {
			if (!_encoders_pushOn[encNum]) {
				_encoders_pushOn[encNum] = true;
				_encoders_pushOnTriggerTimeFired[encNum] = false;
				_encoders_pushOnMillis[encNum] = millis();
				
				return 2;
			}
			if (!_encoders_pushOnTriggerTimeFired[encNum] && buttonPushTriggerDelay>0 && (unsigned long)millis()-_encoders_pushOnMillis[encNum] > buttonPushTriggerDelay)	{
				_encoders_pushOnTriggerTimeFired[encNum] = true;
				return (unsigned long) millis()-_encoders_pushOnMillis[encNum];
			}
		} else {
			if (_encoders_pushOn[encNum])	{
				_encoders_pushOn[encNum] = false;
				if (!_encoders_pushOnTriggerTimeFired[encNum])	{
					return (unsigned long) millis()-_encoders_pushOnMillis[encNum];
				}
			}
		}
	}
		// Default return
	return 0;
}











/**
 * Touch functions:
 */
void SkaarhojUtils::touch_init() {
	_touch_A0 = A0;
	_touch_A1 = A1;
	_touch_A2 = A2;
	_touch_A3 = A3;
	
		// Threshold for reading value that indicates a touch:
	_touch_Xthreshold = 1000;
	_touch_Ythreshold = 1000;
	
	_touch_touchTimeThreshold = 100;	// Lower limit (ms) for a touch being processed at all
	_touch_touchTimeTapAndHold = 800;	// After this time in ms it will return a hold event.
	_touch_touchPerimiterThreshold = 50;	// If the touch is released and dX and dY is outside this distance, the touch is cancelled
	_touch_gestureLength = 100;		// Length of one side in the gesture
	_touch_gestureRatio = 2;		// Ratio between end dX and end dY that will trigger a swipe gesture

	_touch_exitByTapAndHold = false;
	
	_touch_touchStartedTime3=0;
	_touch_touchStartedTime2=0;
	
	// Calibration: use touch_calibrationPointRawCoordinates !
	_touch_marginLeft = (71+72)/2;	
	_touch_marginRight = (976+974)/2;
	_touch_marginTop = (886+889)/2;
	_touch_marginBottom = (140+152)/2;
	_touch_snapToBorderZone = 50;
	_touch_scaleRangeX = 1280;
	_touch_scaleRangeY = 720;
}
void SkaarhojUtils::touch_setExtended(){
#if NUM_ANALOG_INPUTS > 8
	_touch_A0 = A6;
	_touch_A1 = A7;
	_touch_A2 = A8;
	_touch_A3 = A9;
#endif
}
void SkaarhojUtils::touch_setSnapToBorderZone(uint8_t zoneWidth)	{
	_touch_snapToBorderZone = zoneWidth;
}
void SkaarhojUtils::touch_calibrationPointRawCoordinates(int p1x, int p1y, int p2x, int p2y, int p3x, int p3y, int p4x, int p4y) 	{
	_touch_marginLeft = float ((p1x-(p2x-p1x)/2)+(p4x-(p3x-p4x)/2))/2;
	_touch_marginRight = float ((p2x+(p2x-p1x)/2)+(p3x+(p3x-p4x)/2))/2;
	_touch_marginTop = float ((p1y+(p1y-p4y)/2)+(p2y+(p2y-p3y)/2))/2;
	_touch_marginBottom = float ((p4y-(p1y-p4y)/2)+(p3y-(p2y-p3y)/2))/2;	
}
bool SkaarhojUtils::touch_isTouched() {
	// Based on GNU/GPL code from 2009 Jonathan Oxer <jon@oxer.com.au>
	
	// Reads the touch values to internal memory and returns true if touch is detected

	// Set up the analog pins in preparation for reading the X value
	// from the touchscreen.
	pinMode(_touch_A1, INPUT_PULLUP);     // Analog pin 1
	pinMode(_touch_A3, INPUT_PULLUP);     // Analog pin 3
	pinMode(_touch_A0, OUTPUT);    // Analog pin 0
	pinMode(_touch_A2, OUTPUT);    // Analog pin 2
	digitalWrite(_touch_A0, LOW);  // Use analog pin 0 as a GND connection
	digitalWrite(_touch_A2, HIGH); // Use analog pin 2 as a +5V connection
	delay(1);
	_touch_rawXVal = analogRead(_touch_A1);   // Read the X value

	// Set up the analog pins in preparation for reading the Y value
	// from the touchscreen
	pinMode(_touch_A0, INPUT_PULLUP);     // Analog pin 0
	pinMode(_touch_A2, INPUT_PULLUP);     // Analog pin 2
	pinMode(_touch_A1, OUTPUT);    // Analog pin 1
	pinMode(_touch_A3, OUTPUT);    // Analog pin 3
	digitalWrite(_touch_A1, LOW);  // Use analog pin 1 as a GND connection
	digitalWrite(_touch_A3, HIGH); // Use analog pin 3 as a +5V connection
	delay(1);
	_touch_rawYVal = analogRead(_touch_A0);   // Read the Y value

	// Return true if touched:
	return (_touch_rawYVal < _touch_Ythreshold && _touch_rawXVal < _touch_Xthreshold);
}
bool SkaarhojUtils::touch_isInProgress()	{
	return _touch_isTouchedState && _touch_touchEndedRawXVal>0;
}
uint8_t SkaarhojUtils::touch_state() {
	
	bool isTouchedNow = touch_isTouched();
	if (_touch_exitByTapAndHold)	{
		//Serial.print("_touch_exitByTapAndHold is set: ");
		if (isTouchedNow)	{
			isTouchedNow = false;
			//Serial.println("keeps isTouchedNow low");
		} else {
			_touch_exitByTapAndHold = false;
			//Serial.println("resets _touch_exitByu....");
		}
	}
		
		// If screen is touched currently:
	if (isTouchedNow)	{
			// New touch registered:
		if (!_touch_isTouchedState)	{
			_touch_isTouchedState = true;
			_touch_checkingForTapAndHold = false;

			// Init variables:
			_touch_touchStartedRawXVal = 0;
			_touch_touchStartedRawYVal = 0;
			_touch_touchCoordRoundRobinIdx = 0;
			_touch_touchStartedTime = millis();	// long unsigned
			
			_touch_touchRawXValMax = 0;
			_touch_touchRawXValMin = 1024;
			_touch_touchRawYValMax = 0;
			_touch_touchRawYValMin = 1024;
			
			_touch_touchEndedRawXVal = 0;
			_touch_touchEndedRawYVal = 0;
		}
		
			// Round robin accumulation of the coordinates:
		_touch_touchRawXValRoundRobin[_touch_touchCoordRoundRobinIdx % 8] = _touch_rawXVal;
		_touch_touchRawYValRoundRobin[_touch_touchCoordRoundRobinIdx % 8] = _touch_rawYVal;
		_touch_touchCoordRoundRobinIdx++;
		
			// Set the start touch coordinate:
			// First, there must be 8 recent values in the round-robin base, second the start value must not already be set and thirdly, we  register it only AFTER the touchTimeThreshold has been reached. This is all about "stabilizing" the touch value since it fluctuates most when people put their finger to the screen first time.
		if (_touch_touchCoordRoundRobinIdx>=8 && _touch_touchStartedRawXVal==0 && (unsigned long)millis()-_touch_touchStartedTime>_touch_touchTimeThreshold)	{
			for(uint8_t i=0;i<8;i++)	{
				_touch_touchStartedRawXVal+=_touch_touchRawXValRoundRobin[i];
				_touch_touchStartedRawYVal+=_touch_touchRawYValRoundRobin[i];
			}
			_touch_touchStartedRawXVal/=8;
			_touch_touchStartedRawYVal/=8;
		}
		
			// Min / max:
		if (_touch_touchStartedRawXVal>0 && _touch_touchCoordRoundRobinIdx>=12)	{
			_touch_endedValueCalculation();
			
			if (_touch_touchEndedRawXVal > 	_touch_touchRawXValMax)	_touch_touchRawXValMax = _touch_touchEndedRawXVal;
			if (_touch_touchEndedRawXVal < 	_touch_touchRawXValMin)	_touch_touchRawXValMin = _touch_touchEndedRawXVal;
			if (_touch_touchEndedRawYVal > 	_touch_touchRawYValMax)	_touch_touchRawYValMax = _touch_touchEndedRawYVal;
			if (_touch_touchEndedRawYVal < 	_touch_touchRawYValMin)	_touch_touchRawYValMin = _touch_touchEndedRawYVal;

		}		

			// Checking if a single touch is the case and if so exits the active touch in case the tapAndHold threshold has been exceeded.
		if (!_touch_checkingForTapAndHold && (unsigned long)millis()-_touch_touchStartedTime > _touch_touchTimeTapAndHold)	{
			//Serial.println("Check for tapAndHold...");
			_touch_checkingForTapAndHold = true;
			_touch_endedValueCalculation();
			if (touch_type()==1)	{
				//Serial.println("Exceeded time, exit");
				_touch_exitByTapAndHold = true;
			} else {
				//Serial.println("Nope...!");
			}
		}
	} else {	// Touch ended:
		if (_touch_isTouchedState)	{
			_touch_isTouchedState = false;
			unsigned long touchTime = (unsigned long)millis()-_touch_touchStartedTime;
			
			if (touchTime > _touch_touchTimeThreshold && _touch_touchStartedRawXVal>0)	{
				_touch_endedValueCalculation();
				
				uint8_t touchType = touch_type();


/*
								Serial.print("Start: ");
								Serial.print(_touch_touchStartedRawXVal);
								Serial.print(",");
								Serial.println(_touch_touchStartedRawYVal);
								Serial.print("End: ");
								Serial.print(_touch_touchEndedRawXVal);
								Serial.print(",");
								Serial.println(_touch_touchEndedRawYVal);

								unsigned int dX = abs(_touch_touchStartedRawXVal-_touch_touchEndedRawXVal);
								unsigned int dY = abs(_touch_touchStartedRawYVal-_touch_touchEndedRawYVal);

								Serial.print("dX,dY: ");
								Serial.print(dX);
								Serial.print(",");
								Serial.println(dY);

								Serial.print("dX/dY, dY/dX: ");
								Serial.print((float)dX/dY);
								Serial.print(",");
								Serial.println((float)dY/dX);

								Serial.print("xMax,yMax: ");
								Serial.print(_touch_touchRawXValMax);
								Serial.print(",");
								Serial.println(_touch_touchRawYValMax);

								Serial.print("xMin,yMin: ");
								Serial.print(_touch_touchRawXValMin);
								Serial.print(",");
								Serial.println(_touch_touchRawYValMin);

								Serial.print("Bounding Box: ");
								Serial.print(_touch_touchRawXValMax-_touch_touchRawXValMin);
								Serial.print(",");
								Serial.println(_touch_touchRawYValMax-_touch_touchRawYValMin);

								Serial.print("Touch Type:");
								Serial.println(touch_type());

*/
				
					// Single:
				if (touchType==1)	{
					uint8_t retVal =0;
					
					if ((unsigned long)_touch_touchStartedTime-_touch_touchStartedTime2<500) {
						if ((unsigned long)_touch_touchStartedTime2-_touch_touchStartedTime3<500)	{
							_touch_touchStartedTime3=0;
							_touch_touchStartedTime2=0;
							_touch_touchStartedTime=0;
							retVal= 3;	// Triple
						} else {
							retVal= 2;	// Double
						}
					} else {
						if (touchTime > _touch_touchTimeTapAndHold)	{
							retVal= 9;	// Single + hold
						} else {
							retVal= 1;	// Single
						}
					}

					_touch_touchStartedTime3=_touch_touchStartedTime2;
					_touch_touchStartedTime2=_touch_touchStartedTime;
					
					return retVal;
				}
				
					// Gesture:
				if (touchType>=10 && touchType<=13)	{
					return touchType;
				}
				
				return 255;	// Something else touched...
			}
		}
	}
	
	return 0;
}
void SkaarhojUtils::_touch_endedValueCalculation()	{
	// This takes the oldest four values in the round-robin. This is in order to avoid the most recent values which in the case of a release of a finger will be contaminated by fluctuations
	_touch_touchEndedRawXVal = 0;
	_touch_touchEndedRawYVal = 0;

	for(uint8_t i=0;i<4;i++)	{
		uint8_t idx = ((_touch_touchCoordRoundRobinIdx % 8)+1+i)%8;
		_touch_touchEndedRawXVal+=_touch_touchRawXValRoundRobin[idx];
		_touch_touchEndedRawYVal+=_touch_touchRawYValRoundRobin[idx];
	}

	_touch_touchEndedRawXVal/=4;
	_touch_touchEndedRawYVal/=4;		
}
uint8_t SkaarhojUtils::touch_type()	{
	unsigned int dX = abs(_touch_touchStartedRawXVal-_touch_touchEndedRawXVal);
	unsigned int dY = abs(_touch_touchStartedRawYVal-_touch_touchEndedRawYVal);
	
		// Normal point touch:
	if (dX<_touch_touchPerimiterThreshold && dY<_touch_touchPerimiterThreshold)	{
		return 1;
	}
	
	if (dX>_touch_gestureLength || dY>_touch_gestureLength)	{
			// Horizonal gesture:
		if (dY==0 || (float) dX/dY>_touch_gestureRatio)	{
			return _touch_touchStartedRawXVal < _touch_touchEndedRawXVal ? 10 : 11;	// 10 = Left->Right; 11 = Right->Left
		}
			// Vertical gesture:
		if (dX==0 || (float) dY/dX>_touch_gestureRatio)	{
			return _touch_touchStartedRawYVal < _touch_touchEndedRawYVal ? 13 : 12;	// 12 = Up->Down; 13 = Down->Up
		}
	}
	return 0;	// Invalid touch type...
}
int SkaarhojUtils::touch_coordX(int rawCoordX)	{
	int coordX = rawCoordX-_touch_marginLeft;
	if (coordX < _touch_snapToBorderZone)	coordX=0;
	if (coordX > (_touch_marginRight-_touch_marginLeft)-_touch_snapToBorderZone)	coordX=(_touch_marginRight-_touch_marginLeft);
	coordX = (unsigned long) coordX  * _touch_scaleRangeX / (_touch_marginRight-_touch_marginLeft);
	return coordX;
}
int SkaarhojUtils::touch_coordY(int rawCoordY)	{
	int coordY = rawCoordY-_touch_marginBottom;
	if (coordY < _touch_snapToBorderZone)	coordY=0;
	if (coordY > (_touch_marginTop-_touch_marginBottom)-_touch_snapToBorderZone)	coordY=(_touch_marginTop-_touch_marginBottom);
	coordY = (unsigned long) coordY  * _touch_scaleRangeY / (_touch_marginTop-_touch_marginBottom);
	return coordY;
}

int SkaarhojUtils::touch_getRawXVal() {
	return _touch_rawXVal;
}
int SkaarhojUtils::touch_getRawYVal() {
	return _touch_rawYVal;
}

int SkaarhojUtils::touch_getStartedXCoord() {
	return touch_coordX(_touch_touchStartedRawXVal);
}
int SkaarhojUtils::touch_getStartedYCoord() {
	return touch_coordY(_touch_touchStartedRawYVal);
}

int SkaarhojUtils::touch_getEndedXCoord() {
	return touch_coordX(_touch_touchEndedRawXVal);
}
int SkaarhojUtils::touch_getEndedYCoord() {
	return touch_coordY(_touch_touchEndedRawYVal);
}

int SkaarhojUtils::touch_getRawXValMax() {
	return _touch_touchRawXValMax;
}
int SkaarhojUtils::touch_getRawXValMin() {
	return _touch_touchRawXValMin;
}
int SkaarhojUtils::touch_getRawYValMax() {
	return _touch_touchRawYValMax;
}
int SkaarhojUtils::touch_getRawYValMin() {
	return _touch_touchRawYValMin;
}
