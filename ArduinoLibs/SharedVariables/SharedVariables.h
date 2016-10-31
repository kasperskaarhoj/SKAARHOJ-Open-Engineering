/*
Copyright 2015 Kasper Skårhøj, KS Engineering, kasperskaarhoj@gmail.com
Developed for LearningLab DTU, Technical University of Denmark

The SharedVariables library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The SharedVariables library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the SharedVariables library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/

/*

Features at a glance:


Shared Variables:
- Seemless: Plug-in interface to global variables
- Supports uint8, uint16, int, long, unsigned long, float, bool, char and strings (of chars) as well as uint8 and int one-dimensional arrays
- read/write permissions
- meta data support
- UDP (binary) / TCP/Telnet (ASCII) / Serial  (ASCII) for access
- TODO: Master PinCode
- TODO: Widget meta data support for web and app interfaces

UDP messenger
- Extremely lightweight and efficient
- Multimaster / Multislave
- Mixed Master/Slave node
- Single port, single socket
- Ping + broadcast discover of nodes
- AutoUpdated variables
- TODO: Checksum(?)













*/


#ifndef SharedVariables_h
#define SharedVariables_h

#include "Arduino.h"
//  #include "SkaarhojPgmspace.h"  - 23/2 2014
#include "Streaming.h"
#include "SkaarhojBufferTools.h"
#include <UDPmessenger.h>



#define SharedVariables_MAX_ATTACHMENTS 40

class SharedVariables
{
  protected:
  	uint8_t _numberOfVars;
  	void **_varReferences;
  	uint8_t *_varType;	// Bit7-6=RW flag, Bit5-4=N/A, Bit3-0=Variable type
	uint8_t *_varSize;	// Size of array types (string, arrays of uint8_t and integers)
	int *_varMin;	// Min values
	int *_varMax;	// Max values
	const char ** _varName;	// PSTR() pointers. NOTE: Names for sharing variables is the string by which the variable is referenced via the buffer interface - make sure not to name two variables the same or substrings of each other, for instance dont use names like "My Var" and "My Var2" because "My Var" is a substring of "My Var2". This may lead to unexpected behaviours
	const char ** _varDescr;	// PSTR() pointers
	
	void (*_externalChangeOfVariableCallback)(uint8_t idx);
	void (*_externalReadResponseCallback)(const uint8_t slaveIdx, const uint8_t slaveAddress, const uint8_t dataLength, const uint8_t *dataArray);


  	uint8_t _numberOfWidgets;
  	uint8_t _widgetPointer;
  	uint8_t *_widgetIdx;
	const char ** _widgetLabel;	// PSTR() pointers
	const char ** _widgetCategory;	// PSTR() pointers
  	uint8_t *_widgetSortOrder;
  	uint8_t *_widgetType;
  	uint8_t *_widgetFlags;
	const char ** _widgetParams;	// PSTR() pointers
  	int *_widgetPin;
  	uint8_t *_widgetCmdVariable;


	
	UDPmessenger * _messenger;
	
	uint8_t _currentBundleAddr;

	uint8_t _attachPointer;
	uint8_t _attachments[SharedVariables_MAX_ATTACHMENTS][4];	// localIdx, slaveAddress, remoteIdx, RW bits
	
  public:
	  SharedVariables(uint8_t numberOfVariables, uint8_t numberOfWidgets=0);
	  void shareLocalVariable(uint8_t idx, int16_t &variableRef, uint8_t rw, const char *name, const char *descr, int16_t min=0, int16_t max=0);
	  void shareLocalVariable(uint8_t idx, bool &variableRef, uint8_t rw, const char *name, const char *descr);
	  void shareLocalVariable(uint8_t idx, uint8_t &variableRef, uint8_t rw, const char *name, const char *descr, uint8_t min=0, uint8_t max=0);
	  void shareLocalVariable(uint8_t idx, uint16_t &variableRef, uint8_t rw, const char *name, const char *descr, uint16_t min=0, uint16_t max=0);
	  void shareLocalVariable(uint8_t idx, int32_t &variableRef, uint8_t rw, const char *name, const char *descr);
	  void shareLocalVariable(uint8_t idx, uint32_t &variableRef, uint8_t rw, const char *name, const char *descr);
	  void shareLocalVariable(uint8_t idx, float &variableRef, uint8_t rw, const char *name, const char *descr, int min=0, int max=0);
	  void shareLocalVariable(uint8_t idx, char &variableRef, uint8_t rw, const char *name, const char *descr);
	  void shareLocalVariable(uint8_t idx, const char * variableRef, uint8_t theSize, uint8_t rw, const char *name, const char *descr);
	  void shareLocalVariable(uint8_t idx, const uint8_t * variableRef, uint8_t theSize, uint8_t rw, const char *name, const char *descr);
	  void shareLocalVariable(uint8_t idx, const int16_t * variableRef, uint16_t theSize, uint8_t rw, const char *name, const char *descr);
	  void shareLocalVariable(uint8_t idx, const uint16_t * variableRef, uint16_t theSize, uint8_t rw, const char *name, const char *descr);
	  void shareLocalVariable(uint8_t idx, const int32_t * variableRef, uint16_t theSize, uint8_t rw, const char *name, const char *descr);
	  
	  void addWidget(uint8_t idx, const char *label, const char *category, uint8_t sortOrder, uint8_t widgetType, uint8_t flags, const char *parameters, int pinCode=0, uint8_t cmdVariable=255);
				
	  void test(uint8_t idx);
	  void printOverview(Print &output);
	  void printValues(Print &output, bool readOnlyCheck=false);
	  void printSingleValue(Print &output, uint8_t idx);
	  
	  void setExternalChangeOfVariableCallback(void (*fptr)(uint8_t idx));
	  void setExternalReadResponseCallback(void (*fptr)(const uint8_t slaveIdx, const uint8_t slaveAddress, const uint8_t dataLength, const uint8_t *dataArray));
	  bool validIdx(uint8_t idx);

	  // For Telnet:
	  void incomingASCIILine(SkaarhojBufferTools &bufferObj, Print &output);

	  // For UDP:
	  void incomingBinBuffer(UDPmessenger &UDPmessengerObj, const uint8_t cmd, const uint8_t from, const uint8_t dataLength, const uint8_t *dataArray);
	  void messengerObject(UDPmessenger &UDPmessengerObj);

	  void setRemoteVariableOverUDP(int variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx);
	  void setRemoteVariableOverUDP(bool variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx);
	  void setRemoteVariableOverUDP(uint8_t variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx);
	  void setRemoteVariableOverUDP(uint16_t variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx);
	  void setRemoteVariableOverUDP(long variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx);
	  void setRemoteVariableOverUDP(unsigned long variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx);
	  void setRemoteVariableOverUDP(float variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx);
	  void setRemoteVariableOverUDP(double variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx);
	  void setRemoteVariableOverUDP(char variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx);
	  void setRemoteVariableOverUDP(const char * variableRef, uint8_t theSize, const uint8_t slaveAddress, const uint8_t slaveIdx);
	  void setRemoteVariableOverUDP(const uint8_t * variableRef, uint8_t theSize, const uint8_t slaveAddress, const uint8_t slaveIdx);
	  void setRemoteVariableOverUDP(const int * variableRef, uint8_t theSize, const uint8_t slaveAddress, const uint8_t slaveIdx);
	  void getRemoteVariableOverUDP(const uint8_t slaveAddress, const uint8_t slaveIdx);
		  	  
	  void startBundle(const uint8_t slaveAddress);
	  void endBundle();

	  bool attachLocalVariableToRemoteVariable(const uint8_t localIdx, const uint8_t slaveAddress, const uint8_t slaveIdx, bool push);
	  void UDPautoExchange(uint16_t period);
	  uint8_t bytesPerElement(uint8_t type);

	  void sendUDPPing(const uint8_t slaveAddress);

	  bool hasTimedOut(unsigned long time, unsigned long timeout);
		  
	  int16_t read_int(const uint8_t *dataArray);
	  bool read_bool(const uint8_t *dataArray);
	  uint8_t read_uint8_t(const uint8_t *dataArray);
	  uint16_t read_uint16_t(const uint8_t *dataArray);
	  long read_long(const uint8_t *dataArray);
	  unsigned long read_unsigned_long(const uint8_t *dataArray);
	  float read_float(const uint8_t *dataArray);
	  char read_char(const uint8_t *dataArray);
	  void read_string(const uint8_t *dataArray, char * variableRef, uint8_t theSize);
	  void read_uint8_array(const uint8_t *dataArray, uint8_t * variableRef, uint8_t theSize);
	  void read_int_array(const uint8_t *dataArray, int * variableRef, uint8_t theSize);
			  
  private:
	  void shareLocalVariable(uint8_t idx, void *voidPointer, const char *name, const char *descr, int min=0, int max=0);

	  bool readEnabled(uint8_t idx);
	  bool writeEnabled(uint8_t idx);
	  bool hasCustomRange(uint8_t idx);

	  void sendBinaryInfo(UDPmessenger &UDPmessengerObj, const uint8_t idx);
	  void sendBinaryWidgetSetup(UDPmessenger &UDPmessengerObj, const uint8_t widgetPointer);
	  void sendBinaryReadResponse(UDPmessenger &UDPmessengerObj, const uint8_t idx, bool noErrorIfReadEnabled=false);
	  void storeBinaryValue(UDPmessenger &UDPmessengerObj, const uint8_t idx, const uint8_t dataLength, const uint8_t *dataArray);	  	  

	  void setRemoteVariableOverUDP(void * voidPointer, uint8_t dLen, const uint8_t slaveAddress, const uint8_t slaveIdx);
};

#endif

