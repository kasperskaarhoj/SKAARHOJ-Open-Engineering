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


/**
  Version 1.0.0
	(Semantic Versioning)
**/

/**
  Connection with DB-9 to SKAARHOJ RS-232 port: Use crossed cable
**/

#ifndef ClientKramerVP728_h
#define ClientKramerVP728_h

#include "Arduino.h"
#include "SkaarhojSerialClient.h"

//  #include "SkaarhojPgmspace.h"  - 23/2 2014









#define ClientKramerVP728_IN_INPUT1 0
#define ClientKramerVP728_IN_INPUT2 1
#define ClientKramerVP728_IN_INPUT3 2
#define ClientKramerVP728_IN_INPUT4 3
#define ClientKramerVP728_IN_VGA1 4
#define ClientKramerVP728_IN_VGA2 5
#define ClientKramerVP728_IN_HDMI1 6
#define ClientKramerVP728_IN_HDMI2 7
#define ClientKramerVP728_IN_USB 8

#define ClientKramerVP728_RES_NATIVEHDMI 0
#define ClientKramerVP728_RES_640x480_60 1
// ... more to come... TODO

#define ClientKramerVP728_ASPECT_BESTFIT 0
#define ClientKramerVP728_ASPECT_LETTERBOX 1
#define ClientKramerVP728_ASPECT_FOLLOWOUTPUT 2
#define ClientKramerVP728_ASPECT_VIRTUALWIDE 3
#define ClientKramerVP728_ASPECT_FOLLOWINPUT 4
#define ClientKramerVP728_ASPECT_CUSTOM 5

#define ClientKramerVP728_HDCP_FOLLOWOUTPUT 0
#define ClientKramerVP728_HDCP_FOLLOWINPUT 0

#define ClientKramerVP728_QUEUESIZE 10
#define ClientKramerVP728_QUEUEENTRYLEN 15



class ClientKramerVP728 : public SkaarhojSerialClient 
{
  private:
	char _queue[ClientKramerVP728_QUEUESIZE][ClientKramerVP728_QUEUEENTRYLEN];
	uint8_t _queueWriteIndex;	// Index in queue, last written
	uint8_t _queueReadIndex;	// Index in queue, last sent
	uint8_t _queueEntryWritePointer; 	// Index to chars in the current queue entry.
  uint8_t i;


	// States:
  uint8_t _VP728_inputSource;
  uint8_t _VP728_brightness;
  uint8_t _VP728_contrast;
  uint8_t _VP728_color;
  uint8_t _VP728_hue;
  uint8_t _VP728_sharpness;
  uint8_t _VP728_gamma;
  uint8_t _VP728_outputRes;
  uint8_t _VP728_aspectRatio;
  uint8_t _VP728_zoom;
  int _VP728_audioInputVolume;
  int _VP728_audioOutputVolume;
  int _VP728_audioBass;
  int _VP728_audioTreble;
  int _VP728_audioBalance;
  int _VP728_audioDelay;
  bool _VP728_freeze;
  bool _VP728_blank;
  bool _VP728_power;
  bool _VP728_mute;
  bool _VP728_lock;
  uint8_t _VP728_inputStatus;
  uint8_t _VP728_HDCPSetting;
  bool _VP728_HDCPInputState[2];
  
  
	  
  public:
    ClientKramerVP728(HardwareSerial &serial);
    void begin();

  private:	// Overloading:
    void _resetDeviceStateVariables();
  	void _parseline();
  	void _sendStatus();
  	void _sendPing();

    void _sendingQueue();
    bool _newQueueEntry();
    void _addStringToQueue(const String command);
    void _addStringToQueue_P(const char *str);
    void _runSubLoop();
	
  public:
   bool setFunction(uint8_t function, int value);
   bool getFunction(uint8_t function);
   void setInputSource(uint8_t input);
   uint8_t getInputSource();
   void setBrightness(uint8_t brightness);
   uint8_t getBrightness();
   void setContrast(uint8_t contrast);
   uint8_t getContrast();
   void setColor(uint8_t color);
   uint8_t getColor();
   void setHue(uint16_t hue);
   uint16_t getHue();
   void setSharpness(uint8_t sharpness);
   uint8_t getSharpness();
   void setGamma(uint8_t gamma);
   uint8_t getGamma();
   void setOutputResolution(uint8_t outputRes);
   uint8_t getOutputResolution();
   void setAspectRatioType(uint8_t aspectRatio);
   uint8_t getAspectRatioType();
   void setAspectRatioFreely(int hpan,int vpan,int hzoom,int vzoom);
   void setZoomType(uint8_t zoom);
   uint8_t getZoomType();
   void setZoomFreely(uint8_t zoomLevel, uint8_t hpan, uint8_t vpan);
   void setAudioInputVolume(int volume);
   int getAudioInputVolume();
   void setAudioOutputVolume(int volume);
   int getAudioOutputVolume();
   void setAudioBass(int bass);
   int getAudioBass();
   void setAudioTreble(int treble);
   int getAudioTreble();
   void setAudioBalance(int balance);
   int getAudioBalance();
   void setAudioDelay(uint16_t delayms);
   int getAudioDelay();
   void recallPreset(uint8_t preset);
   void setFreeze(boolean freeze);
   bool getFreeze();
   void setBlank(boolean blank);
   bool getBlank();
   void setPower(boolean power);
   bool getPower();
   void setMute(boolean mute);
   bool getMute();
   void setLock(boolean lock);
   bool getLock();
   uint8_t getInputStatus();
   void setHDCPSetting(uint8_t HDCPsetting);
   uint8_t getHDCPSetting();
   void setHDCPonInputState(uint8_t input, boolean state);
   bool getHDCPonInputState(uint8_t input);
};

#endif