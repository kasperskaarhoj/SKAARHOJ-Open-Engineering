/*
Copyright 2012 Kasper Skårhøj, SKAARHOJ, kasperskaarhoj@gmail.com

This file is part of the ATEM library for Arduino

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

*/


/**
  Version 1.0.2
**/


#ifndef ATEM_h
#define ATEM_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif


#include <EthernetUdp.h>

#ifndef __arm__
	#include <avr/pgmspace.h>
#endif

class ATEM
{
  private:
	EthernetUDP _Udp;			// Udp Object for communication, see constructor.
	uint16_t _localPort; 		// local port to send from
	IPAddress _switcherIP;		// IP address of the switcher
	boolean _serialOutput;		// If set, the library will print status/debug information to the Serial object

	uint16_t _sessionID;					// Used internally for storing packet size during communication
	uint16_t _lastRemotePacketID;		// The most recent Remote Packet Id from switcher
	uint8_t _packetBuffer[96];   			// Buffer for storing segments of the packets from ATEM and creating answer packets.
	uint16_t _cmdLength;					// Used when parsing packets
	uint16_t _cmdPointer;					// Used when parsing packets

	uint16_t _localPacketIdCounter;  	// This is our counter for the command packages we might like to send to ATEM
	boolean _hasInitialized;  			// If true, the initial reception of the ATEM memory has passed and we can begin to respond during the runLoop()
	unsigned long _lastContact;			// Last time (millis) the switcher sent a packet to us.
	unsigned long _isConnectingTime;	// Set to millis() after the connect() function was called - and it will force runLoop() to finish the connection session.

		// Selected ATEM State values. Naming attempts to match the switchers own protocol names
		// Set through _parsePacket() when the switcher sends state information
		// Accessed through getter methods
	uint8_t _ATEM_VidM;		// Video format used: 525i59.94 NTSC (0), 625i50 PAL (1), 720p50 (2), 720p59.94 (3), 1080i50 (4), 1080i59.94 (5)
	uint16_t _ATEM_PrgI;		// Program input
	uint16_t _ATEM_PrvI;		// Preview input
	uint8_t _ATEM_TlIn[16];	// Inputs 1-16, bit 0 = Prg tally, bit 1 = Prv tally. Both can be set simultaneously.
	boolean _ATEM_TrPr;		// Transition Preview: Is it on or not?
	uint8_t _ATEM_TrSS_KeyersOnNextTransition;	// Bit 0: Background; Bit 1-4: Key 1-4
	uint8_t _ATEM_TrSS_TransitionStyle;			// 0=MIX, 1=DIP, 2=WIPE, 3=DVE, 4=STING
	boolean _ATEM_KeOn[4];	// Upstream Keyer 1-4 On state
	boolean _ATEM_DskOn[2];	// Downstream Keyer 1-2 On state
	boolean _ATEM_DskTie[2];	// Downstream Keyer Tie 1-2 On state
	uint8_t _ATEM_TrPs_frameCount;	// Count down of frames in case of a transition (manual or auto)
	uint16_t _ATEM_TrPs_position;	// Position from 0-1000 of the current transition in progress
	boolean _ATEM_FtbS_state;       // State of Fade To Black, 0 = off and 1 = activated
	uint8_t _ATEM_FtbS_frameCount;	// Count down of frames in case of fade-to-black
	uint8_t	_ATEM_FtbP_time;		// Transition time for Fade-to-black
	uint8_t	_ATEM_TMxP_time;		// Transition time for Mix Transitions
	uint16_t _ATEM_AuxS[3];	// Aux Outputs 1-3 source
	uint8_t _ATEM_MPType[2];	// Media Player 1/2: Type (1=Clip, 2=Still)
	uint8_t _ATEM_MPStill[2];	// Still number (if MPType==2)
	uint8_t _ATEM_MPClip[2];	// Clip number (if MPType==1)
	uint16_t _ATEM_AMLv[2];	// Audio Meter Levels for a specific channel, see _ATEM_AMLv_channel
	uint8_t _ATEM_AMLv_channel;		// The channel to read audio levels for.
	uint8_t _ATEM_AudioChannelMode[16];	// Audio channel mode (ON=1, AFV=2, OFF=0/other)
	uint8_t _ATEM_AudioChannelModeSpc[4];	// Audio channel mode for MP1, MP2, XLR and RCA
	
	
  public:
	char _ATEM_pin[17];		// String holding the id of the mixer, for instance "ATEM 1 M/E Produ"
	uint8_t	_ATEM_ver_m;	// Firmware version, "left of decimal point" (what is that called anyway?)
	uint8_t	_ATEM_ver_l;	// Firmware version, decimals ("right of decimal point")

    ATEM();
    ATEM(const IPAddress ip, const uint16_t localPort);
    void begin(const IPAddress ip, const uint16_t localPort);
    void connect();
    void runLoop();
	bool isConnectionTimedOut();
	void delay(const unsigned int delayTimeMillis);
	uint16_t getSessionID();

  private:
	void _parsePacket(uint16_t packetLength);
	bool _readToPacketBuffer();
	bool _readToPacketBuffer(uint8_t maxBytes);
	void _sendAnswerPacket(uint16_t remotePacketID);
	void _sendCommandPacket(const char cmd[4], uint8_t commandBytes[16], uint8_t cmdBytes);
	void _wipeCleanPacketBuffer();
	void _sendPacketBufferCmdData(const char cmd[4], uint8_t cmdBytes);

  public:

/********************************
 * General Getter/Setter methods
 ********************************/
  	void serialOutput(boolean serialOutput);
	bool hasInitialized();
	uint16_t getATEM_lastRemotePacketId();
	uint8_t getATEMmodel();

/********************************
* ATEM Switcher state methods
* Returns the most recent information we've 
* got about the switchers state
 ********************************/
	uint16_t getProgramInput();
	uint16_t getPreviewInput();
	boolean getProgramTally(uint8_t inputNumber);
	boolean getPreviewTally(uint8_t inputNumber);
	boolean getUpstreamKeyerStatus(uint8_t inputNumber);
	boolean getUpstreamKeyerOnNextTransitionStatus(uint8_t inputNumber);
	boolean getDownstreamKeyerStatus(uint8_t inputNumber);
	uint16_t getTransitionPosition();
	bool getTransitionPreview();
	uint8_t getTransitionType();
	uint8_t getTransitionMixTime();
    boolean getFadeToBlackState();
	uint8_t getFadeToBlackFrameCount();
	uint8_t getFadeToBlackTime();
	bool getDownstreamKeyTie(uint8_t keyer);
	uint16_t getAuxState(uint8_t auxOutput);
	uint8_t getMediaPlayerType(uint8_t mediaPlayer);
	uint8_t getMediaPlayerStill(uint8_t mediaPlayer);
	uint8_t getMediaPlayerClip(uint8_t mediaPlayer);
	uint16_t getAudioLevels(uint8_t channel);
	uint8_t getAudioChannelMode(uint16_t channelNumber);

/********************************
 * ATEM Switcher Change methods
 * Asks the switcher to changes something
 ********************************/
	void changeProgramInput(uint16_t inputNumber);
	void changePreviewInput(uint16_t inputNumber);
	void doCut();
	void doAuto();
	void doAuto(uint8_t me);
	void fadeToBlackActivate();
	void changeTransitionPosition(word value);
	void changeTransitionPositionDone();
	void changeTransitionPreview(bool state);
	void changeTransitionType(uint8_t type);
	void changeTransitionMixTime(uint8_t frames);
	void changeFadeToBlackTime(uint8_t frames);
	void changeUpstreamKeyOn(uint8_t keyer, bool state);
	void changeUpstreamKeyNextTransition(uint8_t keyer, bool state);
	void changeDownstreamKeyOn(uint8_t keyer, bool state);
	void changeDownstreamKeyTie(uint8_t keyer, bool state);	
	void doAutoDownstreamKeyer(uint8_t keyer);
	void changeAuxState(uint8_t auxOutput, uint16_t inputNumber);
	void settingsMemorySave();
	void settingsMemoryClear();
	void changeColorValue(uint8_t colorGenerator, uint16_t hue, uint16_t saturation, uint16_t lightness);
	void mediaPlayerSelectSource(uint8_t mediaPlayer, boolean movieclip, uint8_t sourceIndex);
	void mediaPlayerClipStart(uint8_t mediaPlayer);

	void changeSwitcherVideoFormat(uint8_t format);
	void changeDVESettingsTemp(unsigned long Xpos,unsigned long Ypos,unsigned long Xsize,unsigned long Ysize);
	void changeDVEMaskTemp(unsigned long top,unsigned long bottom,unsigned long left,unsigned long right);
	void changeDVEBorder(bool enableBorder);
		
	void changeUpstreamKeyFillSource(uint8_t keyer, uint16_t inputNumber);
	void changeUpstreamKeyBlending(uint8_t keyer, bool preMultipliedAlpha, uint16_t clip, uint16_t gain, bool invKey);	
	void changeDownstreamKeyBlending(uint8_t keyer, bool preMultipliedAlpha, uint16_t clip, uint16_t gain, bool invKey);
	void changeDownstreamKeyFillSource(uint8_t keyer, uint16_t inputNumber);
	void changeDownstreamKeyKeySource(uint8_t keyer, uint16_t inputNumber);
	void changeDVESettingsTemp_RunKeyFrame(uint8_t runType);
	void changeDVESettingsTemp_Rate(uint8_t rateFrames);
	void changeKeyerMask(uint16_t topMask, uint16_t bottomMask, uint16_t leftMask, uint16_t rightMask);
	void changeDownstreamKeyMask(uint8_t keyer, uint16_t topMask, uint16_t bottomMask, uint16_t leftMask, uint16_t rightMask);
	
	void changeAudioChannelMode(uint16_t channelNumber, uint8_t mode);
	void changeAudioChannelVolume(uint16_t channelNumber, uint16_t volume);
	void changeAudioMasterVolume(uint16_t volume);
	void sendAudioLevelNumbers(bool enable);
	void setAudioLevelReadoutChannel(uint8_t AMLv);
	
	bool ver42();
	
	void setWipeReverseDirection(bool reverse);
};

#endif

