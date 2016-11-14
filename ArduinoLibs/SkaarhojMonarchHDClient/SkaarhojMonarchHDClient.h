#ifndef SkaarhojMonarchHDClient_h
#define SkaarhojMonarchHDClient_h

#include <GenericHTTPClient.h>
#include <HTTPSharedClientWrapper.h>
#include <Ethernet.h>
#include <Arduino.h>

#define PORT 80

// Designed for Monarch HD API version 2.1.1.0030

class SkaarhojMonarchHDClient: public GenericHTTPClient 
{
private:
	String _authStr;
	bool _isRecording;
	bool _isStreaming;
	uint16_t _streamingVideoDataRate;

	String _rtmp;
	String _rtsp;

protected:
	bool sendCommand(String cmd, IPAddress ip, int port);

public:
	SkaarhojMonarchHDClient();
	//void begin(HTTPSharedClientWrapper sharedClient, uint8_t ownerID, IPAddress ip, String authStr);
	void begin(HTTPSharedClientWrapper sharedClient, uint8_t ownerID, IPAddress ip);
	
	bool startRecording();
	bool stopRecording();
	bool startStreaming();
	bool stopStreaming();

	bool getStatus();

	bool startStreamingAndRecording();
	bool stopStreamingAndRecording();
	bool setStreamingVideoDataRate(uint16_t rate);
	uint16_t getStreamingVideoDataRate();
	String getRTSP();
	String getRTMP();
	
	bool setRTMP(String rtmp);
	bool setRTMP(String rtmp, String name, String username, String password);
	bool setRTSP(String rtsp);
	bool setRTSP(String rtsp, int port);

	bool isRecording();
	bool isStreaming();

	void runLoop();
};

#endif