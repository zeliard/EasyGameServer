#pragma once
#include <string>
#include "CircularBuffer.h"

#ifndef _WIN32
#include <netdb.h>
#include <unistd.h>
#else
#include <winsock2.h>
#endif

#define BUF_SIZE	32768

class TcpClient
{
public:
	static TcpClient* getInstance();
	static void destroyInstance();

	bool connect(const char* serverAddr, int port);
	

	/// request test 
	void loginRequest();
	void chatRequest(const char* chat);
	void moveRequest(float x, float y);

	
private:
	TcpClient();
	virtual ~TcpClient();

	bool initialize();
	bool send(const char* data, int length);
	
	void networkThread();
	void processPacket();

private:

	SOCKET			m_sock;
	CircularBuffer	m_recvBuffer;

	int m_loginId;

};

