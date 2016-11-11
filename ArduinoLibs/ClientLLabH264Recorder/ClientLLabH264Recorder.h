#ifndef ClientLLabH264Recorder_h
#define ClientLLabH264Recorder_h

#include <GenericHTTPClient.h>
#include <HTTPSharedClientWrapper.h>
#include <Ethernet.h>
#include <Arduino.h>

#define PORT 9993

class ClientLLabH264Recorder: public GenericHTTPClient 
{
private:
	String _username;
	String _firstname;
	String _email;
	
	bool _isRecording;
	bool _isStreaming;
	bool _isAutoSnapshot;
	bool _isUSBAvailable;
	bool _isUSBRecording;

	void updateStatus();
	String requestAndWait(String request, unsigned long timeout);
	String requestAndWait(String request);

public:
	ClientLLabH264Recorder();
	void begin(HTTPSharedClientWrapper sharedClient, uint8_t ownerID, IPAddress ip);
	
	bool startRecording();
	bool stopRecording();
	bool startStreaming();
	bool stopStreaming();
	bool startAutoSnapshots();
	bool stopAutoSnapshots();
	bool takeSnapshot();

	bool clearUserData();

	bool isRecording();
	bool isStreaming();
	bool isAutoSnapshot();

	bool isUSBRecording();
	bool isUSBAvailable();

	bool setUsername(String username);
	bool setEmail(String email);
	bool setFirstname(String firstname);

	
	void runLoop();

	// Getters
	String getUsername();
	String getEmail();
};

#endif