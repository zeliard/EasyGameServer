#include "stdafx.h"
#include "EasyServer.h"
#include "DatabaseJobManager.h"

DatabaseJobManager* GDatabaseJobManager = nullptr ;


bool DatabaseJobManager::Initialize()
{
	//TODO: DB 연결하고 초기화 등등

	return true ;
}

void DatabaseJobManager::Finalize()
{
	//TODO: DB 연결 끊고 자원해제 등등
}


/// 아래 함수는 DB 처리 쓰레드에서 불려야 한다
bool DatabaseJobManager::ExecuteDatabaseJobs()
{
	assert( LThreadType == THREAD_DATABASE ) ;

	/// Request가 있는한 처리를 하고 리턴

	DatabaseJobRequest requestJob ;
	while ( mDbJobRequestQueue.PopFront(requestJob) )
	{
		/// 여기서 DB호출해서 처리하고 


		/// 그 결과를 result queue에 담아 놓음
		DatabaseJobResult result ;
		result.mSockKey = requestJob.mSockKey ;

		//TODO ... ...

		result.mSuccess = true ;
		mDbJobResultQueue.PushBack(result) ;
	}
	
	
	return true ;
}

/// 아래 함수는 클라이언트 처리 쓰레드에서 불려야 한다
bool DatabaseJobManager::PushDatabaseJobRequest(const DatabaseJobRequest& request)
{
	assert( LThreadType == THREAD_CLIENT ) ;
	return mDbJobRequestQueue.PushBack(request) ;
}

/// 아래 함수는 클라이언트 처리 쓰레드에서 불려야 한다
bool DatabaseJobManager::PopDatabaseJobResult(DatabaseJobResult& result)
{
	assert( LThreadType == THREAD_CLIENT ) ;
	return mDbJobResultQueue.PopFront(result) ;
}