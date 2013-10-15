#pragma once

#include <WinSock2.h>
#include "ObjectPool.h"


struct DatabaseJobRequest : public ObjectPool<DatabaseJobRequest>
{
	DatabaseJobRequest() : mSockKey(NULL) 
	{}

	SOCKET mSockKey ; ///< 어느 클라이언트에서 요청했는지 확인하는 키로 사용
} ;

struct DatabaseJobResult : public ObjectPool<DatabaseJobResult>
{
	DatabaseJobResult() : mSockKey(NULL), mSuccess(false) 
	{}

	SOCKET	mSockKey ;
	bool	mSuccess ;
	
} ;
