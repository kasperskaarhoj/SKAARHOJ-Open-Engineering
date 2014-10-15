/*
Copyright 2012-2014 Kasper Skårhøj, LearningLab DTU, kaska@llab.dtu.dk

This library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

This library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ClientKramerVP728 library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/

#include <SPI.h>
#include <Ethernet.h>

#ifndef HTTPSharedClientWrapper_h
#define HTTPSharedClientWrapper_h


class HTTPSharedClientWrapper
{
  private:
		// Network:
	EthernetClient _client;
	
		// Connection:
	uint8_t _activeConnectionOwner;	// Owner ID of the external process owning this request.
	bool _parsingHeaders;		// If true, the response reading will look for the ending of the HTTP response header and set this flag false when found.
	unsigned long _requestTime;	// The time when the request was made...
	
	bool _serialOutput;
	
	bool _connect();
	unsigned long _requestTimeLen();
	
  public:
	char response[256];		
	uint16_t responsePointer;

	HTTPSharedClientWrapper();
	void begin();
	
	void serialOutput(bool flag);

	bool receiveData(const uint8_t owner);
	bool sendRequest(const String path, const uint8_t ownerID, IPAddress ip, int port);
	bool sendRequest(const String path, const uint8_t ownerID, IPAddress ip, int port, bool raw);
};
#endif 
