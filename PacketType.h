#pragma once

#define MAX_NAME_LEN	32
#define MAX_CHAT_LEN	1024

enum PacketTypes
{
	PKT_NONE	= 0,
	
	PKT_CS_LOGIN	= 1,
	PKT_SC_LOGIN	= 2,
	
	PKT_CS_CHAT		= 3,
	PKT_SC_CHAT		= 4,
	
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
	double	mPosX ;
	double	mPosY ;
	double	mPosZ ;
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
	char mChat[1024] ;
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

#pragma pack(pop)