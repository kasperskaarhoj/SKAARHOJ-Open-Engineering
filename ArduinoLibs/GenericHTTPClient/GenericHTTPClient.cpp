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

#include "Arduino.h"
#include "GenericHTTPClient.h"

// Empty constructor
GenericHTTPClient::GenericHTTPClient(){};

// Initialization function
void GenericHTTPClient::begin(HTTPSharedClientWrapper sharedClient, uint8_t ownerID) {
    _sharedClient = sharedClient;
    _ownerID = ownerID;
    _lastRequest = String("");
    _retryCount = 1;
}

// Define the callback method which is called every time
// the HTTP Client receives replies.
void GenericHTTPClient::setResponseCallback(void (*fptr)(String request, String response, bool success)) {
    _responseCallback = fptr;
}

// Override the standard retry count. If set to 0, the
// system will try indefinitely.
void GenericHTTPClient::setRetryCount(uint8_t retryCount) {
    _retryCount = retryCount;
}

// URL request which sets _lastRequest to url
bool GenericHTTPClient::getURL(String url, IPAddress ip, int port) {
    GenericHTTPClient::getURL(url, url, ip, port);
}

// Perform an URL request with the given parameters
// Alias is used for the _lastRequest variable, and 
// identifies the request in callbacks
bool GenericHTTPClient::getURL(String url, String alias, IPAddress ip, int port) {
    int i = 0;
    bool returnFlag;
    while(i++ < _retryCount || _retryCount == 0) {
        returnFlag = _sharedClient.sendRequest(url, _ownerID, ip, port);
        if(returnFlag) {
            _pendingAnswer = true;
            _lastRequest = alias;
            break;
        }
    }
    return returnFlag;
}

// Send a raw (not HTTP) request with the given parameters
bool GenericHTTPClient::sendRaw(String raw, IPAddress ip, int port) {
    int i = 0;
    bool returnFlag;
    while(i++ < _retryCount || _retryCount == 0) {
        returnFlag = _sharedClient.sendRequest(raw, _ownerID, ip, port, true);
        if(returnFlag) {
            _pendingAnswer = true;
            _lastRequest = raw;
            break;
        }
    }
    return returnFlag;
}

unsigned long GenericHTTPClient::lastResponse() {
    return _lastIncomingMsg;
}

// This method checks whether there is any data in the
// incoming buffer
String GenericHTTPClient::receiveData() {
    if(_sharedClient.receiveData(_ownerID)) {
        _resetLastIncomingMsg();
        return _sharedClient.response;
    }
    return "";
}

// Runloop, called by main sketch
void GenericHTTPClient::runLoop() {
    String response = GenericHTTPClient::receiveData();
    // Communicate the received response to a callback function
    if(_responseCallback) {
        _responseCallback(_lastRequest, response, true);
    }
}