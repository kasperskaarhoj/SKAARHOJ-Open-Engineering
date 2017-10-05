/*
Copyright 2016 Filip Sandborg-Olsen, SKAARHOJ, filip@skaarhoj.com

This file is part of the ClientAJAKumoTCP library for Arduino

The ClientAJAKumoTCP library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ClientAJAKumoTCP library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ClientAJAKumoTCP library. If not, see http://www.gnu.org/licenses/.

*/

#include "ClientAJAKumoTCP.h"

ClientAJAKumoTCP::ClientAJAKumoTCP() {};

void ClientAJAKumoTCP::begin(IPAddress ip) {
	_IP = ip;
	_sessionID = 0;

	_inputPos = 0;
	_isConnected = false;
	_lastConnectionAttempt = 0;
	_lastRoutingUpdate = 0;
	_updatePointer = 0;

	_serialOutput = 0;
	_receivedRouting = 0;
	_bundleEnabled = false;
	_outputPos = 0;

	memset(routingState, 0xFF, ClientAJAKumoTCP_NUMOUTPUTS);
}

bool ClientAJAKumoTCP::isConnected() {
	return _isConnected;
}

void ClientAJAKumoTCP::serialOutput(uint8_t level) {
	_serialOutput = level;
}

uint16_t ClientAJAKumoTCP::calculateChecksum(uint8_t* packet, uint8_t len) {
	uint16_t retVal = 0;
	uint16_t sum = 0;
	uint8_t idx = 1;
	while(idx++ < len) sum += packet[idx-1];
	sum = 256 - (sum & 255);

	retVal = toASCII(sum >> 4) << 8;
	retVal |= toASCII(sum & 0xF);

	return retVal;
}

void ClientAJAKumoTCP::transmitPacket(char* data, bool useBuffer) {
	if(useBuffer) {
		_client.write(_outputBuffer, _outputPos);
		_lastTransmit = millis();
		_outputPos = 0;
		return;
	}

	uint8_t len = strlen(data);
	if(len > 100) {
		Serial << "Output too large! " << len << " bytes\n";
		return;
	}

	uint8_t *_buffer = (uint8_t*)malloc(6 + len);

	// Prepare packet
	_buffer[0] = 0x01; // SOH
	_buffer[1] = 'N'; // Protocol ID
	_buffer[2] = toASCII(_sessionID & 0xF);

	for(uint8_t i = 0; i < len; i++) {
		if(data[i] == ',') {
			_buffer[3+i] = 0x09; // HT
		} else {
			_buffer[3+i] = data[i];
		}
	}

	uint16_t checksum = calculateChecksum(_buffer, 3+len);

	_buffer[3+len] = checksum >> 8;
	_buffer[4+len] = checksum & 0xFF;

	_buffer[5+len] = 0x04; // EOT

	if(_client.connected()) {
		if(_serialOutput > 1) {
			Serial << "Outgoing: ";
			for(uint8_t j=0; j<6+len; j++) {
				Serial << _HEXPADL(_buffer[j], 2, "0") << ":";
			}
			Serial << "\n";
		}

		if(_bundleEnabled) {
			if(_outputPos + 6 + len > 255) {
				transmitPacket(0, true);
			}
			memcpy(_outputBuffer + _outputPos, _buffer, 6 + len);
			_outputPos += 6 + len;
		} else {
			_client.write(_buffer, 6 + len);
		}
	} else {
		Serial << "UNCONNECTED!\n";
	}
	free(_buffer);
}

void ClientAJAKumoTCP::routeInputToOutput(uint8_t input, uint8_t output, bool wait) {
	char buffer[10];
	sprintf(buffer, "TI,%X,%X", output, input);
	transmitPacket(buffer);

	if (wait)	{
	  uint32_t timer = millis();
	  while(getRoute(output) != input && millis()-200 < timer)	{
	  	runLoop();
	  }
  }
}

char ClientAJAKumoTCP::toASCII(uint8_t c) {
	if(c >= 0 && c <= 9) return '0' + c;
	if(c >= 10 && c <= 15) return 'A' + (c - 10);

	// Just to return something
	return '0';
}

uint8_t ClientAJAKumoTCP::fromASCII(char c) {
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'A' && c <= 'F') return 10 + c - 'A';
	if(c >= 'a' && c <= 'f') return 10 + c - 'a';

	return 0;
}

uint16_t ClientAJAKumoTCP::hexToDec(char* str, uint8_t len) {
	uint16_t value = 0;
	for(uint8_t i = 0; i < len; i++) {
		value |= fromASCII(str[len-i-1]) << (i*4);
	}
	return value;
}

void ClientAJAKumoTCP::handleCmd(char *cmd, char* parameter, char* data) {
	uint16_t command = (cmd[0] << 8) | cmd[1];
	switch(command) {
		case 'QI': { // Output routing info
			uint8_t output = hexToDec(parameter, 4);
			uint8_t input = hexToDec(data + strlen(data) - 4, 4);
			uint8_t lock = (data[6] == 'P');
			_receivedRouting = output-1;

			if(output <= ClientAJAKumoTCP_NUMOUTPUTS) {
				if(input && output && routingState[output-1] != input-1) {
					if(_serialOutput > 0)
						Serial << "Route changed: " << input << " --> " << output << "\n";
					routingState[output-1] = input-1;
					_outputLocks[output-1] = lock;
				}
			}

			break;
		}
		case 'QN': {
			uint8_t state = 0;
			char name[21];
			uint8_t src = 0;
			char* nameData = data + 3;
			char* token;
			bool isSource = parameter[0] == 'S';
			while((token = strsep(&nameData, "\t")) != NULL) {
				state++;
				switch(state) {
					case 1:
						//memset(name, 0, 21);
						strncpy(name, token, 20);
						break;
					case 2:
						src = atoi(token);
						break;
					case 4:
						if(_serialOutput > 1) {
							Serial << (isSource?"Source #":"Dest #") << src << ": " << name << "\n";
						}
						if(isSource) {
							if(src > 0 && src <= ClientAJAKumoTCP_LABELCOUNT) {
								strncpy(_sourceNames[src - 1], name, 20);
							}
						} else {
							if(src > 0 && src <= ClientAJAKumoTCP_LABELCOUNT) {
								strncpy(_destNames[src - 1], name, 20);
							}
						}
						state = 0;
						break;
				}
			}
			break;
		}
		default:
			if(_serialOutput > 1) 
				Serial << "Unhandled incoming " << cmd << "," << parameter << " = " << data << "\n";
	}
}

uint8_t ClientAJAKumoTCP::getRoute(uint8_t output) {
	return routingState[output-1]+1;
}

void ClientAJAKumoTCP::parseIncoming(uint8_t *buffer, uint8_t len) {
	if(buffer[0] == 0x01 && buffer[len-1] == 0x04) {
		uint16_t checksum = calculateChecksum(buffer, len-3);
		if(checksum == (buffer[len-3]<<8) | buffer[len-2]) {
			if(buffer[3] == 'E' && buffer[4] == 'R') {
				if(buffer[6] == '0' && buffer[7] == '0') {
					if(_serialOutput > 1) {
						Serial << "OK\n";
					}
				} else {
					if(_serialOutput > 1) {
						Serial << "Received error response from AJA KUMO\n";
					}
				}
			} else {
					char cmd[3];
					char parameter[11];
					char data[200];

					memset(cmd, 0, 3);
					memset(parameter, 0, 11);
					memset(data, 0, 51);

					cmd[0] = buffer[4];
					cmd[1] = buffer[3];

					uint8_t i = 5;
					uint8_t pos = 0;
					while(i++ < len-3) {
						if(buffer[i] == 0x09 || pos++ == 10) {
							strncpy(parameter, (char*)(buffer+i-pos), pos);
							break;
						}	
					}

					
					strncpy(data, (char*)(buffer+i), len-i-4 > 199 ? 199 : len-i-4);

					handleCmd(cmd, parameter, data);
			}
		} else {
			if(_serialOutput > 1) {
				Serial << "Checksum mismatch!\n";
			}
		}
	} else {
		if(_serialOutput > 1) 
			Serial << "ERROR: Malformed packet\n";
	}
}

void ClientAJAKumoTCP::updateRouting(uint8_t num) {
	char buffer[10];
	if(num < ClientAJAKumoTCP_NUMOUTPUTS) {
		sprintf(buffer, "QI,%X", num+1);
		transmitPacket(buffer);
	}
}

void ClientAJAKumoTCP::startBundle() {
	_bundleEnabled = true;
}

void ClientAJAKumoTCP::endBundle() {
	_bundleEnabled = false;
	if(_outputPos > 0) {
		transmitPacket(0, true);
	}
}

void ClientAJAKumoTCP::receiveData() {
	_inputPos = 0;
	memset(_inputBuffer, 0, ClientAJAKumoTCP_INPUTBUFFER_SIZE);
	while(_client.available() && _inputPos < ClientAJAKumoTCP_INPUTBUFFER_SIZE) {
		char c = _client.read();
		_inputBuffer[_inputPos++] = c;
		if(_serialOutput > 1) 
			Serial << _HEXPADL(c, 2, "0") << ":";
		
		if(c == 0x04) {
			if(_serialOutput > 1) {
				Serial << " : " << (char*)_inputBuffer << "\n";
			}

			parseIncoming(_inputBuffer, _inputPos);
			_inputPos = 0;
		}
	}
}

char* ClientAJAKumoTCP::getInputLabel(uint8_t input) {
	char label[10];
	snprintf(label, 9, "Input %d", input);

	return (input > 0 && input <= ClientAJAKumoTCP_LABELCOUNT) ? _sourceNames[input-1] : label;
}

char* ClientAJAKumoTCP::getOutputLabel(uint8_t output) {
	char label[10];
	snprintf(label, 9, "Dest %d", output);

	return (output > 0 && output <= ClientAJAKumoTCP_LABELCOUNT) ? _destNames[output-1] : label;
}

bool ClientAJAKumoTCP::getLock(uint8_t output) {
	return _outputLocks[output - 1]; 
}

void ClientAJAKumoTCP::setLock(uint8_t output) {

}

void ClientAJAKumoTCP::runLoop(bool noWait) {
	if(_client.connected()) {
		receiveData();

		if(millis() - _lastRoutingUpdate > 300 || noWait) {
			if(_receivedRouting == _updatePointer || millis() - _lastSingleRouteUpdate > 50 || noWait) {
				_updatePointer = ++_updatePointer % ClientAJAKumoTCP_NUMOUTPUTS;
				updateRouting(_updatePointer);
				_lastSingleRouteUpdate = millis();
			}

			if(_updatePointer == 0) {
				_lastRoutingUpdate = millis();
			}
		}

		if(millis() - _lastNameUpdate > 2000) {
			startBundle();
			transmitPacket("QN,IS"); // Query source names
			transmitPacket("QN,ID"); // Query destination names
			endBundle();

			_lastNameUpdate = millis();
		}
	} else {
		_client.stop();
		if(millis() - _lastConnectionAttempt >  1000) {
			if(_client.connect(_IP, 12345)) {
				//if(_serialOutput > 1)
					Serial.println("Connected to AJA KUMO!");
				_isConnected = true;
			} else {
				//if(_serialOutput > 1)
					Serial.println("Could not connect to AJA KUMO");
				_isConnected = false;
			}
			_lastConnectionAttempt = millis();
		}
	}
}