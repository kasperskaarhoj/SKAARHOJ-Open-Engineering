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



#include "ATEMbase.h"



/**
 * Constructor
 */
ATEMbase::ATEMbase(){}

/**
 * Setting up IP address for the switcher (and local port to send packets from)
 * Using local port here is deprecated. Rather let the library pick a random one
 */
void ATEMbase::begin(const IPAddress ip){
	begin(ip, random(50100,65300));
}
void ATEMbase::begin(const IPAddress ip, const uint16_t localPort){

		// Set up Udp communication object:
	EthernetUDP Udp;
	_Udp = Udp;
	
	_switcherIP = ip;			// Set switcher IP address
	_localPort = localPort;		// Set default local port

	_lastContact = 0;
	_serialOutput = 0;
	
	resetCommandBundle();
}

/**
 * Initiating connection handshake to the ATEM switcher
 */
void ATEMbase::connect() {
	connect(false);
}

/**
 * Initiating connection handshake to the ATEM switcher
 * If useFixedPortNumber is true, the same port number will be used on subsequent connects, otherwise - and recommended - a new, random port number is used.
 */
void ATEMbase::connect(const boolean useFixedPortNumber) {
	_localPacketIdCounter = 0;		// Init localPacketIDCounter to 0;
	_initPayloadSent = false;		// Will be true after initial payload of data is delivered (regular 12-byte ping packages are transmitted.)
	_hasInitialized = false;		// Will be true after initial payload of data is resent and received well
	_isConnected = false;			// Will be true after the initial hello-package handshakes.
	_sessionID = 0x53AB;			// Temporary session ID - a new will be given back from ATEM.
	_lastContact = millis();  		// Setting this, because even though we haven't had contact, it constitutes an attempt that should be responded to at least
	memset(_missedInitializationPackages, 0xFF, (ATEM_maxInitPackageCount+7)/8);
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
	_createCommandHeader(ATEM_headerCmd_HelloPacket, 12+8);
	_packetBuffer[12] = 0x01;	// This seems to be what the client should send upon first request. 
	_packetBuffer[9] = 0x3a;	// This seems to be what the client should send upon first request. 
	
	_sendPacketBuffer(20);  
}

/**
 * Keeps connection to the switcher alive
 * Therefore: Call this in the Arduino loop() function and make sure it gets call at least 2 times a second
 * Other recommendations might come up in the future.
 */
void ATEMbase::runLoop() {
	runLoop(0);
}
void ATEMbase::runLoop(uint16_t delayTime) {

	unsigned long enterTime = millis();
	
	static boolean waitingForIncoming = false;

	do {
		while(true) {	// Iterate until UDP buffer is empty
			uint16_t packetSize = _Udp.parsePacket();
		
			if (_Udp.available())   {  	

				_Udp.read(_packetBuffer,12);	// Read header
				 _sessionID = word(_packetBuffer[2], _packetBuffer[3]);
				 uint8_t headerBitmask = _packetBuffer[0]>>3;
				 _lastRemotePacketID = word(_packetBuffer[10],_packetBuffer[11]);
			 	 if (_lastRemotePacketID < ATEM_maxInitPackageCount)	{
			 	 	_missedInitializationPackages[_lastRemotePacketID>>3] &= ~(B1<<(_lastRemotePacketID&0x07));
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
						_createCommandHeader(ATEM_headerCmd_Ack, 12);
						_packetBuffer[9] = 0x03;	// This seems to be what the client should send upon first request. 
						_sendPacketBuffer(12);  
					}

					// If a packet is 12 bytes long it indicates that all the initial information 
					// has been delivered from the ATEM and we can begin to answer back on every request
					// Currently we don't know any other way to decide if an answer should be sent back...
					// The QT lib uses the "InCm" command to indicate this, but in the latest version of the firmware (2.14)
					// all the camera control information comes AFTER this command, so it's not a clear ending token anymore.
					// However, I'm not sure if I checked the _lastRemotePacketID of the packages with the additional camera control info - if it was a resend, 
					// "InCm" may still indicate the number of the last init-package and that's all I need to request the missing ones....
			
					// BTW: It has been observed on an old 10Mbit hub that packages could arrive in a different order than sent and this may 
					// mess things up a bit on the initialization. So it's recommended to has as direct routes as possible.
					if(!_initPayloadSent && packetSize == 12 && _lastRemotePacketID>1) {
						_initPayloadSent = true;
						_initPayloadSentAtPacketId = _lastRemotePacketID;
						#if ATEM_debug 
						if (_serialOutput & 0x80) {
							Serial.print(F("_initPayloadSent=TRUE @rpID "));
							Serial.println(_initPayloadSentAtPacketId);
							Serial.print(F("Session ID: "));
							Serial.println(_sessionID, DEC);
						}
						#endif
					} 

					if (_initPayloadSent && (headerBitmask & ATEM_headerCmd_AckRequest) && (_hasInitialized || !(headerBitmask & ATEM_headerCmd_Resend))) { 	// Respond to request for acknowledge	(and to resends also, whatever...  
						_wipeCleanPacketBuffer();
						_createCommandHeader(ATEM_headerCmd_Ack, 12, _lastRemotePacketID);
						_sendPacketBuffer(12); 
					
						#if ATEM_debug 
				        if (_serialOutput & 0x80) {
							Serial.print(F("rpID: "));
				        	Serial.print(_lastRemotePacketID, DEC);
							Serial.print(F(", Head: 0x"));
							Serial.print(headerBitmask, HEX);
							Serial.print(F(", Len: "));
				        	Serial.print(packetLength, DEC);
							Serial.print(F(" bytes"));

							Serial.println(F(" - ACK!"));
						} else 
						#endif
						if (_serialOutput>1)	{
							Serial.print(F("rpID: "));
				        	Serial.print(_lastRemotePacketID, DEC);
							Serial.println(F(" - ACK!"));
						} 
					} else if(_initPayloadSent && (headerBitmask & ATEM_headerCmd_RequestNextAfter) && _hasInitialized) {	// ATEM is requesting a previously sent package which must have dropped out of the order. We return an empty one so the ATEM doesnt' crash (which some models will, if it doesn't get an answer before another 63 commands gets sent from the controller.)
						uint8_t b1 = _packetBuffer[6];
						uint8_t b2 = _packetBuffer[7];
						_wipeCleanPacketBuffer();
						_createCommandHeader(ATEM_headerCmd_Ack, 12, 0);
						_packetBuffer[0] = ATEM_headerCmd_AckRequest << 3;	// Overruling this. A small trick because createCommandHeader shouldn't increment local package ID counter
						_packetBuffer[10] = b1;
						_packetBuffer[11] = b2;
						_sendPacketBuffer(12); 

						if (_serialOutput>1)	{
							Serial.print(F("ATEM asking to resend "));
				        	Serial.println((b1<<8)|b2, DEC);
						}
					} else {
						#if ATEM_debug 
				        if (_serialOutput & 0x80) {
							Serial.print(F("rpID: "));
				        	Serial.print(_lastRemotePacketID, DEC);
							Serial.print(F(", Head: 0x"));
							Serial.print(headerBitmask, HEX);
							Serial.print(F(", Len: "));
				        	Serial.print(packetLength, DEC);
							Serial.println(F(" bytes"));
						} else 
						#endif
						if (_serialOutput>1)	{
							Serial.print(F("rpID: "));
				        	Serial.println(_lastRemotePacketID, DEC);
						}
					}
				
					if (!(headerBitmask & ATEM_headerCmd_HelloPacket) && packetLength>12)	{
						_parsePacket(packetLength);
					}
			    } else {
					#if ATEM_debug
					if (_serialOutput & 0x80) 	{
			      		Serial.print(F("ERROR: Packet size mismatch: "));
					    Serial.print(packetSize, DEC);
					    Serial.print(F(" != "));
					    Serial.println(packetLength, DEC);
					}
					#endif
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
				if (_missedInitializationPackages[i>>3] & (B1<<(i & 0x7)))	{

					#if ATEM_debug
					if (_serialOutput & 0x80) 	{
			      		Serial.print(F("Asking for package "));
					    Serial.println(i, DEC);
					}
					#endif
					_wipeCleanPacketBuffer();
					_createCommandHeader(ATEM_headerCmd_RequestNextAfter, 12);
				    _packetBuffer[6] = highByte(i-1);  // Resend Packet ID, MSB
				    _packetBuffer[7] = lowByte(i-1);  // Resend Packet ID, LSB
				    _packetBuffer[8] = 0x01;
				
					_sendPacketBuffer(12);  
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

	} while (delayTime>0 && !hasTimedOut(enterTime,delayTime));
	

    // If connection is gone anyway, try to reconnect:
	if (hasTimedOut(_lastContact, 5000))	{
      if (_serialOutput) Serial.println(F("Connection to ATEM Switcher has timed out - reconnecting!"));
      connect();
    }	
}

/**
 * Returns last Remote Packet ID
 */
uint16_t ATEMbase::getATEM_lastRemotePacketId()	{
	return _lastRemotePacketID;
}

/**
 * Get ATEM session ID
 */
uint16_t ATEMbase::getSessionID() {
	return _sessionID;
}

/**
 * If true, we had a response from the switcher when trying to send a hello packet.
 */
bool ATEMbase::isConnected()	{
	return _isConnected;
}

/**
 * If true, the initial handshake and "stressful" information exchange has occured and now the switcher connection should be ready for operation. 
 */
bool ATEMbase::hasInitialized()	{
	return _hasInitialized;
}






/**************
 *
 * Buffer work
 *
 **************/

void ATEMbase::_createCommandHeader(const uint8_t headerCmd, const uint16_t lengthOfData)	{
	_createCommandHeader(headerCmd, lengthOfData, 0);
}
void ATEMbase::_createCommandHeader(const uint8_t headerCmd, const uint16_t lengthOfData, const uint16_t remotePacketID)	{

    _packetBuffer[0] = (headerCmd << 3) | (highByte(lengthOfData) & 0x07);  // Command bits + length MSB
    _packetBuffer[1] = lowByte(lengthOfData);  // length LSB

    _packetBuffer[2] = highByte(_sessionID);  // Session ID
    _packetBuffer[3] = lowByte(_sessionID);  // Session ID

    _packetBuffer[4] = highByte(remotePacketID);  // Remote Packet ID, MSB
    _packetBuffer[5] = lowByte(remotePacketID);  // Remote Packet ID, LSB
		
    if(!(headerCmd & (ATEM_headerCmd_HelloPacket | ATEM_headerCmd_Ack | ATEM_headerCmd_RequestNextAfter))) {
        _localPacketIdCounter++;

//		if ((_localPacketIdCounter & 0xF) == 0xF) _localPacketIdCounter++;	// Uncommenting this line will jump the local package ID counter every 15 command - thereby introducing a stress test of the robustness of the "resent package" function from the ATEM switcher.

	    _packetBuffer[10] = highByte(_localPacketIdCounter);  // Local Packet ID, MSB
	    _packetBuffer[11] = lowByte(_localPacketIdCounter);  // Local Packet ID, LSB
    }
}
void ATEMbase::_sendPacketBuffer(uint8_t length)	{
	_Udp.beginPacket(_switcherIP,  9910);
	_Udp.write(_packetBuffer,length);
	_Udp.endPacket(); 	// TODO: Figure out why this may hang!!
}

/**
 * Sets all zeros in packet buffer:
 */
void ATEMbase::_wipeCleanPacketBuffer() {
	memset(_packetBuffer, 0, ATEM_packetBufferLength);
}

/**
 * Reads from UDP channel to buffer. Will fill the buffer to the max or to the size of the current segment being parsed
 * Returns false if there are no more bytes, otherwise true 
 */
bool ATEMbase::_readToPacketBuffer() {
	return _readToPacketBuffer(ATEM_packetBufferLength);
}
bool ATEMbase::_readToPacketBuffer(uint8_t maxBytes) {
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
void ATEMbase::_parsePacket(uint16_t packetLength)	{
	
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
			_parseGetCommands(cmdStr);

			while (_readToPacketBuffer())	{}	// Empty, if not done yet.
			indexPointer+=_cmdLength;
        } else { 
      		indexPointer = 2000;
         	#if ATEM_debug 
				if (_serialOutput & 0x80) Serial.println(F("Bad CMD length, flushing..."));
			#endif
		  
			// Flushing the buffer:
	          while(_Udp.available()) {
	              _Udp.read(_packetBuffer, ATEM_packetBufferLength);
	          }
        }
      }
}

/**
 * This method should be overloaded in subclasses in order to handle specific get-commands
 */
void ATEMbase::_parseGetCommands(const char *cmdString)	{
//	uint8_t mE, keyer, mediaPlayer, aUXChannel, windowIndex, multiViewer, memory, colorGenerator, box;
//	uint16_t audioSource, videoSource;
//	long temp;
	
	uint8_t numberOfReads=1;
	while(_readToPacketBuffer())	{
		numberOfReads++;
	}

	#if ATEM_debug
 	if (_serialOutput & 0x80) {
		Serial.print(cmdString);
		Serial.print(", len: ");
		Serial.print(_cmdLength);
		Serial.print(", rds: ");
		Serial.println(numberOfReads);
	}
	#endif
}

void ATEMbase::_prepareCommandPacket(const char *cmdString, uint8_t cmdBytes, bool indexMatch)  {
	
		// First, in case of a command bundle, check if indexes are different OR if it's an entirely different command, then increase offset to accommodate new command:
		if (_cBundle)	{
			if (_returnPacketLength>0 && (!indexMatch || strncmp_P((char *)(_packetBuffer+12+_cBBO+4), cmdString, 4)))	{
				_cBBO = _returnPacketLength-12;
	  		}
		} else { 
			_wipeCleanPacketBuffer();	// For command bundles, this is already done...
		}

  	  _returnPacketLength = 12+_cBBO+(4+4+cmdBytes);
  
	  // Because we increased length of command, we need to check for buffer overflow:
  	  if (_returnPacketLength > ATEM_packetBufferLength)	{
  	  	  Serial.println(F("FATAL ERROR: Packet Buffer Overflow in the ATEM Library! Too long or too many commands bundled!\n HALT"));
  		  while(true){}	// STOP!
  	  }	

  		// Copy Command String:
		if (strlen_P(cmdString)==4)	{
			strncpy_P((char *)(_packetBuffer+12+_cBBO+4), cmdString, 4);
		} 
		#if ATEM_debug
		else Serial.println(F("Command Length > 4 ERROR"));
		#endif

	  // Command length:
	  _packetBuffer[12+_cBBO] = 0;	// MSB - but it's always under 256, so....
	  _packetBuffer[12+1+_cBBO] = 4+4+cmdBytes;	// LSB
}

void ATEMbase::_finishCommandPacket()	{
	if (!_cBundle)	{	

	  _createCommandHeader(ATEM_headerCmd_AckRequest, _returnPacketLength);
	  _sendPacketBuffer(_returnPacketLength);
	  _returnPacketLength = 0;

	} else {
    	  // Debugging info:
    /*	  for(uint8_t a=0; a<_returnPacketLength; a++)	{
  		  if (_packetBuffer[a]<16)	Serial.print("0");
    		  Serial.print(_packetBuffer[a], HEX);
    		  Serial.print(F("-"));
    	  }
    	  Serial.println();
		*/
	}
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
void ATEMbase::serialOutput(uint8_t level) {
	_serialOutput = level;
}

/**
 * Timeout check
 */
bool ATEMbase::hasTimedOut(unsigned long time, unsigned long timeout)  {
  if ((unsigned long)(time + timeout) <= (unsigned long)millis())  {  // This should "wrap around" if time+timout is larger than the size of unsigned-longs, right?
    return true;
  } 
  else {
    return false;
  }
}






uint8_t ATEMbase::getATEMmodel()	{
	return _ATEMmodel;
}






float ATEMbase::audioWord2Db(uint16_t input)  {  // -48 to +6 output
  // Formular: log10(input/128)*20-48;
  if (input<=32)  return -60;
  return (log10(input)-2.1072099696)*20-48;
}
uint16_t ATEMbase::audioDb2Word(float input)  {  // -48 to +6 input
  return (float)pow(10,(input+48)/20)*128;
}









uint8_t ATEMbase::getVideoSrcIndex(uint16_t videoSrc)	{
	switch(videoSrc){
		case 0:  // Black
			return 0;
		case 1:  // Input 1
			return 1;
		case 2:  // Input 2
			return 2;
		case 3:  // Input 3
			return 3;
		case 4:  // Input 4
			return 4;
		case 5:  // Input 5
			return 5;
		case 6:  // Input 6
			return 6;
		case 7:  // Input 7
			return 7;
		case 8:  // Input 8
			return 8;
		case 9:  // Input 9
			return 9;
		case 10:  // Input 10
			return 10;
		case 11:  // Input 11
			return 11;
		case 12:  // Input 12
			return 12;
		case 13:  // Input 13
			return 13;
		case 14:  // Input 14
			return 14;
		case 15:  // Input 15
			return 15;
		case 16:  // Input 16
			return 16;
		case 17:  // Input 17
			return 17;
		case 18:  // Input 18
			return 18;
		case 19:  // Input 19
			return 19;
		case 20:  // Input 20
			return 20;
		case 1000:  // Color Bars
			return 21;
		case 2001:  // Color 1
			return 22;
		case 2002:  // Color 2
			return 23;
		case 3010:  // Media Player 1
			return 24;
		case 3011:  // Media Player 1 Key
			return 25;
		case 3020:  // Media Player 2
			return 26;
		case 3021:  // Media Player 2 Key
			return 27;
		case 4010:  // Key 1 Mask
			return 28;
		case 4020:  // Key 2 Mask
			return 29;
		case 4030:  // Key 3 Mask
			return 30;
		case 4040:  // Key 4 Mask
			return 31;
		case 5010:  // DSK 1 Mask
			return 32;
		case 5020:  // DSK 2 Mask
			return 33;
		case 6000:  // Super Source
			return 34;
		case 7001:  // Clean Feed 1
			return 35;
		case 7002:  // Clean Feed 2
			return 36;
		case 8001:  // Auxilary 1
			return 37;
		case 8002:  // Auxilary 2
			return 38;
		case 8003:  // Auxilary 3
			return 39;
		case 8004:  // Auxilary 4
			return 40;
		case 8005:  // Auxilary 5
			return 41;
		case 8006:  // Auxilary 6
			return 42;
		case 10010:  // ME 1 Prog
			return 43;
		case 10011:  // ME 1 Prev
			return 44;
		case 10020:  // ME 2 Prog
			return 45;
		case 10021:  // ME 2 Prev
			return 46;
		default:
			return 0;
	}
}
		
uint8_t ATEMbase::getAudioSrcIndex(uint16_t audioSrc)	{
	switch(audioSrc){
		case 1:  // Input 1
			return 0;
		case 2:  // Input 2
			return 1;
		case 3:  // Input 3
			return 2;
		case 4:  // Input 4
			return 3;
		case 5:  // Input 5
			return 4;
		case 6:  // Input 6
			return 5;
		case 7:  // Input 7
			return 6;
		case 8:  // Input 8
			return 7;
		case 9:  // Input 9
			return 8;
		case 10:  // Input 10
			return 9;
		case 11:  // Input 11
			return 10;
		case 12:  // Input 12
			return 11;
		case 13:  // Input 13
			return 12;
		case 14:  // Input 14
			return 13;
		case 15:  // Input 15
			return 14;
		case 16:  // Input 16
			return 15;
		case 17:  // Input 17
			return 16;
		case 18:  // Input 18
			return 17;
		case 19:  // Input 19
			return 18;
		case 20:  // Input 20
			return 19;
		case 1001:  // XLR
			return 20;
		case 1101:  // AES/EBU
			return 21;
		case 1201:  // RCA
			return 22;
		case 2001:  // MP1
			return 23;
		case 2002:  // MP2
			return 24;
		default:
			return 0;
	}
}

/*
 * Translating a index to a video source
 */
uint16_t ATEMbase::getVideoIndexSrc(uint8_t index)	{
  switch (index) {
    case 0:  // Black
      return 0;
    case 1:  // Input 1
      return 1;
    case 2:  // Input 2
      return 2;
    case 3:  // Input 3
      return 3;
    case 4:  // Input 4
      return 4;
    case 5:  // Input 5
      return 5;
    case 6:  // Input 6
      return 6;
    case 7:  // Input 7
      return 7;
    case 8:  // Input 8
      return 8;
    case 9:  // Input 9
      return 9;
    case 10:  // Input 10
      return 10;
    case 11:  // Input 11
      return 11;
    case 12:  // Input 12
      return 12;
    case 13:  // Input 13
      return 13;
    case 14:  // Input 14
      return 14;
    case 15:  // Input 15
      return 15;
    case 16:  // Input 16
      return 16;
    case 17:  // Input 17
      return 17;
    case 18:  // Input 18
      return 18;
    case 19:  // Input 19
      return 19;
    case 20:  // Input 20
      return 20;
    case 21:  // Color Bars
      return 1000;
    case 22:  // Color 1
      return 2001;
    case 23:  // Color 2
      return 2002;
    case 24:  // Media Player 1
      return 3010;
    case 25:  // Media Player 1 Key
      return 3011;
    case 26:  // Media Player 2
      return 3020;
    case 27:  // Media Player 2 Key
      return 3021;
    case 28:  // Key 1 Mask
      return 4010;
    case 29:  // Key 2 Mask
      return 4020;
    case 30:  // Key 3 Mask
      return 4030;
    case 31:  // Key 4 Mask
      return 4040;
    case 32:  // DSK 1 Mask
      return 5010;
    case 33:  // DSK 2 Mask
      return 5020;
    case 34:  // Super Source
      return 6000;
    case 35:  // Clean Feed 1
      return 7001;
    case 36:  // Clean Feed 2
      return 7002;
    case 37:  // Auxilary 1
      return 8001;
    case 38:  // Auxilary 2
      return 8002;
    case 39:  // Auxilary 3
      return 8003;
    case 40:  // Auxilary 4
      return 8004;
    case 41:  // Auxilary 5
      return 8005;
    case 42:  // Auxilary 6
      return 8006;
    case 43:  // ME 1 Prog
      return 10010;
    case 44:  // ME 1 Prev
      return 10011;
    case 45:  // ME 2 Prog
      return 10020;
    case 46:  // ME 2 Prev
      return 10021;
    default:
      return 0;
  }
}

/*
 * Translating a index to a audio source
 */
uint16_t ATEMbase::getAudioIndexSrc(uint8_t index)	{
  switch (index) {
    case 0:  // Input 1
      return 1;
    case 1:  // Input 2
      return 2;
    case 2:  // Input 3
      return 3;
    case 3:  // Input 4
      return 4;
    case 4:  // Input 5
      return 5;
    case 5:  // Input 6
      return 6;
    case 6:  // Input 7
      return 7;
    case 7:  // Input 8
      return 8;
    case 8:  // Input 9
      return 9;
    case 9:  // Input 10
      return 10;
    case 10:  // Input 11
      return 11;
    case 11:  // Input 12
      return 12;
    case 12:  // Input 13
      return 13;
    case 13:  // Input 14
      return 14;
    case 14:  // Input 15
      return 15;
    case 15:  // Input 16
      return 16;
    case 16:  // Input 17
      return 17;
    case 17:  // Input 18
      return 18;
    case 18:  // Input 19
      return 19;
    case 19:  // Input 20
      return 20;
    case 20:  // XLR
      return 1001;
    case 21:  // AES/EBU
      return 1101;
    case 22:  // RCA
      return 1201;
    case 23:  // MP1
      return 2001;
    case 24:  // MP2
      return 2002;
    default:
      return 0;
  }
}

uint8_t ATEMbase::maxAtemSeriesVideoInputs()	{
	return 47;	// For the largest ATEM switcher, this is the number of video inputs. The max "index" number from the list above
}


void ATEMbase::commandBundleStart()	{
	resetCommandBundle();
	_wipeCleanPacketBuffer();
	_cBundle = true;
}
void ATEMbase::commandBundleEnd()	{
	if (_cBundle && _returnPacketLength > 0)	{

  	  _createCommandHeader(ATEM_headerCmd_AckRequest, _returnPacketLength);
  	  _sendPacketBuffer(_returnPacketLength);
  	  _returnPacketLength = 0;
	}
	resetCommandBundle();
}
void ATEMbase::resetCommandBundle()	{
	_cBundle = false;
	_cBBO = 0;
}