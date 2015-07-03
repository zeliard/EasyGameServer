#pragma once

#include <map>
#include <sys/socket.h>
#include <netdb.h>

#include "Config.h"
#include "EasyGameServerLinux.h"
#include "PacketType.h"
#include "CircularBuffer.h"
#include "ObjectPool.h"
#include "RefCountable.h"


class ClientSession ;
class ClientManager ;
struct DatabaseJobContext ;


class ClientSession : public RefCountable, public ObjectPool<ClientSession>
{
public:
	ClientSession(SOCKET sock)
		: mConnected(false), mLogon(false), mSocket(sock), mPlayerId(-1), mSendBuffer(BUFSIZE), mRecvBuffer(BUFSIZE)
		, mPosX(0), mPosY(0)
	{
		memset(&mClientAddr, 0, sizeof(sockaddr_in)) ;
		memset(mPlayerName, 0, sizeof(mPlayerName)) ;
	}

	virtual ~ClientSession();

	void	OnConnect(sockaddr_in* addr);
	void	Disconnect();
	void	OnRelease();
	void	OnReceive();

	bool	SendRequest(PacketHeader* pkt);
	bool	SendFlush(); ///< Send요청 중인것들 모아서 보냄

public:
	int	GetPlayerId() const	{ return mPlayerId; }
	const char* GetPlayerName() const { return mPlayerName;  }
	SOCKET GetSocketKey() const { return mSocket;  }
	void SetPosition(float x, float y) { mPosX = x; mPosY = y; }

	void	LoginDone(int pid, float x, float y, const char* name);
	void	UpdateDone();

public: 
	bool	IsConnected() const { return mConnected; }
	void	OnTick();
	void	OnDbUpdate(); ///< 주기적으로 데이터베이스에 업데이트

	template <class PKT_TYPE>
	bool ParsePacket(PKT_TYPE& pkt)
	{
		return mRecvBuffer.Read((char*)&pkt, pkt.mSize);
	}

	void DispatchPacket();
	bool Broadcast(PacketHeader* pkt) ;
	void DatabaseJobDone(DatabaseJobContext* result);

private:
	float			mPosX ;
	float			mPosY ;
	char			mPlayerName[MAX_NAME_LEN] ;

private:
	bool			mConnected ;
	bool			mLogon ;
	SOCKET			mSocket ;

	int				mPlayerId ;
	sockaddr_in		mClientAddr ;

	CircularBuffer	mSendBuffer ;
	CircularBuffer	mRecvBuffer ;

	friend class ClientManager ;
} ;


