#ifndef ClientRolandVR50IP_h
#define ClientRolandVR50IP_h

#define ClientRolandVR50IP_Cams 1

#include <Arduino.h>
#include <EthernetUdp.h>

class ClientRolandVR50IP {
private:
  bool _hasInitialized;
  IPAddress _switcherIP; // IP address of the switcher
  uint8_t _serialOutput;


public:
  ClientRolandVR50IP();

  void serialOutput(uint8_t level);
  bool hasInitialized();

  void begin(const IPAddress ip);
  void runLoop(uint16_t delayTime = 0);
};

#endif
