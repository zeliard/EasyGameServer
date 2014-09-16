#pragma once

#include <map>
#include <WinSock2.h>
#include "Config.h"
#include "..\..\PacketType.h"
#include "CircularBuffer.h"
#include "ObjectPool.h"

#define BUFSIZE	(1024*10)

class ClientSession ;
class ClientManager ;
struct DatabaseJobContext ;

struct OverlappedIO : public OVERLAPPED
{
	OverlappedIO() : mObject(nullptr)
	{}

	ClientSession* mObject ;
} ;

class ClientSession : public ObjectPool<ClientSession>
{
public:
	ClientSession(SOCKET sock)
		: mConnected(false), mLogon(false), mSocket(sock), mPlayerId(-1), mSendBuffer(BUFSIZE), mRecvBuffer(BUFSIZE), mOverlappedRequested(0)
		, mPosX(0), mPosY(0), mDbUpdateCount(0)
	{
		memset(&mClientAddr, 0, sizeof(SOCKADDR_IN)) ;
		memset(mPlayerName, 0, sizeof(mPlayerName)) ;
	}
	~ClientSession() {}


public:

	int	GetPlayerId() const	{ return mPlayerId; }
	const char* GetPlayerName() const { return mPlayerName;  }
	SOCKET GetSocketKey() const { return mSocket;  }
	void SetPosition(float x, float y) { 	mPosX = x; 	mPosY = y; }

	void	LoginDone(int pid, float x, float y, const char* name);
	void	UpdateDone();


public: 

	template <class PKT_TYPE>
	bool ParsePacket(PKT_TYPE& pkt)
	{
		return mRecvBuffer.Read((char*)&pkt, pkt.mSize);
	}

	void	OnRead(size_t len) ;
	void	OnWriteComplete(size_t len) ;

	bool	OnConnect(SOCKADDR_IN* addr) ;
	
	bool	PostRecv() ;

	bool	SendRequest(PacketHeader* pkt) ;
	bool	Broadcast(PacketHeader* pkt) ;

	void	Disconnect() ;

	bool	IsConnected() const { return mConnected ; }

	void	DatabaseJobDone(DatabaseJobContext* result) ;

	/// 현재 Send/Recv 요청 중인 상태인지 검사하기 위함
	void	IncOverlappedRequest()		{ ++mOverlappedRequested ; }
	void	DecOverlappedRequest()		{ --mOverlappedRequested ; }
	bool	DoingOverlappedOperation() const { return mOverlappedRequested > 0 ; }

	bool	SendFlush(); ///< Send요청 중인것들 모아서 보냄
	void	OnTick() ;


private:
	float			mPosX ;
	float			mPosY ;
	char			mPlayerName[MAX_NAME_LEN] ;

private:
	bool			mConnected ;
	bool			mLogon ;
	SOCKET			mSocket ;

	int				mPlayerId ;
	SOCKADDR_IN		mClientAddr ;

	CircularBuffer	mSendBuffer ;
	CircularBuffer	mRecvBuffer ;

	OverlappedIO	mOverlappedSend ;
	OverlappedIO	mOverlappedRecv ;
	int				mOverlappedRequested ;

	int				mDbUpdateCount ; ///< DB에 주기적으로 업데이트 하기 위한 변수

	friend class ClientManager ;
} ;


void CALLBACK RecvCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags) ;
void CALLBACK SendCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags) ;
