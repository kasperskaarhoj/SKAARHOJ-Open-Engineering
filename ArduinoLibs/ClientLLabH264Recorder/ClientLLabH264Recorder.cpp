#include "ClientLLabH264Recorder.h"

ClientLLabH264Recorder::ClientLLabH264Recorder(){};

// Initialize the H264 Recorder object
void ClientLLabH264Recorder::begin(HTTPSharedClientWrapper sharedClient, uint8_t ownerID, IPAddress ip) {
	_ip = ip;
	GenericHTTPClient::begin(sharedClient, ownerID);
}

// SERVER COMMANDS

// These methods run requests the the H.264 Recorder server, which do not require
// response. The method names should explain themselves.

bool ClientLLabH264Recorder::startRecording() {
	return GenericHTTPClient::sendRaw(String("START_RECORDING"), _ip, PORT);
}
bool ClientLLabH264Recorder::stopRecording() {
	return GenericHTTPClient::sendRaw(String("STOP_RECORDING"), _ip, PORT);
}
bool ClientLLabH264Recorder::startStreaming() {
	return GenericHTTPClient::sendRaw(String("START_STREAMING"), _ip, PORT);
}
bool ClientLLabH264Recorder::stopStreaming() {
	return GenericHTTPClient::sendRaw(String("STOP_STREAMING"), _ip, PORT);
}
bool ClientLLabH264Recorder::startAutoSnapshots() {
	return GenericHTTPClient::sendRaw(String("START_AUTOSNAPSHOTS"), _ip, PORT);
}
bool ClientLLabH264Recorder::stopAutoSnapshots() {
	return GenericHTTPClient::sendRaw(String("STOP_AUTOSNAPSHOTS"), _ip, PORT);
}
bool ClientLLabH264Recorder::takeSnapshot() {
	return GenericHTTPClient::sendRaw(String("TAKE_SNAPSHOT"), _ip, PORT);
}
bool ClientLLabH264Recorder::clearUserData() {
	return GenericHTTPClient::sendRaw(String("CLEAR_USER"), _ip, PORT);
}

// STATE VARIABLE GETTERS

// The responses to these getters is received through the defined callback method
// Also, the current state _before_ the request is returned.
bool ClientLLabH264Recorder::isRecording() {
	GenericHTTPClient::sendRaw("IS_RECORDING", _ip, PORT);
	return _isRecording;
}

bool ClientLLabH264Recorder::isStreaming() {
	GenericHTTPClient::sendRaw("IS_STREAMING", _ip, PORT);
	return _isRecording;
}

bool ClientLLabH264Recorder::isAutoSnapshot() {
	GenericHTTPClient::sendRaw("IS_AUTOSNAPSHOTS_RUNNING", _ip, PORT);
	return _isAutoSnapshot;
}

bool ClientLLabH264Recorder::isUSBRecording() {
	GenericHTTPClient::sendRaw("IS_USB_RECORDING", _ip, PORT);
	return _isUSBRecording;
}
bool ClientLLabH264Recorder::isUSBAvailable() {
	GenericHTTPClient::sendRaw("IS_USB_AVAILABLE", _ip, PORT);
	return _isUSBAvailable;
}
String ClientLLabH264Recorder::getUsername() {
	GenericHTTPClient::sendRaw("GET_USER", _ip, PORT);
 	return _username;
}

String ClientLLabH264Recorder::getEmail() {
	GenericHTTPClient::sendRaw("GET_EMAIL", _ip, PORT);
	return _email;
}

// STATE VARIABLE SETTERS

// Used to set variables on the H.264 Recorder server

bool ClientLLabH264Recorder::setUsername(String username) {
	return GenericHTTPClient::sendRaw("GET_USER:username=" + username, _ip, PORT);
}

bool ClientLLabH264Recorder::setEmail(String email) {
	return GenericHTTPClient::sendRaw("GET_EMAIL:email=" + email, _ip, PORT);
}

bool ClientLLabH264Recorder::setFirstname(String firstname) {
	return GenericHTTPClient::sendRaw(":first=" + firstname, _ip, PORT);
}

void ClientLLabH264Recorder::runLoop() {
	String response = GenericHTTPClient::receiveData();
	// The response is parsed, and action is taken according to the last command executed
	if (response && response != "") {
		// Remove trailing newline from response
		if(response[response.length()-1] == '\n') {
			response = response.substring(0,response.length() - 1);
		}
		// Boolean state. Only useful if the command in question can only return TRUE/FALSE
		bool state = response.startsWith("TRUE");

		if(_lastRequest == "IS_RECORDING") {
			_isRecording = state;
		} else 
		if(_lastRequest == "IS_STREAMING") {
			_isStreaming = state;
		} else
		if (_lastRequest == "IS_AUTOSNAPSHOTS_RUNNING") {
			_isAutoSnapshot = state;
		} else
		if (_lastRequest == "IS_USB_AVAILABLE") {
			_isUSBAvailable = state;
		} else
		if (_lastRequest == "IS_USB_RECORDING") {
			_isUSBRecording = state;
		} else
		if (_lastRequest == "GET_USER") {
			_username = response;
		} else
		if (_lastRequest == "GET_EMAIL") {
			_email = response;
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