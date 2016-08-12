#include <BMDSDIControl.h>

/**
 *  Blackmagic Design 3G-SDI Shield Example Sketch
 *    Auto Iris Example
 *
 *  This sketch demonstrates a camera control command which requests that a
 *  connected camera automatically configures its iris state.
 *
 *  The iris on all connected camera(s) should automatically adjust to the
 *  optimal level for the current conditions each time the sketch is run.
 *
 *  Setup Steps:
 *    1) Connect the Blackmagic Design 3G-SDI Shield to an Arduino board.
 *    2) Attach a camera's SDI input connector to the output SDI connector of
 *       the shield. Configure the camera as camera number 1.
 *    3) Build and run the example sketch.
 */

const int                 shieldAddress = 0x6E;
BMD_SDICameraControl_I2C  sdiCameraControl(shieldAddress);

void setup() {
  sdiCameraControl.begin();

  // Turn on control overrides
  sdiCameraControl.setOverride(true);

  // Send an auto iris adjustment command
  autoIrisWithSimpleCommand();
  // autoIrisWithManualPacket();
}

void loop() {
  // Unused
}

void autoIrisWithSimpleCommand() {
  // This sends an Auto Iris adjustment command, with a simple camera control packet.

  sdiCameraControl.writeCommandVoid(
      1,   // Destination:    Camera 1
      0,   // Category:       Lens
      5    // Param:          Auto Iris
    );
}

void autoIrisWithManualPacket() {
  // This sends an Auto Iris adjustment command, with a manually crafted camera
  // control packet (see Blackmagic Design Camera manual).

  const byte autoIrisCommand[] = {
    /* Header */
    1,     // Destination:    Camera 1
    5,     // Payload Length: 5 bytes (not including any zero padding)
    0,     // Command:        Set and Update Values
    0,     // Source:         Unused

    /* Payload */
    0,     // Category:       Lens
    5,     // Param:          Auto Iris
    0,     // Type:           Void/Bool
    0,     // Operation:      Assign Value
    0,     // Dummy parameter byte, required for void type commands

    /* Padding */
    0,     // Three padding bytes required to make this a multiple of 4 bytes
    0,
    0,
  };

  sdiCameraControl.write(autoIrisCommand);
}
