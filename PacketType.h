#pragma once


enum PacketTypes
{
	PKT_NONE	= 0,
	
	PKT_CS_PING	= 1,
	PKT_SC_PONG	= 2,
	
	PKT_CS_PING2= 3,
	PKT_SC_PONG2= 4,
	
} ;

#pragma pack(push, 1)

struct PacketHeader
{
	PacketHeader() : mSize(0), mType(PKT_NONE) 	{}
	short mSize ;
	short mType ;
} ;



struct TestPing : public PacketHeader
{
	TestPing()
	{
		mSize = sizeof(TestPing) ;
		mType = PKT_CS_PING ;
		mPlayerId = -1 ;
		mPosX = 0 ;
		mPosY = 0 ;
		mPosZ = 0 ;
	}

	int	mPlayerId ;
	float mPosX ;
	float mPosY ;
	float mPosZ ;
} ;

struct TestPong : public PacketHeader
{
	TestPong()
	{
		mSize = sizeof(TestPong) ;
		mType = PKT_SC_PONG ;
		mPlayerId = -1 ;
		mResult = false ;
		memset(mData, 0, 1024) ;
	}
	int	mPlayerId ;
	char mData[1024] ;
	bool mResult ;
} ;

struct TestPing2 : public PacketHeader
{
	TestPing2()
	{
		mSize = sizeof(TestPing2) ;
		mType = PKT_CS_PING2 ;
		mPlayerId = -1 ;
		mPosX = 0 ;
		mPosY = 0 ;
		mPosZ = 0 ;
		memset(mData, 0, 1024) ;
	}

	int	mPlayerId ;
	char mData[1024] ;
	float mPosX ;
	float mPosY ;
	float mPosZ ;
} ;

struct TestPong2 : public PacketHeader
{
	TestPong2()
	{
		mSize = sizeof(TestPong2) ;
		mType = PKT_SC_PONG2 ;
		mPlayerId = -1 ;
		mResult = false ;
		mPosX = 0 ;
		mPosY = 0 ;
		mPosZ = 0 ;
	}
	
	int	mPlayerId ;
	bool mResult ;
	float mPosX ;
	float mPosY ;
	float mPosZ ;

} ;

#pragma pack(pop)