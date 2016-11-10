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

#include "Arduino.h"
#include "ClientBMDHyperdeckStudio.h"
#include "SkaarhojTCPClient.h"

/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ClientBMDHyperdeckStudio::ClientBMDHyperdeckStudio() {}

/**
 * Overloading method: Setting up IP address for the videohub (and local port to open telnet connection to)
 */
void ClientBMDHyperdeckStudio::begin(IPAddress ip) {

  SkaarhojTCPClient::begin(ip);

  _localPort = 9993; // Set local port

  _wasRejected = false;
  _askForClips = false;
  _askForClipNames = false;

  clipCounter = 0;
  clipIndex = 0;
  statusRequestCycleStep = 0;
}

/**
 * Overloading method: Disconnecting nicely from Hyperdeck Studio.
 */
void ClientBMDHyperdeckStudio::disconnect() {
  if (_serialOutput)
    Serial.println(F("Disconnecting from Hyperdeck Studio..."));

  _resetBuffer();
  _addToBuffer_P(PSTR("quit\n"));
  _sendBuffer();

  _lastReconnectAttempt = millis();
}

/**
 * Resets local device state variables. (Overloading from superclass)
 */
void ClientBMDHyperdeckStudio::_resetDeviceStateVariables() {
  _section = 0;

  _noInput = -1;

  // Resetting device state variables:
  _Hyperdeck_currentSlotId = 1;
  _Hyperdeck_slotStatus[0] = 0;
  _Hyperdeck_slotStatus[1] = 0;
  _Hyperdeck_slotRecordingTime[0] = 0;
  _Hyperdeck_slotRecordingTime[1] = 0;
  _Hyperdeck_transportStatus = 0;
  _Hyperdeck_playSpeed = 0;
  _Hyperdeck_clipId = 0;
  _Hyperdeck_playBackSlotId = 1;
  _Hyperdeck_tc_hh = 0;
  _Hyperdeck_tc_mm = 0;
  _Hyperdeck_tc_ss = 0;
  _Hyperdeck_tc_ff = 0;
  _Hyperdeck_videoFormat = 0;
  _Hyperdeck_notifyStatus = 0;
  _Hyperdeck_remoteEnabled = false;
  _Hyperdeck_remoteOverride = false;
  _Hyperdeck_audioInput = 0;
  _Hyperdeck_videoInput = 0;
  _Hyperdeck_fileFormat = 0;
  _Hyperdeck_lastJogDirection = 0;

  for (uint8_t clipNum = 0; clipNum < ClientBMDHyperdeckStudio_CLIPS; clipNum++) {
    _Hyperdeck_fileidlist[clipNum] = 0;
    memset(_Hyperdeck_filelist[clipNum], 0, ClientBMDHyperdeckStudio_CLIPNAMELEN);
  }

  memset(_Hyperdeck_currentFile, 0, ClientBMDHyperdeckStudio_CLIPNAMELEN);
}

/**
 * Parses a line from client. (Overloading from superclass)
 */
void ClientBMDHyperdeckStudio::_parseline() {
  // ISSUE:
  // If you get a return code "120 connection rejected", it seems to mean that another connection (or a previous, old "hanging" connection!) is still active. After a certain timeout time, this state is usually resolved automatically (within a minute or so.)
  // If you experience the problem above with an old hanging connection from yourself, try to call "disconnect()" immediately after a connection. This has proven to work at some point and was attempted implemented using "_wasRejected" below. However, it didn't turn out to work as well as hoped and it's still frustrating and unclear how to get a more predictable connection established to the HyperDeck.

  if (!strcmp(_buffer, "")) {
    _section = 0;
  } else if (!strcmp_P(_buffer, PSTR("500 connection info:"))) {
    if (_serialOutput) {
      Serial.print(F("Connection to Hyperdeck "));
      Serial.print(_IP);
      Serial.println(F(" confirmed, pulling status"));
    }
    _hasInitialized = true;
    _pullStatus(0); // All...
    setRemoteEnable(true);
    _enableNotifications();
    _lastStatusRequest = millis();
  } else if (!strcmp_P(_buffer, PSTR("120 connection rejected"))) {
    if (_serialOutput)
      Serial.println(F("Rejected."));
    _wasRejected = true;
  } else if(!strcmp_P(_buffer, PSTR("109 out of range"))) {
    if(_Hyperdeck_lastJogDirection > 0) {
      gotoTimelineEnd();
    } else if(_Hyperdeck_lastJogDirection < 0) {
      gotoTimelineStart();
    }
  } else if (!strcmp_P(_buffer, PSTR("110 no input"))) {
    _noInput = 1;
  } else if (!strcmp_P(_buffer, PSTR("202 slot info:")) || !strcmp_P(_buffer, PSTR("502 slot info:"))) {
    _section = 202;
  } else if (!strcmp_P(_buffer, PSTR("205 clips info:"))) {
    _section = 205;
    clipCounter = 0;
    clipIndex = 0;
  } else if (!strcmp_P(_buffer, PSTR("208 transport info:"))) {
    _section = 208;
  } else if(!strcmp_P(_buffer, PSTR("508 transport info:"))) {
    _section = 508;
  } else if (!strcmp_P(_buffer, PSTR("209 notify:"))) {
    _section = 209;
  } else if (!strcmp_P(_buffer, PSTR("210 remote info:")) || !strcmp_P(_buffer, PSTR("510 remote info:"))) {
    _section = 210;
  } else if (!strcmp_P(_buffer, PSTR("211 configuration:")) || !strcmp_P(_buffer, PSTR("511 configuration:"))) {
    _section = 211;
  } else {
    _bufferReadIndex = 0;

    switch (_section) {
    case 202: // Slot info:
      if (isNextPartOfBuffer_P(PSTR("slot id: "))) {
        _Hyperdeck_currentSlotId = parseInt();
        if (_Hyperdeck_currentSlotId < 1 || _Hyperdeck_currentSlotId > 2) {
          _Hyperdeck_currentSlotId = 1;
        } // Safety measure!

        if (_serialOutput > 1)
          Serial.print(F("_Hyperdeck_currentSlotId="));
        if (_serialOutput > 1)
          Serial.println(_Hyperdeck_currentSlotId);
      }

      else if (isNextPartOfBuffer_P(PSTR("status: "))) {
        if (isNextPartOfBuffer_P(PSTR("mounted"))) { // mounted==1,
          _Hyperdeck_slotStatus[_Hyperdeck_currentSlotId - 1] = 1;
        } else if (isNextPartOfBuffer_P(PSTR("empty"))) { // empty==2
          _Hyperdeck_slotStatus[_Hyperdeck_currentSlotId - 1] = 2;
        } else if (isNextPartOfBuffer_P(PSTR("mounting"))) { // mounting==3
          _Hyperdeck_slotStatus[_Hyperdeck_currentSlotId - 1] = 3;
        } else {
          if (_serialOutput > 1)
            Serial.print(F("UNKNOWN status:"));
          if (_serialOutput > 1)
            Serial.println(_buffer + _bufferReadIndex);
          _Hyperdeck_slotStatus[_Hyperdeck_currentSlotId - 1] = 0;
        }
        if (_serialOutput > 1)
          Serial.print(F("_Hyperdeck_slotStatus="));
        if (_serialOutput > 1)
          Serial.println(_Hyperdeck_slotStatus[_Hyperdeck_currentSlotId - 1]);
      }

      else if (isNextPartOfBuffer_P(PSTR("recording time: "))) {
        _Hyperdeck_slotRecordingTime[_Hyperdeck_currentSlotId - 1] = parseInt();
        if (_serialOutput > 1)
          Serial.print(F("_Hyperdeck_slotRecordingTime="));
        if (_serialOutput > 1)
          Serial.println(_Hyperdeck_slotRecordingTime[_Hyperdeck_currentSlotId - 1]);
      }

      else if (isNextPartOfBuffer_P(PSTR("volume name: "))) {
        // Not implemented
      }

      else {
        if (_serialOutput > 1)
          Serial.print(F("UNKNOWN type:"));
        if (_serialOutput > 1)
          Serial.println(_buffer + _bufferReadIndex);
      }
      break;
    case 205:
      if (isNextPartOfBuffer_P(PSTR("clip count: "))) {
        _Hyperdeck_clipCount = parseInt();
      } else {
        clipCounter++;

        // Saving the current file name separately
        uint8_t fileIndex = parseInt();
        if(fileIndex == getClipId()) {
          uint8_t nameLen = strchr(_buffer + _bufferReadIndex + 2, ' ') - (_buffer + _bufferReadIndex + 2); // Substrating two pointer
          strncpy(_Hyperdeck_currentFile, _buffer + _bufferReadIndex + 2, nameLen < (ClientBMDHyperdeckStudio_CLIPNAMELEN - 1) ? nameLen : (ClientBMDHyperdeckStudio_CLIPNAMELEN - 1));
          _Hyperdeck_currentClipId = fileIndex;
        }

        if (clipCounter > (int)(_Hyperdeck_clipCount - ClientBMDHyperdeckStudio_CLIPS) && clipIndex < ClientBMDHyperdeckStudio_CLIPS) { // Last comparison is a security check...
          _Hyperdeck_fileidlist[clipIndex] = fileIndex;
          uint8_t nameLen = strchr(_buffer + _bufferReadIndex + 2, ' ') - (_buffer + _bufferReadIndex + 2); // Substrating two pointer
          strncpy(_Hyperdeck_filelist[clipIndex], _buffer + _bufferReadIndex + 2, nameLen < (ClientBMDHyperdeckStudio_CLIPNAMELEN - 1) ? nameLen : (ClientBMDHyperdeckStudio_CLIPNAMELEN - 1));

          _bufferReadIndex += 2 + nameLen + 8 + 3 + 1 + 1;
          _Hyperdeck_filelen_hh[clipIndex] = parseInt();
          _bufferReadIndex++;
          _Hyperdeck_filelen_mm[clipIndex] = parseInt();
          _bufferReadIndex++;
          _Hyperdeck_filelen_ss[clipIndex] = parseInt();
          _bufferReadIndex++;
          _Hyperdeck_filelen_ff[clipIndex] = parseInt();

          clipIndex++;
        }
      }
      break;
    case 208: // Transport info:
    case 508:
      if (isNextPartOfBuffer_P(PSTR("status: "))) {
        if (isNextPartOfBuffer_P(PSTR("preview"))) { // preview==1
          _Hyperdeck_transportStatus = ClientBMDHyperdeckStudio_TRANSPORT_PREVIEW;
        } else if (isNextPartOfBuffer_P(PSTR("stopped"))) { // stopped==2
          _Hyperdeck_transportStatus = ClientBMDHyperdeckStudio_TRANSPORT_STOPPED;
        } else if (isNextPartOfBuffer_P(PSTR("play"))) { // play==3
          _Hyperdeck_transportStatus = ClientBMDHyperdeckStudio_TRANSPORT_PLAY;
        } else if (isNextPartOfBuffer_P(PSTR("record"))) { // record==4
          _Hyperdeck_transportStatus = ClientBMDHyperdeckStudio_TRANSPORT_RECORD;
          _noInput = 0;
        } else if (isNextPartOfBuffer_P(PSTR("jog"))) { // jog==5
          _Hyperdeck_transportStatus = ClientBMDHyperdeckStudio_TRANSPORT_JOG;
        } else if (isNextPartOfBuffer_P(PSTR("forward"))) { // forward==6
          _Hyperdeck_transportStatus = ClientBMDHyperdeckStudio_TRANSPORT_FORWARD;
        } else if (isNextPartOfBuffer_P(PSTR("rewind"))) { // rewind==7
          _Hyperdeck_transportStatus = ClientBMDHyperdeckStudio_TRANSPORT_REWIND;
        } else {
          if (_serialOutput > 1)
            Serial.print(F("UNKNOWN status:"));
          if (_serialOutput > 1)
            Serial.println(_buffer + _bufferReadIndex);
          _Hyperdeck_transportStatus = 0;
        }
        if (_serialOutput > 1)
          Serial.print(F("_Hyperdeck_transportStatus="));
        if (_serialOutput > 1)
          Serial.println(_Hyperdeck_transportStatus);
      }

      else if (isNextPartOfBuffer_P(PSTR("speed: "))) {
        _Hyperdeck_playSpeed = parseInt();
        if (_serialOutput > 1)
          Serial.print(F("_Hyperdeck_playSpeed="));
        if (_serialOutput > 1)
          Serial.println(_Hyperdeck_playSpeed);
      }

      else if (isNextPartOfBuffer_P(PSTR("slot id: ")) || isNextPartOfBuffer_P(PSTR("active slot: "))) {
        _Hyperdeck_playBackSlotId = parseInt();
        if (_serialOutput > 1)
          Serial.print(F("_Hyperdeck_playBackSlotId="));
        if (_serialOutput > 1)
          Serial.println(_Hyperdeck_playBackSlotId);
      }

      else if (isNextPartOfBuffer_P(PSTR("active slot: "))) {
        // Not implemented
      }

      else if (isNextPartOfBuffer_P(PSTR("clip id: "))) {
        if(isNextPartOfBuffer_P(PSTR("none"))) {
          _Hyperdeck_clipId = 255;
          // Reset current file name
          memset(_Hyperdeck_currentFile, 0, ClientBMDHyperdeckStudio_CLIPNAMELEN);
        } else {
          _Hyperdeck_clipId = parseInt();
          if(_section == 508 && _askForClipNames) {
            _pullStatus(2);
          }
        }

        if (_serialOutput > 1)
          Serial.print(F("_Hyperdeck_clipId="));
        if (_serialOutput > 1)
          Serial.println(_Hyperdeck_clipId);
      }

      else if (isNextPartOfBuffer_P(PSTR("display timecode: "))) {
        // Not implemented
      }

      else if (isNextPartOfBuffer_P(PSTR("timecode: "))) {
        _Hyperdeck_tc_hh = parseInt();
        _bufferReadIndex++;
        _Hyperdeck_tc_mm = parseInt();
        _bufferReadIndex++;
        _Hyperdeck_tc_ss = parseInt();
        _bufferReadIndex++;
        _Hyperdeck_tc_ff = parseInt();

        if (_serialOutput > 1) {
          Serial.print(F("_Hyperdeck_tc_hh="));
          Serial.println(_Hyperdeck_tc_hh);
          Serial.print(F("_Hyperdeck_tc_mm="));
          Serial.println(_Hyperdeck_tc_mm);
          Serial.print(F("_Hyperdeck_tc_ss="));
          Serial.println(_Hyperdeck_tc_ss);
          Serial.print(F("_Hyperdeck_tc_ff="));
          Serial.println(_Hyperdeck_tc_ff);
        }
      }

      else if (isNextPartOfBuffer_P(PSTR("video format: "))) {
        if (isNextPartOfBuffer_P(PSTR("NTSC"))) {
          _Hyperdeck_videoFormat = 1;
        } else if (isNextPartOfBuffer_P(PSTR("PAL"))) {
          _Hyperdeck_videoFormat = 2;
        } else if (isNextPartOfBuffer_P(PSTR("720p50"))) {
          _Hyperdeck_videoFormat = 3;
        } else if (isNextPartOfBuffer_P(PSTR("720p5994"))) {
          _Hyperdeck_videoFormat = 4;
        } else if (isNextPartOfBuffer_P(PSTR("1080i50"))) {
          _Hyperdeck_videoFormat = 5;
        } else if (isNextPartOfBuffer_P(PSTR("1080i5994"))) {
          _Hyperdeck_videoFormat = 6;
        } else if (isNextPartOfBuffer_P(PSTR("1080p2338"))) {
          _Hyperdeck_videoFormat = 7;
        } else if (isNextPartOfBuffer_P(PSTR("1080p24"))) {
          _Hyperdeck_videoFormat = 8;
        } else if (isNextPartOfBuffer_P(PSTR("1080p25"))) {
          _Hyperdeck_videoFormat = 9;
        } else if (isNextPartOfBuffer_P(PSTR("1080p2997"))) {
          _Hyperdeck_videoFormat = 10;
        } else if (isNextPartOfBuffer_P(PSTR("1080p50"))) {
          _Hyperdeck_videoFormat = 11;
        } else if (isNextPartOfBuffer_P(PSTR("1080p5994"))) {
          _Hyperdeck_videoFormat = 12;
        } else {
          if (_serialOutput > 1)
            Serial.print(F("UNKNOWN video format:"));
          if (_serialOutput > 1)
            Serial.println(_buffer + _bufferReadIndex);
          _Hyperdeck_videoFormat = 0; // Unknown
        }
        if (_serialOutput > 1)
          Serial.print(F("_Hyperdeck_videoFormat="));
        if (_serialOutput > 1)
          Serial.println(_Hyperdeck_videoFormat);
      }

      else {
        if (_serialOutput > 1)
          Serial.print(F("UNKNOWN type:"));
        if (_serialOutput > 1)
          Serial.println(_buffer + _bufferReadIndex);
      }
      break;
    case 209: // Notify:
      if (isNextPartOfBuffer_P(PSTR("transport: "))) {
        if (isNextPartOfBuffer_P(PSTR("true"))) {
          _Hyperdeck_notifyStatus |= B0001;
        } else {
          _Hyperdeck_notifyStatus &= B1110;
        }
      } else if (isNextPartOfBuffer_P(PSTR("slot: "))) {
        if (isNextPartOfBuffer_P(PSTR("true"))) {
          _Hyperdeck_notifyStatus |= B0010;
        } else {
          _Hyperdeck_notifyStatus &= B1101;
        }
      } else if (isNextPartOfBuffer_P(PSTR("remote: "))) {
        if (isNextPartOfBuffer_P(PSTR("true"))) {
          _Hyperdeck_notifyStatus |= B0100;
        } else {
          _Hyperdeck_notifyStatus &= B1011;
        }
      } else if (isNextPartOfBuffer_P(PSTR("configuration: "))) {
        if (isNextPartOfBuffer_P(PSTR("true"))) {
          _Hyperdeck_notifyStatus |= B1000;
        } else {
          _Hyperdeck_notifyStatus &= B0111;
        }
      }

      else {
        if (_serialOutput > 1)
          Serial.print(F("UNKNOWN type:"));
        if (_serialOutput > 1)
          Serial.println(_buffer + _bufferReadIndex);
      }

      if (_serialOutput > 1)
        Serial.print(F("_Hyperdeck_notifyStatus="));
      if (_serialOutput > 1)
        Serial.println(_Hyperdeck_notifyStatus);
      break;
    case 210: // Remote:
      if (isNextPartOfBuffer_P(PSTR("enabled: "))) {
        _Hyperdeck_remoteEnabled = isNextPartOfBuffer_P(PSTR("true"));
        if (_serialOutput > 1)
          Serial.print(F("_Hyperdeck_remoteEnabled="));
        if (_serialOutput > 1)
          Serial.println(_Hyperdeck_remoteEnabled);
      } else if (isNextPartOfBuffer_P(PSTR("override: "))) {
        _Hyperdeck_remoteOverride = isNextPartOfBuffer_P(PSTR("true"));
        if (_serialOutput > 1)
          Serial.print(F("_Hyperdeck_remoteOverride="));
        if (_serialOutput > 1)
          Serial.println(_Hyperdeck_remoteOverride);
      }

      else {
        if (_serialOutput > 1)
          Serial.print(F("UNKNOWN type:"));
        if (_serialOutput > 1)
          Serial.println(_buffer + _bufferReadIndex);
      }
      break;
    case 211: // Configuration:
      if (isNextPartOfBuffer_P(PSTR("audio input: "))) {
        if (isNextPartOfBuffer_P(PSTR("embedded"))) { // embedded==1
          _Hyperdeck_audioInput = ClientBMDHyperdeckStudio_AUDIOINPUT_EMBEDDED;
        } else if (isNextPartOfBuffer_P(PSTR("XLR"))) { // XLR==2
          _Hyperdeck_audioInput = ClientBMDHyperdeckStudio_AUDIOINPUT_XLR;
        } else if (isNextPartOfBuffer_P(PSTR("RCA"))) { // RCA==3
          _Hyperdeck_audioInput = ClientBMDHyperdeckStudio_AUDIOINPUT_RCA;
        } else {
          if (_serialOutput > 1)
            Serial.print(F("UNKNOWN audio format:"));
          if (_serialOutput > 1)
            Serial.println(_buffer + _bufferReadIndex);
          _Hyperdeck_audioInput = 0; // Unknown
        }
        if (_serialOutput > 1)
          Serial.print(F("_Hyperdeck_audioInput="));
        if (_serialOutput > 1)
          Serial.println(_Hyperdeck_audioInput);
      } else if (isNextPartOfBuffer_P(PSTR("video input: "))) {
        if (isNextPartOfBuffer_P(PSTR("SDI"))) { // SDI==1
          _Hyperdeck_videoInput = ClientBMDHyperdeckStudio_VIDEOINPUT_SDI;
        } else if (isNextPartOfBuffer_P(PSTR("HDMI"))) { // HDMI==2
          _Hyperdeck_videoInput = ClientBMDHyperdeckStudio_VIDEOINPUT_HDMI;
        } else if (isNextPartOfBuffer_P(PSTR("component"))) { // component==3
          _Hyperdeck_videoInput = ClientBMDHyperdeckStudio_VIDEOINPUT_COMPONENT;
        } else {
          if (_serialOutput > 1)
            Serial.print(F("UNKNOWN video format:"));
          if (_serialOutput > 1)
            Serial.println(_buffer + _bufferReadIndex);
          _Hyperdeck_videoInput = 0; // Unknown
        }
        if (_serialOutput > 1)
          Serial.print(F("_Hyperdeck_videoInput="));
        if (_serialOutput > 1)
          Serial.println(_Hyperdeck_videoInput);
      } else if (isNextPartOfBuffer_P(PSTR("file format: "))) {
        if (isNextPartOfBuffer_P(PSTR("QuickTimeUncompressed"))) {
          _Hyperdeck_fileFormat = 1;
        } else if (isNextPartOfBuffer_P(PSTR("QuickTimeProResHQ"))) {
          _Hyperdeck_fileFormat = 2;
        } else if (isNextPartOfBuffer_P(PSTR("unknown"))) { // Strangely enough, there must be a bug since ProRes, ProRes LT and ProRes Proxy is reported as "unknown". We reserve numbers 3,4 and 5 for those...
          _Hyperdeck_fileFormat = 3;
        } else if (isNextPartOfBuffer_P(PSTR("DNxHD220"))) {
          _Hyperdeck_fileFormat = 6;
        } else if (isNextPartOfBuffer_P(PSTR("QuickTimeDNxHD220"))) {
          _Hyperdeck_fileFormat = 7;
        } else {
          if (_serialOutput > 1)
            Serial.print(F("UNKNOWN file format:"));
          if (_serialOutput > 1)
            Serial.println(_buffer + _bufferReadIndex);
          _Hyperdeck_fileFormat = 0; // Unknown
        }
        if (_serialOutput > 1)
          Serial.print(F("_Hyperdeck_fileFormat="));
        if (_serialOutput > 1)
          Serial.println(_Hyperdeck_fileFormat);
      }

      else {
        if (_serialOutput > 1)
          Serial.print(F("UNKNOWN type:"));
        if (_serialOutput > 1)
          Serial.println(_buffer + _bufferReadIndex);
      }
      break;
    }
  }
}

/**
 * Sends status command. (Overloading from superclass)
 */
void ClientBMDHyperdeckStudio::_sendStatus() {
  _enableNotifications(); // Check notifications status.
  setRemoteEnable(true);  // Force remote to be set!

  statusRequestCycleStep++;
  _pullStatus(statusRequestCycleStep);
}

/**
 * Sends ping command. (Overloading from superclass)
 */
void ClientBMDHyperdeckStudio::_sendPing() {
  _resetBuffer();
  _addToBuffer_P(PSTR("ping\n"));
  _sendBuffer();
}

/**
 * Adds the input integer to buffer, padded with zero (2 digits, for timecode values)
 */
void ClientBMDHyperdeckStudio::_addToBuffer_tcValue(const uint8_t val) {
  if (val < 10) {
    _addToBuffer_P(PSTR("0"));
  }
  _addToBuffer(String(val));
}

/**************************************
 *
 * State getter and setters. Public.
 *
 **************************************/

/**
 * Enables all notifications if not already
 */
void ClientBMDHyperdeckStudio::_enableNotifications() {

  // If some notifications is not yet enabled:
  if (_Hyperdeck_notifyStatus != B1111) {
    _resetBuffer();
    _addToBuffer_P(PSTR("notify: transport: true\n"));
    _sendBuffer();

    _resetBuffer();
    _addToBuffer_P(PSTR("notify: slot: true\n"));
    _sendBuffer();

    _resetBuffer();
    _addToBuffer_P(PSTR("notify: remote: true\n"));
    _sendBuffer();

    _resetBuffer();
    _addToBuffer_P(PSTR("notify: configuration: true\n"));
    _sendBuffer();

    _resetBuffer();
    _addToBuffer_P(PSTR("notify\n"));
    _sendBuffer();
  }
}

/**
 * Pull status
 */
void ClientBMDHyperdeckStudio::_pullStatus(uint8_t step) {

  // step = 0;
  //_askForClips = true;

  if (step == 0 || (step % 3) == 1) {
    _resetBuffer();
    _addToBuffer_P(PSTR("transport info\n"));
    _addToBuffer_P(PSTR("slot info: slot id: 1\n"));
    _addToBuffer_P(PSTR("slot info: slot id: 2\n"));
    _sendBuffer();
  }

  if (step == 0 || (step % 3) == 0) {
    _resetBuffer();
    _addToBuffer_P(PSTR("transport info\n"));
    _addToBuffer_P(PSTR("slot info\n"));
    _addToBuffer_P(PSTR("notify\n"));
    _addToBuffer_P(PSTR("remote\n"));
    _addToBuffer_P(PSTR("configuration\n"));
    _sendBuffer();
  }

  if (step == 0 || (step % 3) == 2) {
    if (_askForClips) {
      _resetBuffer();
      _addToBuffer_P(PSTR("transport info\n"));
      _addToBuffer_P(PSTR("clips get\n"));
      _sendBuffer();
    } else {
      _resetBuffer();
      _addToBuffer_P(PSTR("transport info\n"));
      _sendBuffer();
    }
  }
  /*
          _resetBuffer();
          _addToBuffer_P(PSTR("transport info\n"));
          _sendBuffer();

          if (step==0 || step==2)	{
                  _resetBuffer();
                  _addToBuffer_P(PSTR("slot info: slot id: 1\n"));
                  _sendBuffer();
          }
          if (step==0 || step==3)	{
                  _resetBuffer();
                  _addToBuffer_P(PSTR("slot info: slot id: 2\n"));
                  _sendBuffer();
          }
          // Asking this one so the last received info is the currently selected slot:
          if (step==0 || step==2 || step==3)	{
                  _resetBuffer();
                  _addToBuffer_P(PSTR("slot info\n"));
                  _sendBuffer();
          }
          if (step==0 || step==4)	{
                  _resetBuffer();
                  _addToBuffer_P(PSTR("notify\n"));
                  _sendBuffer();
          }

          if (step==0 || step==5)	{
                  _resetBuffer();
                  _addToBuffer_P(PSTR("remote\n"));
                  _sendBuffer();
          }

          if (step==0 || step==6)	{
                  _resetBuffer();
                  _addToBuffer_P(PSTR("configuration\n"));
                  _sendBuffer();
          }
          */
}

// This functions is called if you wish to pull the list of clips from the HyperDeck. The reason why you may not want that is, that IF the harddrive has a LOT of files (in my test 70 files will be enough, maybe less), it will somehow mess up the communication so the connection is disconnected right after. It's still unknown why this happens and it needs to be bug-fixed. But for now, getting all clip names is also less typical for Arduino control, so it's not such a big deal.
void ClientBMDHyperdeckStudio::askForClips(bool askForClips) { _askForClips = askForClips; }
void ClientBMDHyperdeckStudio::askForClipNames(bool askForNames) { _askForClipNames = askForNames; }

/********************
 GETTING VALUES
 ********************/

// Use getPlaybackSlotId() instead!
// uint8_t ClientBMDHyperdeckStudio::getCurrentSlotId() { return _Hyperdeck_currentSlotId; }

uint8_t ClientBMDHyperdeckStudio::getSlotStatus(uint8_t slotId) { return _Hyperdeck_slotStatus[slotId - 1]; }
unsigned long ClientBMDHyperdeckStudio::getSlotRecordingTime(uint8_t slotId) { return _Hyperdeck_slotRecordingTime[slotId - 1]; }
uint8_t ClientBMDHyperdeckStudio::getTransportStatus() { return _Hyperdeck_transportStatus; }
bool ClientBMDHyperdeckStudio::isPlaying() { return _Hyperdeck_transportStatus == ClientBMDHyperdeckStudio_TRANSPORT_PLAY; }
bool ClientBMDHyperdeckStudio::isRecording() { return _Hyperdeck_transportStatus == ClientBMDHyperdeckStudio_TRANSPORT_RECORD; }
bool ClientBMDHyperdeckStudio::isStopped() { return _Hyperdeck_transportStatus == ClientBMDHyperdeckStudio_TRANSPORT_STOPPED; }
bool ClientBMDHyperdeckStudio::isInPreview() { return _Hyperdeck_transportStatus == ClientBMDHyperdeckStudio_TRANSPORT_PREVIEW; }
int ClientBMDHyperdeckStudio::getPlaySpeed() { return _Hyperdeck_playSpeed; }
uint8_t ClientBMDHyperdeckStudio::getClipId() { return _Hyperdeck_clipId; }
uint8_t ClientBMDHyperdeckStudio::getPlayBackSlotId() { return _Hyperdeck_playBackSlotId; }
uint8_t ClientBMDHyperdeckStudio::getTChh() { return _Hyperdeck_tc_hh; }
uint8_t ClientBMDHyperdeckStudio::getTCmm() { return _Hyperdeck_tc_mm; }
uint8_t ClientBMDHyperdeckStudio::getTCss() { return _Hyperdeck_tc_ss; }
uint8_t ClientBMDHyperdeckStudio::getTCff() { return _Hyperdeck_tc_ff; }
uint8_t ClientBMDHyperdeckStudio::getVideoFormat() { return _Hyperdeck_videoFormat; }
bool ClientBMDHyperdeckStudio::isRemoteEnabled() { return _Hyperdeck_remoteEnabled; }
bool ClientBMDHyperdeckStudio::isRemoteOverride() { return _Hyperdeck_remoteOverride; }
uint8_t ClientBMDHyperdeckStudio::getAudioInput() { return _Hyperdeck_audioInput; }
uint8_t ClientBMDHyperdeckStudio::getVideoInput() { return _Hyperdeck_videoInput; }
uint8_t ClientBMDHyperdeckStudio::getFileFormat() { return _Hyperdeck_fileFormat; }
int ClientBMDHyperdeckStudio::getInputStatus() { return _noInput; }
uint8_t ClientBMDHyperdeckStudio::getTotalClipCount() { return _Hyperdeck_clipCount; }
uint8_t ClientBMDHyperdeckStudio::getFileClipId(uint8_t index) { return _Hyperdeck_fileidlist[index]; }
char *ClientBMDHyperdeckStudio::getFileName(uint8_t index) { 
  if(index < ClientBMDHyperdeckStudio_CLIPS) {
    return _Hyperdeck_filelist[index];
  } else if(_Hyperdeck_currentClipId == _Hyperdeck_clipId) {
    return _Hyperdeck_currentFile;
  }

  return "N/A";
}
char *ClientBMDHyperdeckStudio::getCurrentFileName() { 
  if(_Hyperdeck_currentClipId != _Hyperdeck_clipId) {
    return "Loading...";
  } else {
    return _Hyperdeck_currentFile; 
  }
}

/********************
 SETTING VALUES
 ********************/

// switch to preview or output:
void ClientBMDHyperdeckStudio::previewEnable(boolean enable) {
  _resetBuffer();

  _addToBuffer_P(PSTR("preview: enable: "));
  _addToBuffer_P(enable ? PSTR("true\n") : PSTR("false\n"));

  _sendBuffer();
}

// play from current timecode
void ClientBMDHyperdeckStudio::play() {
  _resetBuffer();

  _addToBuffer_P(PSTR("play\n"));

  _sendBuffer();
}
// play at specific speed, play: speed: {-1600 to 1600}
void ClientBMDHyperdeckStudio::playWithSpeed(int speed) {
  _resetBuffer();

  _addToBuffer_P(PSTR("play: speed: "));
  _addToBuffer(String(speed));
  _addToBuffer_P(PSTR("\n"));

  _sendBuffer();
}

// play in loops or stop-at-end
void ClientBMDHyperdeckStudio::playWithLoop(boolean loop) {
  _resetBuffer();

  _addToBuffer_P(PSTR("play: loop: "));
  _addToBuffer_P(loop ? PSTR("true\n") : PSTR("false\n"));

  _sendBuffer();
}

// play current clip or all clips
void ClientBMDHyperdeckStudio::playSingleClip(boolean single) {
  _resetBuffer();

  _addToBuffer_P(PSTR("play: single clip: "));
  _addToBuffer_P(single ? PSTR("true\n") : PSTR("false\n"));

  _sendBuffer();
}

// record from current input
void ClientBMDHyperdeckStudio::record() {
  _resetBuffer();

  _addToBuffer_P(PSTR("record\n"));

  _sendBuffer();
}

// record named clip
void ClientBMDHyperdeckStudio::recordWithName(const char *str) {
  _resetBuffer();

  _addToBuffer_P(PSTR("record: name: \""));
  _addToBuffer(String(str));
  _addToBuffer_P(PSTR("\"\n"));

  _sendBuffer();
}

// stop playback or recording
void ClientBMDHyperdeckStudio::stop() {
  _resetBuffer();

  _addToBuffer_P(PSTR("stop\n"));

  _sendBuffer();
}

// switch to specified slot
void ClientBMDHyperdeckStudio::slotSelect(uint8_t slot) {
  _resetBuffer();

  _addToBuffer_P(PSTR("slot select: slot id: "));
  _addToBuffer(String(slot));
  _addToBuffer_P(PSTR("\n"));

  _sendBuffer();
}

// load clips of specified format
void ClientBMDHyperdeckStudio::slotSelectVideoFormat(uint8_t format) {
  _resetBuffer();

  _addToBuffer_P(PSTR("slot select: video format: "));
  _addToBuffer(String(format));
  _addToBuffer_P(PSTR("\n"));

  _sendBuffer();
}

// goto clip id {n}
void ClientBMDHyperdeckStudio::gotoClipID(uint8_t id) {
  _resetBuffer();

  _addToBuffer_P(PSTR("goto: clip id: "));
  _addToBuffer(String(id));
  _addToBuffer_P(PSTR("\n"));

  _sendBuffer();
}

// goto start of clip
void ClientBMDHyperdeckStudio::gotoClipStart() {
  _resetBuffer();

  _addToBuffer_P(PSTR("goto: clip: start\n"));

  _sendBuffer();
}

// goto end of clip
void ClientBMDHyperdeckStudio::gotoClipEnd() {
  _resetBuffer();

  _addToBuffer_P(PSTR("goto: clip: end\n"));

  _sendBuffer();
}

// goto start of timeline
void ClientBMDHyperdeckStudio::gotoTimelineStart() {
  _resetBuffer();

  _addToBuffer_P(PSTR("goto: timeline: start\n"));

  _sendBuffer();
}

// goto end of timeline
void ClientBMDHyperdeckStudio::gotoTimelineEnd() {
  _resetBuffer();

  _addToBuffer_P(PSTR("goto: timeline: end\n"));

  _sendBuffer();
}

// goto specified timecode
void ClientBMDHyperdeckStudio::gotoTimecode(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t ff) { gotoTimecode(hh, mm, ss, ff, 0); }

// goto specified timecode
void ClientBMDHyperdeckStudio::gotoTimecode(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t ff, int relative) {
  _resetBuffer();

  _addToBuffer_P(PSTR("goto: timecode: "));
  if (relative < 0) {
    _addToBuffer_P(PSTR("-"));
  } else if (relative > 0) {
    _addToBuffer_P(PSTR("+"));
  }
  _addToBuffer_tcValue(hh);
  _addToBuffer_P(PSTR(":"));
  _addToBuffer_tcValue(mm);
  _addToBuffer_P(PSTR(":"));
  _addToBuffer_tcValue(ss);
  _addToBuffer_P(PSTR(":"));
  _addToBuffer_tcValue(ff);
  _addToBuffer_P(PSTR("\n"));

  _Hyperdeck_lastJogDirection = relative;

  _sendBuffer();
}

// go forward {timecode} duration
void ClientBMDHyperdeckStudio::goForwardInTimecode(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t ff) { gotoTimecode(hh, mm, ss, ff, 1); }

// go backward {timecode} duration
void ClientBMDHyperdeckStudio::goBackwardsInTimecode(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t ff) { gotoTimecode(hh, mm, ss, ff, -1); }

// enable or disable remote control
void ClientBMDHyperdeckStudio::setRemoteEnable(boolean enable) {
  _resetBuffer();

  _addToBuffer_P(PSTR("remote: enable: "));
  _addToBuffer_P(enable ? PSTR("true\n") : PSTR("false\n"));

  _sendBuffer();
}

// session override remote control
void ClientBMDHyperdeckStudio::setRemoteOverride(boolean override) {
  _resetBuffer();

  _addToBuffer_P(PSTR("remote: override: "));
  _addToBuffer_P(override ? PSTR("true\n") : PSTR("false\n"));

  _sendBuffer();
}

// switch to xxx input
void ClientBMDHyperdeckStudio::setVideoInput(uint8_t videoInput) {
  _resetBuffer();

  _addToBuffer_P(PSTR("configuration: video input: "));

  switch (videoInput) {
  case ClientBMDHyperdeckStudio_VIDEOINPUT_SDI:
    _addToBuffer_P(PSTR("SDI"));
    break;
  case ClientBMDHyperdeckStudio_VIDEOINPUT_HDMI:
    _addToBuffer_P(PSTR("HDMI"));
    break;
  case ClientBMDHyperdeckStudio_VIDEOINPUT_COMPONENT:
    _addToBuffer_P(PSTR("component"));
    break;
  }

  _addToBuffer_P(PSTR("\n"));

  _sendBuffer();
}

// switch to xxx input
void ClientBMDHyperdeckStudio::setAudioInput(uint8_t audioInput) {
  _resetBuffer();

  _addToBuffer_P(PSTR("configuration: audio input: "));

  switch (audioInput) {
  case ClientBMDHyperdeckStudio_AUDIOINPUT_EMBEDDED:
    _addToBuffer_P(PSTR("embedded"));
    break;
  case ClientBMDHyperdeckStudio_AUDIOINPUT_XLR:
    _addToBuffer_P(PSTR("XLR"));
    break;
  case ClientBMDHyperdeckStudio_AUDIOINPUT_RCA:
    _addToBuffer_P(PSTR("RCA"));
    break;
  }

  _addToBuffer_P(PSTR("\n"));

  _sendBuffer();
}

void ClientBMDHyperdeckStudio::setFileFormat(uint8_t fileFormat) {
  _resetBuffer();

  _addToBuffer_P(PSTR("configuration: file format: "));

  switch (fileFormat) {
  case 1:
    _addToBuffer_P(PSTR("QuickTimeUncompressed"));
    break;
  case 2:
    _addToBuffer_P(PSTR("QuickTimeProResHQ"));
    break;
  // Missing 3-5 - don't know values!
  case 6:
    _addToBuffer_P(PSTR("DNxHD220"));
    break;
  case 7:
    _addToBuffer_P(PSTR("QuickTimeDNxHD220"));
    break;
  }

  _addToBuffer_P(PSTR("\n"));

  _sendBuffer();
}
