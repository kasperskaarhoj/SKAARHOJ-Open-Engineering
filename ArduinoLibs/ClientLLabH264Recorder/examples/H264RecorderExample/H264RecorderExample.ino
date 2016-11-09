#include <SPI.h>
#include <Ethernet.h>
#include <utility/w5100.h>


// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}

#include <HTTPSharedClientWrapper.h>
#include <SkaarhojBufferTools.h>
#include <SkaarhojASCIIClient.h>
#include <GenericHTTPClient.h>
#include <ClientLLabH264Recorder.h>

HTTPSharedClientWrapper sharedClient;
ClientLLabH264Recorder H264Recorder;

#include <SkaarhojSerialServer.h>
SkaarhojSerialServer SerialServer(Serial);

IPAddress serverip(10,0,0,124);

// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(10, 0, 0, 249);                 // <= SETUP!  IP address of the Arduino

void handleSerialIncoming()  { 
  char* serialBuffer = SerialServer.getRemainingBuffer();
  if(SerialServer.isBufferEqualTo_P(PSTR("START_RECORDING"))) {
    H264Recorder.startRecording();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("STOP_RECORDING"))) {
    H264Recorder.stopRecording();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("IS_RECORDING"))) {
    H264Recorder.isRecording();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("START_STREAMING"))) {
    H264Recorder.startStreaming();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("STOP_STREAMING"))) {
    H264Recorder.stopStreaming();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("IS_STREAMING"))) {
    H264Recorder.isStreaming();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("START_AUTOSNAPSHOTS"))) {
    H264Recorder.startAutoSnapshots();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("STOP_AUTOSNAPSHOTS"))) {
    H264Recorder.stopAutoSnapshots();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("IS_AUTOSNAPSHOTS_RUNNING"))) {
    H264Recorder.isAutoSnapshot();
  }  else if (SerialServer.isBufferEqualTo_P(PSTR("IS_USB_AVAILABLE"))) {
    H264Recorder.isUSBAvailable();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("IS_USB_RECORDING"))) {
    H264Recorder.isUSBRecording();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("TAKE_SNAPSHOT"))) {
    H264Recorder.takeSnapshot();
  } else if(SerialServer.isBufferEqualTo_P(PSTR("GET_USER"))) {
    Serial.println(H264Recorder.getUsername());
  } else if(SerialServer.isBufferEqualTo_P(PSTR("GET_EMAIL"))) {
    Serial.println(H264Recorder.getEmail());
  }
}
 
void receivedResponse(String request, String response, bool success) {
  Serial.println("Received response: " + response + " to request: " + request);
}

void setup() {
  Ethernet.begin(mac,ip);
    
  // start the serial SERVER library (taking in commands from serial monitor):
  SerialServer.begin(115200);
  SerialServer.setHandleIncomingLine(handleSerialIncoming);  // Put only the name of the function
  SerialServer.serialOutput(3);
  SerialServer.enableEOLTimeout();  // Calling this without parameters sets EOL Timeout to 2ms which is enough for 9600 baud and up. Use only if the serial client (like Arduinos serial monitor) doesn't by itself send a <cr> token.
  Serial << F("Type stuff in the serial monitor to send over telnet...\n");  
  Serial << F("\nSupported commands:\n\n");
  Serial << F("START_RECORDING\n");
  Serial << F("STOP_RECORDING\n");
  Serial << F("IS_RECORDING\n");
  Serial << F("START_STREAMING\n");
  Serial << F("STOP_STREAMING\n");
  Serial << F("IS_STREAMING\n");
  Serial << F("START_AUTOSNAPSHOTS\n");
  Serial << F("STOP_AUTOSNAPSHOTS\n");
  Serial << F("IS_AUTOSNAPSHOTS_RUNNING\n");
  Serial << F("IS_USB_AVAILABLE\n");
  Serial << F("IS_USB_RECORDING\n");
  Serial << F("TAKE_SNAPSHOT\n");
  Serial << F("GET_USER\n");
  Serial << F("GET_EMAIL\n\n");

  
  W5100.setRetransmissionTime(1000);  // Milli seconds
  W5100.setRetransmissionCount(1);
  
  sharedClient.serialOutput(false);
  
  H264Recorder.begin(sharedClient, 1, serverip);
  H264Recorder.setResponseCallback(receivedResponse);
  H264Recorder.serialOutput(1);
}

void loop() {
  SerialServer.runLoop();
  H264Recorder.runLoop();
}
