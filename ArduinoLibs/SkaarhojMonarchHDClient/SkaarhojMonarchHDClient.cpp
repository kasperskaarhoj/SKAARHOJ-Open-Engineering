#include "SkaarhojMonarchHDClient.h"

SkaarhojMonarchHDClient::SkaarhojMonarchHDClient(){};

// Initialize the Monarch HD object
//void SkaarhojMonarchHDClient::begin(HTTPSharedClientWrapper sharedClient, uint8_t ownerID, IPAddress ip, String authStr) {
void SkaarhojMonarchHDClient::begin(HTTPSharedClientWrapper sharedClient, uint8_t ownerID, IPAddress ip) {
	_ip = ip;
	//_authStr = authStr;
	_rtmp = "";
	_rtsp = "";
	_streamingVideoDataRate = 0;


	GenericHTTPClient::begin(sharedClient, ownerID);
}

bool SkaarhojMonarchHDClient::sendCommand(String cmd, IPAddress ip, int port) {
	return GenericHTTPClient::getURL(String("/Monarch/syncconnect/sdk.aspx?command=") + cmd, cmd, _ip, port);
	// If AUTH is needed
	//return GenericHTTPClient::getURL(String("/Monarch/syncconnect/sdk.aspx?command=") + cmd, _authStr, cmd, _ip, port);
}

// SERVER COMMANDS

// These methods run requests the the Monarch HD, which do not require
// response. The method names should explain themselves.

bool SkaarhojMonarchHDClient::startRecording() {
	return sendCommand(String("StartRecording"), _ip, PORT);
}
bool SkaarhojMonarchHDClient::stopRecording() {
	return sendCommand(String("StopRecording"), _ip, PORT);
}
bool SkaarhojMonarchHDClient::startStreaming() {
	return sendCommand(String("StartStreaming"), _ip, PORT);
}
bool SkaarhojMonarchHDClient::stopStreaming() {
	return sendCommand(String("StopStreaming"), _ip, PORT);
}

bool SkaarhojMonarchHDClient::getStatus() {
	return sendCommand(String("GetStatus"), _ip, PORT);
}

bool SkaarhojMonarchHDClient::startStreamingAndRecording() {
	return sendCommand(String("StartStreamingAndRecording"), _ip, PORT);
}

bool SkaarhojMonarchHDClient::stopStreamingAndRecording() {
	return sendCommand(String("StopStreamingAndRecording"), _ip, PORT);
}

bool SkaarhojMonarchHDClient::setStreamingVideoDataRate(uint16_t rate) {
	return sendCommand(String("SetStreamingVideoDataRate") + String(rate), _ip, PORT);
}

uint16_t SkaarhojMonarchHDClient::getStreamingVideoDataRate() {
	sendCommand(String("GetStreamingVideoDataRate"), _ip, PORT);
	return _streamingVideoDataRate;
}

// Will request a new value, and return the current
String SkaarhojMonarchHDClient::getRTMP() {
	sendCommand(String("GetRTMP"), _ip, PORT);
	return _rtmp;
}

// Will request a new value, and return the current
String SkaarhojMonarchHDClient::getRTSP() {
	sendCommand(String("GetRTSP"), _ip, PORT);
	return _rtsp;
}

bool SkaarhojMonarchHDClient::setRTMP(String rtmp, String name, String username, String password) {
	if(username.length() > 0 && password.length() > 0) {
		return setRTMP(rtmp + "," + name + "," + username + "," + password);
	} else {
		return setRTMP(rtmp + "," + name);
	}
}

bool SkaarhojMonarchHDClient::setRTMP(String rtmp) {
	return sendCommand(String("SetRTMP,") + rtmp, _ip, PORT);
}

bool SkaarhojMonarchHDClient::setRTSP(String rtsp) {
	return setRTSP(rtsp, 8554);
}

// Port MUST be either 8554 or 554
bool SkaarhojMonarchHDClient::setRTSP(String rtsp, int port) {
	return sendCommand(String("SetRTSP,") + rtsp + "," + String(port), _ip, PORT);
}


// STATE VARIABLE GETTERS

// The responses to these getters is received through the defined callback method
// Also, the current state _before_ the request is returned.
bool SkaarhojMonarchHDClient::isRecording() {
	getStatus();
	return _isRecording;
}

bool SkaarhojMonarchHDClient::isStreaming() {
	getStatus();
	return _isStreaming;
}

void SkaarhojMonarchHDClient::runLoop() {
	String response = GenericHTTPClient::receiveData();
	// The response is parsed, and action is taken according to the last command executed
	if (response && response != "") {
		// Remove trailing newline from response
		if(response[response.length()-1] == '\n') {
			response = response.substring(0,response.length() - 1);
		}
		// Boolean state. Only useful if the command in question can only return TRUE/FALSE
		bool isTrue = response.startsWith("TRUE");

		if(_lastRequest == "GetStatus") {
			int state = 0;
			int resultPointer = 0;
			int startPointer = 0;

			while (resultPointer < response.length()) {
				char c = response.charAt(resultPointer);
				switch(state) {
					Serial.println(state);
					case 0:
						resultPointer = 6;
						startPointer = 7;
						state = 1;
						break;
					case 1: // Record state
						if(startPointer == resultPointer) {
							_isRecording = (c == 'O'); // If the first char is O (as in ON), recording is on
						}
						if(c == ',') {
							resultPointer = resultPointer + 6;
							startPointer = resultPointer + 7;
							state = 2;
						}
						break;
					case 2:
						if(c == ',') {
							startPointer = resultPointer + 1;
							state = 3;
						}
						break;
					case 3:
						if(startPointer == resultPointer) {
							_isStreaming = (c == 'O'); // If the first char is O (as in ON), streaming is on
						}
						break;
				}
				resultPointer++;
			}

			Serial.print("Is streaming: ");
			Serial.print(_isStreaming);
			Serial.print(" Is Recording: ");
			Serial.println(_isRecording);
		} else if(_lastRequest == "GetRTMP") {
			_rtmp = response;
		} else if(_lastRequest == "GetRTSP") {
			_rtsp = response;
		} else if(_lastRequest == "GetStreamingVideoDatarate") {
			_streamingVideoDataRate = response.toInt();
		}
		
		// Communicate the received response to a callback function
		if(_responseCallback) {
			_responseCallback(_lastRequest, response, true);
		}

		if(_serialOutput > 1) {
			Serial.print("Response to '" + _lastRequest + "' : ");
			Serial.println(response);
		}
	}
}