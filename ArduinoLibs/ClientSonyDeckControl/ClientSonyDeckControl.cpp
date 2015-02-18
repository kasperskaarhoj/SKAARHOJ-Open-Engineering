/*
Copyright 2012-2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Sony Deck Control RS-422 Client library for Arduino

The ClientSonyDeckControl library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ClientSonyDeckControl library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ClientSonyDeckControl library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/


#include "Arduino.h"
#include "ClientSonyDeckControl.h"

/**
 * Constructor.
 * Pass the hardware serial object you want to use
 */
ClientSonyDeckControl::ClientSonyDeckControl(HardwareSerial &serial) : SkaarhojSerialClient(serial) {}

/**
 * Overloading method: Setting up baud rate etc.
 */
void ClientSonyDeckControl::begin(){
	
	SkaarhojSerialClient::begin(38400, SERIAL_8O1);	// Odd parity, see http://arduino.cc/en/Serial/begin
		
	_binBufferSize = ClientSonyDeckControl_BUFFERLEN;
	
	_statusRequestInterval = 0;
	_ackMsgInterval = 200;
}

/**
 * Resets local device state variables. (Overloading from superclass)
 */
void ClientSonyDeckControl::_resetDeviceStateVariables()	{
	_isPlaying = false;
	_isRecording = false;
	_isForwarding = false;
	_isRewinding = false;
	_isStopped = false;

	_isCassetteOut = false;
	_isInLocalModeOnly = false;
	_isStandby = false;
	_isInJogMode = false;
	_isDirectionBackwards = false;
	_isStill = false;
	_isNearEOT = false;
	_isEOT = false;		
}

/**
 * Sends status command. (Overloading from superclass)
 */
void ClientSonyDeckControl::_sendStatus()	{
  _binarybuffer[0] = 0x61;
  _binarybuffer[1] = 0x20;
  _binarybuffer[2] = 0x09;
  _bufferWriteIndex=3;
  _sendBuffer();
}

/**
 * Sends ping command. (Overloading from superclass)
 */
void ClientSonyDeckControl::_sendPing()	{
	_sendStatus();
}

/**
 * Sets ping timeout
 */
void ClientSonyDeckControl::pingTimeout(uint16_t timeout)	{
	_ackMsgInterval = timeout;
}

/**
 * Resets the byte buffer, sets it to zeros all through. (Overloading from superclass)
 */
void ClientSonyDeckControl::_resetBuffer()	{
	memset(_binarybuffer, 0, _binBufferSize);
	_bufferWriteIndex = 0;
}

/**
 * Sends the byte buffer. (Overloading from superclass)
 * Also calculates the outgoing checksum
 */
void ClientSonyDeckControl::_sendBuffer() {
	if (_bufferWriteIndex && _bufferWriteIndex<_binBufferSize)	{
		for(uint8_t i=0; i<_bufferWriteIndex; i++)	{
			_binarybuffer[_bufferWriteIndex]+= _binarybuffer[i];
		}
		_bufferWriteIndex++;
		_HardSerial.write(_binarybuffer, _bufferWriteIndex);
		if (_serialOutput>2) {
			Serial.print(F("_sendBuffer: \n"));
			for(uint8_t i=0; i<_bufferWriteIndex; i++)	{
				if (_binarybuffer[i]<16)	{
					Serial.print(0);
				}
				Serial.print(_binarybuffer[i],HEX);
				Serial.print(':');
				if (i==20)	{
					Serial.print(' ');
				}
			}
			Serial.print(F(" Length="));
			Serial.println(_bufferWriteIndex);
		}
	
		_resetBuffer();
		_pendingAnswer = true;
	}
}

/**
 * Read from client buffer
 */
void ClientSonyDeckControl::_readFromClient()	{

		// Read from Serial Port 1:
	  while (_HardSerial.available()) {
		  uint8_t inputVal = _HardSerial.read();

		  if (_bufferWriteIndex<_binBufferSize)	{
		  	_binarybuffer[_bufferWriteIndex] = inputVal;

			if (_bufferWriteIndex==0)	{	// start of session... init it
				_binarybufferCheckSum = 0;	// Reset checksum register
				_binarybufferExpectedLength = (_binarybuffer[0] & B00001111)+2;	// Get length of the packet
			}	
				// If the pointer is equal to the expected length it means we have just read the checksum byte, so lets evaluate that:
			if (_bufferWriteIndex == _binarybufferExpectedLength)	{
				if (_binarybufferCheckSum==_binarybuffer[_bufferWriteIndex])	{
					_bufferWriteIndex++;
					_parselineDispatch();
					_resetLastIncomingMsg();
					_resetBuffer();
				} else {
					if (_serialOutput)	Serial.println(F("Bad checksum"));
					delay(5);	// Let more data arrive, so we can flush it.
					while (_HardSerial.available()) {_HardSerial.read();} 
				}
				_bufferWriteIndex=0;
				_pendingAnswer = false;
			} else {
					// If we are not at the end (checksum byte), we continously calculate the checksum:
				_binarybufferCheckSum+=_binarybuffer[_bufferWriteIndex];
				_bufferWriteIndex++;
			}
		} else {
			if (_serialOutput)	Serial.println(F("ERROR: Buffer overflow."));
			delay(5);	// Let more data arrive, so we can flush it.
			while (_HardSerial.available()) {_HardSerial.read();} 
			_pendingAnswer = false;
		}
	  }
}

/**
 * Returns true if it's ok to send a command (it's not ok if we are currently receiving data from the deck or if we are waiting for a response!)
 */
bool ClientSonyDeckControl::TXready()	{
	return !_pendingAnswer;
}

/**
 * Parses data from client. (Overloading from superclass)
 */
void ClientSonyDeckControl::_parselineDispatch()	{

	if (_serialOutput>2) {
		Serial.print(F("INCOMING DATA: \n"));
		for(uint8_t i=0; i<_bufferWriteIndex; i++)	{
			if (_binarybuffer[i]<16)	{
				Serial.print(0);
			}
			Serial.print(_binarybuffer[i],HEX);
			Serial.print(':');
			if (i==20)	{
				Serial.print(' ');
			}
		}
		Serial.print(F(" Length="));
		Serial.println(_bufferWriteIndex);
	}


	unsigned int cmd = ((unsigned int)_binarybuffer[0]<<8) + (unsigned int)_binarybuffer[1];
	
	switch(cmd)	{
		case 0x7920:
				// Both flags are set in one go:
			_hasInitialized = true;	
			_initialize();

			if (_binarybufferCheckSum != _lastStatusCheckSum)	{	// Only update if new information (we use the checksum to determine that, since it depends entirely on the data transmitted)
				_lastStatusCheckSum = _binarybufferCheckSum;
				if (_serialOutput>1)	Serial.println(F("*** Updating states:"));
				
				// Playing:
				if (_isPlaying != ((_binarybuffer[3] & B00000001)>0))	{
					_isPlaying = ((_binarybuffer[3] & B00000001)>0);
					if (_serialOutput>1)	Serial.print(F("isPlaying: "));
					if (_serialOutput>1)	Serial.println(isPlaying());
				}
				
				// Recording
				if (_isRecording != ((_binarybuffer[3] & B00000010)>0))	{
					_isRecording = ((_binarybuffer[3] & B00000010)>0);
					if (_serialOutput>1)	Serial.print(F("isRecording: "));
					if (_serialOutput>1)	Serial.println(isRecording());
				}
				
				// Forwarding x2 or more:
				if (_isForwarding != ((_binarybuffer[3] & B00000100)>0))	{
					_isForwarding = ((_binarybuffer[3] & B00000100)>0);
					if (_serialOutput>1)	Serial.print(F("isForwarding: "));
					if (_serialOutput>1)	Serial.println(isForwarding());
				}
				
				// Rewinding x1 or more:
				if (_isRewinding != ((_binarybuffer[3] & B00001000)>0))	{
					_isRewinding = ((_binarybuffer[3] & B00001000)>0);
					if (_serialOutput>1)	Serial.print(F("isRewinding: "));
					if (_serialOutput>1)	Serial.println(isRewinding());
				}
				
				// Stopped:
				if (_isStopped != ((_binarybuffer[3] & B00100000)>0))	{
					_isStopped = ((_binarybuffer[3] & B00100000)>0);
					if (_serialOutput>1)	Serial.print(F("isStopped: "));
					if (_serialOutput>1)	Serial.println(isStopped());
				}
				
				// Cassette Out:
				if (_isCassetteOut != ((_binarybuffer[2] & B00100000)>0))	{
					_isCassetteOut = ((_binarybuffer[2] & B00100000)>0);
					if (_serialOutput>1)	Serial.print(F("isCassetteOut: "));
					if (_serialOutput>1)	Serial.println(isCassetteOut());
				}

				// Local mode (If REM is not enabled):
				if (_isInLocalModeOnly != ((_binarybuffer[2] & B00000001)>0))	{
					_isInLocalModeOnly = ((_binarybuffer[2] & B00000001)>0);
					if (_serialOutput>1)	Serial.print(F("isInLocalModeOnly: "));
					if (_serialOutput>1)	Serial.println(isInLocalModeOnly());
				}

				// Stand By:
				if (_isStandby != ((_binarybuffer[3] & B10000000)>0))	{
					_isStandby = ((_binarybuffer[3] & B10000000)>0);
					if (_serialOutput>1)	Serial.print(F("isStandby: "));
					if (_serialOutput>1)	Serial.println(isStandby());
				}

				// Jog Mode:
				if (_isInJogMode != ((_binarybuffer[4] & B00010000)>0))	{
					_isInJogMode = ((_binarybuffer[4] & B00010000)>0);
					if (_serialOutput>1)	Serial.print(F("isInJogMode: "));
					if (_serialOutput>1)	Serial.println(isInJogMode());
				}

				// Direction Backwards:
				if (_isDirectionBackwards != ((_binarybuffer[4] & B00000100)>0))	{
					_isDirectionBackwards = ((_binarybuffer[4] & B00000100)>0);
					if (_serialOutput>1)	Serial.print(F("isDirectionBackwards: "));
					if (_serialOutput>1)	Serial.println(isDirectionBackwards());
				}

				// Still:
				if (_isStill != ((_binarybuffer[4] & B00000010)>0))	{
					_isStill = ((_binarybuffer[4] & B00000010)>0);
					if (_serialOutput>1)	Serial.print(F("isStill: "));
					if (_serialOutput>1)	Serial.println(isStill());
				}

				// Near EOT:
				if (_isNearEOT != ((_binarybuffer[10] & B00100000)>0))	{
					_isNearEOT = ((_binarybuffer[10] & B00100000)>0);
					if (_serialOutput>1)	Serial.print(F("isNearEOT: "));
					if (_serialOutput>1)	Serial.println(isNearEOT());
				}

				// EOT:
				if (_isEOT != ((_binarybuffer[10] & B00010000)>0))	{
					_isEOT = ((_binarybuffer[10] & B00010000)>0);
					if (_serialOutput>1)	Serial.print(F("isEOT: "));
					if (_serialOutput>1)	Serial.println(isEOT());
				}
			}
		break;
		case 0x1001:
			if (_serialOutput>1)	Serial.println(F("ACK"));
		break;
		case 0x1112:
			if (_serialOutput>1)	Serial.println(F("NACK"));
			if (_serialOutput>1)	Serial.println(_binarybuffer[2]);
		break;
		default:
		if (_serialOutput)	{
			if (_serialOutput>1)	Serial.println(F("Unsupported Data Packet:"));
			for(uint8_t i=0; i<_binarybufferExpectedLength; i++)	{
				Serial.println(_binarybuffer[i], HEX);
			}
			if (_serialOutput>1)	Serial.println(F("==="));
		}
		break;
	}
}




/**
 * 
 */
bool ClientSonyDeckControl::isPlaying() {
	return _isPlaying;
}

/**
 * 
 */
bool ClientSonyDeckControl::isRecording() {
	return _isRecording;
}

/**
 * 
 */
bool ClientSonyDeckControl::isForwarding() {
	return _isForwarding;
}

/**
 * 
 */
bool ClientSonyDeckControl::isRewinding() {
	return _isRewinding;
}

/**
 * 
 */
bool ClientSonyDeckControl::isStopped() {
	return _isStopped;
}

	// Other status registers:

/**
 * 
 */
bool ClientSonyDeckControl::isCassetteOut() {
	return _isCassetteOut;
}

/**
 * 
 */
bool ClientSonyDeckControl::isInLocalModeOnly() {
	return _isInLocalModeOnly;
}

/**
 * 
 */
bool ClientSonyDeckControl::isStandby() {
	return _isStandby;
}

/**
 * 
 */
bool ClientSonyDeckControl::isInJogMode() {
	return _isInJogMode;
}

/**
 * 
 */
bool ClientSonyDeckControl::isDirectionBackwards() {
	return _isDirectionBackwards;
}

/**
 * 
 */
bool ClientSonyDeckControl::isStill() {
	return _isStill;
}

/**
 * 
 */
bool ClientSonyDeckControl::isNearEOT() {
	return _isNearEOT;
}

/**
 * 
 */
bool ClientSonyDeckControl::isEOT() {
	return _isEOT;
}









/**
 * 
 */
void ClientSonyDeckControl::doPlay() {
	 if (TXready())  {
	     _binarybuffer[0] = 0x20;
	     _binarybuffer[1] = 0x01;
	     _bufferWriteIndex=2;
	     _sendBuffer();
	 } 
}

/**
 * 
 */
void ClientSonyDeckControl::doRecord() {
	 if (TXready())  {
	     _binarybuffer[0] = 0x20;
	     _binarybuffer[1] = 0x02;
	     _bufferWriteIndex=2;
	     _sendBuffer();
	 } 
}

/**
 * 
 */
void ClientSonyDeckControl::doFastForward() {
	 if (TXready())  {
	     _binarybuffer[0] = 0x20;
	     _binarybuffer[1] = 0x10;
	     _bufferWriteIndex=2;
	     _sendBuffer();
	 } 
}

/**
 * 
 */
void ClientSonyDeckControl::doRewind() {
	 if (TXready())  {
	     _binarybuffer[0] = 0x20;
	     _binarybuffer[1] = 0x20;
	     _bufferWriteIndex=2;
	     _sendBuffer();
	 } 
}

/**
 * 
 */
void ClientSonyDeckControl::doStop() {
	 if (TXready())  {
	     _binarybuffer[0] = 0x20;
	     _binarybuffer[1] = 0x00;
	     _bufferWriteIndex=2;
	     _sendBuffer();
	 } 
}

