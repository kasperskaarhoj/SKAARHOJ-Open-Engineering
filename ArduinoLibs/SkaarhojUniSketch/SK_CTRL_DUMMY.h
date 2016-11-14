/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay()	{
    Serial << F("SK_MODEL: SK_DUMMY\n");
}

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL()	{
    Serial << F("No hardware set up\n");
	return 0;
}

/**
 * Hardware test
 */
void HWtestL()	{
}

/**
 * Hardware runloop
 */
void HWrunLoop()	{
}

uint8_t HWnumOfAnalogComponents() { return 0; }

int16_t HWAnalogComponentValue(uint8_t num) {
	return 0;
}

void HWanalogComponentName(uint8_t num, char* buffer, uint8_t len) {
	memset(buffer, 0, len);
}

uint16_t *HWMinCalibrationValues(uint8_t num) {
  static uint16_t values[3] = {0,0,0};
  return values;
}
