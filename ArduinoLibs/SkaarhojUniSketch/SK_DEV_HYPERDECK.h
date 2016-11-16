// Button return colors:
// 0 = off
// 5 = dimmed
// 1,2,3,4 = full (yellow), red, green, yellow
// Bit 4 (16) = blink flag, filter out for KP01 buttons.
uint16_t evaluateAction_HYPERDECK(const uint8_t devIndex, const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown = false, const bool actUp = false, int16_t pulses = 0, const int16_t value = 0) {
  uint16_t retVal = 0;
  if (actDown || actUp) {
    Serial << "HYPERDECK action " << globalConfigMem[actionPtr] << "\n";
  }
  int16_t playspeeds[] = {100, 25, 50, 200, 400, 800, 1600, -25, -50, -100, -200, -400, -800, -1600};
  int16_t speed = 0;

  switch (globalConfigMem[actionPtr]) {
  case 0: // Play
    speed = playspeeds[globalConfigMem[actionPtr + 1]];
    if (actDown) {
      _systemHWcActionCache[HWc][actIdx] = HyperDeck[devIndex].getPlaySpeed();
      if (globalConfigMem[actionPtr + 2] == 1 && HyperDeck[devIndex].isPlaying()) {
        HyperDeck[devIndex].stop();
      } else {
        HyperDeck[devIndex].playWithSpeed(speed);
      }
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 2) { // globalConfigMem[actionPtr + 2] == "Hold Down"
      HyperDeck[devIndex].stop();
    }

    // Cycle between play/pause if pulsed input is used
    if(pulses & 0xFFFE) {
      if(HyperDeck[devIndex].isPlaying()) {
        HyperDeck[devIndex].stop();
      } else {
        HyperDeck[devIndex].play();
      }
    }

    retVal = HyperDeck[devIndex].getPlaySpeed() == speed ? (3 | 0x20) : 5;

    if(extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("Play"));
      extRetValLongLabel(PSTR("Play"));

      extRetValTxt(HyperDeck[devIndex].isPlaying()?"Playing":"Stopped", 0);
      extRetVal2(0);
      //extRetValTxt(HyperDeck[devIndex].isPlaying()?"Playing":"Stopped", 0);

      if(HyperDeck[devIndex].getClipId() == 255) {
        extRetValTxt("No clip", 1);
      } else {
        extRetValTxt(HyperDeck[devIndex].getFileName(HyperDeck[devIndex].getClipId()), 1);
      }
    }
    return retVal;
    break;
  case 1: // Stop
    if (actDown) {
       if (HyperDeck[devIndex].isStopped() && globalConfigMem[actionPtr + 1] == 2) {
        HyperDeck[devIndex].gotoClipStart();
      } else {
        HyperDeck[devIndex].stop();
        if (globalConfigMem[actionPtr + 1] == 1) {
          HyperDeck[devIndex].previewEnable(!HyperDeck[devIndex].isInPreview());
        }
      }
    }

    if(pulses & 0xFFFE) {
      if(!HyperDeck[devIndex].isStopped()) {
        HyperDeck[devIndex].stop();
      }
    }

    if(extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("Hyperdeck"));
      extRetValLongLabel(PSTR("Hyperdeck"));
      extRetValTxt(HyperDeck[devIndex].isPlaying()?"Play":"Stop", 0);
    }

    return HyperDeck[devIndex].isStopped() ? (4 | 0x20) : 5;
    break;
  case 2: // Record
    if (actDown) {
      if (globalConfigMem[actionPtr + 1] == 1 && HyperDeck[devIndex].isRecording()) {
        HyperDeck[devIndex].stop();
      } else {
        HyperDeck[devIndex].record();
      }
    }
    if (actUp && globalConfigMem[actionPtr + 1] == 2) { // globalConfigMem[actionPtr + 2] == "Hold Down"
      HyperDeck[devIndex].stop();
    }

    if(pulses & 0xFFFE) {
      if(HyperDeck[devIndex].isRecording()) {
        HyperDeck[devIndex].stop();
      } else {
        HyperDeck[devIndex].record();
      }
    }

    if(extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("Hyperdeck"));
      extRetValLongLabel(PSTR("Hyperdeck"));
      extRetValTxtShort(HyperDeck[devIndex].isPlaying()?"Rec":"Stop");
    }

    return HyperDeck[devIndex].isRecording() ? (2 | 0x20) : 5;
    break;
  case 3: // Preview
    if (actDown) {
      if(HyperDeck[devIndex].isInPreview()) {
        HyperDeck[devIndex].previewEnable(false);
      } else {
        HyperDeck[devIndex].previewEnable(true);
      }
    }

    if(pulses & 0xFFFE) {
      if(HyperDeck[devIndex].isInPreview()) {
        HyperDeck[devIndex].previewEnable(false);
      } else {
        HyperDeck[devIndex].previewEnable(true);
      }
    }

    if(extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("Hyperdeck"));
      extRetValLongLabel(PSTR("Hyperdeck"));
      extRetValTxt(HyperDeck[devIndex].isInPreview()?"Preview mode":"Output mode", 0);
      extRetValTxtShort(HyperDeck[devIndex].isInPreview()?"Prev":"Outp");
    }

    return HyperDeck[devIndex].isInPreview() ? (4 | 0x20) : 5;
    break;
    case 4: // Next Clip
    case 5: // Previous clip
      retVal = 5;

      if(actDown && value == BINARY_EVENT) {
        pulses = (globalConfigMem[actionPtr] == 4 ? 2 : -2);
        retVal = 4 | 0x20;
      }

      if(pulses & 0xFFFE) {
        uint8_t newClip = pulsesHelper(HyperDeck[devIndex].getClipId(), 1, HyperDeck[devIndex].getTotalClipCount(), true, pulses, 1, 1);
        HyperDeck[devIndex].gotoClipID(newClip);
      }

      if(extRetValIsWanted()) {
        extRetVal(0, 7);
        extRetValShortLabel(PSTR("Hyperdeck"));
        extRetValLongLabel(PSTR("Hyperdeck"));
        extRetValTxt(globalConfigMem[actionPtr]==4?"Next clip":"Prev clip", 0);
        extRetVal2(0);
      }

      return retVal;
      break;
    case 6: // Fast forward
      speed = HyperDeck[devIndex].getPlaySpeed();

      if(actDown && value == BINARY_EVENT) {
        pulses = 2;
      }

      if(pulses & 0xFFFE) {

        uint8_t val = 0;
        switch(speed) {
          case 800:
            val = 3;
            break;
          case 400:
            val = 2;
            break;
          case 200:
            val = 1;
            break;
        }

        speed = 100 << pulsesHelper(val, 1, 3, true, pulses, 1, 1);
        HyperDeck[devIndex].playWithSpeed(speed);
      }

      switch(speed) {
        case 200:
          retVal = (millis() & 512) ? 1 : 0;
          break;
        case 400:
          retVal = (millis() & 256) ? 1 : 0;
          break;
        case 800:
        case 1600:
          retVal = (millis() & 128) ? 1 : 0;
          break;
        default:
          retVal = 5; 
      }

      if(extRetValIsWanted()) {
        extRetVal(0, 7);
        extRetValShortLabel(PSTR("Fast FW"));
        extRetValLongLabel(PSTR("Fast forward"));

        char cache[4];
        sprintf(cache, "%dx", speed/100);
        extRetValTxt(cache, 0);
      }

      // Activate binary output on speed >= 2x
      return retVal | (speed>=200?0x20:0);
      break;
    case 7: // Reverse

      speed = -HyperDeck[devIndex].getPlaySpeed();

      if(actDown && value == BINARY_EVENT) {
        pulses = 2;
      }

      if(pulses & 0xFFFE) {

        uint8_t val = 0;
        switch(speed) {
          case 800:
            val = 3;
            break;
          case 400:
            val = 2;
            break;
          case 200:
            val = 1;
            break;
        }
        
        speed = 100 << pulsesHelper(val, 1, 3, true, pulses, 1, 1);
        HyperDeck[devIndex].playWithSpeed(-speed);
      }

      switch(speed) {
        case 200:
          retVal = (millis() & 512) ? 1 : 0;
          break;
        case 400:
          retVal = (millis() & 256) ? 1 : 0;
          break;
        case 800:
        case 1600:
          retVal = (millis() & 128) ? 1 : 0;
          break;
        default:
          retVal = 5; 
      }

      if(extRetValIsWanted()) {
        extRetVal(0, 7);
        extRetValShortLabel(PSTR("Fast RW"));
        extRetValLongLabel(PSTR("Fast reverse"));

        char cache[4];
        sprintf(cache, "%dx", -speed/100);
        extRetValTxt(cache, 0);
      }

      // Activate binary output on speed <= -2x
      return retVal | (speed>=200?0x20:0);
      break;
    case 8: // Jog
      if(actDown && value == BINARY_EVENT) {
        _systemHWcActionCache[HWc][actIdx] = millis();
        _systemHWcActionCacheFlag[HWc][actIdx] = 1;
      }

      if((_systemHWcActionCacheFlag[HWc][actIdx] == 1 && (uint16_t) millis() - _systemHWcActionCache[HWc][actIdx] > 300) || (_systemHWcActionCacheFlag[HWc][actIdx] == 2 && (uint16_t) millis() - _systemHWcActionCache[HWc][actIdx] > 40)) {
        _systemHWcActionCacheFlag[HWc][actIdx] = 2;
        pulses = (globalConfigMem[actionPtr+1] == 0 ? 2 : -2);
        _systemHWcActionCache[HWc][actIdx] = millis();
      }

      if(actUp) {
        if( _systemHWcActionCacheFlag[HWc][actIdx] == 1) {
          pulses = (globalConfigMem[actionPtr+1] == 0 ? 2 : -2);
        }
        _systemHWcActionCacheFlag[HWc][actIdx] = 0;
      }


      if(pulses & 0xFFFE) {
        if((actDown && _systemHWcActionCacheFlag[HWc][actIdx]) || !actDown) {
          if(pulses < 0) {
            HyperDeck[devIndex].goBackwardsInTimecode(0,0,0,abs(pulses >> 1));
          } else if (pulses > 0) {
            HyperDeck[devIndex].goForwardInTimecode(0,0,0,pulses >> 1);
          }
        }
      }

      if(extRetValIsWanted()) {
        extRetVal(0, 7);
        extRetValShortLabel(PSTR("Hyperdeck"));
        extRetValLongLabel(PSTR("Hyperdeck"));
        extRetValColor(B101010);
        extRetValTxtShort("JOG");
        extRetValTxt("JOG", 0);
      }

      return (_systemHWcActionCacheFlag[HWc][actIdx]?(1 | 0x20):5);
      break;
    case 11: // Slot select
      retVal = 0;

      if(actDown && value == BINARY_EVENT) {
        switch(globalConfigMem[actionPtr + 1]) {
          case 0: // Cycle
            pulses = 2;
            break;
          case 1: // A
            HyperDeck[devIndex].slotSelect(1);
            retVal |= 0x20;
            break;
          case 2: // B
            HyperDeck[devIndex].slotSelect(2);
            retVal |= 0x20;
            break;
        }
      }

      if(pulses & 0xFFFE) {
        if(HyperDeck[devIndex].getPlayBackSlotId() == 1) {
          HyperDeck[devIndex].slotSelect(2);
        } else {
          HyperDeck[devIndex].slotSelect(1);
        }

        retVal |= 0x20;
      }

      switch(globalConfigMem[actionPtr + 1]) {
        case 0:
          if(HyperDeck[devIndex].getPlayBackSlotId() == 1) {
            retVal |= (HyperDeck[devIndex].getSlotStatus(1) == 1 ? 3 : 0);
          } else if(HyperDeck[devIndex].getPlayBackSlotId() == 2) {
            retVal |= (HyperDeck[devIndex].getSlotStatus(2) == 1 ? 2 : 0);
          }
          break;
        case 1:
          retVal |= (HyperDeck[devIndex].getSlotStatus(1) == 1 ? (HyperDeck[devIndex].getPlayBackSlotId() == 1?3:5) : 0);
          break;
        case 2:
          retVal |= (HyperDeck[devIndex].getSlotStatus(2) == 1 ? (HyperDeck[devIndex].getPlayBackSlotId() == 2?2:5) : 0);
          break;
      }

      if(extRetValIsWanted()) {
        extRetVal(0, 7);
        extRetValShortLabel(PSTR("Slot"));
        extRetValLongLabel(PSTR("Slot"));
        char slot[2] = {0,0};
        slot[0] = 'A'+(HyperDeck[devIndex].getPlayBackSlotId()-1);
        extRetValTxt(HyperDeck[devIndex].getPlayBackSlotId() == 0?"N/A":slot, 0);
      }

      return retVal;
      break;
    case 12: { // Input
      uint8_t videoInput = HyperDeck[devIndex].getVideoInput();
      uint8_t audioInput = HyperDeck[devIndex].getAudioInput();

      if(actDown && value == BINARY_EVENT) {
        HyperDeck[devIndex].setVideoInput(globalConfigMem[actionPtr+1]+1);
        HyperDeck[devIndex].setAudioInput(globalConfigMem[actionPtr+2]+1);
      }

      if(pulses & 0xFFFE) {
        videoInput = (++videoInput % 3) + 1;
      }

      char *input;
      memset(input, 0, 5);
      switch(videoInput) {
        case 1:
          input = "SDI";
          break;
        case 2:
          input = "HDMI";
          break;
        case 3:
          input = "COMP";
          break;
      }

      if(extRetValIsWanted()) {
        extRetVal(0, 7);
        extRetValLongLabel(PSTR("Input"));
        extRetValTxt(input,0);
      }
      break;
    }
    case 14: // Shuttle
      break;
    //case 12: // Replay speed
    //  break;
  }
  return retVal;
}
