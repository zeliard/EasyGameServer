#pragma once

#include <string.h>

#define MAX_CHAT_LEN	256

#define MAX_NAME_LEN	30
#define MAX_COMMENT_LEN	40

enum PacketTypes
{
	PKT_NONE	= 0,
	
	PKT_CS_LOGIN	= 1,
	PKT_SC_LOGIN	= 2,
	
	PKT_CS_CHAT		= 11,
	PKT_SC_CHAT		= 12,

	PKT_CS_MOVE		= 21,
	PKT_SC_MOVE		= 22,

	PKT_MAX	= 1024
} ;

#pragma pack(push, 1)

struct PacketHeader
{
	PacketHeader() : mSize(0), mType(PKT_NONE) 	{}
	short mSize ;
	short mType ;
} ;



struct LoginRequest : public PacketHeader
{
	LoginRequest()
	{
		mSize = sizeof(LoginRequest) ;
		mType = PKT_CS_LOGIN ;
		mPlayerId = -1 ;
	}

	int	mPlayerId ;
} ;

struct LoginResult : public PacketHeader
{
	LoginResult()
	{
		mSize = sizeof(LoginResult) ;
		mType = PKT_SC_LOGIN ;
		mPlayerId = -1 ;
		memset(mName, 0, MAX_NAME_LEN) ;
	}

	int		mPlayerId ;
	float	mPosX ;
	float	mPosY ;
	char	mName[MAX_NAME_LEN] ;

} ;

struct ChatBroadcastRequest : public PacketHeader
{
	ChatBroadcastRequest()
	{
		mSize = sizeof(ChatBroadcastRequest) ;
		mType = PKT_CS_CHAT ;
		mPlayerId = -1 ;
	
		memset(mChat, 0, MAX_CHAT_LEN) ;
	}

	int	mPlayerId ;
	char mChat[MAX_CHAT_LEN] ;
} ;

struct ChatBroadcastResult : public PacketHeader
{
	ChatBroadcastResult()
	{
		mSize = sizeof(ChatBroadcastResult) ;
		mType = PKT_SC_CHAT ;
		mPlayerId = -1 ;
		
		memset(mName, 0, MAX_NAME_LEN) ;
		memset(mChat, 0, MAX_CHAT_LEN) ;
	}
	
	int	mPlayerId ;
	char mName[MAX_NAME_LEN] ;
	char mChat[MAX_CHAT_LEN] ;
} ;


struct MoveRequest : public PacketHeader
{
	MoveRequest()
	{
		mSize = sizeof(MoveRequest);
		mType = PKT_CS_MOVE;
		mPlayerId = -1;
		mPosX = 0;
		mPosY = 0;
	}

	int		mPlayerId;
	float	mPosX;
	float	mPosY;
};

struct MoveBroadcastResult : public PacketHeader
{
	MoveBroadcastResult()
	{
		mSize = sizeof(MoveBroadcastResult);
		mType = PKT_SC_MOVE;
		mPlayerId = -1;
		mPosX = 0;
		mPosY = 0;
	}

	int		mPlayerId;
	float	mPosX;
	float	mPosY;
};



#pragma pack(pop)