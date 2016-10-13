#include "ClientMatroxMonarch.h"

ClientMatroxMonarch::ClientMatroxMonarch() { _hasInitialized = false; }

void ClientMatroxMonarch::begin(const IPAddress ip) {}
void ClientMatroxMonarch::serialOutput(uint8_t level) { _serialOutput = level; }

bool ClientMatroxMonarch::hasInitialized() { return _hasInitialized; }

void ClientMatroxMonarch::runLoop(uint16_t delayTime) {}
