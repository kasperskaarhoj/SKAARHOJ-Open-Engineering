#include <BMDSDIControl.h>

/**
 *  Blackmagic Design 3G-SDI Shield Example Sketch
 *    Shield Info Example
 *
 *  This sketch extracts out the various version identifiers from the
 *  Blackmagic Design 3G-SDI Shield control library and hardware.
 *
 *  The version information of the shield and library should be printed to
 *  the serial port when run.
 *
 *  Setup Steps:
 *    1) Connect the Blackmagic Design 3G-SDI Shield to an Arduino board.
 *    2) Build and run the example sketch.
 *    3) Observe the information printed on the Serial Monitor.
 */

const int                shieldAddress = 0x6E;
BMD_SDICameraControl_I2C sdiCameraControl(shieldAddress);

void setup() {
  Serial.begin(9600);

  Serial.println("Blackmagic Design SDI Control Shield");

  Serial.print("Getting shield version info...");
  sdiCameraControl.begin();
  Serial.println("done.\n");

  // Show BMD SDI Control Shield Arduino library version
  BMD_Version libraryVersion = sdiCameraControl.getLibraryVersion();
  Serial.print("Library Version:\t");
  Serial.print(libraryVersion.Major);
  Serial.print('.');
  Serial.println(libraryVersion.Minor);

  // Show shield firmware version
  BMD_Version firmwareVersion = sdiCameraControl.getFirmwareVersion();
  Serial.print("Firmware Version:\t");
  Serial.print(firmwareVersion.Major);
  Serial.print('.');
  Serial.println(firmwareVersion.Minor);

  // Show shield protocol version
  BMD_Version protocolVersion = sdiCameraControl.getProtocolVersion();
  Serial.print("Protocol Version:\t");
  Serial.print(protocolVersion.Major);
  Serial.print('.');
  Serial.println(protocolVersion.Minor);
}

void loop() {
  // Unused
}
