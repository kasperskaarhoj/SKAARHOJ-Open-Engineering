#include <BMDSDIControl.h>

/**
 *  Blackmagic Design 3G-SDI Shield Example Sketch
 *    Joystick Shield Example
 *
 *  This sketch demonstrates a more complex example using a custom shield,
 *  which contains a joystick and buttons, to control a connected camera.
 *
 *  The three buttons are mapped as follows:
 *    - Button 1, which changes the tally light on the camera
 *    - Button 2, which cycles through various color adjustments
 *    - Button 3, which triggers automatic focus on the camera
 *
 *  The joystick is mapped as follows:
 *    - X axis adjusts the camera black level
 *    - Y axis adjusts the camera aperture level
 *    - Pressing the Joystick resets all correction parameters and requests
 *      auto-aperture adjustment
 *
 *  Setup Steps:
 *    1) Connect the Blackmagic Design 3G-SDI Shield to an Arduino board.
 *    2) Connect the custom shield to the Arduino board.
 *    3) Attach a camera's SDI input connector to the output SDI connector of
 *       the shield. Configure the camera as camera number 1.
 *    4) Build and run the example sketch.
 */

// Hardware pin mappings
const int             joystickButtonPin    = A0;
const int             joystickXPin         = A1;
const int             joystickYPin         = A2;
const int             button1Pin           = 5;
const int             button2Pin           = 6;
const int             button3Pin           = 7;

// Blackmagic Design SDI control shield globals
const int                 shieldAddress = 0x6E;
BMD_SDITallyControl_I2C   sdiTallyControl(shieldAddress);
BMD_SDICameraControl_I2C  sdiCameraControl(shieldAddress);

// Button debouncing globals
unsigned long         lastStableButtonTime[32];
int                   rawButtonLevels[32];
int                   stableButtonLevels[32];

// Joystick sample rate limiter globals
unsigned long         lastJoystickUpdateTime;

// Camera parameter adjustment state globals
int                   currentColorMode      = 0;
float                 currentApertureAdjust = 0.0;
float                 currentOffsetAdjust   = 0.0;

void setup() {
  // Configure digital inputs
  pinMode(joystickButtonPin, INPUT_PULLUP);
  pinMode(button1Pin       , INPUT_PULLUP);
  pinMode(button2Pin       , INPUT_PULLUP);
  pinMode(button3Pin       , INPUT_PULLUP);

  // Set up the BMD SDI control library
  sdiTallyControl.begin();
  sdiCameraControl.begin();

  // The shield supports up to 400KHz, use faster
  // I2C speed to reduce latency
  Wire.setClock(400000);

  // Enable both tally and control overrides
  sdiTallyControl.setOverride(true);
  sdiCameraControl.setOverride(true);
}

void loop() {
  // If the tally button state has changed, update camera tally
  if (getButtonStableEdge(button1Pin) == true) {
    // When the button is low (pressed) the camera tally is on
    bool newTallyState = getButtonStableLevel(button1Pin) == LOW;

    // Send new tally state to the camera
    sdiTallyControl.setCameraTally(
        1,                   // Camera Number
        newTallyState,       // Program Tally
        false                // Preview Tally
      );
  }

  // If the color mode button is pressed, cycle through different color corrections
  if (getButtonStableEdge(button2Pin) == true && getButtonStableLevel(button2Pin) == LOW) {
    // Go to the next color mode in the sequence
    currentColorMode = currentColorMode + 1;
    if (currentColorMode == 5)
      currentColorMode = 0;

    float newSaturation = 1.0;
    float newGamma      = 0.0;
    float newGain       = 1.0;

    if (currentColorMode == 1) {
      newSaturation = 0.0;   // Black and white
    }
    else if (currentColorMode == 2) {
      newSaturation = 2.0;   // Super-saturated
    }
    else if (currentColorMode == 3) {
      newGamma = 1.0;        // High gamma
    }
    else if (currentColorMode == 4) {
      newGamma = 1.0;        // High gamma
      newGain  = 2.0;        // and Y gain
    }

    // Send the new hue/saturation values to the camera
    float hueSaturationValues[2] = {
        0.0,                 // Hue        (-1.0 to 1.0)
        newSaturation        // Saturation (0.0 to 2.0)
      };
    sdiCameraControl.writeCommandFixed16(
        1,                   // Destination:    Camera 1
        8,                   // Category:       Colour Correction
        6,                   // Param:          Colour Adjust
        0,                   // Operation:      Assign Value
        hueSaturationValues  // Values
      );

    // Send the new gamma values to the camera
    float gammaValues[4] = {
        newGamma,            // Red channel   (-4.0 to 4.0)
        newGamma,            // Green channel (-4.0 to 4.0)
        newGamma,            // Blue channel  (-4.0 to 4.0)
        newGamma             // Luma channel  (-4.0 to 4.0)
      };
    sdiCameraControl.writeCommandFixed16(
        1,                   // Destination:    Camera 1
        8,                   // Category:       Color Correction
        1,                   // Param:          Gamma Adjust
        0,                   // Operation:      Assign Value
        gammaValues          // Values
      );

    // Send the new gain values to the camera
    float gainValues[4] = {
        1.0,                 // Red channel   (0.0 to 16.0)
        1.0,                 // Green channel (0.0 to 16.0)
        1.0,                 // Blue channel  (0.0 to 16.0)
        newGain              // Luma channel  (0.0 to 16.0)
      };
    sdiCameraControl.writeCommandFixed16(
        1,                   // Destination:    Camera 1
        8,                   // Category:       Color Correction
        2,                   // Param:          Gain Adjust
        0,                   // Operation:      Assign Value
        gainValues           // Values
      );
  }

  // If the focus button is pressed, request the camera auto-focus
  if (getButtonStableEdge(button3Pin) == true && getButtonStableLevel(button3Pin) == LOW) {
    // Send the auto-focus request to the camera
    sdiCameraControl.writeCommandVoid(
        1,                   // Destination:    Camera 1
        0,                   // Category:       Lens
        1                    // Param:          Instantaneous Autofocus
      );
  }

  // If the reset button is pressed, clear all color correction values
  if (getButtonStableEdge(joystickButtonPin) == true && getButtonStableLevel(joystickButtonPin) == LOW) {
    // Reset color correction globals
    currentColorMode      = 0;
    currentApertureAdjust = 0.0;
    currentOffsetAdjust   = 0.0;

    // Reset all color correction parameters in the camera
    sdiCameraControl.writeCommandVoid(
        1,                   // Destination:    Camera 1
        8,                   // Category:       Color Correction
        7                    // Param:          Reset Defaults
      );

    // Request auto-iris adjustment in the camera
    sdiCameraControl.writeCommandVoid(
        1,                   // Destination:    Camera 1
        0,                   // Category:       Lens
        5                    // Param:          Auto Iris
      );
  }

  if (getJoystickUpdateReady() == true) {
    // The joystick X axis is the offset adjust
    int currentJoystickX = getJoystickAxisPercent(joystickXPin);
    if (currentJoystickX > 80) {
      currentOffsetAdjust = currentOffsetAdjust - 0.03;
    }
    else if (currentJoystickX > 0) {
      currentOffsetAdjust = currentOffsetAdjust - 0.01;
    }
    else if (currentJoystickX < -80) {
      currentOffsetAdjust = currentOffsetAdjust + 0.03;
    }
    else if (currentJoystickX < 0) {
      currentOffsetAdjust = currentOffsetAdjust + 0.01;
    }

    // The joystick Y axis is the aperture adjust
    int currentJoystickY = getJoystickAxisPercent(joystickYPin);
    if (currentJoystickY > 80) {
      currentApertureAdjust = currentApertureAdjust + 0.01;
    }
    else if (currentJoystickY > 0) {
      currentApertureAdjust = currentApertureAdjust + 0.005;
    }
    else if (currentJoystickY < -80) {
      currentApertureAdjust = currentApertureAdjust - 0.01;
    }
    else if (currentJoystickY < 0) {
      currentApertureAdjust = currentApertureAdjust - 0.005;
    }

    // Don't allow the offset or aperture values to exceed the minimum/maximum values
    currentOffsetAdjust   = constrain(currentOffsetAdjust  , -8.0, 8.0);
    currentApertureAdjust = constrain(currentApertureAdjust,  0.0, 1.0);

    // Send new offset adjustment to the camera
    float offsetValues[] = {
        currentOffsetAdjust,  // Red   (-8.0 to 8.0)
        currentOffsetAdjust,  // Green (-8.0 to 8.0)
        currentOffsetAdjust,  // Blue  (-8.0 to 8.0)
        currentOffsetAdjust   // Luma  (-8.0 to 8.0)
      };
    sdiCameraControl.writeCommandFixed16(
        1,                    // Destination:    Camera 1
        8,                    // Category:       Color Correction
        3,                    // Param:          Offset Adjust
        0,                    // Operation:      Set Absolute,
        offsetValues          // Values
      );

    // Send new aperture adjustment to the camera
    sdiCameraControl.writeCommandFixed16(
        1,                    // Destination:    Camera 1
        0,                    // Category:       Lens
        3,                    // Param:          Aperture (Ordinal)
        0,                    // Operation:      Set Absolute,
        currentApertureAdjust // Values
      );
  }
}

bool getJoystickUpdateReady() {
  // Determines if we are ready for another joystick update (this is
  // a rate limiter to ensure that the user can slowly adjust the
  // parameters easily with the joystick)

  unsigned long currentTime = millis();
  if ((lastJoystickUpdateTime - currentTime) > 100) {
     lastJoystickUpdateTime = currentTime;
     return true;
  }

  return false;
}

int getJoystickAxisPercent(int analogPin) {
  // Reads the joystick axis on the given analog pin as a [-100 - 100] scaled value

  int rawAnalogValue    = analogRead(analogPin);
  int scaledAnalogValue = map(rawAnalogValue, 0, 1023, -100, 100);

  // Consider values close to zero as zero, so that when the joystick is
  // centered it reports zero even if it is slightly mis-aligned
  if (abs(scaledAnalogValue) < 10) {
    scaledAnalogValue = 0;
  }

  return scaledAnalogValue;
}

bool getButtonStableEdge(int digitalPin) {
  // Detects debounced edges (i.e. presses and releases) of a button

  bool previousLevel = stableButtonLevels[digitalPin];
  bool newLevel      = getButtonStableLevel(digitalPin);

  return previousLevel != newLevel;
}

int getButtonStableLevel(int digitalPin) {
  // Reads a digital pin and filters it, returning the stable button position

  int           pinLevel    = digitalRead(digitalPin);
  unsigned long currentTime = millis();

  // If the button is rapidly changing (bouncing) during a press, keep
  // resetting the last stable time count
  if (pinLevel != rawButtonLevels[digitalPin]) {
    lastStableButtonTime[digitalPin] = currentTime;
    rawButtonLevels[digitalPin]      = pinLevel;
  }

  // Once the button has been stable for
  if ((currentTime - lastStableButtonTime[digitalPin]) > 20) {
    stableButtonLevels[digitalPin] = pinLevel;
  }

  return stableButtonLevels[digitalPin];
}
