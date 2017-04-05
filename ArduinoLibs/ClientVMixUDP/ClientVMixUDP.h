/*
Copyright 2016 Kasper Skårhøj, SKAARHOJ K/S, kasper@skaarhoj.com

This file is part of the vMix UDP Client library for Arduino

The vMix UDP Client library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The vMix UDP Client library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the vMix UDP Client library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/

#ifndef ClientVMix_h
#define ClientVMix_h

#include "Arduino.h"

#ifdef ESP8266
#include <WifiUDP.h>
#else
#include <EthernetUdp.h>
#endif

#include <SkaarhojPgmspace.h>

#define VMIX_headerCmd_AckRequest 0x1 // Please acknowledge reception of this package...
#define VMIX_headerCmd_HelloPacket 0x2
#define VMIX_headerCmd_Resend 0x4           // This is a resent information
#define VMIX_headerCmd_RequestNextAfter 0x8 // I'm requesting you to resend something to me.
#define VMIX_headerCmd_Ack 0x10             // This package is an acknowledge to package id (byte 4-5) VMIX_headerCmd_AckRequest

#define VMIX_maxInitPackageCount 40 // The maximum number of initialization packages. By observation on a 2M/E 4K can be up to (not fixed!) 32. We allocate a few more then...
#define VMIX_packetBufferLength 96  // Size of packet buffer

#define VMIX_debug 1 // If "1" (true), more debugging information may hit the serial monitor, in particular when _serialDebug = 0x80. Setting this to "0" is recommended for production environments since it saves on flash memory.

class ClientVMixUDP {
protected:
#ifdef ESP8266
  WiFiUDP _Udp;
#else
  EthernetUDP _Udp; // UDP object for communication, see constructor.
#endif
  IPAddress _switcherIP; // IP address of the switcher
  uint8_t _serialOutput; // If set, the library will print status/debug information to the Serial object

  // VMIX Connection Basics
  uint16_t _localPacketIdCounter; // This is our counter for the command packages we might like to send to vMix
  boolean _hasInitialized;        // If true, all initial payload packets has been received during requests for resent - and we are completely ready to rock!
  boolean _isConnected;           // Set true if we have received a hello package from the switcher.
  boolean _initPayloadSent;
  unsigned long _lastContact;     // Last time (millis) the switcher sent a packet to us.
  uint16_t _lastRemotePacketID;   // The most recent Remote Packet Id from switcher
  uint8_t _returnPacketLength;
  uint16_t _sessionID;

  uint8_t _packetBuffer[VMIX_packetBufferLength]; // Buffer for storing segments of the packets from VMIX and creating answer packets.

  uint16_t _cmdLength;  // Used when parsing packets
  uint16_t _cmdPointer; // Used when parsing packets

  bool _cBundle; // If set, we are building a set-command bundle.
  uint8_t _cBBO; // Bundle Buffer Offset; This is an offset if you want to add more commands.

public:
  ClientVMixUDP();
  void begin(const IPAddress ip);
  void connect();
  void runLoop(uint16_t delayTime = 0);

  uint16_t getVMIX_lastRemotePacketId();

  bool isConnected();
  bool hasInitialized();

  void serialOutput(uint8_t level);
  bool hasTimedOut(unsigned long time, unsigned long timeout);

  void commandBundleStart();
  void commandBundleEnd();
  void resetCommandBundle();

protected:
  void _createCommandHeader(const uint8_t headerCmd, const uint16_t lengthOfData, const uint16_t remotePacketID = 0);
  void _sendPacketBuffer(uint8_t length);
  void _wipeCleanPacketBuffer();

  void _parsePacket(uint16_t packetLength);
  bool _readToPacketBuffer();
  bool _readToPacketBuffer(uint8_t maxBytes);
  void _prepareCommandPacket(const char *cmdString, uint8_t cmdBytes, bool indexMatch = true);
  void _finishCommandPacket();

private:
	bool neverConnected;




// *********************************
// **
// ** Implementations in ClientVMixUDP.h:
// **
// *********************************

private:
	void _parseGetCommands(const char *cmdStr);
	
			// Private Variables in ClientVMixUDP.h:
	
			uint8_t vmixActiveInput;
			uint8_t vmixPreviewInput;
			bool vmixFadeToBlackActive;
			bool vmixOverlayActive[6];
			uint8_t vmixOverlayInput[6];
			bool vmixRecordActive;
			bool vmixStreamActive;
			int vmixAudioLevelsLeft[100];
			int vmixAudioLevelsRight[100];
			int vmixMasterAudioLevelsLeft;
			int vmixMasterAudioLevelsRight;
			uint16_t vmixInputPosition[100];
			int vmixXamlValue[100][11];
			char vmixInputPropertiesLongName[100][21];
			char vmixInputPropertiesShortName[100][5];
			bool vmixInputPropertiesLoop[100];
			uint8_t vmixInputPropertiesType[100];
			uint8_t vmixInputPropertiesState[100];
			uint16_t vmixInputPropertiesDuration[100];
			bool vmixAudioPropertiesSolo[100];
			int vmixAudioPropertiesBalance[100];
			int vmixAudioPropertiesVolume[100];
			bool vmixAudioPropertiesMuted[100];
			uint8_t vmixAudioPropertiesBusses[100];
			int vmixMasterAudioPropertiesVolume;
			int vmixMasterAudioPropertiesHeadphoneVolume;
			bool vmixMasterAudioPropertiesMuted;
			uint16_t vmixvMixVersionA;
			uint16_t vmixvMixVersionB;
			uint16_t vmixvMixVersionC;
			uint16_t vmixvMixVersionD;
			uint8_t vmixTopologyInputs;
	
public:
			// Public Methods in ClientVMixUDP.h:
	
			uint8_t getActiveInput();
			void setActiveInput(uint8_t input);
			void setActiveInputTransitionDuration(uint8_t input, uint8_t transition, uint16_t duration);
			uint8_t getPreviewInput();
			void setPreviewInput(uint8_t input);
			void performCutAction(bool action);
			void performFadeFader(uint8_t fader);
			void performFadeToBlackAction(bool action);
			bool getFadeToBlackActive();
			void setTransitionPosition(uint8_t position);
			bool getOverlayActive(uint8_t overlay);
			uint8_t getOverlayInput(uint8_t overlay);
			void setOverlayInputOn(uint8_t overlay, uint8_t inputOn);
			void setOverlayInputOff(uint8_t overlay, bool inputOff);
			void setOverlayInputInstantOff(uint8_t overlay, bool inputInstantOff);
			void setOverlayInputZoom(uint8_t overlay, uint8_t inputZoom);
			void setOverlayInputPreview(uint8_t overlay, uint8_t inputPreview);
			bool getRecordActive();
			void performRecordAction(bool action);
			bool getStreamActive();
			void performStreamAction(bool action);
			void setAudioLevelsEnable(bool enable);
			int getAudioLevelsLeft(uint8_t input);
			int getAudioLevelsRight(uint8_t input);
			int getMasterAudioLevelsLeft();
			int getMasterAudioLevelsRight();
			uint16_t getInputPosition(uint8_t input);
			void setInputPositionPosition(uint8_t input, uint16_t position);
			void setInputPositionEnable(bool enable);
			void performReplayLastevent(bool lastevent);
			void performReplaySelectedevent(bool selectedevent);
			void setReplayPropertiesSeconds(uint16_t seconds);
			void setReplayPropertiesSpeed(uint16_t speed);
			void setReplayPropertiesEvents(uint8_t events);
			void setReplayPropertiesJump(uint8_t jump);
			void setReplayEventsSettingsMoveTo(uint8_t moveTo);
			void setReplayEventsSettingsCopyTo(uint8_t copyTo);
			void setReplayEventsSettingsNext(uint8_t next);
			void setReplayEventsSettingsPrevious(uint8_t previous);
			void setReplayEventsSettingsTogglecamera(uint8_t togglecamera);
			void setReplayEventsSettingsTogglelive(bool togglelive);
			void setReplayEventsSettingsToggledirection(bool toggledirection);
			int getXamlValue(uint8_t input, uint8_t index);
			void setXamlValue(uint8_t input, uint8_t index, int value);
			void setCountDownMode(uint8_t input, uint8_t index, uint8_t mode);
			char *  getInputPropertiesLongName(uint8_t input);
			char *  getInputPropertiesShortName(uint8_t input);
			bool getInputPropertiesLoop(uint8_t input);
			uint8_t getInputPropertiesType(uint8_t input);
			uint8_t getInputPropertiesState(uint8_t input);
			uint16_t getInputPropertiesDuration(uint8_t input);
			void setInputPropertiesLoop(uint8_t input, bool loop);
			void setInputPropertiesPlayPause(uint8_t input, bool playPause);
			bool getAudioPropertiesSolo(uint8_t input);
			int getAudioPropertiesBalance(uint8_t input);
			int getAudioPropertiesVolume(uint8_t input);
			bool getAudioPropertiesMuted(uint8_t input);
			uint8_t getAudioPropertiesBusses(uint8_t input);
			void setAudioPropertiesSolo(uint8_t input, bool solo);
			void setAudioPropertiesBalance(uint8_t input, int balance);
			void setAudioPropertiesVolume(uint8_t input, int volume);
			void setAudioPropertiesMuted(uint8_t input, bool muted);
			void setAudioPropertiesBusses(uint8_t input, uint8_t busses);
			int getMasterAudioPropertiesVolume();
			int getMasterAudioPropertiesHeadphoneVolume();
			bool getMasterAudioPropertiesMuted();
			void setMasterAudioPropertiesVolume(int volume);
			void setMasterAudioPropertiesHeadphoneVolume(int headphoneVolume);
			void setMasterAudioPropertiesMuted(bool muted);
			uint16_t getvMixVersionA();
			uint16_t getvMixVersionB();
			uint16_t getvMixVersionC();
			uint16_t getvMixVersionD();
			uint8_t getTopologyInputs();
};









#endif
