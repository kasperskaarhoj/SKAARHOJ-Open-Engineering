/*
#define SK_CUSTOM_HANDLER_A
uint16_t customActionHandlerA(const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown, const bool actUp, const uint8_t pulses, const uint16_t value)    {
  if (actDown) {
    Serial << "Custom Handler Invoked for Hardware Component #" << HWc << " on Down event\n";
  }    
  if (actUp) {
    Serial << "Custom Handler Invoked for Hardware Component #" << HWc << " on Up event\n";
  }    

  return ((millis()&128)>0) ? 4 : 5;    // Will blink the button fast
}
*/



/*
#define SK_CUSTOM_HANDLER_B
uint16_t customActionHandlerB(const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown, const bool actUp, const uint8_t pulses, const uint16_t value)    {
  if (actDown) {
    Serial << "Custom Handler Invoked for Hardware Component #" << HWc << " on Down event\n";
  }    
  if (actUp) {
    Serial << "Custom Handler Invoked for Hardware Component #" << HWc << " on Up event\n";
  }    

  return ((millis()&128)>0) ? 4 : 5;    // Will blink the button fast
}
*/



/*
#define SK_CUSTOM_HANDLER_C
uint16_t customActionHandlerC(const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown, const bool actUp, const uint8_t pulses, const uint16_t value)    {
  if (actDown) {
    Serial << "Custom Handler Invoked for Hardware Component #" << HWc << " on Down event\n";
  }    
  if (actUp) {
    Serial << "Custom Handler Invoked for Hardware Component #" << HWc << " on Up event\n";
  }    

  return ((millis()&128)>0) ? 4 : 5;    // Will blink the button fast
}
*/



/*
#define SK_CUSTOM_HANDLER_D
uint16_t customActionHandlerD(const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown, const bool actUp, const uint8_t pulses, const uint16_t value)    {
  if (actDown) {
    Serial << "Custom Handler Invoked for Hardware Component #" << HWc << " on Down event\n";
  }    
  if (actUp) {
    Serial << "Custom Handler Invoked for Hardware Component #" << HWc << " on Up event\n";
  }    

  return ((millis()&128)>0) ? 4 : 5;    // Will blink the button fast
}
*/
