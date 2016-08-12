Blackmagic Design - 3G SDI Control Shield Library
=================================================

This document describes the interface into the Blackmagic Design 3G SDI Control Shield Library.



Setup Requirements:
-------------------

Any Arduino compatible with the official Wiring library is supported. Note that only the I2C interface of the shield is supported by this library. The Blackmagic Design 3G SDI Control Shield should be inserted to the Arduino before use. At a minimum, the SDI output connector should be connected to a Blackmagic Design camera which supports device control over SDI.



Required Reading:
-----------------

To fully utilize this library, you should install the latest version of the [Blackmagic Design Camera software](http://www.blackmagicdesign.com) and reference the _"Blackmagic Video Device Embedded Control Protocol"_ section of the _"Developer Information"_ chapter in the camera manual. This text outlines the low level packet structure and command values which may be sent to a compatible Blackmagic Design camera.



High Level Overview:
--------------------

The library provides two core objects, `BMD_SDITallyControl_*` and `BMD_SDICameraControl_*`, which can be used to interface with the shield's tally and camera control functionalities. Either (or both) of these objects can be created in your sketch to issue camera control commands, or read and write tally data respectively over a supported physical communication bus to the shield. When creating these objects in your sketch, replace the `*` with the physical bus name (see sections below).

### I2C Interface

To use the **I2C** interface to the shield:

```
// NOTE: Must match address set in the setup utility software
const int                shieldAddress = 0x6E;
BMD_SDICameraControl_I2C sdiCameraControl(shieldAddress);
BMD_SDITallyControl_I2C  sdiTallyControl(shieldAddress);
```

### Serial Interface

To use the **Serial** interface to the shield:

```
BMD_SDICameraControl_Serial sdiCameraControl;
BMD_SDITallyControl_Serial  sdiTallyControl;
```

Note that the library will configure the Arduino serial interface at the required 38400 baud rate. If the Serial Monitor is used, some binary data will be visible as the IDE will be unable to distinguish between user messages and shield commands. Ensure that you configure the Serial Monitor to the correct baud rate to be able to view serial data printed by your sketch.

### Example Usage

Once created in a sketch, these objects will allow you to issue commands to the shield over selected bus by calling functions on the created object(s). A minimal sketch that uses the library via the I2C bus is shown below.

```
// NOTE: Must match address set in the setup utility software
const int                shieldAddress = 0x6E;
BMD_SDICameraControl_I2C sdiCameraControl(shieldAddress);
BMD_SDITallyControl_I2C  sdiTallyControl(shieldAddress);

void setup() {
  // Must be called before the objects can be used
  sdiCameraControl.begin();
  sdiTallyControl.begin();

  // Turn on camera control overrides in the shield
  sdiCameraControl.setOverride(true);

  // Turn on tally overrides in the shield
  sdiTallyControl.setOverride(true);
}

void loop() {
	// Unused
}
```

The list of functions that may be called on the created objects are listed further on in this document. Note that before use, you must call the **begin** function on each object before issuing any other commands.

Some example sketches demonstrating this library are included; open the from the Arduino IDE's **File**->**Examples**->**BMDSDIControl** menu.



**Common** API:
----------------------------

The following functions are supported in this version of the library, for both `BMD_SDITallyControl` and `BMD_SDICameraControl` objects:


#### begin()

Initializes the shield ready for use. This must be called before any other functions of the library are used.


#### setOutputSignalOverride(bool enabled)

Enables or disables the internal output generator of the shield, which overrides the input signal. When enabled, the shield will generate an internal signal on the output SDI connector, rather than passing through the input signal.


#### getOutputSignalOverride()

Determines the current internal output generator state of the shield, which overrides the input signal. When enabled, this will return `true`, when disabled, this will return `false`.


#### getLibraryVersion()

Retrieves the current Arduino library version. This returns a `BMD_Version` type structure, which contains `Major` and `Minor` version components.


#### getFirmwareVersion()

Retrieves the current shield firmware version. This returns a `BMD_Version` type structure, which contains `Major` and `Minor` version components.


#### getProtocolVersion()

Retrieves the current shield protocol version. This returns a `BMD_Version` type structure, which contains `Major` and `Minor` version components.



**BMD_SDITallyControl** API:
----------------------------

The following functions are supported in this version of the library, for `BMD_SDITallyControl` objects:


#### setOverride(bool enabled)

Enables or disables overridden tally data on the SDI output connector . If disabled, incoming tally data is passed through unmodified from the SDI input. If enabled, custom tally data can be written to the SDI output.


#### availableForWrite()

Determines if the shield is ready to accept a new data packet for writing. This will return `true` if the shield has successfully sent the last set tally data in at least one video frame, and `false` if the last written tally data is yet to be sent.


#### write(byte data[])

Writes an array of custom tally data to the shield, which will be sent in all future video frames while tally overrides are enabled.


#### write(byte data[], int dataLength)

Writes an array of custom tally data to the shield, which will be sent in all future video frames while tally overrides are enabled. This variant requires the length of the array to be given explicitly.


#### flushWrite()

Waits until any written Tally data is completely sent to the shield, and sent out via SDI on at least one video frame.


#### reset()

Resets the tally override data to the current tally input data. Note that this does not enable tally overrides.


#### getCameraTally(int cameraNumber, bool& programTally, bool& previewTally)

Convenience function to read the tally states of a single camera from the shield. The passed in `programTally` and `previewTally` variables are updated with the current tally state for the given camera. This will return `true` if the camera's tally state was read from the shield successfully, and `false` if an out-of-range camera number was supplied or if no tally data is available.


#### setCameraTally(int cameraNumber, bool programTally, bool previewTally)

Convenience function to update the tally states of a single camera, and sends it to the shield.


#### available()

Determines if the shield is ready to accept a new tally packet for reading. This will return `true` if the shield has successfully received a new tally packet in least one video frame, and `false` if the no new tally data has been received since the last data was read.


#### read(byte data[])

Reads out the tally data from the shield from the last received video frame since the previous data was read. Returns the number of bytes of data that were read, or -1 if there was insufficient space in the provided array.


#### read(byte data[], int dataLength)

Reads out the tally data from the shield from the last received video frame since the previous data was read. This variant requires the length of the array to be given explicitly. Returns the number of bytes of data that were read, or -1 if there was insufficient space in the provided array.


#### flushRead()

Discards any received Tally data and requests that shield receive new Tally data from the next video frame.



**BMD_SDICameraControl** API:
-----------------------------

The following functions are supported in this version of the library, for `BMD_SDICameraControl` objects:



#### setOverride(bool enabled)

Enables or disables overridden Control data on the SDI output connector. If disabled, incoming control data is passed through unmodified from the SDI input. If enabled, custom control data can be written to the SDI output.


#### availableForWrite()

Determines if the shield is ready to accept a new control packet for writing. This will return `true` if the shield has successfully sent the last set control data in a video frame, and `false` if the last written control data is yet to be sent.


#### write(byte data[])

Writes an array of custom control data to the shield, which will be sent in a future video frame whilst control overrides are enabled.


#### write(byte data[], int dataLength)

Writes an array of custom control data to the shield, which will be sent in a future video frame whilst control overrides are enabled. This variant requires the length of the array to be given explicitly.


#### flushWrite()

Waits until any written Control data is completely sent to the shield, and sent out via SDI on at least one video frame.


#### writeCommandVoid(byte camera, byte category, byte parameter)

Convenience function for `write()`, which constructs a single command message with the given camera, category and parameter values and sends it to the shield. This function should be used for _Void_ type parameters which have no associated value.


#### writeCommandBool(byte camera, byte category, byte parameter, byte operation, bool value)

Convenience function for `write()`, which constructs a single command message with the given camera, category and parameter values and sends it to the shield. This function should be used for _Boolean_ type parameters which have a true or false value.


#### writeCommandInt8(byte camera, byte category, byte parameter, byte operation, int8_t value)

Convenience function for `write()`, which constructs a single command message with the given camera, category, parameter, operation and value and sends it to the shield. This function should be used for _Int8_ commands which accept signed 8-bit value arguments.


#### writeCommandInt8(byte camera, byte category, byte parameter, byte operation, int8_t values[])

Variant of `writeCommandInt8()` which accepts an array of values, rather than a single value.


#### writeCommandInt16(byte camera, byte category, byte parameter, byte operation, int16_t value)

Convenience function for `write`, which constructs a single command message with the given camera, category, parameter, operation and value and sends it to the shield. This function should be used for _Int16_ commands which accept signed 16-bit value arguments.


#### writeCommandInt16(byte camera, byte category, byte parameter, byte operation, int16_t values[])

Variant of `writeCommandInt16()` which accepts an array of values, rather than a single value.


#### writeCommandInt32(byte camera, byte category, byte parameter, byte operation, int32_t value)

Convenience function for `write()`, which constructs a single command message with the given camera, category, parameter, operation and value and sends it to the shield. This function should be used for _Int32_ commands which accept signed 32-bit value arguments.


#### writeCommandInt32(byte camera, byte category, byte parameter, byte operation, int32_t values[])

Variant of `writeCommandInt32()` which accepts an array of values, rather than a single value.


#### writeCommandInt64(byte camera, byte category, byte parameter, byte operation, int64_t value)

Convenience function for `write()`, which constructs a single command message with the given camera, category, parameter, operation and value and sends it to the shield. This function should be used for _Int64_ commands which accept signed 64-bit value arguments.


#### writeCommandInt64(byte camera, byte category, byte parameter, byte operation, int64_t values[])

Variant of `writeCommandInt64()` which accepts an array of values, rather than a single value.


#### writeCommandUTF8(byte camera, byte category, byte parameter, byte operation, const char value[])

Convenience function for `write()`, which constructs a single command message with the given camera, category, parameter, operation and UTF8 string and sends it to the shield. This function should be used for _UTF8_ commands which accept a UTF8 string.


#### writeCommandUTF8(byte camera, byte category, byte parameter, byte operation, const String value)

Variant of `writeCommandUTF8()` which accepts an Arduino `String` object, rather than a _char_ array.


#### writeCommandFixed16(byte camera, byte category, byte parameter, byte operation, float value)

Convenience function for `write()`, which constructs a single command message with the given camera, category, parameter, operation and value and sends it to the shield. This function should be used for _Fixed16_ commands which accept 16-bit fixed-point value arguments.


#### writeCommandFixed16(byte camera, byte category, byte parameter, byte operation, float values[])

Variant of `writeCommandFixed16()` which accepts an array of values, rather than a single value.


#### available()

Determines if the shield is ready to accept a new control packet for reading. This will return `true` if the shield has successfully received a new control packet in least one video frame, and `false` if the no new control data has been received since the last data was read.


#### read(byte data[])

Reads out the control data from the shield from the last received video frame since the previous data was read. Returns the number of bytes of data that were read, or -1 if there was insufficient space in the provided array.


#### read(byte data[], int dataLength)

Reads out the control data from the shield from the last received video frame since the previous data was read. This variant requires the length of the array to be given explicitly. Returns the number of bytes of data that were read, or -1 if there was insufficient space in the provided array.


#### flushRead()

Discards any received control data and requests that shield receive new control data from the next video frame.


#### toFixed16(float value)

Converts a floating-point value to a fixed-point 16-bit integer, returning this as an `int16_t`.


#### fromFixed16(int16_t value)

Converts a fixed-point 16-bit integer to a floating-point type, returning this as a `float`.
