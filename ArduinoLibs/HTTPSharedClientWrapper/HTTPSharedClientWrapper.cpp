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

#include "HTTPSharedClientWrapper.h"
#include "Arduino.h"


// Empty constructor.
HTTPSharedClientWrapper::HTTPSharedClientWrapper(){
	begin();
}

/**
 * Create the HTTPSharedClientWrapper as a global object...
 */ 
void HTTPSharedClientWrapper::begin() {
	EthernetClient client;
	_client = client;
	_serialOutput = false;
	_activeConnectionOwner = 0;
	_parsingHeaders = true;
}

/**
 * Enable/Disable debug output to Serial:
 */ 
void HTTPSharedClientWrapper::serialOutput(bool flag){
	_serialOutput = flag;
}

/**
 * Call this from the various owner applications. 
 * Returns true if at some point there is a full data buffer for the owner ID. When that happens, the client is also "released" so any other owners may like to use it for a request. 
 * Therefore, upon receiving "true" from this function, you should read the buffer if you are interested in its content.
 *
 * @ownerID	- a number you select to identify a specific "client" using this object.
 */
bool HTTPSharedClientWrapper::receiveData(const uint8_t ownerID)	{
	bool returnValue = false;
	
		// Reading from the client
	while(_client.available()) {
		char c = _client.read();
		if (_activeConnectionOwner > 0)	{	// If there is a client asking for this...
			if (_parsingHeaders)	{	// HTTP header, looking for two line breaks:
				response[responsePointer%4] = c;
				responsePointer++;
				if (responsePointer>4 
						&& response[(responsePointer-4)%4]==13
						&& response[(responsePointer-3)%4]==10
						&& response[(responsePointer-2)%4]==13
						&& response[(responsePointer-1)%4]==10
						) {
							_parsingHeaders = false;
							responsePointer = 0;
						}
			} else {	// Body:
//				Serial.println((uint8_t)c,HEX);
				if (responsePointer<256-1)	{
					response[responsePointer] = c;
					responsePointer++;
				}
			}
		} else {
		   	if (_serialOutput) {
				Serial.println(F("HSCW: Reading data, but no connection ownerID!"));
			}
		}
	}	

	 if (_activeConnectionOwner == ownerID)	{	// If an connection is going on...:
	 	// If the server's disconnected, stop the client:
		if (!_client.connected()) {
			returnValue = true;
			response[responsePointer] = 0;
		   	_client.stop();

		   	if (_serialOutput) {
				Serial.print(F("HSCW: Client "));
				Serial.print(_activeConnectionOwner);
				Serial.print(F(": Disconnecting. Request time, ms: "));
				Serial.println(_requestTimeLen());
			}
		   	if (_serialOutput) {
				Serial.println(F("HSCW: Response:"));
				Serial.println(response);
			}

			_activeConnectionOwner = 0;
		} else if(_requestTimeLen()>10000) {		// Timeout of 10 seconds:
		   	if (_serialOutput) {
				Serial.print(F("HSCW: Client "));
				Serial.print(_activeConnectionOwner);
				Serial.print(F(": Request timeout ERROR! Request time, ms: "));
				Serial.println(_requestTimeLen());
			}

			returnValue = false;
		   	_client.stop();
			_activeConnectionOwner = 0;
		}
	 }
	
	return returnValue;
}

/**
 * Returns how long ago (ms) the request was sent to the server.
 */ 
unsigned long HTTPSharedClientWrapper::_requestTimeLen()	{
	return (unsigned long)((unsigned long)millis()-(unsigned long)_requestTime);
}

/**
 * Sending a request the "path" on ip:port for ownerID.
 */
bool HTTPSharedClientWrapper::sendRequest(const String path, const uint8_t ownerID, IPAddress ip, int port) {
	return sendRequest(path, ownerID, ip, port, false);
}
bool HTTPSharedClientWrapper::sendRequest(const String path, const uint8_t ownerID, IPAddress ip, int port, bool raw) {

  if (_activeConnectionOwner == 0)	{
    if (_client.connect(ip, port)) {
	    if (_serialOutput) {
			Serial.print(F("HSCW: Owner#"));
			Serial.print(ownerID);
			Serial.print(F(": Connecting to "));
			Serial.print(ip);
			Serial.print(":");
			Serial.print(port);
			if (raw)	Serial.println(F(" [RAW]:"));
			Serial.println(path);
		}
	    // send the HTTP GET request:
		_activeConnectionOwner = ownerID;
		_parsingHeaders = raw ? false : true;
		_requestTime = millis();
		responsePointer = 0;

		if (raw)	{	// Up to 128 chars:
				// THE REASON for not just printing the String object is that the Arduino client will split it up in many smaller TCP packets.
				// For a particular implementation of mine this yielded an error (Streaming Computer python script),
				// but thinking more about this... could it be the reason why TCP requests to the robotic camera felt slow?
				// I think an HTTP request should be contained in a single package - anything else must be much more heavy to handle...
			char charBuf[128];
			path.toCharArray(charBuf, 127);
			_client.print(charBuf);
		} else {
			_client.print(F("GET "));
			_client.print(path);
			_client.println(F(" HTTP/1.1"));
		    _client.print(F("Host: "));			// TODO!!!
		    _client.println(ip);
		    _client.println(F("Connection: Close"));
		    _client.println();
		}
		return true;
	  } 
	  else {
	    // if you couldn't make a connection: 
		/* Notice, the timeout for this can be defined with something like:
		  W5100.setRetransmissionTime(0xD0);	//0xD0 = 208ms
		  W5100.setRetransmissionCount(1);
		*/
	    if (_serialOutput || true) {
			Serial.print(F("HSCW: Owner#"));
			Serial.print(ownerID);
			Serial.print(F(": Connecting failed! URL:"));
			Serial.print(ip);
			Serial.print(":");
			Serial.print(port);
			Serial.println(path);
		}
	    _client.stop();
		_activeConnectionOwner = 0;
	  }
	} else {
	    if (_serialOutput) {
			Serial.print(F("HSCW: Owner#"));
			Serial.print(ownerID);
			Serial.print(F(": Client was busy (for owner "));
			Serial.print(_activeConnectionOwner);
			Serial.println(")");
		}
	}

  return false;
}


