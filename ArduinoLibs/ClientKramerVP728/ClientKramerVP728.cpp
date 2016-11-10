/*
Copyright 2014 Kasper Skårhøj, LearningLab DTU, kaska@llab.dtu.dk

This file is part of the Kramer VP-728 Presentation Scaler Serial Client library for Arduino

The ClientKramerVP728 library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ClientKramerVP728 library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ClientKramerVP728 library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/


#include "Arduino.h"
#include "ClientKramerVP728.h"


/**
 * Constructor
 */
ClientKramerVP728::ClientKramerVP728(HardwareSerial &serial) : SkaarhojSerialClient(serial) {}

/**
 * Overloading method: Setting up baud rate and other settings
 */
void ClientKramerVP728::begin(){
	
	SkaarhojSerialClient::begin(9600);	// Must call!
	
	_EOTChar = '>';
	_statusRequestInterval = 60000;
	
		// A command queue has been implemented in this subclass.
	_queueWriteIndex = 0;
	_queueReadIndex = 0;

	i = 0;
}

/**
 * Resets local device state variables. (Overloading from superclass)
 */
void ClientKramerVP728::_resetDeviceStateVariables()	{
	_queueWriteIndex = 0;
	_queueReadIndex = 0;

    _VP728_inputSource = 255;
    _VP728_brightness = 255;
    _VP728_contrast = 255;
    _VP728_color = 255;
    _VP728_hue = 255;
    _VP728_sharpness = 255;
    _VP728_gamma = 255;
    _VP728_outputRes = 255;
    _VP728_aspectRatio = 255;
    _VP728_zoom = 255;
    _VP728_audioInputVolume = 0xFFFF;
    _VP728_audioOutputVolume = 0xFFFF;
    _VP728_audioBass = 0xFFFF;
    _VP728_audioTreble = 0xFFFF;
    _VP728_audioBalance = 0xFFFF;
    _VP728_audioDelay = 0xFFFF;
    _VP728_freeze = false;
    _VP728_blank = false;
    _VP728_power = false;
    _VP728_mute = false;
    _VP728_lock = false;
    _VP728_inputStatus = 255;
    _VP728_HDCPSetting = 255;
    _VP728_HDCPInputState[0] = false;
    _VP728_HDCPInputState[1] = false;
}

/**
 * Parses a line from client. (Overloading from superclass)
 */
void ClientKramerVP728::_parseline()	{
	_bufferReadIndex = 0;
	uint8_t function;
	int value;

	if (isNextPartOfBuffer_P(PSTR("Z 0 ")) || isNextPartOfBuffer_P(PSTR("Z 1 ")))	{
		_hasInitialized = true;
		
		function = parseInt();
		isNextPartOfBuffer_P(PSTR(" "));
		value = parseInt();
		
		switch(function)	{
			case 0:
				_VP728_inputSource = value;
				if (_serialOutput>1) Serial.print(F("_VP728_inputSource="));
				if (_serialOutput>1) Serial.println(_VP728_inputSource);			
			break;
			case 12:
				_VP728_brightness = value;
				if (_serialOutput>1) Serial.print(F("_VP728_brightness="));
				if (_serialOutput>1) Serial.println(_VP728_brightness);			
			break;
			case 13:
				_VP728_contrast = value;
				if (_serialOutput>1) Serial.print(F("_VP728_contrast="));
				if (_serialOutput>1) Serial.println(_VP728_contrast);			
			break;
			case 14:
				_VP728_color = value;
				if (_serialOutput>1) Serial.print(F("_VP728_color="));
				if (_serialOutput>1) Serial.println(_VP728_color);			
			break;
			case 15:
				_VP728_hue = value;
				if (_serialOutput>1) Serial.print(F("_VP728_hue="));
				if (_serialOutput>1) Serial.println(_VP728_hue);			
			break;
			case 16:
				_VP728_sharpness = value;
				if (_serialOutput>1) Serial.print(F("_VP728_sharpness="));
				if (_serialOutput>1) Serial.println(_VP728_sharpness);			
			break;
			case 17:
				_VP728_gamma = value;
				if (_serialOutput>1) Serial.print(F("_VP728_gamma="));
				if (_serialOutput>1) Serial.println(_VP728_gamma);			
			break;
			case 25:
				_VP728_outputRes = value;
				if (_serialOutput>1) Serial.print(F("_VP728_outputRes="));
				if (_serialOutput>1) Serial.println(_VP728_outputRes);			
			break;
			case 27:
				_VP728_aspectRatio = value;
				if (_serialOutput>1) Serial.print(F("_VP728_aspectRatio="));
				if (_serialOutput>1) Serial.println(_VP728_aspectRatio);			
			break;
			case 32:
				_VP728_zoom = value;
				if (_serialOutput>1) Serial.print(F("_VP728_zoom="));
				if (_serialOutput>1) Serial.println(_VP728_zoom);			
			break;
			case 47:
				_VP728_audioInputVolume = value;
				if (_serialOutput>1) Serial.print(F("_VP728_audioInputVolume="));
				if (_serialOutput>1) Serial.println(_VP728_audioInputVolume);			
			break;
			case 48:
				_VP728_audioOutputVolume = value;
				if (_serialOutput>1) Serial.print(F("_VP728_audioOutputVolume="));
				if (_serialOutput>1) Serial.println(_VP728_audioOutputVolume);			
			break;
			case 49:
				_VP728_audioBass = value;
				if (_serialOutput>1) Serial.print(F("_VP728_audioBass="));
				if (_serialOutput>1) Serial.println(_VP728_audioBass);			
			break;
			case 50:
				_VP728_audioTreble = value;
				if (_serialOutput>1) Serial.print(F("_VP728_audioTreble="));
				if (_serialOutput>1) Serial.println(_VP728_audioTreble);			
			break;
			case 51:
				_VP728_audioBalance = value;
				if (_serialOutput>1) Serial.print(F("_VP728_audioBalance="));
				if (_serialOutput>1) Serial.println(_VP728_audioBalance);			
			break;
			case 54:
				_VP728_audioDelay = value;
				if (_serialOutput>1) Serial.print(F("_VP728_audioDelay="));
				if (_serialOutput>1) Serial.println(_VP728_audioDelay);			
			break;
			case 89:
				_VP728_freeze = value;
				if (_serialOutput>1) Serial.print(F("_VP728_freeze="));
				if (_serialOutput>1) Serial.println(_VP728_freeze);			
			break;
			case 90:
				_VP728_blank = value;
				if (_serialOutput>1) Serial.print(F("_VP728_blank="));
				if (_serialOutput>1) Serial.println(_VP728_blank);			
			break;
			case 91:
				_VP728_power = value;
				if (_serialOutput>1) Serial.print(F("_VP728_power="));
				if (_serialOutput>1) Serial.println(_VP728_power);			
			break;
			case 101:
				_VP728_mute = value;
				if (_serialOutput>1) Serial.print(F("_VP728_mute="));
				if (_serialOutput>1) Serial.println(_VP728_mute);			
			break;
			case 102:
				_VP728_lock = value;
				if (_serialOutput>1) Serial.print(F("_VP728_lock="));
				if (_serialOutput>1) Serial.println(_VP728_lock);			
			break;
			case 103:
				_VP728_inputStatus = value;
				if (_serialOutput>1) Serial.print(F("_VP728_inputStatus="));
				if (_serialOutput>1) Serial.println(_VP728_inputStatus);			
			break;
			case 135:
				_VP728_HDCPSetting = value;
				if (_serialOutput>1) Serial.print(F("_VP728_HDCPSetting="));
				if (_serialOutput>1) Serial.println(_VP728_HDCPSetting);			
			break;
			case 153:
				_VP728_HDCPInputState[0] = value;
				if (_serialOutput>1) Serial.print(F("_VP728_HDCPInputState[0]="));
				if (_serialOutput>1) Serial.println(_VP728_HDCPInputState[0]);			
			break;
			case 154:
				_VP728_HDCPInputState[1] = value;
				if (_serialOutput>1) Serial.print(F("_VP728_HDCPInputState[1]="));
				if (_serialOutput>1) Serial.println(_VP728_HDCPInputState[1]);			
			break;
		}
	}
}

/**
 * Sends status command. (Overloading from superclass)
 * Since every function value has to be queried individually, this will use a queue system to get the job done:
 */
void ClientKramerVP728::_sendStatus()	{
	// This array has a list of all "functions" from the scaler for which to retrieve a status value:
	const static uint8_t functionListToStatusUpdate[] = {0,12,13,14,15,16,17,25,27,32,47,48,49,50,51,54,89,90,91,101,102,103,135,153,154};
	
		// Initialize:
	if (_statusRequestInterval!=500)	{
		_statusRequestInterval = 500;	// This is because we want sendStatus to be continuously called until we have gotten all commands sent - then we will set it back to 60000 ms.
		i = 0;
	}
	
		// For as long as there are function numbers to get status for:
	for(uint8_t c=i; c < sizeof(functionListToStatusUpdate); c++)	{
		if (getFunction(functionListToStatusUpdate[i]))	{	// If the command got queued, increase index pointer.
			i++;
			if (i==sizeof(functionListToStatusUpdate))	{	// If i has reached the size of the array, set the request interval back to normal.
				_statusRequestInterval = 60000;
				break;
			}
		} else {
			break;
		}
	}
}

/**
 * Sends ping command. (Overloading from superclass)
 */
void ClientKramerVP728::_sendPing()	{
	_resetBuffer();
	_addToBuffer_P(PSTR("\r"));
	_sendBuffer();
}

/**
 * Run Sub Loop overloading
 */
void ClientKramerVP728::_runSubLoop()	{
	if (isConnected())	{
		_sendingQueue();
	}
}
	
/**
 * Sending queue
 * Waiting for pending answer flag to turn false and then, send the next command in queue
 */
void ClientKramerVP728::_sendingQueue()	{
	bool qq = _queueWriteIndex;
	if (!_pendingEOT)	{	// Here: Check if we are ready to send stuff... First, any sent message must be handled, second lets way some ms before we send again
		if (_queueWriteIndex!=_queueReadIndex)	{	// If they are different, there is stuff to send...
			_queueReadIndex++;
			if (_queueReadIndex >= ClientKramerVP728_QUEUESIZE)	{
				_queueReadIndex = 0;
			}
			_resetBuffer();
			_addToBuffer(String(_queue[_queueReadIndex]));
			_sendBuffer();
		}
	}
}

/**
 * Returns true if a new queue entry has been prepared
 */
bool ClientKramerVP728::_newQueueEntry()	{
	uint8_t qq = _queueWriteIndex;
	_queueWriteIndex++;
	if (_queueWriteIndex >= ClientKramerVP728_QUEUESIZE)	{
		_queueWriteIndex = 0;
	}
	if (_queueWriteIndex!=_queueReadIndex)	{	// It's good if they are different, otherwise it means the queue is full!
		memset(_queue[_queueWriteIndex], 0, ClientKramerVP728_QUEUEENTRYLEN);
		_queueEntryWritePointer = 0;
		return true;
	} else {
		_queueWriteIndex = qq;	// Set it back to what it was before...
		return false;
	}
}

/**
 * Adds the input String object to queue entry (if it has space for it)
 */
void ClientKramerVP728::_addStringToQueue(const String command) {
	if (_queueEntryWritePointer+command.length() < ClientKramerVP728_QUEUEENTRYLEN)	{
		command.toCharArray(_queue[_queueWriteIndex]+_queueEntryWritePointer, command.length()+1);
		_queueEntryWritePointer+= command.length();
	} else Serial.println(F("_addStringToQueue ERROR"));
}

/**
 * Adds the input PROGMEM string reference to queue entry (if it has space for it)
 */
void ClientKramerVP728::_addStringToQueue_P(const char *str) {	// PROGMEM pointer
	if (_queueEntryWritePointer+strlen_P(str) < ClientKramerVP728_QUEUEENTRYLEN)	{
		strcpy_P(_queue[_queueWriteIndex]+_queueEntryWritePointer, str);
		_queueEntryWritePointer+= strlen_P(str);
	} else Serial.println(F("_addStringToQueue_P ERROR"));
}

/**
 * Generic setting of functions in Kramer scaler
 */
bool ClientKramerVP728::setFunction(uint8_t function, int value) {
	if (_newQueueEntry())	{
		_addStringToQueue_P(PSTR("Y 0 "));
		_addStringToQueue(String(function));
		_addStringToQueue_P(PSTR(" "));
		_addStringToQueue(String(value));
		_addStringToQueue_P(PSTR("\r"));
		return true;
	} else {
		if (_serialOutput>2) Serial.println(F("ERROR: Queue is full!"));
		return false;
	}
}

/**
 * Generic getting of functions in Kramer scaler
 */
bool ClientKramerVP728::getFunction(uint8_t function) {
	if (_newQueueEntry())	{
//		_addStringToQueue_P(PSTR("\r"));	// Had some noise issues and sending the EOL char worked somehow to solve it 
		_addStringToQueue_P(PSTR("Y 1 "));
		_addStringToQueue(String(function));
		_addStringToQueue_P(PSTR("\r"));
		return true;
	} else {
		if (_serialOutput>2) Serial.println(F("ERROR: Queue is full!"));
		return false;
	}
}

/**
 * See defines "ClientKramerVP728_IN_*"
 */
void ClientKramerVP728::setInputSource(uint8_t input)	{
	setFunction(0, input);
}
uint8_t ClientKramerVP728::getInputSource()	{
	return _VP728_inputSource;
}

/**
 * Y 0 12 - Brightness 0-100
 */
void ClientKramerVP728::setBrightness(uint8_t brightness) {
	setFunction(12, brightness);
}
uint8_t ClientKramerVP728::getBrightness()	{
	return _VP728_brightness;
}

/**
 * Y 0 13 - Contrast 0-100
 */
void ClientKramerVP728::setContrast(uint8_t contrast) {
	setFunction(13, contrast);
}
uint8_t ClientKramerVP728::getContrast()	{
	return _VP728_contrast;
}

/**
 * Y 0 14 - Color 0-100
 */
void ClientKramerVP728::setColor(uint8_t color) {
	setFunction(14, color);
}
uint8_t ClientKramerVP728::getColor()	{
	return _VP728_color;
}

/**
 * Y 0 15	- Hue 0-360
 */
void ClientKramerVP728::setHue(uint16_t hue) {
	setFunction(15, hue);
}
uint16_t ClientKramerVP728::getHue()	{
	return _VP728_hue;
}

/**
 * Y 0 16 - Sharpness 0-100
 */
void ClientKramerVP728::setSharpness(uint8_t sharpness) {
	setFunction(16, sharpness);
}
uint8_t ClientKramerVP728::getSharpness()	{
	return _VP728_sharpness;
}

/**
 * Y 0 17 - Gamma 0-4
 */
void ClientKramerVP728::setGamma(uint8_t gamma) {
	setFunction(17, gamma);
}
uint8_t ClientKramerVP728::getGamma()	{
	return _VP728_gamma;
}

/**
 * Y 0 25 - Output Resolution (0-38)
 * See defines "ClientKramerVP728_RES_*"
 */
void ClientKramerVP728::setOutputResolution(uint8_t outputRes) {
	setFunction(25, outputRes);
}
uint8_t ClientKramerVP728::getOutputResolution()	{
	return _VP728_outputRes;
}

/**
 * Y 0 27 - Aspect Ratio 0-5
 * See defines "ClientKramerVP728_ASPECT_*"
 */
void ClientKramerVP728::setAspectRatioType(uint8_t aspectRatio) {
	setFunction(27, aspectRatio);
}
uint8_t ClientKramerVP728::getAspectRatioType()	{
	return _VP728_aspectRatio;
}

/**
 * Y 0 27 - Aspect Ratio Free (Hpan -16-16, Vpan -16-16, Hzoom -8-8, Vzoom -8-8)
 */
void ClientKramerVP728::setAspectRatioFreely(int hpan,int vpan,int hzoom,int vzoom) {
	setFunction(27, 5);
	setFunction(28, hpan);
	setFunction(29, vpan);
	setFunction(30, hzoom);
	setFunction(31, vzoom);
}

/**
 * Y 0 32 - ZoomFixed 0-11
 */
void ClientKramerVP728::setZoomType(uint8_t zoom) {
	setFunction(32, zoom);
}
uint8_t ClientKramerVP728::getZoomType()	{
	return _VP728_zoom;
}

/**
 * Y 0 33 - ZoomFreely 0-32, H-pan 0-31, V-pan 0-31
 */
void ClientKramerVP728::setZoomFreely(uint8_t zoomLevel, uint8_t hpan, uint8_t vpan) {
	setFunction(32, 11);
	setFunction(33, zoomLevel);
	setFunction(34, hpan);
	setFunction(35, vpan);
}

/**
 * Y 0 47 - AudioInputVolume (-22 - 22)
 */
void ClientKramerVP728::setAudioInputVolume(int volume) {
	setFunction(47, volume);
}
int ClientKramerVP728::getAudioInputVolume()	{
	return _VP728_audioInputVolume;
}

/**
 * Y 0 48 - AudioOutputVolume (-100-24)
 */
void ClientKramerVP728::setAudioOutputVolume(int volume) {
	setFunction(48, volume);
}
int ClientKramerVP728::getAudioOutputVolume()	{
	return _VP728_audioOutputVolume;
}

/**
 * Y 0 49 - AudioBass (-36-36)
 */
void ClientKramerVP728::setAudioBass(int bass) {
	setFunction(49, bass);
}
int ClientKramerVP728::getAudioBass()	{
	return _VP728_audioBass;
}

/**
 * Y 0 50 - AudioTreble (-36-36)
 */
void ClientKramerVP728::setAudioTreble(int treble) {
	setFunction(50, treble);
}
int ClientKramerVP728::getAudioTreble()	{
	return _VP728_audioTreble;
}

/**
 * Y 0 51 - AudioBalance (-10-10)
 */
void ClientKramerVP728::setAudioBalance(int balance) {
	setFunction(51, balance);
}
int ClientKramerVP728::getAudioBalance()	{
	return _VP728_audioBalance;
}

/**
 * Y 0 53/54 - AudioDelay (0-340ms (+ audio delay))
 */
void ClientKramerVP728::setAudioDelay(uint16_t delayms) {
	setFunction(53, 1);
	setFunction(54, delayms);
}
int ClientKramerVP728::getAudioDelay()	{
	return _VP728_audioDelay;
}

/**
 * Y 0 73 - RecallPreset(0-7)
 */
void ClientKramerVP728::recallPreset(uint8_t preset) {
	setFunction(73, preset);
}

/**
 * Y 0 89 - Freeze
 */
void ClientKramerVP728::setFreeze(boolean freeze) {
	setFunction(89, freeze);
}
bool ClientKramerVP728::getFreeze()	{
	return _VP728_freeze;
}

/**
 * Y 0 90 - Blank
 */
void ClientKramerVP728::setBlank(boolean blank) {
	setFunction(90, blank);
}
bool ClientKramerVP728::getBlank()	{
	return _VP728_blank;
}

/**
 * Y 0 91 - Power
 */
void ClientKramerVP728::setPower(boolean power) {
	setFunction(91, power);
}
bool ClientKramerVP728::getPower()	{
	return _VP728_power;
}

/**
 * Y 0 101 - Mute
 */
void ClientKramerVP728::setMute(boolean mute) {
	setFunction(101, mute);
}
bool ClientKramerVP728::getMute()	{
	return _VP728_mute;
}

/**
 * Y 0 102 - Lock
 */
void ClientKramerVP728::setLock(boolean lock) {
	setFunction(102, lock);
}
bool ClientKramerVP728::getLock()	{
	return _VP728_lock;
}


/**
 * Y 1 103
 * 98 = no input detected. Refer to protocol specs for all other signal types
 */
uint8_t ClientKramerVP728::getInputStatus()	{
	return _VP728_inputStatus;
}


/**
 * Y 0 135 - HDCPSetting 
 * See defines "ClientKramerVP728_HDCP_*"
 */
void ClientKramerVP728::setHDCPSetting(uint8_t HDCPsetting) {
	setFunction(135, HDCPsetting);
}
uint8_t ClientKramerVP728::getHDCPSetting()	{
	return _VP728_HDCPSetting;
}


/**
 * Y 0 153/154 - InputHDCP(input, boolean)
 */
void ClientKramerVP728::setHDCPonInputState(uint8_t input, boolean state) {
	if (input==1 || input==2)	{
		setFunction(input==1?153:154, state);
	}
}
bool ClientKramerVP728::getHDCPonInputState(uint8_t input)	{
	if (input==1 || input==2)	{
		return _VP728_HDCPInputState[input-1];
	}
}
