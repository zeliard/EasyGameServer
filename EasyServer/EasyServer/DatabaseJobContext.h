#pragma once

#include <WinSock2.h>
#include "Config.h"
#include "..\..\PacketType.h"
#include "ObjectPool.h"


/// 모든 DB관련 작업은 아래의 추상 클래스를 상속받아 사용해야 함
struct DatabaseJobContext
{
	DatabaseJobContext() : mSuccess(false)
	{}

	DatabaseJobContext(SOCKET socketKey) :  mSockKey(socketKey), mSuccess(false) 
	{}

	virtual ~DatabaseJobContext() {}

	virtual bool OnExecute() = 0 ;

	void SetSuccess(bool success) { mSuccess = success ; }

	SOCKET	mSockKey ;
	bool	mSuccess ;
} ;


/// player load 작업
struct LoadPlayerDataContext : public DatabaseJobContext, public ObjectPool<LoadPlayerDataContext>
{
	LoadPlayerDataContext(SOCKET socketKey, int playerId) : DatabaseJobContext(socketKey)
		, mPlayerId(playerId), mPosX(0), mPosY(0), mPosZ(0)
	{
		memset(mPlayerName, 0, sizeof(mPlayerName)) ;
	}


	virtual bool OnExecute() ;


	int		mPlayerId ;

	///참고: 현재 DB에서는 xyz좌표를 double로 쓰고 있지만, 게임에서는 float로 xy만 쓰고 있음
	double	mPosX ;
	double	mPosY ;
	double	mPosZ ; 
	char	mPlayerName[MAX_NAME_LEN] ;

} ;


/// Player 생성 작업
struct CreatePlayerDataContext : public DatabaseJobContext, public ObjectPool<CreatePlayerDataContext>
{
	CreatePlayerDataContext() 
		: mPlayerId(-1), mPosX(0), mPosY(0), mPosZ(0)
	{
		memset(mPlayerName, 0, sizeof(mPlayerName)) ;
		memset(mComment, 0, sizeof(mComment)) ;
	}

	virtual bool OnExecute() ;

	int		mPlayerId ;

	double	mPosX ;
	double	mPosY ;
	double	mPosZ ;
	char	mPlayerName[MAX_NAME_LEN] ;
	char	mComment[MAX_COMMENT_LEN] ;
} ;



/// Player 삭제 작업
struct DeletePlayerDataContext : public DatabaseJobContext, public ObjectPool<DeletePlayerDataContext>
{
	DeletePlayerDataContext(int playerId) : mPlayerId(playerId)
	{}

	virtual bool OnExecute() ;

	int		mPlayerId ;

} ;

/// Player 업데이트 작업
struct UpdatePlayerDataContext : public DatabaseJobContext, public ObjectPool<UpdatePlayerDataContext>
{
	UpdatePlayerDataContext(SOCKET socketKey, int playerId) : DatabaseJobContext(socketKey)
		, mPlayerId(playerId), mPosX(0), mPosY(0), mPosZ(0)
	{
		memset(mComment, 0, sizeof(mComment)) ;
	}

	virtual bool OnExecute() ;

	int		mPlayerId ;

	double	mPosX ;
	double	mPosY ;
	double	mPosZ ;
	char	mComment[MAX_COMMENT_LEN] ;
} ;
