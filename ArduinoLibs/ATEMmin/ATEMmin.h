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




#ifndef ATEMmin_h
#define ATEMmin_h

#include "Arduino.h"
#include "ATEMbase.h"
#include "EthernetUdp.h"


class ATEMmin : public ATEMbase
{
  public:
	ATEMmin();  
	  
// *********************************
// **
// ** Implementations in ATEMmin.h:
// **
// *********************************





// *********************************
// **
// ** Implementations in ATEMmin.h:
// **
// *********************************



private:
	void _parseGetCommands(const char *cmdStr);

			// Private Variables in ATEM.h:
	
			uint16_t atemProgramInputVideoSource[2];
			uint16_t atemPreviewInputVideoSource[2];
			bool atemTransitionInTransition[2];
			uint8_t atemTransitionFramesRemaining[2];
			uint16_t atemTransitionPosition[2];
			bool atemKeyerOnAirEnabled[2][4];
			bool atemDownstreamKeyerOnAir[2];
			bool atemDownstreamKeyerInTransition[2];
			bool atemDownstreamKeyerIsAutoTransitioning[2];
			uint8_t atemDownstreamKeyerFramesRemaining[2];
			bool atemFadeToBlackStateFullyBlack[2];
			bool atemFadeToBlackStateInTransition[2];
			uint8_t atemFadeToBlackStateFramesRemaining[2];
			uint16_t atemAuxSourceInput[6];
			uint16_t atemTallyByIndexSources;
			uint8_t atemTallyByIndexTallyFlags[21];

public:
			// Public Methods in ATEM.h:
	
			uint16_t getProgramInputVideoSource(uint8_t mE);
			void setProgramInputVideoSource(uint8_t mE, uint16_t videoSource);
			uint16_t getPreviewInputVideoSource(uint8_t mE);
			void setPreviewInputVideoSource(uint8_t mE, uint16_t videoSource);
			void performCutME(uint8_t mE);
			void performAutoME(uint8_t mE);
			bool getTransitionInTransition(uint8_t mE);
			uint8_t getTransitionFramesRemaining(uint8_t mE);
			uint16_t getTransitionPosition(uint8_t mE);
			void setTransitionPosition(uint8_t mE, uint16_t position);
			bool getKeyerOnAirEnabled(uint8_t mE, uint8_t keyer);
			void setKeyerOnAirEnabled(uint8_t mE, uint8_t keyer, bool enabled);
			bool getDownstreamKeyerOnAir(uint8_t keyer);
			bool getDownstreamKeyerInTransition(uint8_t keyer);
			bool getDownstreamKeyerIsAutoTransitioning(uint8_t keyer);
			uint8_t getDownstreamKeyerFramesRemaining(uint8_t keyer);
			void setDownstreamKeyerOnAir(uint8_t keyer, bool onAir);
			bool getFadeToBlackStateFullyBlack(uint8_t mE);
			bool getFadeToBlackStateInTransition(uint8_t mE);
			uint8_t getFadeToBlackStateFramesRemaining(uint8_t mE);
			void performFadeToBlackME(uint8_t mE);
			uint16_t getAuxSourceInput(uint8_t aUXChannel);
			void setAuxSourceInput(uint8_t aUXChannel, uint16_t input);
			uint16_t getTallyByIndexSources();
			uint8_t getTallyByIndexTallyFlags(uint16_t sources);
};

#endif

