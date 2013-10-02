#pragma once

#include "..\..\PacketType.h"
#include "CircularBuffer.h"
#include <map>
#include <WinSock2.h>

#define BUFSIZE	(1024*10)
class ClientSession ;
class ClientManager ;

struct OverlappedIO : public OVERLAPPED
{
	OverlappedIO() :mObject(nullptr)
	{}

	ClientSession* mObject ;
} ;

class ClientSession
{
public:
	ClientSession(SOCKET sock, int idx)
		: mSocket(sock), mClientId(idx), mSendBuffer(BUFSIZE), mRecvBuffer(BUFSIZE)
	{
		memset(&mClientAddr, 0, sizeof(SOCKADDR_IN)) ;
	}
	~ClientSession() {}


	void	OnRead(size_t len) ;
	void	OnWriteComplete(size_t len) ;

	bool	OnConnect(SOCKADDR_IN* addr) ;
	
	bool	PostRecv() ;

	bool	Send(PacketHeader* pkt) ;
	void	Disconnect() ;


private:
	SOCKET			mSocket ;

	int				mClientId ;
	SOCKADDR_IN		mClientAddr ;

	CircularBuffer	mSendBuffer ;
	CircularBuffer	mRecvBuffer ;

	OverlappedIO	mOverlappedSend ;
	OverlappedIO	mOverlappedRecv ;

	friend class ClientManager ;
} ;




void CALLBACK RecvCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags) ;
void CALLBACK SendCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags) ;
