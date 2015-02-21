/*
Copyright 2012-2014 Kasper Skårhøj, SKAARHOJ K/S, kasper@skaarhoj.com

This file is part of the Blackmagic Design ATEM Client library for Arduino

The ATEM library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ATEM library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ATEM library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/



#include "Arduino.h"
#include "ATEM.h"


/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ATEM::ATEM(){}
ATEM::ATEM(const IPAddress ip, const uint16_t localPort){
	begin(ip, localPort);
}

/**
 * Setting up IP address for the switcher (and local port to send packets from)
 * Using local port here is deprecated. Rather let the library pick a random one
 */
void ATEM::begin(const IPAddress ip){
	begin(ip, random(50100,65300));
}
void ATEM::begin(const IPAddress ip, const uint16_t localPort){

		// Set up Udp communication object:
	EthernetUDP Udp;
	_Udp = Udp;
	
	_switcherIP = ip;			// Set switcher IP address
	_localPort = localPort;		// Set default local port

	_serialOutput = 0;
	
	_ATEM_FtbS_state = false;
	_ATEM_AMLv_channel=0;
}

/**
 * Initiating connection handshake to the ATEM switcher
 */
void ATEM::connect() {
	connect(false);
}
void ATEM::connect(const boolean useFixedPortNumber) {
	_localPacketIdCounter = 0;		// Init localPacketIDCounter to 0;
	_initPayloadSent = false;		// Will be true after initial payload of data is delivered (regular 12-byte ping packages are transmitted.)
	_hasInitialized = false;		// Will be true after initial payload of data is resent and received well
	_isConnected = false;			// Will be true after the initial hello-package handshakes.
	_sessionID = 0x53AB;			// Temporary session ID - a new will be given back from ATEM.
	_lastContact = millis();  		// Setting this, because even though we haven't had contact, it constitutes an attempt that should be responded to at least
	memset(_missedInitializationPackages, 1, ATEM_maxInitPackageCount);
	_initPayloadSentAtPacketId = ATEM_maxInitPackageCount;	// The max value it can be
	uint16_t portNumber = useFixedPortNumber ? _localPort : random(50100,65300);

	_Udp.begin(portNumber);		

		
	// Send connectString to ATEM:
	if (_serialOutput) 	{
  		Serial.print(F("Sending connect packet to ATEM switcher on IP "));
		Serial.print(_switcherIP);
		Serial.print(F(" from port "));
		Serial.println(portNumber);
	}
	
	_wipeCleanPacketBuffer();
	createCommandHeader(ATEM_headerCmd_HelloPacket, 12+8);
	_packetBuffer[12] = 0x01;	// This seems to be what the client should send upon first request. 
	_packetBuffer[9] = 0x3a;	// This seems to be what the client should send upon first request. 
	
	sendPacketBuffer(20);  
}

/**
 * Keeps connection to the switcher alive
 * Therefore: Call this in the Arduino loop() function and make sure it gets call at least 2 times a second
 * Other recommendations might come up in the future.
 */
void ATEM::runLoop() {
	static boolean waitingForIncoming = false;
	
	while(true) {	// Iterate until UDP buffer is empty
		uint16_t packetSize = _Udp.parsePacket();
		
		if (_Udp.available())   {  	

			_Udp.read(_packetBuffer,12);	// Read header
			 _sessionID = word(_packetBuffer[2], _packetBuffer[3]);
			 uint8_t headerBitmask = _packetBuffer[0]>>3;
			 _lastRemotePacketID = word(_packetBuffer[10],_packetBuffer[11]);
		 	 if (_lastRemotePacketID < ATEM_maxInitPackageCount)	{
		 	 	_missedInitializationPackages[_lastRemotePacketID] = false;
		 	 }

			 uint16_t packetLength = word(_packetBuffer[0] & B00000111, _packetBuffer[1]);

		    if (packetSize==packetLength) {  // Just to make sure these are equal, they should be!
				_lastContact = millis();
				waitingForIncoming = false;
	
				if (headerBitmask & ATEM_headerCmd_HelloPacket)	{	// Respond to "Hello" packages:
					_isConnected = true;
					
					// _packetBuffer[12]	The ATEM will return a "2" in this return package of same length. If the ATEM returns "3" it means "fully booked" (no more clients can connect) and a "4" seems to be a kind of reconnect (seen when you drop the connection and the ATEM desperately tries to figure out what happened...)
					// _packetBuffer[15]	This number seems to increment with about 3 each time a new client tries to connect to ATEM. It may be used to judge how many client connections has been made during the up-time of the switcher?
						
					_wipeCleanPacketBuffer();
					createCommandHeader(ATEM_headerCmd_Ack, 12);
					_packetBuffer[9] = 0x03;	// This seems to be what the client should send upon first request. 
					sendPacketBuffer(12);  
				}

				// If a packet is 12 bytes long it indicates that all the initial information 
				// has been delivered from the ATEM and we can begin to answer back on every request
				// Currently we don't know any other way to decide if an answer should be sent back...
				// The QT lib uses the "InCm" command to indicate this, but in the latest version of the firmware (2.14)
				// all the camera control information comes AFTER this command, so it's not a clear ending token anymore.
			
				// BTW: It has been observed on an old 10Mbit hub that packages could arrive in a different order than sent and this may 
				// mess things up a bit on the initialization. So it's recommended to has as direct routes as possible.
				if(!_initPayloadSent && packetSize == 12) {
					_initPayloadSent = true;
					_initPayloadSentAtPacketId = _lastRemotePacketID;
					if (_serialOutput & 0x80) {
						Serial.print(F("_initPayloadSent=TRUE @rpID "));
						Serial.println(_initPayloadSentAtPacketId);
						Serial.print("Session ID: ");
						Serial.println(_sessionID, DEC);
					}
				} 

				if (_initPayloadSent && (headerBitmask & ATEM_headerCmd_AckRequest) && (_hasInitialized || !(headerBitmask & ATEM_headerCmd_Resend))) { 	// Respond to request for acknowledge	(and to resends also, whatever...  
					_wipeCleanPacketBuffer();
					createCommandHeader(ATEM_headerCmd_Ack, 12, _lastRemotePacketID);
					sendPacketBuffer(12); 
					
			        if (_serialOutput & 0x80) {
						Serial.print(F("rpID: "));
			        	Serial.print(_lastRemotePacketID, DEC);
						Serial.print(F(", Head: 0x"));
						Serial.print(headerBitmask, HEX);
						Serial.print(F(", Len: "));
			        	Serial.print(packetLength, DEC);
						Serial.print(F(" bytes"));

						Serial.println(F(" - ACK!"));
					} else if (_serialOutput>1)	{
						Serial.print(F("rpID: "));
			        	Serial.println(_lastRemotePacketID, DEC);
					} 
				} else {
			        if (_serialOutput & 0x80) {
						Serial.print(F("rpID: "));
			        	Serial.print(_lastRemotePacketID, DEC);
						Serial.print(F(", Head: 0x"));
						Serial.print(headerBitmask, HEX);
						Serial.print(F(", Len: "));
			        	Serial.print(packetLength, DEC);
						Serial.println(F(" bytes"));
					} else if (_serialOutput>1)	{
						Serial.print(F("rpID: "));
			        	Serial.println(_lastRemotePacketID, DEC);
					}
				}
				
				if (!(headerBitmask & ATEM_headerCmd_HelloPacket) && packetLength>12)	{
					_parsePacket(packetLength);
				}
		    } else {
				if (_serialOutput & 0x80) 	{
		      		Serial.print(F("ERROR: Packet size mismatch: "));
				    Serial.print(packetSize, DEC);
				    Serial.print(F(" != "));
				    Serial.println(packetLength, DEC);
				}

				// Flushing:
		        while(_Udp.available()) {
		        	_Udp.read(_packetBuffer, ATEM_packetBufferLength);
		        }
		    }
		} else break;
	}
	
	// After initialization, we check which packages were missed and ask for them:
	if (!_hasInitialized && _initPayloadSent && !waitingForIncoming)	{
		for(uint8_t i=1; i<_initPayloadSentAtPacketId; i++)	{
			if (_missedInitializationPackages[i])	{
				if (_serialOutput & 0x80) 	{
		      		Serial.print(F("Asking for package "));
				    Serial.println(i, DEC);
				}
				_wipeCleanPacketBuffer();
				createCommandHeader(ATEM_headerCmd_RequestNextAfter, 12);
			    _packetBuffer[6] = highByte(i-1);  // Resend Packet ID, MSB
			    _packetBuffer[7] = lowByte(i-1);  // Resend Packet ID, LSB
			    _packetBuffer[8] = 0x01;
				
				sendPacketBuffer(12);  
				waitingForIncoming = true;
				break;
			}
		}
		if (!waitingForIncoming)	{
			_hasInitialized = true;
			if (_serialOutput) {
				Serial.println(F("ATEM _hasInitialized = TRUE"));
			}
		}
	}
}

/**
 * Returns last Remote Packet ID
 */
uint16_t ATEM::getATEM_lastRemotePacketId()	{
	return _lastRemotePacketID;
}

/**
 * Get ATEM session ID
 */
uint16_t ATEM::getSessionID() {
	return _sessionID;
}

/**
 * If true, we had a response from the switcher when trying to send a hello packet.
 */
bool ATEM::isConnected()	{
	return _isConnected;
}

/**
 * If true, the initial handshake and "stressful" information exchange has occured and now the switcher connection should be ready for operation. 
 */
bool ATEM::hasInitialized()	{
	return _hasInitialized;
}

/**
 * If true, the connection to the switcher has stopped
 */
bool ATEM::isConnectionTimedOut()	{
	if (hasTimedOut(_lastContact, 5000))	{
		return true;
	}
	return false;
}





/**************
 *
 * Buffer work
 *
 **************/

void ATEM::createCommandHeader(const uint8_t headerCmd, const uint16_t lengthOfData)	{
	createCommandHeader(headerCmd, lengthOfData, 0);
}
void ATEM::createCommandHeader(const uint8_t headerCmd, const uint16_t lengthOfData, const uint16_t remotePacketID)	{

    _packetBuffer[0] = (headerCmd << 3) | (highByte(lengthOfData) & 0x07);  // Command bits + length MSB
    _packetBuffer[1] = lowByte(lengthOfData);  // length LSB

    _packetBuffer[2] = highByte(_sessionID);  // Session ID
    _packetBuffer[3] = lowByte(_sessionID);  // Session ID

    _packetBuffer[4] = highByte(remotePacketID);  // Remote Packet ID, MSB
    _packetBuffer[5] = lowByte(remotePacketID);  // Remote Packet ID, LSB
		
    if(!(headerCmd & (ATEM_headerCmd_HelloPacket | ATEM_headerCmd_Ack | ATEM_headerCmd_RequestNextAfter))) {
        _localPacketIdCounter++;

	    _packetBuffer[10] = highByte(_localPacketIdCounter);  // Local Packet ID, MSB
	    _packetBuffer[11] = lowByte(_localPacketIdCounter);  // Local Packet ID, LSB
    }
}
void ATEM::sendPacketBuffer(uint8_t length)	{
	_Udp.beginPacket(_switcherIP,  9910);
	_Udp.write(_packetBuffer,length);
	_Udp.endPacket(); 
}

/**
 * Sets all zeros in packet buffer:
 */
void ATEM::_wipeCleanPacketBuffer() {
	memset(_packetBuffer, 0, ATEM_packetBufferLength);
}



/**************
 *
 * Utilities from SkaarhojTools class:
 *
 **************/

/**
 * Setter method: If _serialOutput is set, the library may use Serial.print() to give away information about its operation - mostly for debugging.
 * 0= no output
 * 1= normal output (info)
 * 2= verbose
 * &0x80 (bit 7 set): verbose initial connection information
 */
void ATEM::serialOutput(uint8_t level) {
	_serialOutput = level;
}

/**
 * Timeout check
 */
bool ATEM::hasTimedOut(unsigned long time, unsigned long timeout)  {
  if ((unsigned long)(time + timeout) <= (unsigned long)millis())  {  // This should "wrap around" if time+timout is larger than the size of unsigned-longs, right?
    return true;
  } 
  else {
    return false;
  }
}















void ATEM::delay(const unsigned int delayTimeMillis)	{	// Responsible delay function which keeps the ATEM run loop up! DO NOT USE INSIDE THIS CLASS! Recursion could happen...
	unsigned long timeout = millis();
	timeout+=delayTimeMillis;

	while(timeout > millis())	{
		runLoop();
	}
}

/**
 * Reads from UDP channel to buffer. Will fill the buffer to the max or to the size of the current segment being parsed
 * Returns false if there are no more bytes, otherwise true 
 */
bool ATEM::_readToPacketBuffer() {
	return _readToPacketBuffer(ATEM_packetBufferLength);
}
bool ATEM::_readToPacketBuffer(uint8_t maxBytes) {
	maxBytes = maxBytes<=ATEM_packetBufferLength ? maxBytes : ATEM_packetBufferLength;
	int remainingBytes = _cmdLength-8-_cmdPointer;

	if (remainingBytes>0)	{
		if (remainingBytes <= maxBytes)	{
			_Udp.read(_packetBuffer, remainingBytes);
			_cmdPointer+= remainingBytes;
			return false;	// Returns false if finished.
		} else {
			_Udp.read(_packetBuffer, maxBytes);
			_cmdPointer+= maxBytes;
			return true;	// Returns true if there are still bytes to be read.
		}
	} else {
		return false;
	}
}

/**
 * If a package longer than a normal acknowledgement is received from the ATEM Switcher we must read through the contents.
 * Usually such a package contains updated state information about the mixer
 * Selected information is extracted in this function and transferred to internal variables in this library.
 */
void ATEM::_parsePacket(uint16_t packetLength)	{
	 uint8_t idx;	// General reusable index usable for keyers, mediaplayer etc below.
	
 		// If packet is more than an ACK packet (= if its longer than 12 bytes header), lets parse it:
      uint16_t indexPointer = 12;	// 12 bytes has already been read from the packet...
      while (indexPointer < packetLength)  {

        // Read the length of segment (first word):
        _Udp.read(_packetBuffer, 8);
        _cmdLength = word(_packetBuffer[0], _packetBuffer[1]);
		_cmdPointer = 0;
        
			// Get the "command string", basically this is the 4 char variable name in the ATEM memory holding the various state values of the system:
        char cmdStr[] = { 
          _packetBuffer[4], _packetBuffer[5], _packetBuffer[6], _packetBuffer[7], '\0'};

			// If length of segment larger than 8 (should always be...!)
        if (_cmdLength>8)  {
			if(strcmp(cmdStr, "AMLv"))	{
			  _readToPacketBuffer();	// Fill packet buffer unless it's AMLv (AudioMonitorLevels)
			}

          // Extract the specific state information we like to know about:
          if(strcmp(cmdStr, "PrgI") == 0) {  // Program Bus status
			if (!ver42())	{
				_ATEM_PrgI = _packetBuffer[1];
			} else {
				_ATEM_PrgI = (uint16_t)(_packetBuffer[2]<<8) | _packetBuffer[3];
			}
				if (_serialOutput) Serial.print(F("Program Bus: "));
	          if (_serialOutput) Serial.println(_ATEM_PrgI, DEC);
          } else
          if(strcmp(cmdStr, "PrvI") == 0) {  // Preview Bus status
			if (!ver42())	{
				_ATEM_PrvI = _packetBuffer[1];
			} else {
				_ATEM_PrvI = (uint16_t)(_packetBuffer[2]<<8) | _packetBuffer[3];
			}
            if (_serialOutput) Serial.print(F("Preview Bus: "));
            if (_serialOutput) Serial.println(_ATEM_PrvI, DEC);
          } else
          if(strcmp(cmdStr, "TlIn") == 0) {  // Tally status for inputs 1-8
            uint8_t count = _packetBuffer[1]; // Number of inputs
              // 16 inputs supported so make sure to read max 16.
            if(count > 16) {
              count = 16;
            }
            	// Inputs 1-16, bit 0 = Prg tally, bit 1 = Prv tally. Both can be set simultaneously.
            if (_serialOutput) Serial.println(F("Tally updated: "));
            for(uint8_t i = 0; i < count; ++i) {
              _ATEM_TlIn[i] = _packetBuffer[2+i];
            }

          } else 
          if(strcmp(cmdStr, "Time") == 0) {  // Time. What is this anyway?
		/*	Serial.print(_packetBuffer[0]);
			Serial.print(':');
			Serial.print(_packetBuffer[1]);
			Serial.print(':');
			Serial.print(_packetBuffer[2]);
			Serial.print(':');
			Serial.print(_packetBuffer[3]);
			Serial.println();
	      */} else 
	      if(strcmp(cmdStr, "TrPr") == 0) {  // Transition Preview
			_ATEM_TrPr = _packetBuffer[1] > 0 ? true : false;
            if (_serialOutput) Serial.print(F("Transition Preview: "));
            if (_serialOutput) Serial.println(_ATEM_TrPr, BIN);
          } else
	      if(strcmp(cmdStr, "TrPs") == 0) {  // Transition Position
			_ATEM_TrPs_frameCount = _packetBuffer[2];	// Frames count down
			_ATEM_TrPs_position = _packetBuffer[4]*256 + _packetBuffer[5];	// Position 0-1000 - maybe more in later firmwares?
          } else
	      if(strcmp(cmdStr, "TrSS") == 0) {  // Transition Style and Keyer on next transition
			_ATEM_TrSS_KeyersOnNextTransition = _packetBuffer[2] & B11111;	// Bit 0: Background; Bit 1-4: Key 1-4
            if (_serialOutput) Serial.print(F("Keyers on Next Transition: "));
            if (_serialOutput) Serial.println(_ATEM_TrSS_KeyersOnNextTransition, BIN);

			_ATEM_TrSS_TransitionStyle = _packetBuffer[1];
            if (_serialOutput) Serial.print(F("Transition Style: "));	// 0=MIX, 1=DIP, 2=WIPE, 3=DVE, 4=STING
            if (_serialOutput) Serial.println(_ATEM_TrSS_TransitionStyle, DEC);
          } else
	      if(strcmp(cmdStr, "FtbS") == 0) {  // Fade To Black State
			_ATEM_FtbS_state = _packetBuffer[2]| _packetBuffer[1]; // State of Fade To Black, 0 = off and 1 = activated
			_ATEM_FtbS_frameCount = _packetBuffer[3];	// Frames count down
            if (_serialOutput) Serial.print(F("FTB:"));
            if (_serialOutput) Serial.print(_ATEM_FtbS_state);
            if (_serialOutput) Serial.print(F("/"));
            if (_serialOutput) Serial.println(_ATEM_FtbS_frameCount);
        } else
	      if(strcmp(cmdStr, "FtbP") == 0) {  // Fade To Black - Positions(?) (Transition Time in frames for FTB): 0x01-0xFA
			_ATEM_FtbP_time = _packetBuffer[1];
          } else
	      if(strcmp(cmdStr, "TMxP") == 0) {  // Mix Transition Position(?) (Transition Time in frames for Mix transitions.): 0x01-0xFA
			_ATEM_TMxP_time = _packetBuffer[1];
          } else
	      if(strcmp(cmdStr, "DskS") == 0) {  // Downstream Keyer state. Also contains information about the frame count in case of "Auto"
			idx = _packetBuffer[0];
			if (idx >=0 && idx <=1)	{
				_ATEM_DskOn[idx] = _packetBuffer[1] > 0 ? true : false;
	            if (_serialOutput) Serial.print(F("Dsk Keyer "));
	            if (_serialOutput) Serial.print(idx+1);
	            if (_serialOutput) Serial.print(F(": "));
	            if (_serialOutput) Serial.println(_ATEM_DskOn[idx], BIN);
			}
          } else
	      if(strcmp(cmdStr, "DskP") == 0) {  // Downstream Keyer Tie
			idx = _packetBuffer[0];
			if (idx >=0 && idx <=1)	{
				_ATEM_DskTie[idx] = _packetBuffer[1] > 0 ? true : false;
	            if (_serialOutput) Serial.print(F("Dsk Keyer"));
	            if (_serialOutput) Serial.print(idx+1);
	            if (_serialOutput) Serial.print(F(" Tie: "));
	            if (_serialOutput) Serial.println(_ATEM_DskTie[idx], BIN);
			}
          } else
		  if(strcmp(cmdStr, "KeOn") == 0) {  // Upstream Keyer on
			idx = _packetBuffer[1];
			if (idx >=0 && idx <=3)	{
				_ATEM_KeOn[idx] = _packetBuffer[2] > 0 ? true : false;
	            if (_serialOutput) Serial.print(F("Upstream Keyer "));
	            if (_serialOutput) Serial.print(idx+1);
	            if (_serialOutput) Serial.print(F(": "));
	            if (_serialOutput) Serial.println(_ATEM_KeOn[idx], BIN);
			}
	      } else 
		  if(strcmp(cmdStr, "ColV") == 0) {  // Color Generator Change
				// Todo: Relatively easy: 8 bytes, first is the color generator, the last 6 is hsl words
		  } else 
		  if(strcmp(cmdStr, "MPCE") == 0) {  // Media Player Clip Enable
				idx = _packetBuffer[0];
				if (idx >=0 && idx <=1)	{
					_ATEM_MPType[idx] = _packetBuffer[1];
					_ATEM_MPStill[idx] = _packetBuffer[2];
					_ATEM_MPClip[idx] = _packetBuffer[3];
				}
		  } else 
		  if(strcmp(cmdStr, "AuxS") == 0) {  // Aux Output Source
				uint8_t auxInput = _packetBuffer[0];
				if (auxInput >=0 && auxInput <=2)	{
					if (!ver42())	{
						_ATEM_AuxS[auxInput] = _packetBuffer[1];
					} else {
						_ATEM_AuxS[auxInput] = (uint16_t)(_packetBuffer[2]<<8) | _packetBuffer[3];
					}
		            if (_serialOutput) Serial.print(F("Aux "));
		            if (_serialOutput) Serial.print(auxInput+1);
		            if (_serialOutput) Serial.print(F(" Output: "));
		            if (_serialOutput) Serial.println(_ATEM_AuxS[auxInput], DEC);
				}

		    } else 
		    if(strcmp(cmdStr, "_ver") == 0) {  // Firmware version
				_ATEM_ver_m = _packetBuffer[1];	// Firmware version, "left of decimal point" (what is that called anyway?)
				_ATEM_ver_l = _packetBuffer[3];	// Firmware version, decimals ("right of decimal point")
		    } else 
			if(strcmp(cmdStr, "_pin") == 0) {  // Name
				for(uint8_t i=0;i<16;i++)	{
					_ATEM_pin[i] = _packetBuffer[i];
				}
				_ATEM_pin[16] = 0;	// Termination
		    } else 
			if(strcmp(cmdStr, "AMTl") == 0) {  // Audio Monitor Tally (on/off settings)
				// Same system as for video: "TlIn"... just implement when time.
		    } else 
			// Note for future reveng: For master control, volume at least comes back in "AMMO" (CAMM is the command code.)
			if(strcmp(cmdStr, "AMIP") == 0) {  // Audio Monitor Input P... (state) (On, Off, AFV)
					// 0+1 = Channel (high+low byte):
				uint16_t channelNumber = (uint16_t)(_packetBuffer[0]<<8) | _packetBuffer[1];
			/*	Serial.print("CHANNEL: ");
				Serial.print(channelNumber);
				Serial.print(" = ");
				Serial.println(_packetBuffer[8]);
			*/	
					// 8 = On/Off/AFV
				if (channelNumber>=1 && channelNumber<=16)	{
					_ATEM_AudioChannelMode[channelNumber-1] = _packetBuffer[8];
				} else if (channelNumber==2001) {
					_ATEM_AudioChannelModeSpc[0] = _packetBuffer[8];	// MP1
				} else if (channelNumber==2002) {
					_ATEM_AudioChannelModeSpc[1] = _packetBuffer[8];	// MP2
				} else if (channelNumber==1001) {
					_ATEM_AudioChannelModeSpc[2] = _packetBuffer[8];	// XLR
				} else if (channelNumber==1201) {
					_ATEM_AudioChannelModeSpc[3] = _packetBuffer[8];	// RCA
				}

					// 10+11 = Balance (0xD8F0 - 0x0000 - 0x2710)
					// 8+9 = Volume (0x0020 - 0xFF65)
				

/*				for(uint8_t a=0;a<_cmdLength-8;a++)	{
	            	Serial.print((uint8_t)_packetBuffer[a], HEX);
	            	Serial.print(" ");
				}
				Serial.println("");				
*/				
/*				1M/E:
					0: MASTER
					1: (Monitor?)
					2-9: HDMI1 - SDI8
					10: MP1
					11: MP2
					12: EXT
				
				TVS:
					0: MASTER
					1: (Monitor?)
					2-7: INPUT1-6 (HDMI - HDMI - HDMI/SDI - HDMI/SDI - SDI - SDI)
					8: EXT
				*/
		/*		Serial.print("Audio Channel: ");
				Serial.println(_packetBuffer[0]);	// _packetBuffer[2] seems to be input number (one higher...)
				Serial.print(" - State: ");
				Serial.println(_packetBuffer[3] == 0x01 ? "ON" : (_packetBuffer[3] == 0x02 ? "AFV" : (_packetBuffer[3] > 0 ? "???" : "OFF")));
				Serial.print(" - Volume: ");
				Serial.print((uint16_t)_packetBuffer[4]*256+_packetBuffer[5]);
				Serial.print("/");
				Serial.println((uint16_t)_packetBuffer[6]*256+_packetBuffer[7]);
		   */ } else 
			if(strcmp(cmdStr, "AMLv") == 0) {  // Audio Monitor Levels
				// Get number of channels:
			  	_readToPacketBuffer(4);	// AMLv (AudioMonitorLevels)

				uint8_t numberOfChannels = _packetBuffer[1];
				uint8_t readingOffset=0;
				
			  	_readToPacketBuffer(32);	// AMLv (AudioMonitorLevels)
				if (_ATEM_AMLv_channel<=1)	{	// Master or Monitor vol
					readingOffset= _ATEM_AMLv_channel<<4;
					_ATEM_AMLv[0] = ((uint16_t)(_packetBuffer[readingOffset+1]<<8) | _packetBuffer[readingOffset+2]);	//drops the 8 least sign. bits! -> 15 bit resolution for VU purposes. fine enough.
					readingOffset+=4;
					_ATEM_AMLv[1] = ((uint16_t)(_packetBuffer[readingOffset+1]<<8) | _packetBuffer[readingOffset+2]);	//drops the 8 least sign. bits! -> 15 bit resolution for VU purposes. fine enough.
				} else {
						// Match indexes to input src numbers:
				  	_readToPacketBuffer(numberOfChannels & 1 ? (numberOfChannels+1)<<1 : numberOfChannels<<1);	// The block of input source numbers is always divisible by 4 bytes, so we must read a multiplum of 4 at all times
					for(uint8_t j=0; j<numberOfChannels; j++)	{
//						uint16_t inputNum = ((uint16_t)(_packetBuffer[j<<1]<<8) | _packetBuffer[(j<<1)+1]);
//						Serial.println(inputNum);
						/*
							0x07D1 = 2001 = MP1
							0x07D2 = 2002 = MP2
							0x03E9 = 1001 = XLR
							0x04b1 = 1201 = RCA
							*/
					}
						// Get level data for each input:
					for(uint8_t j=0; j<numberOfChannels; j++)	{
						_readToPacketBuffer(16);
						if (_ATEM_AMLv_channel == j+3)	{
							readingOffset = 0;
							_ATEM_AMLv[0] = ((uint16_t)(_packetBuffer[readingOffset+1]<<8) | _packetBuffer[readingOffset+2]);	//drops the 8 least sign. bits! -> 15 bit resolution for VU purposes. fine enough.
							readingOffset+=4;
							_ATEM_AMLv[1] = ((uint16_t)(_packetBuffer[readingOffset+1]<<8) | _packetBuffer[readingOffset+2]);	//drops the 8 least sign. bits! -> 15 bit resolution for VU purposes. fine enough.
						}
					}
					
				}
			} else 
			if(strcmp(cmdStr, "VidM") == 0) {  // Video format (SD, HD, framerate etc.)
				_ATEM_VidM = _packetBuffer[0];	
		    } else {
			
		
		
			// SHOULD ONLY THE UNCOMMENTED for development and with a high Baud rate on serial - 115200 for instance. Otherwise it will not connect due to serial writing speeds.
			/*
	            if (_serialOutput) {
					Serial.print(("???? Unknown token: "));
					Serial.print(cmdStr);
					Serial.print(" : ");
				}
				for(uint8_t a=(-2+8);a<_cmdLength-2;a++)	{
	            	if (_serialOutput && (uint8_t)_packetBuffer[a]<16) Serial.print(0);
	            	if (_serialOutput) Serial.print((uint8_t)_packetBuffer[a], HEX);
	            	if (_serialOutput) Serial.print(" ");
				}
				if (_serialOutput) Serial.println("");
	        */
			}
			
			// Empty, if long packet and not read yet:
	      while (_readToPacketBuffer())	{}
	
          indexPointer+=_cmdLength;
        } else { 
      		indexPointer = 2000;
          
			// Flushing the buffer:
			// TODO: Other way? _Udp.flush() ??
	          while(_Udp.available()) {
	              _Udp.read(_packetBuffer, ATEM_packetBufferLength);
	          }
        }
      }
}


/**
 * Sending a command packet back (ask the switcher to do something)
 */
void ATEM::_sendCommandPacket(const char cmd[4], uint8_t commandBytes[64], uint8_t cmdBytes)  {	// TEMP: 16->64

  if (cmdBytes <= 64)	{	// Currently, only a lenght up to 16 - can be extended, but then the _packetBuffer buffer must be prolonged as well (to more than 36)	<- TEMP 16->64

	  _wipeCleanPacketBuffer();

	  // Command identifier (4 bytes, after header (12 bytes) and local segment length (4 bytes)):
	  int i;
	  for (i=0; i<4; i++)  {
	    _packetBuffer[12+4+i] = cmd[i];
	  }

  		// Command value (after command):
	  for (i=0; i<cmdBytes; i++)  {
	    _packetBuffer[12+4+4+i] = commandBytes[i];
	  }

	  // Command length:
	  _packetBuffer[12] = (4+4+cmdBytes)/256;
	  _packetBuffer[12+1] = (4+4+cmdBytes)%256;

	  // Create header:
	  uint16_t returnPacketLength = 10+2+(4+4+cmdBytes);

	  createCommandHeader(ATEM_headerCmd_AckRequest, returnPacketLength);
	  sendPacketBuffer(returnPacketLength);  
	}
}



/**
 * Sets all zeros in packet buffer:
 */
void ATEM::_sendPacketBufferCmdData(const char cmd[4], uint8_t cmdBytes)  {
	
  if (cmdBytes <= ATEM_packetBufferLength-20)	{
	  _localPacketIdCounter++;

	  //Answer packet preparations:
	  uint8_t _headerBuffer[20];
	  memset(_headerBuffer, 0, 20);
	  _headerBuffer[2] = _sessionID >> 8;  // Session ID
	  _headerBuffer[3] = _sessionID & 0xFF;  // Session ID
	  _headerBuffer[10] = _localPacketIdCounter/256;  // Remote Packet ID, MSB
	  _headerBuffer[11] = _localPacketIdCounter%256;  // Remote Packet ID, LSB

	  // The rest is zeros.

	  // Command identifier (4 bytes, after header (12 bytes) and local segment length (4 bytes)):
	  int i;
	  for (i=0; i<4; i++)  {
	    _headerBuffer[12+4+i] = cmd[i];
	  }

	  // Command length:
	  _headerBuffer[12] = (4+4+cmdBytes)/256;
	  _headerBuffer[12+1] = (4+4+cmdBytes)%256;

	  // Create header:
	  uint16_t returnPacketLength = 20+cmdBytes;
	  _headerBuffer[0] = returnPacketLength/256;
	  _headerBuffer[1] = returnPacketLength%256;
	  _headerBuffer[0] |= B00001000;

	  // Send connectAnswerString to ATEM:
	  _Udp.beginPacket(_switcherIP,  9910);
	  _Udp.write(_headerBuffer,20);
	  _Udp.write(_packetBuffer,cmdBytes);
	  _Udp.endPacket();  
	}
}










































































uint8_t ATEM::getATEMmodel()	{
/*	Serial.println(_ATEM_pin);
	Serial.println(strcmp(_ATEM_pin, "ATEM Television ") == 0);
	Serial.println(strcmp(_ATEM_pin, "ATEM 1 M/E Produ") == 0);
	Serial.println(strcmp(_ATEM_pin, "ATEM 2 M/E Produ") == 0);	// Didn't test this yet!
*/
	if (_ATEM_pin[5]=='T')	{
		if (_serialOutput) Serial.println(F("ATEM TeleVision Studio Detected"));
		return 0;
	}
	if (_ATEM_pin[5]=='1')	{
		if (_serialOutput) Serial.println(F("ATEM 1 M/E Detected"));
		return 1;
	}
	if (_ATEM_pin[5]=='2')	{
		if (_serialOutput) Serial.println(F("ATEM 2 M/E Detected"));
		return 2;
	}
	if (_ATEM_pin[5]=='P')	{
		if (_serialOutput) Serial.println(F("ATEM Production Studio 4K"));
		return 3;
	}
	return 255;
}







/********************************
 *
 * ATEM Switcher state methods
 * Returns the most recent information we've 
 * got about the switchers state
 *
 ********************************/

uint16_t ATEM::getProgramInput() {
	return _ATEM_PrgI;
}
uint16_t ATEM::getPreviewInput() {
	return _ATEM_PrvI;
}
boolean ATEM::getProgramTally(uint8_t inputNumber) {
  	// TODO: Validate that input number exists on current model! <8 at the moment.
	return (_ATEM_TlIn[inputNumber-1] & 1) >0 ? true : false;
}
boolean ATEM::getPreviewTally(uint8_t inputNumber) {
  	// TODO: Validate that input number exists on current model! 1-8 at the moment.
	return (_ATEM_TlIn[inputNumber-1] & 2) >0 ? true : false;
}
boolean ATEM::getUpstreamKeyerStatus(uint8_t inputNumber) {
	if (inputNumber>=1 && inputNumber<=4)	{
		return _ATEM_KeOn[inputNumber-1];
	}
	return false;
}
boolean ATEM::getUpstreamKeyerOnNextTransitionStatus(uint8_t inputNumber) {	// input 0 = background
	if (inputNumber>=0 && inputNumber<=4)	{
			// Notice: the first bit is set for the "background", not valid.
		return (_ATEM_TrSS_KeyersOnNextTransition & (0x01 << inputNumber)) ? true : false;
	}
	return false;
}

boolean ATEM::getDownstreamKeyerStatus(uint8_t inputNumber) {
	if (inputNumber>=1 && inputNumber<=2)	{
		return _ATEM_DskOn[inputNumber-1];
	}
	return false;
}
uint16_t ATEM::getTransitionPosition() {
	return _ATEM_TrPs_position;
}
bool ATEM::getTransitionPreview()	{
	return _ATEM_TrPr;
}
uint8_t ATEM::getTransitionType()	{
	return _ATEM_TrSS_TransitionStyle;
}
uint8_t ATEM::getTransitionMixTime() {
	return _ATEM_TMxP_time;		// Transition time for Mix Transitions
}
boolean ATEM::getFadeToBlackState() {
	return _ATEM_FtbS_state;    // Active state of Fade-to-black
}
uint8_t ATEM::getFadeToBlackFrameCount() {
	return _ATEM_FtbS_frameCount;    // Returns current frame in the FTB
}
uint8_t ATEM::getFadeToBlackTime() {
	return _ATEM_FtbP_time;		// Transition time for Fade-to-black
}
bool ATEM::getDownstreamKeyTie(uint8_t keyer)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
		return _ATEM_DskTie[keyer-1];
	}
	return false;
}
uint16_t ATEM::getAuxState(uint8_t auxOutput)  {
  // TODO: Validate that input number exists on current model!
	// On ATEM 1M/E: Black (0), 1 (1), 2 (2), 3 (3), 4 (4), 5 (5), 6 (6), 7 (7), 8 (8), Bars (9), Color1 (10), Color 2 (11), Media 1 (12), Media 1 Key (13), Media 2 (14), Media 2 Key (15), Program (16), Preview (17), Clean1 (18), Clean 2 (19)

	if (auxOutput>=1 && auxOutput<=3)	{	// Todo: Should match available aux outputs
		return _ATEM_AuxS[auxOutput-1];
    }
	return 0;
}	
uint8_t ATEM::getMediaPlayerType(uint8_t mediaPlayer)  {
	if (mediaPlayer>=1 && mediaPlayer<=2)	{	// TODO: Adjust to particular ATEM model... (here 1M/E)
		return _ATEM_MPType[mediaPlayer-1];	// Media Player 1/2: Type (1=Clip, 2=Still)
	}
	return 0;
}
uint8_t ATEM::getMediaPlayerStill(uint8_t mediaPlayer)  {
	if (mediaPlayer>=1 && mediaPlayer<=2)	{	// TODO: Adjust to particular ATEM model... (here 1M/E)
		return _ATEM_MPStill[mediaPlayer-1]+1;	// Still number (if MPType==2)
	}
	return 0;
}
uint8_t ATEM::getMediaPlayerClip(uint8_t mediaPlayer)  {
	if (mediaPlayer>=1 && mediaPlayer<=2)	{	// TODO: Adjust to particular ATEM model... (here 1M/E)
		return _ATEM_MPClip[mediaPlayer-1]+1;	// Clip number (if MPType==1)
	}
	return 0;
}
uint16_t ATEM::getAudioLevels(uint8_t channel)	{
		// channel can be 0 (L) or 1 (R)
	return _ATEM_AMLv[channel];
}
uint8_t ATEM::getAudioChannelMode(uint16_t channelNumber)	{
	/*
	1-16 = inputs
	0x07D1 = 2001 = MP1
	0x07D2 = 2002 = MP2
	0x03E9 = 1001 = XLR
	0x04b1 = 1201 = RCA
	*/

	if (channelNumber>=1 && channelNumber<=16)	{
		return _ATEM_AudioChannelMode[channelNumber-1];
	} else if (channelNumber==2001) {
		return _ATEM_AudioChannelModeSpc[0];	// MP1
	} else if (channelNumber==2002) {
		return _ATEM_AudioChannelModeSpc[1];	// MP2
	} else if (channelNumber==1001) {
		return _ATEM_AudioChannelModeSpc[2];	// XLR
	} else if (channelNumber==1201) {
		return _ATEM_AudioChannelModeSpc[3];	// RCA
	}
	
	return 0;
}


/********************************
 *
 * ATEM Switcher Change methods
 * Asks the switcher to changes something
 *
 ********************************/



void ATEM::changeProgramInput(uint16_t inputNumber)  {
  // TODO: Validate that input number exists on current model!
	// On ATEM 1M/E: Black (0), 1 (1), 2 (2), 3 (3), 4 (4), 5 (5), 6 (6), 7 (7), 8 (8), Bars (9), Color1 (10), Color 2 (11), Media 1 (12), Media 2 (14)

  _wipeCleanPacketBuffer();
  if (!ver42())	{
	  _packetBuffer[1] = inputNumber;
  } else {
	  _packetBuffer[2] = (inputNumber >> 8);
	  _packetBuffer[3] = (inputNumber & 0xFF);
  }
  _sendPacketBufferCmdData("CPgI", 4);
}
void ATEM::changePreviewInput(uint16_t inputNumber)  {
  // TODO: Validate that input number exists on current model!

  _wipeCleanPacketBuffer();
  if (!ver42())	{
	  _packetBuffer[1] = inputNumber;
  } else {
	  _packetBuffer[2] = (inputNumber >> 8);
	  _packetBuffer[3] = (inputNumber & 0xFF);
  }
  _sendPacketBufferCmdData("CPvI", 4);
}
void ATEM::doCut()	{
  _wipeCleanPacketBuffer();
  _packetBuffer[1] = 0xef;
  _packetBuffer[2] = 0xbf;
  _packetBuffer[3] = 0x5f;
  _sendPacketBufferCmdData("DCut", 4);
}
void ATEM::doAuto()	{
	doAuto(0);
}
void ATEM::doAuto(uint8_t me)	{
  _wipeCleanPacketBuffer();
  _packetBuffer[0] = me;
  _sendPacketBufferCmdData("DAut", 4);
}
void ATEM::fadeToBlackActivate()	{
  _wipeCleanPacketBuffer();
  _packetBuffer[1] = 0x02;
  _packetBuffer[2] = 0x58;
  _packetBuffer[3] = 0x99;
  _sendPacketBufferCmdData("FtbA", 4);	// Reflected back from ATEM in "FtbS"
}
void ATEM::changeTransitionPosition(word value)	{
	if (value>0 && value<=1000)	{
		uint8_t commandBytes[4] = {0, 0xe4, (value*10)/256, (value*10)%256};
		_sendCommandPacket("CTPs", commandBytes, 4);  // Change Transition Position (CTPs)
	}
}
void ATEM::changeTransitionPositionDone()	{	// When the last value of the transition is sent (1000), send this one too (we are done, change tally lights and preview bus!)
	uint8_t commandBytes[4] = {0, 0xf6, 0, 0};  	// Done
	_sendCommandPacket("CTPs", commandBytes, 4);  // Change Transition Position (CTPs)
}
void ATEM::changeTransitionPreview(bool state)	{
	uint8_t commandBytes[4] = {0x00, state ? 0x01 : 0x00, 0x00, 0x00};
	_sendCommandPacket("CTPr", commandBytes, 4);	// Reflected back from ATEM in "TrPr"
}
void ATEM::changeTransitionType(uint8_t type)	{
	if (type>=0 && type<=4)	{	// 0=MIX, 1=DIP, 2=WIPE, 3=DVE, 4=STING
		uint8_t commandBytes[4] = {0x01, 0x00, type, 0x02};
		_sendCommandPacket("CTTp", commandBytes, 4);	// Reflected back from ATEM in "TrSS"
	}
}
void ATEM::changeTransitionMixTime(uint8_t frames)	{
	if (frames>=1 && frames<=0xFA)	{
		uint8_t commandBytes[4] = {0x00, frames, 0x00, 0x00};
		_sendCommandPacket("CTMx", commandBytes, 4);	// Reflected back from ATEM in "TMxP"
	}
}
void ATEM::changeFadeToBlackTime(uint8_t frames)	{
	if (frames>=1 && frames<=0xFA)	{
		uint8_t commandBytes[4] = {0x01, 0x00, frames, 0x02};
		_sendCommandPacket("FtbC", commandBytes, 4);	// Reflected back from ATEM in "FtbP"
	}
}
void ATEM::changeUpstreamKeyOn(uint8_t keyer, bool state)	{
	if (keyer>=1 && keyer<=4)	{	// Todo: Should match available keyers depending on model?
	  _wipeCleanPacketBuffer();
	  _packetBuffer[1] = keyer-1;
	  _packetBuffer[2] = state ? 0x01 : 0x00;
	  _packetBuffer[3] = 0x90;
	  _sendPacketBufferCmdData("CKOn", 4);	// Reflected back from ATEM in "KeOn"
	}
}
void ATEM::changeUpstreamKeyNextTransition(uint8_t keyer, bool state)	{	// Supporting "Background" by "0"
	if (keyer>=0 && keyer<=4)	{	// Todo: Should match available keyers depending on model?
		uint8_t stateValue = _ATEM_TrSS_KeyersOnNextTransition;
		if (state)	{
			stateValue = stateValue | (B1 << keyer);
		} else {
			stateValue = stateValue & (~(B1 << keyer));
		}
				// TODO: Requires internal storage of state here so we can preserve all other states when changing the one we want to change.
					// Below: Byte 2 is which ME (1 or 2):
		uint8_t commandBytes[4] = {0x02, 0x00, 0x6a, stateValue & B11111};
		_sendCommandPacket("CTTp", commandBytes, 4);	// Reflected back from ATEM in "TrSS"
	}
}
void ATEM::changeDownstreamKeyOn(uint8_t keyer, bool state)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
		
		uint8_t commandBytes[4] = {keyer-1, state ? 0x01 : 0x00, 0xff, 0xff};
		_sendCommandPacket("CDsL", commandBytes, 4);	// Reflected back from ATEM in "DskP" and "DskS"
	}
}
void ATEM::changeDownstreamKeyTie(uint8_t keyer, bool state)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
		uint8_t commandBytes[4] = {keyer-1, state ? 0x01 : 0x00, 0xff, 0xff};
		_sendCommandPacket("CDsT", commandBytes, 4);
	}
}
void ATEM::doAutoDownstreamKeyer(uint8_t keyer)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
  		uint8_t commandBytes[4] = {keyer-1, 0x32, 0x16, 0x02};	// I don't know what that actually means...
  		_sendCommandPacket("DDsA", commandBytes, 4);
	}
}
void ATEM::changeAuxState(uint8_t auxOutput, uint16_t inputNumber)  {
  // TODO: Validate that input number exists on current model!
	// On ATEM 1M/E: Black (0), 1 (1), 2 (2), 3 (3), 4 (4), 5 (5), 6 (6), 7 (7), 8 (8), Bars (9), Color1 (10), Color 2 (11), Media 1 (12), Media 1 Key (13), Media 2 (14), Media 2 Key (15), Program (16), Preview (17), Clean1 (18), Clean 2 (19)

	if (auxOutput>=1 && auxOutput<=3)	{	// Todo: Should match available aux outputs
		if (!ver42())	{
	  		uint8_t commandBytes[4] = {auxOutput-1, inputNumber, 0, 0};
	  		_sendCommandPacket("CAuS", commandBytes, 4);
		} else {
	  		uint8_t commandBytes[8] = {0x01, auxOutput-1, inputNumber >> 8, inputNumber & 0xFF, 0,0,0,0};
	  		_sendCommandPacket("CAuS", commandBytes, 8);
		}
		//Serial.print("freeMemory()=");
		//Serial.println(freeMemory());
    }
}
void ATEM::settingsMemorySave()	{
	uint8_t commandBytes[4] = {0, 0, 0, 0};
	_sendCommandPacket("SRsv", commandBytes, 4);
}
void ATEM::settingsMemoryClear()	{
	uint8_t commandBytes[4] = {0, 0, 0, 0};
	_sendCommandPacket("SRcl", commandBytes, 4);
}
void ATEM::changeColorValue(uint8_t colorGenerator, uint16_t hue, uint16_t saturation, uint16_t lightness)  {
	if (colorGenerator>=1 && colorGenerator<=2
			&& hue>=0 && hue<=3600 
			&& saturation >=0 && saturation <=1000 
			&& lightness >=0 && lightness <= 1000
		)	{	// Todo: Should match available aux outputs
  		uint8_t commandBytes[8] = {0x07, colorGenerator-1, 
			highByte(hue), lowByte(hue),
			highByte(saturation), lowByte(saturation),
			highByte(lightness), lowByte(lightness)
							};
  		_sendCommandPacket("CClV", commandBytes, 8);
    }
}
void ATEM::mediaPlayerSelectSource(uint8_t mediaPlayer, boolean movieclip, uint8_t sourceIndex)  {
	if (mediaPlayer>=1 && mediaPlayer<=2)	{	// TODO: Adjust to particular ATEM model... (here 1M/E)
		uint8_t commandBytes[8];
		memset(commandBytes, 0, 8);
  		commandBytes[1] = mediaPlayer-1;
		if (movieclip)	{
			commandBytes[0] = 4;
			if (sourceIndex>=1 && sourceIndex<=2)	{
				commandBytes[4] = sourceIndex-1;
			}
		} else {
			commandBytes[0] = 0x03;
			if (sourceIndex>=1 && sourceIndex<=32)	{
				commandBytes[3] = sourceIndex-1;
			}
		}
		commandBytes[9] = 0x10;
		_sendCommandPacket("MPSS", commandBytes, 8);
			
			// For some reason you have to send this command immediate after (or in fact it could be in the same packet)
			// If not done, the clip will not change if there is a shift from stills to clips or vice versa.
		uint8_t commandBytes2[8] = {0x01, mediaPlayer-1, movieclip?2:1, 0xbf, movieclip?0x96:0xd5, 0xb6, 0x04, 0};
		_sendCommandPacket("MPSS", commandBytes2, 8);
	}
}

void ATEM::mediaPlayerClipStart(uint8_t mediaPlayer)  {
	if (mediaPlayer>=1 && mediaPlayer<=2)	{
		uint8_t commandBytes2[8] = {0x01, mediaPlayer-1, 0x01, 0xbf, 0x21, 0xa9, 0x94, 0xfa}; // 3rd byte is "start", remaining 5 bytes seems random...
		_sendCommandPacket("SCPS", commandBytes2, 8);
	}
}


void ATEM::changeSwitcherVideoFormat(uint8_t format)	{
	// Changing the video format it uses: 525i59.94 NTSC (0), 625i50 PAL (1), 720p50 (2), 720p59.94 (3), 1080i50 (4), 1080i59.94 (5)
	if (format>=0 && format<=5)	{	// Todo: Should match available aux outputs
  		uint8_t commandBytes[4] = {format, 0xeb, 0xff, 0xbf};
  		_sendCommandPacket("CVdM", commandBytes, 4);
    }	
}



void ATEM::changeDVESettingsTemp(unsigned long Xpos,unsigned long Ypos,unsigned long Xsize,unsigned long Ysize)	{	// TEMP
  		uint8_t commandBytes[64] = {0x00, 0x00, 0x00, B1111, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, highByte(Xsize), lowByte(Xsize), 0x00, 0x00, highByte(Ysize), lowByte(Ysize), (Xpos >>24) & 0xFF, (Xpos >>16) & 0xFF, (Xpos >>8) & 0xFF, (Xpos >>0) & 0xFF, (Ypos >>24) & 0xFF, (Ypos >>16) & 0xFF, (Ypos >>8) & 0xFF, (Ypos >>0) & 0xFF, 0xbf, 0xff, 0xdb, 0x7f, 0xc2, 0xa2, 0x09, 0x90, 0xdb, 0x7e, 0xbf, 0xff, 0x82, 0x34, 0x2e, 0x0b, 0x05, 0x00, 0x00, 0x00, 0x34, 0xc1, 0x00, 0x2c, 0xe2, 0x00, 0x4e, 0x02, 0xa3, 0x98, 0xac, 0x02, 0xdb, 0xd9, 0xbf, 0xff, 0x74, 0x34, 0xe9, 0x01};
  		_sendCommandPacket("CKDV", commandBytes, 64);
}
void ATEM::changeDVEMaskTemp(unsigned long top,unsigned long bottom,unsigned long left,unsigned long right)	{	// TEMP
  		uint8_t commandBytes[64] = {0x03, 0xc0 | 0x20, 0x00, 0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01, highByte(top), lowByte(top), highByte(bottom), lowByte(bottom), highByte(left), lowByte(left), highByte(right), lowByte(right),0,0,0,0};
  		_sendCommandPacket("CKDV", commandBytes, 64);
}
void ATEM::changeDVEBorder(bool enableBorder)	{	// TEMP
  		uint8_t commandBytes[64] = {0x00, 0x00, 0x00, 0x20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, enableBorder?1:0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0};
  		_sendCommandPacket("CKDV", commandBytes, 64);
}

void ATEM::changeDVESettingsTemp_Rate(uint8_t rateFrames)	{	// TEMP
  		uint8_t commandBytes[64] = {B100, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbf, 0xff, 0xdb, 0x7f, 0xc2, 0xa2, 0x09, 0x90, 0xdb, 0x7e, 0xbf, 0xff, 0x82, 0x34, 0x2e, 0x0b, 0x05, 0x00, 0x00, 0x00, 0x34, 0xc1, 0x00, 0x2c, 0xe2, 0x00, 0x4e, 0x02, 0xa3, 0x98, 0xac, 0x02, 0xdb, 0xd9, 0xbf, 0xff, rateFrames, 0x34, 0xe9, 0x01};
  		_sendCommandPacket("CKDV", commandBytes, 64);
}
void ATEM::changeDVESettingsTemp_RunKeyFrame(uint8_t runType)	{	// runType: 1=A, 2=B, 3=Full, 4=on of the others (with an extra paramter:)
  		uint8_t commandBytes[8] = {0x02, 0x00, 0x00, 0x02, 0x00, runType, 0xff, 0xff};
  		_sendCommandPacket("RFlK", commandBytes, 8);
}
void ATEM::changeKeyerMask(uint16_t topMask, uint16_t bottomMask, uint16_t leftMask, uint16_t rightMask) {
	changeKeyerMask(0, topMask, bottomMask, leftMask, rightMask);
}
void ATEM::changeKeyerMask(uint8_t keyer, uint16_t topMask, uint16_t bottomMask, uint16_t leftMask, uint16_t rightMask)	{
		// In "B11110", bits are (from right to left): 0=?, 1=topMask, 2=bottomMask, 3=leftMask, 4=rightMask
  		uint8_t commandBytes[12] = {B11110, 0x00, keyer-1, 0x00, highByte(topMask), lowByte(topMask), highByte(bottomMask), lowByte(bottomMask), highByte(leftMask), lowByte(leftMask), highByte(rightMask), lowByte(rightMask)};
  		_sendCommandPacket("CKMs", commandBytes, 12);
}

void ATEM::changeDownstreamKeyMask(uint8_t keyer, uint16_t topMask, uint16_t bottomMask, uint16_t leftMask, uint16_t rightMask)	{
		// In "B11110", bits are (from right to left): 0=?, 1=topMask, 2=bottomMask, 3=leftMask, 4=rightMask
		if (keyer>=1 && keyer<=2)	{
  			uint8_t commandBytes[12] = {B11110, keyer-1, 0x00, 0x00, highByte(topMask), lowByte(topMask), highByte(bottomMask), lowByte(bottomMask), highByte(leftMask), lowByte(leftMask), highByte(rightMask), lowByte(rightMask)};
  			_sendCommandPacket("CDsM", commandBytes, 12);
		}
}



void ATEM::changeUpstreamKeyFillSource(uint8_t keyer, uint16_t inputNumber)	{
	if (keyer>=1 && keyer<=4)	{	// Todo: Should match available keyers depending on model?
	  	// TODO: Validate that input number exists on current model!
		// 0-15 on 1M/E
		if (!ver42())	{
			uint8_t commandBytes[4] = {0, keyer-1, inputNumber, 0};
			_sendCommandPacket("CKeF", commandBytes, 4);
		} else {
			uint8_t commandBytes[4] = {0, keyer-1, highByte(inputNumber), lowByte(inputNumber)};
			_sendCommandPacket("CKeF", commandBytes, 4);
		}
	}
}

// TODO: ONLY clip works right now! there is a bug...
void ATEM::changeUpstreamKeyBlending(uint8_t keyer, bool preMultipliedAlpha, uint16_t clip, uint16_t gain, bool invKey)	{
	if (keyer>=1 && keyer<=4)	{	// Todo: Should match available keyers depending on model?
		uint8_t commandBytes[12] = {0x02, keyer-1, 0, preMultipliedAlpha?1:0, highByte(clip), lowByte(clip), highByte(gain), lowByte(gain), invKey?1:0, 0, 0, 0};
		_sendCommandPacket("CKLm", commandBytes, 12);
	}
}

// TODO: ONLY clip works right now! there is a bug...
void ATEM::changeDownstreamKeyBlending(uint8_t keyer, bool preMultipliedAlpha, uint16_t clip, uint16_t gain, bool invKey)	{
	if (keyer>=1 && keyer<=4)	{	// Todo: Should match available keyers depending on model?
		uint8_t commandBytes[12] = {0x02, keyer-1, preMultipliedAlpha?1:0, 0, highByte(clip), lowByte(clip), highByte(gain), lowByte(gain), invKey?1:0, 0, 0, 0};
		_sendCommandPacket("CDsG", commandBytes, 12);
	}
}

// Statuskode retur: DskB, data byte 2 derefter er fill source, data byte 3 er key source, data byte 1 er keyer 1-2 (0-1)
// Key source command er : CDsC - og ellers ens med...
void ATEM::changeDownstreamKeyFillSource(uint8_t keyer, uint16_t inputNumber)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
	  	// TODO: Validate that input number exists on current model!
		// 0-15 on 1M/E
		if (!ver42())	{
			uint8_t commandBytes[4] = {keyer-1, inputNumber, 0, 0};
			_sendCommandPacket("CDsF", commandBytes, 4);
		} else {
			uint8_t commandBytes[4] = {keyer-1, 0, highByte(inputNumber), lowByte(inputNumber)};
			_sendCommandPacket("CDsF", commandBytes, 4);
		}
	}
}

void ATEM::changeDownstreamKeyKeySource(uint8_t keyer, uint16_t inputNumber)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
	  	// TODO: Validate that input number exists on current model!
		// 0-15 on 1M/E
		if (!ver42())	{
			uint8_t commandBytes[4] = {keyer-1, inputNumber, 0, 0};
			_sendCommandPacket("CDsC", commandBytes, 4);
		} else {
			uint8_t commandBytes[4] = {keyer-1, 0, highByte(inputNumber), lowByte(inputNumber)};
			_sendCommandPacket("CDsC", commandBytes, 4);
		}
	}
}

void ATEM::changeAudioChannelMode(uint16_t channelNumber, uint8_t mode)	{	// Mode: 0=Off, 1=On, 2=AFV
  if (mode<=2)	{
	  _wipeCleanPacketBuffer();
		if (!ver42())	{
		  _packetBuffer[0] = 0x01;	// Setting ON/OFF/AFV
		  _packetBuffer[1] = channelNumber;	// Input 1-8 = channel 0-7(!), Media Player 1+2 = channel 8-9, Ext = channel 10 (For 1M/E!)
		  _packetBuffer[2] = mode;	// 0=Off, 1=On, 2=AFV
		  _packetBuffer[3] = 0x03;	
		  _sendPacketBufferCmdData("CAMI", 12);	// Reflected back from ATEM as "AMIP"
		} else {
		  _packetBuffer[0] = 0x01;	// Setting ON/OFF/AFV
		  _packetBuffer[2] = highByte(channelNumber);
		  _packetBuffer[3] = lowByte(channelNumber);
		  _packetBuffer[4] = mode;	// 0=Off, 1=On, 2=AFV
		  _sendPacketBufferCmdData("CAMI", 12);	// Reflected back from ATEM as "AMIP"
		}
  }
}
void ATEM::changeAudioChannelVolume(uint16_t channelNumber, uint16_t volume)	{

	/*
	Based on data from the ATEM switcher, this is an approximation to the integer value vs. the dB value:
	dB	+60 added	Number from protocol		Interpolated
	6	66	65381		65381
	3	63	46286		46301,04
	0	60	32768		32789,13
	-3	57	23198		23220,37
	-6	54	16423		16444,03
	-9	51	11627		11645,22
	-20	40	3377		3285,93
	-30	30	1036		1040,21
	-40	20	328		329,3
	-50	10	104		104,24
	-60	0	33		33

	for (int i=-60; i<=6; i=i+3)  {
	   Serial.print(i);
	   Serial.print(" dB = ");
	   Serial.print(33*pow(1.121898585, i+60));
	   Serial.println();
	}


	*/

// CAMI command structure:  	CAMI    [01=buttons, 02=vol, 04=pan (toggle bits)] - [input number, 0-…] - [buttons] - [buttons] - [vol] - [vol] - [pan] - [pan]
// CAMM: 01:de:80:00:e4:10:ff:bf (master) [volume is 80:00]

  _wipeCleanPacketBuffer();

	if (!ver42())	{
	  _packetBuffer[0] = 0x02;	// Setting Volume Level
	  _packetBuffer[1] = channelNumber;	// Input 1-8 = channel 0-7(!), Media Player 1+2 = channel 8-9, Ext = channel 10 (For 1M/E!)		///		Input 1-6 = channel 0-5(!), Ext = channel 6 (For TVS!)
		if (volume > 0xff65)	{
			volume = 0xff65;
		}
	  _packetBuffer[4] = volume/256;	
	  _packetBuffer[5] = volume%256;	

	  _sendPacketBufferCmdData("CAMI", 8);
	} else {
	  _packetBuffer[0] = 0x02;	// Setting Volume Level

	  _packetBuffer[2] = highByte(channelNumber);
	  _packetBuffer[3] = lowByte(channelNumber);

		if (volume > 0xff65)	{
			volume = 0xff65;
		}
	  _packetBuffer[6] = highByte(volume);	
	  _packetBuffer[7] = lowByte(volume);	

	  _sendPacketBufferCmdData("CAMI", 12);
		
	}
}

void ATEM::changeAudioMasterVolume(uint16_t volume)	{

// CAMI command structure:  	CAMI    [01=but, 02=vol, 04=pan (toggle bits)] - [input number, 0-…] - [buttons] - [buttons] - [vol] - [vol] - [pan] - [pan]
// CAMM: 01:de:80:00:e4:10:ff:bf (master) [volume is 80:00]

  _wipeCleanPacketBuffer();

  _packetBuffer[0] = 0x01;

	if (volume > 0xff65)	{
		volume = 0xff65;
	}

  _packetBuffer[2] = volume/256;	
  _packetBuffer[3] = volume%256;	

  _sendPacketBufferCmdData("CAMM", 8);
}
void ATEM::sendAudioLevelNumbers(bool enable)	{
  _wipeCleanPacketBuffer();
  _packetBuffer[0] = enable ? 1 : 0;
  _sendPacketBufferCmdData("SALN", 4);
}
void ATEM::setAudioLevelReadoutChannel(uint8_t AMLv)	{
				/*
				Channels on an 1M/E: (16 byte segments:)
				0: MASTER
				1: (Monitor?)
				2-9: HDMI1 - SDI8
				10: MP1
				11: MP2
				12: EXT
				
Values:
FCP			HyperDeck	ATEM			Value
Output		Studio 		Input			in
Level:		Playback:	Colors:			Protocol:

0 			red			red				32767
-3 			red			red				23228
-6 			red/yellow	red				16444
-9 			yellow		red/yellow		11640
-12 		yellow		yellow			8240
-18 		green		yellow/green	4130
-24			green		green			2070
-42			green		green			260				


(Values = 32809,85 * 1,12^dB (trendline based on numbers above))
(HyperDeck Studio: red=>yellow @ -6db, yellow=>green @ -15db (assumed))
(ATEM: red=>yellow @ -9db, yellow=>green @ -20db)
(ATEM Input registered the exact same level values, FCP had been writing to the ProRes 422 file.)				
				
				*/
	_ATEM_AMLv_channel = AMLv;	// Should check that it's in range 0-12
}

bool ATEM::ver42()	{
	//Serial.println(_ATEM_ver_m);
	//Serial.println(_ATEM_ver_l);
	
	// ATEM Control Panel software v. 4.2 = firmware version 2.12
	
//	Serial.println((_ATEM_ver_m>2) || (_ATEM_ver_m>=2 && _ATEM_ver_l>=12));
	
	return (_ATEM_ver_m>2) || (_ATEM_ver_m>=2 && _ATEM_ver_l>=12);
}





void ATEM::setWipeReverseDirection(bool reverse) {

  _wipeCleanPacketBuffer();

  _packetBuffer[0] = 0x01;	
  _packetBuffer[18] = reverse;	

  _sendPacketBufferCmdData("CTWp", 20);
}

