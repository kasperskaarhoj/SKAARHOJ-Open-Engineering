/*
Copyright 2013 Kasper Skårhøj, SKAARHOJ, kasperskaarhoj@gmail.com

This file is part of the ClientGrassValleyKayak library for Arduino

The ClientGrassValleyKayak library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ClientGrassValleyKayak library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ClientGrassValleyKayak library. If not, see http://www.gnu.org/licenses/.

*/

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "ClientGrassValleyKayak.h"


/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ClientGrassValleyKayak::ClientGrassValleyKayak(){}

/**
 * Setting up IP address for the switcher
 */
void ClientGrassValleyKayak::begin(const IPAddress ip){

	// Set up Udp communication object for commands:
	EthernetUDP Udp;
	_Udp = Udp;
	
	_switcherIP = ip;	// Set switcher IP address
	_commandPort = 56629;	// Set command port
	
	_serialOutput = false;

	_Udp.begin(_commandPort);
}

/**
 */
void ClientGrassValleyKayak::connect() {
}

/**
 */
void ClientGrassValleyKayak::runLoop() {
}

/**
 */
bool ClientGrassValleyKayak::isConnectionTimedOut()	{
}

/**
 */
void ClientGrassValleyKayak::_parsePacket(uint16_t packetLength)	{
}

/**
 */
void ClientGrassValleyKayak::_sendAnswerPacket(uint16_t remotePacketID)  {
}




/********************************
 *
 * General Getter/Setter methods
 *
 ********************************/


/**
 * Setter method: If _serialOutput is set, the library may use Serial.print() to give away information about its operation - mostly for debugging.
 */
void ClientGrassValleyKayak::serialOutput(boolean serialOutput) {
	_serialOutput = serialOutput;
}

void ClientGrassValleyKayak::_sendCommand(uint8_t len)	{
  _Udp.beginPacket(_switcherIP, _commandPort);
  _Udp.write(_packetBuffer,len);
  _Udp.endPacket();
}


/********************************
 *
 * ClientGrassValleyKayak Switcher state methods
 * Returns the most recent information we've 
 * got about the switchers state
 *
 ********************************/

uint8_t ClientGrassValleyKayak::getProgramInput() {
//	return _ClientGrassValleyKayak_PrgI;
}
uint8_t ClientGrassValleyKayak::getPreviewInput() {
//	return _ClientGrassValleyKayak_PrvI;
}
boolean ClientGrassValleyKayak::getProgramTally(uint8_t inputNumber) {
  	// TODO: Validate that input number exists on current model! <8 at the moment.
//	return (_ClientGrassValleyKayak_TlIn[inputNumber-1] & 1) >0 ? true : false;
}
boolean ClientGrassValleyKayak::getPreviewTally(uint8_t inputNumber) {
  	// TODO: Validate that input number exists on current model! 1-8 at the moment.
//	return (_ClientGrassValleyKayak_TlIn[inputNumber-1] & 2) >0 ? true : false;
}
uint16_t ClientGrassValleyKayak::getTransitionPosition() {
//	return _ClientGrassValleyKayak_TrPs_position;
}


/********************************
 *
 * ClientGrassValleyKayak Switcher Change methods
 * Asks the switcher to changes something
 *
 ********************************/

void ClientGrassValleyKayak::changeProgramInput(uint8_t inputNumber)  {	// inputNumber starting from 1

	_packetBuffer[0] = 0x06;
	_packetBuffer[1] = 0x00;
	_packetBuffer[2] = 0x25;
	_packetBuffer[3] = 0x00;
	_packetBuffer[4] = 0x01;
	_packetBuffer[5] = 0x01;
	_packetBuffer[6] = inputNumber;
	_packetBuffer[7] = 0x01;

	_sendCommand(8);	
}
void ClientGrassValleyKayak::changePresetInput(uint8_t inputNumber)  {	// inputNumber starting from 1

	_packetBuffer[0] = 0x06;
	_packetBuffer[1] = 0x00;
	_packetBuffer[2] = 0x25;
	_packetBuffer[3] = 0x00;
	_packetBuffer[4] = 0x01;
	_packetBuffer[5] = 0x02;
	_packetBuffer[6] = inputNumber;
	_packetBuffer[7] = 0x01;

	_sendCommand(8);	
}
void ClientGrassValleyKayak::doCut()	{

	_packetBuffer[0] = 0x05;
	_packetBuffer[1] = 0x00;
	_packetBuffer[2] = 0xe4;
	_packetBuffer[3] = 0x00;
	_packetBuffer[4] = 0x08;
	_packetBuffer[5] = 0x06;
	_packetBuffer[6] = 0x00;

	_sendCommand(7);	
}
void ClientGrassValleyKayak::doAuto()	{

	_packetBuffer[0] = 0x05;
	_packetBuffer[1] = 0x00;
	_packetBuffer[2] = 0xe4;
	_packetBuffer[3] = 0x00;
	_packetBuffer[4] = 0x08;
	_packetBuffer[5] = 0x05;
	_packetBuffer[6] = 0x00;

	_sendCommand(7);	
}
void ClientGrassValleyKayak::changeTransitionPosition(unsigned int value)	{	// Range: 0-1000

	// Normalize from 0-1000 to 0-32767:
    unsigned int transitionValue = (unsigned long)(value * 32768 / 1000);	
    if (transitionValue> 0x7FFF)    {
      transitionValue = 0x7FFF;
    }

	_packetBuffer[0] = 0x06;
	_packetBuffer[1] = 0x00;
	_packetBuffer[2] = 0xa5;
	_packetBuffer[3] = 0x00;
	_packetBuffer[4] = 0x08;
	_packetBuffer[5] = 0x0b;
	_packetBuffer[6] = transitionValue & 0xFF;
	_packetBuffer[7] = (transitionValue >> 8) & 0x7F;

	Serial.println((_packetBuffer[7] << 8)+_packetBuffer[6]);

	_sendCommand(8);
}
