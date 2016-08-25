#ifndef ClientSonyRCP_h
#define ClientSonyRCP_h

#define ClientSonyRCP_Cams 1

#include <Arduino.h>

class ClientSonyRCP {
private:
  bool _hasInitialized;
  uint8_t _serialOutput;


public:
  ClientSonyRCP();

  void serialOutput(uint8_t level);
  bool hasInitialized();

  void begin();
  void runLoop();
};

#endif
