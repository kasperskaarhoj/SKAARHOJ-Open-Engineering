/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Blackmagic Design Hyperdeck Studio Client library for Arduino

The ClientBMDHyperdeckStudio library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The ClientBMDHyperdeckStudio library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the ClientBMDHyperdeckStudio library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/

/**
  Version 1.0.0
        (Semantic Versioning)

  ClientBMDHyperdeckStudio Ethernet Protocol:
        - One telnet connection at a time (port 9993)

**/

#ifndef ClientBMDHyperdeckStudio_h
#define ClientBMDHyperdeckStudio_h

#include "Arduino.h"
#include "SkaarhojTCPClient.h"

#include <Ethernet.h>
//  #include "SkaarhojPgmspace.h"  - 23/2 2014

#define ClientBMDHyperdeckStudio_CLIPS 10
#define ClientBMDHyperdeckStudio_CLIPNAMELEN 21

#define ClientBMDHyperdeckStudio_SLOTSTATUS_MOUNTED 1
#define ClientBMDHyperdeckStudio_SLOTSTATUS_EMPTY 2
#define ClientBMDHyperdeckStudio_SLOTSTATUS_MOUNTING 3

#define ClientBMDHyperdeckStudio_AUDIOINPUT_EMBEDDED 1
#define ClientBMDHyperdeckStudio_AUDIOINPUT_XLR 2
#define ClientBMDHyperdeckStudio_AUDIOINPUT_RCA 3

#define ClientBMDHyperdeckStudio_VIDEOINPUT_SDI 1
#define ClientBMDHyperdeckStudio_VIDEOINPUT_HDMI 2
#define ClientBMDHyperdeckStudio_VIDEOINPUT_COMPONENT 3

#define ClientBMDHyperdeckStudio_TRANSPORT_PREVIEW 1
#define ClientBMDHyperdeckStudio_TRANSPORT_STOPPED 2
#define ClientBMDHyperdeckStudio_TRANSPORT_PLAY 3
#define ClientBMDHyperdeckStudio_TRANSPORT_RECORD 4
#define ClientBMDHyperdeckStudio_TRANSPORT_JOG 5
#define ClientBMDHyperdeckStudio_TRANSPORT_FORWARD 6
#define ClientBMDHyperdeckStudio_TRANSPORT_REWIND 7

class ClientBMDHyperdeckStudio : public SkaarhojTCPClient {
private:
  bool _wasRejected; // used to track if we encountered a rejection previously. A part of the connect() disconnect() management.

  int _noInput; // Used to check if there is input or not.
  bool _askForClips;

  // States:
  uint8_t _Hyperdeck_currentSlotId;
  uint8_t _Hyperdeck_slotStatus[2];
  unsigned long _Hyperdeck_slotRecordingTime[2];
  uint8_t _Hyperdeck_transportStatus;
  int _Hyperdeck_playSpeed;
  uint8_t _Hyperdeck_clipId;
  uint8_t _Hyperdeck_playBackSlotId;
  uint8_t _Hyperdeck_tc_hh;
  uint8_t _Hyperdeck_tc_mm;
  uint8_t _Hyperdeck_tc_ss;
  uint8_t _Hyperdeck_tc_ff;
  uint8_t _Hyperdeck_videoFormat;
  uint8_t _Hyperdeck_notifyStatus;
  bool _Hyperdeck_remoteEnabled;
  bool _Hyperdeck_remoteOverride;
  uint8_t _Hyperdeck_audioInput;
  uint8_t _Hyperdeck_videoInput;
  uint8_t _Hyperdeck_fileFormat;
  int8_t _Hyperdeck_lastJogDirection;
  bool _askForClipNames;

  uint8_t _Hyperdeck_clipCount;
  uint8_t _Hyperdeck_fileidlist[ClientBMDHyperdeckStudio_CLIPS];
  char _Hyperdeck_filelist[ClientBMDHyperdeckStudio_CLIPS][ClientBMDHyperdeckStudio_CLIPNAMELEN];
  uint8_t _Hyperdeck_filelen_hh[ClientBMDHyperdeckStudio_CLIPS];
  uint8_t _Hyperdeck_filelen_mm[ClientBMDHyperdeckStudio_CLIPS];
  uint8_t _Hyperdeck_filelen_ss[ClientBMDHyperdeckStudio_CLIPS];
  uint8_t _Hyperdeck_filelen_ff[ClientBMDHyperdeckStudio_CLIPS];

  char _Hyperdeck_currentFile[ClientBMDHyperdeckStudio_CLIPNAMELEN];
  uint8_t _Hyperdeck_currentClipId;

  uint8_t clipCounter;
  uint8_t clipIndex;
  uint8_t statusRequestCycleStep;

public:
  ClientBMDHyperdeckStudio();
  void begin(IPAddress ip);
  void disconnect();

private: // Overloading:
  void _resetDeviceStateVariables();
  void _parseline();
  void _sendStatus();
  void _sendPing();

private:
  void _addToBuffer_tcValue(const uint8_t val);
  void _enableNotifications();
  void _pullStatus(uint8_t step);

public:
  void askForClips(bool askForClips);
  void askForClipNames(bool askForNames);

  // These commands get the internal state of this class. Those states are updated through subscription to updates (the "notify:" command) as well as asking periodically to pull status on all accounts (_pullStatus()). After sending  a command, these values will typically receive an update within a few hundred milliseconds.

  // This function seems unuseful, it doesn't return the active slot
  // Use getPlayBackSlotId() instead
  //uint8_t getCurrentSlotId();
  uint8_t getSlotStatus(uint8_t slotId);
  unsigned long getSlotRecordingTime(uint8_t slotId);
  uint8_t getTransportStatus();
  bool isPlaying();
  bool isRecording();
  bool isStopped();
  bool isInPreview();
  int getPlaySpeed();
  uint8_t getClipId();
  uint8_t getPlayBackSlotId();
  uint8_t getTChh();
  uint8_t getTCmm();
  uint8_t getTCss();
  uint8_t getTCff();
  uint8_t getVideoFormat();
  bool isRemoteEnabled();
  bool isRemoteOverride();
  uint8_t getAudioInput();
  uint8_t getVideoInput();
  uint8_t getFileFormat();
  int getInputStatus();
  uint8_t getTotalClipCount();
  uint8_t getFileClipId(uint8_t index);
  char *getFileName(uint8_t index);
  char *getCurrentFileName();

  // These commands send commands to the device (no matter if the device is ready or not)
  void previewEnable(boolean enable);
  void play();
  void playWithSpeed(int speed);
  void playWithLoop(boolean loop);
  void playSingleClip(boolean single);
  void record();
  void recordWithName(const char *str);
  void stop(); // Pause
  void slotSelect(uint8_t slot);
  void slotSelectVideoFormat(uint8_t format);
  void gotoClipID(uint8_t id);
  void gotoClipStart();
  void gotoClipEnd();
  void gotoTimelineEnd();
  void gotoTimelineStart();
  void gotoTimecode(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t ff);
  void gotoTimecode(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t ff, int relative);
  void goForwardInTimecode(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t ff);
  void goBackwardsInTimecode(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t ff);
  void setRemoteEnable(boolean enable);
  void setRemoteOverride(boolean override);
  void setVideoInput(uint8_t videoInput);
  void setAudioInput(uint8_t audioInput);
  void setFileFormat(uint8_t fileFormat);
};

#endif
