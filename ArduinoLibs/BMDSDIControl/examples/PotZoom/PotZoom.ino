#include <BMDSDIControl.h>

/**
 *  Blackmagic Design 3G-SDI Shield Example Sketch
 *    Pot Zoom Example
 *
 *  This sketch demonstrates a camera control command which requests that a
 *  connected camera adjust its zoom level to that of a connected pot.
 *
 *  The zoom on all connected camera(s) should continuously adjust to the
 *  current pot level each time the sketch is run.
 *
 *  Setup Steps:
 *    1) Connect the Blackmagic Design 3G-SDI Shield to an Arduino board.
 *    2) Attach a camera's SDI input connector to the output SDI connector of
 *       the shield. Configure the camera as camera number 1.
 *    3) Attach a pot on analog channel A0.
 *    4) Build and run the example sketch.
 */

const int                shieldAddress = 0x6E;
BMD_SDICameraControl_I2C sdiCameraControl(shieldAddress);

const int                potAnalogChannel = 0;


void setup() {
  sdiCameraControl.begin();

  // Turn on control overrides
  sdiCameraControl.setOverride(true);
}

void loop() {
  static bool everSent = false;
  static float lastScaledZoom = 0;
  static unsigned long lastSentTimeMs = 0;
  static const unsigned long kMinimumTimeBetweenCommandsMs = 60;

  // Read the analog pot value and scale it to [0.0 - 1.0]
  int   rawPotValue = analogRead(potAnalogChannel);
  float scaledZoom  = rawPotValue / 1023.0;

  // Get time elapsed since last sent
  unsigned long timeElapsedSinceLastSentMs = millis() - lastSentTimeMs;

  // Adjust the camera zoom level to the current pot setting
  // Check that a minimum time has passed between commands
  // to make sure the previous command has been processed
  if (! everSent || (lastScaledZoom != scaledZoom && timeElapsedSinceLastSentMs >= kMinimumTimeBetweenCommandsMs))
  {
    setZoomLevelWithSimpleCommand(scaledZoom);
    // setZoomLevelWithManualPacket(scaledZoom);

    everSent = true;
    lastScaledZoom = scaledZoom;
    lastSentTimeMs = millis();
  }
}

void setZoomLevelWithSimpleCommand(float scaledZoom) {
  // This sends an Absolute Zoom (Normalized) adjustment command, with a simple camera control packet.

  sdiCameraControl.writeCommandFixed16(
      1,         // Destination:    Camera 1
      0,         // Category:       Lens
      8,         // Param:          Zoom (Normalised)
      0,         // Operation:      Assign Value
      scaledZoom // Values
    );
}

void setZoomLevelWithManualPacket(float scaledZoom) {
  // This sends an Absolute Zoom (Normalized) adjustment command, with a
  // manually crafted camera control packet (see Blackmagic Design Camera
  // manual).

  // Convert the floating point `scaledZoom` value into a Fixed16 type
  int16_t zoomFixed16 = sdiCameraControl.toFixed16(scaledZoom);

  byte zoomCommand[] = {
    /* Header */
    1,                     // Destination:    Camera 1
    6,                     // Payload Length: 6 bytes (not including any zero padding)
    0,                     // Command:        Set and Update Values
    0,                     // Source:         Unused

    /* Payload */
    0,                     // Category:       Lens
    8,                     // Param:          Zoom (Normalised)
    0,                     // Type:           Void/Bool
    0,                     // Operation:      Assign Value
    lowByte(zoomFixed16),  // Fixed16 Low Byte
    highByte(zoomFixed16), // Fixed16 High Byte

    /* Padding */
    0,                     // Two padding bytes required to make this a multiple of 4 bytes
    0,
  };

  sdiCameraControl.write(zoomCommand);
}
