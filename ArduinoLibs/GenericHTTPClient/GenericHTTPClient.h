/*
Copyright 2014 Filip Sandborg, LearningLab DTU, filipsandborg@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GenericHTTPClient_h
#define GenericHTTPClient_h


#include <Arduino.h>
#include <SkaarhojASCIIClient.h>

#include <Ethernet.h>
#include <HTTPSharedClientWrapper.h>




class GenericHTTPClient : public SkaarhojASCIIClient 
{
protected:
	uint8_t	_ownerID;		// HTTPSharedClientWrapper ID
	uint8_t _retryCount;
	void (*_responseCallback)(String request, String response, bool success);

public:
	IPAddress _ip;
	HTTPSharedClientWrapper _sharedClient;
	String _lastRequest;

	void setResponseCallback(void (*fptr)(String reqeuest, String response, bool success));
	void setRetryCount(uint8_t retryCount);
	GenericHTTPClient();
	void begin(HTTPSharedClientWrapper sharedClient, uint8_t ownerID);
	unsigned long lastResponse();

	bool getURL(String url, IPAddress ip, int port);
	bool getURL(String url, String alias, IPAddress ip, int port);
	bool sendRaw(String raw, IPAddress ip, int port);

	String receiveData();
	String requestAndWait(String request, int port, unsigned long timeout, bool raw);

	virtual void runLoop();
};

#endif