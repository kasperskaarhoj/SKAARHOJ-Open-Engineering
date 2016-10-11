#include <SPI.h>
#include <Ethernet.h>
#include <utility/w5100.h>

#include <Streaming.h>

#include <HTTPSharedClientWrapper.h>
#include <SkaarhojBufferTools.h>
#include <SkaarhojASCIIClient.h>
#include <GenericHTTPClient.h>
#include <SkaarhojMonarchHDClient.h>

HTTPSharedClientWrapper sharedClient;
SkaarhojMonarchHDClient MonarchHD;

#include <SkaarhojSerialServer.h>
SkaarhojSerialServer SerialServer(Serial);

IPAddress serverip(192,168,10,200);

// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0xAE, 0xB7 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192,168,10,201);                 // <= SETUP!  IP address of the Arduino

void handleSerialIncoming()  { 
  char* serialBuffer = SerialServer.getRemainingBuffer();
  if(SerialServer.isBufferEqualTo_P(PSTR("StartRecording"))) {
    MonarchHD.startRecording();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("StopRecording"))) {
    MonarchHD.stopRecording();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("StartStreaming"))) {
    MonarchHD.startStreaming();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("StopStreaming"))) {
    MonarchHD.stopStreaming();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("GetStatus"))) {
    MonarchHD.getStatus();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("IsRecording"))) {
    MonarchHD.isStreaming();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("IsStreaming"))) {
    MonarchHD.isRecording();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("GetRTMP"))) {
    MonarchHD.getRTMP();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("GetRTSP"))) {
    MonarchHD.getRTSP();
  } else if (SerialServer.isBufferEqualTo_P(PSTR("GetStreamingVideoDataRate"))) {
    MonarchHD.getStreamingVideoDataRate();
  } else if(strncmp(serialBuffer, "SetRTMP=", 8) == 0) {
    MonarchHD.setRTMP(String(serialBuffer + 8));
  } else if(strncmp(serialBuffer, "SetRTSP=", 8) == 0) {
    MonarchHD.setRTSP(String(serialBuffer + 8));
  } else if(strncmp(serialBuffer, "SetStreamingVideoDataRate=", 8) == 0) {
    MonarchHD.setStreamingVideoDataRate(atol(serialBuffer + 26));
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
  Serial << F("StartRecording\n");
  Serial << F("StopRecording\n");
  Serial << F("StartStreaming\n");
  Serial << F("StopStreaming\n");
  Serial << F("GetStatus\n");
  Serial << F("IsRecording\n");
  Serial << F("IsStreaming\n");
  Serial << F("GetRTMP\n");
  Serial << F("GetRTSP\n");
  Serial << F("SetRTMP=\n");
  Serial << F("SetRTSP=\n");
  Serial << F("GetStreamingVideoDataRate\n");
  Serial << F("SetStreamingVideoDataRate\n\n\n");

  
  W5100.setRetransmissionTime(1000);  // Milli seconds
  W5100.setRetransmissionCount(1);
  
  sharedClient.serialOutput(false);
  

  // If AUTH is needed
  //MonarchHD.begin(sharedClient, 1, serverip, "YWRtaW46YWRtaW4=");
  
  MonarchHD.begin(sharedClient, 1, serverip);

  MonarchHD.setResponseCallback(receivedResponse);
  MonarchHD.serialOutput(1);

  MonarchHD.startStreaming();
}

void loop() {
  SerialServer.runLoop();
  MonarchHD.runLoop();
}
