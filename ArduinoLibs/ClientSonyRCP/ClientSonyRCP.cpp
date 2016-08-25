#include "ClientSonyRCP.h"

ClientSonyRCP::ClientSonyRCP() { _hasInitialized = false; }

void ClientSonyRCP::begin() {
}
void ClientSonyRCP::serialOutput(uint8_t level) { _serialOutput = level; }

bool ClientSonyRCP::hasInitialized() { return _hasInitialized; }

void ClientSonyRCP::runLoop() {
}
