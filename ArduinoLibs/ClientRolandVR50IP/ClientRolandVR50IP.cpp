#include "ClientRolandVR50IP.h"

ClientRolandVR50IP::ClientRolandVR50IP() { _hasInitialized = false; }

void ClientRolandVR50IP::begin(const IPAddress ip) {}
void ClientRolandVR50IP::serialOutput(uint8_t level) { _serialOutput = level; }

bool ClientRolandVR50IP::hasInitialized() { return _hasInitialized; }

void ClientRolandVR50IP::runLoop(uint16_t delayTime) {}
